#include <iostream>
#include <deque>
#include <pez.h>
#include <glew.h>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include "glvideo.h"
#include "debug.h"

using namespace std;

static const std::string VERTEX_SHADER_SOURCE =
R"EOF(
#version 130
in vec2 aPosition;
in vec2 aTexCoord;
out vec2 vTexCoord;
uniform mat4 uModelMatrix;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = uModelMatrix * vec4(aPosition, 0, 1);
}
)EOF";

static const std::string FRAGMENT_SHADER_SOURCE =
R"EOF(
#version 130
in vec2 vTexCoord;
uniform sampler2DRect Sampler;
out vec4 oColor;

void main()
{
   oColor = texture(Sampler, vTexCoord);
}
)EOF";

static const std::string YCoCg_FRAGMENT_SHADER_SOURCE =
R"EOF(
#version 130
in vec2 vTexCoord;
uniform sampler2DRect Sampler;
out vec4 oColor;

void main()
{
    vec4 color = texture(Sampler, vTexCoord);
    float Co = color.x - ( 0.5 * 256.0 / 255.0 );
    float Cg = color.y - ( 0.5 * 256.0 / 255.0 );
    float Y = color.w;
    oColor = vec4( Y + Co - Cg, Y + Cg, Y - Co - Cg, color.a );
}
)EOF";


const float COORD_EXTENTS = 1.f;
const int NUM_MOVIES = 9;
std::vector< glvideo::Movie::ref > movies;
glvideo::Context::ref context;

static void BuildGeometry( int width, int height );

static GLuint LoadEffect( bool isYCoCg = false );

deque<unsigned int> frameTimes;
static unsigned int sumElapsedMilliseconds = 0;
typedef chrono::high_resolution_clock hrclock;
static hrclock::time_point lastReportTime = hrclock::now();
GLuint shader;

enum {
    PositionSlot, TexCoordSlot
};

float randf() { return static_cast< float >( rand() ) / static_cast< float >( RAND_MAX ); }


void PezHandleMouse( int x, int y, int action ) {}

void PezUpdate( unsigned int elapsedMilliseconds )
{
    frameTimes.push_back( elapsedMilliseconds );
    sumElapsedMilliseconds += elapsedMilliseconds;
    while ( frameTimes.size() > 100 ) {
        sumElapsedMilliseconds -= frameTimes.front();
        frameTimes.pop_front();
    }

    for ( auto & movie : movies ) movie->update();

    auto now = hrclock::now();
    if ( chrono::duration_cast<chrono::seconds>( now - lastReportTime ).count() > 1 ) {
        double avg = (double) sumElapsedMilliseconds / (double) frameTimes.size();
        double fps = 1000.0 / avg;
        DBOUT( "Frame AVG ms: " << setprecision( 2 ) << avg << "ms (" << fps << " fps)" );
        lastReportTime = now;

		//for ( auto & movie : movies ) {
  //          if ( randf() < 0.5f ) {
  //              movie->seek( randf() * movie->getDuration() );
  //              DBOUT( "SEEK" );
  //          }
  //          else {
  //              if ( movie->isPlaying() && randf() < 0.5f ) {
  //                  movie->stop();
  //                  DBOUT( "STOP" );
  //              }
  //              else if ( ! movie->isPlaying() && randf() < 0.75f ) {
  //                  movie->play();
  //                  DBOUT( "PLAY" );
  //              }
  //          }
		//}
    }
}


void PezRender()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glActiveTexture( GL_TEXTURE0 );

	int n = (int)sqrt( movies.size() );
	float size = COORD_EXTENTS * 2.f / (float)n;
	int i = 0;
	for ( auto & movie : movies ) {
		auto frame = movie->getCurrentFrame();
		if ( frame ) {
			float x = -COORD_EXTENTS + size * (float)( i % n ) + size * 0.5f;
			float y = COORD_EXTENTS - size * (float)( (int)( i / n ) ) - size * 0.5f;
			float s[ 3 ] = { size * 0.5f, size * 0.5f, 1.f };
			float tx[ 3 ] = { x, y, 0.f };
			float mtx[ 16 ] = {
				s[ 0 ],		0.f,		0.f,		0.f,
				0.f,		s[ 1 ],		0.f,		0.f,
				0.f,		0.f,		s[ 2 ],		0.f,
				tx[ 0 ],	tx[ 1 ],	tx[ 2 ],	1.f
			};
			GLint loc = glGetUniformLocation( shader, "uModelMatrix" );
			glUniformMatrix4fv( loc, 1, GL_FALSE, mtx );

			glBindTexture( frame->getTextureTarget(), frame->getTextureId() );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			glBindTexture( frame->getTextureTarget(), 0 );
		}
		else {
			DBOUT( "NO FRAME!" );
		}

		++i;
	}

	checkGlError( __FILE__, __LINE__ );
}

const char *PezInitialize( int width, int height )
{
    string filename = "examples/videos/hap-1920x1080-24fps.mov";
	srand( static_cast< unsigned >( time( 0 ) ) );

    DBOUT( "OpenGL version: " << glGetString( GL_VERSION ) );

	context = glvideo::Context::create( 4 );
    auto options = glvideo::Movie::Options();
	for ( int i = 0; i < NUM_MOVIES; ++i ) {
		auto movie = glvideo::Movie::create( context, filename, options );
		movies.push_back( movie );

		DBOUT( "Format: " << movie->getFormat() );
		DBOUT( "Duration (seconds): " << movie->getDuration() );
		DBOUT( "Size: " << movie->getWidth() << "x" << movie->getHeight() );
		DBOUT( "Framerate: " << movie->getFramerate() );
		DBOUT( "Number of tracks: " << movie->getNumTracks() );
		for ( int i = 0; i < movie->getNumTracks(); ++i ) {
			DBOUT( "\tTrack " << i << " type: " << movie->getTrackDescription( i ) );
		}

		movie->loop().play();
	}

    BuildGeometry( movies[0]->getWidth(), movies[0]->getHeight() );
	shader = LoadEffect( movies[0]->getCodec() == "HapY" );


    return "Test Playback";
}


static void BuildGeometry( int width, int height )
{
    float X = COORD_EXTENTS;
    float Y = COORD_EXTENTS;
    float verts[] = {
            -X, -Y, 0.f * (float)width, 1.f * (float)height,
            -X, +Y, 0.f * (float)width, 0.f * (float)height,
            +X, +Y, 1.f * (float)width, 0.f * (float)height,
            +X, +Y, 1.f * (float)width, 0.f * (float)height,
            +X, -Y, 1.f * (float)width, 1.f * (float)height,
            -X, -Y, 0.f * (float)width, 1.f * (float)height,
    };

    GLuint vboHandle;
    GLsizeiptr vboSize = sizeof( verts );
    GLsizei stride = 4 * sizeof( float );
    GLenum usage = GL_STATIC_DRAW;
    GLvoid *texCoordOffset = (GLvoid *) (sizeof( float ) * 2);

#if defined(GLVIDEO_MSW)
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
#endif

    glGenBuffers( 1, &vboHandle );
	glBindBuffer( GL_ARRAY_BUFFER, vboHandle );
	glBufferData( GL_ARRAY_BUFFER, vboSize, verts, usage );
	glVertexAttribPointer( PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0 );
	glVertexAttribPointer( TexCoordSlot, 2, GL_FLOAT, GL_FALSE, stride, texCoordOffset );
	glEnableVertexAttribArray( PositionSlot );
	glEnableVertexAttribArray( TexCoordSlot );
}

static GLuint LoadEffect( bool isYCoCg )
{
    const char *vsSource = VERTEX_SHADER_SOURCE.c_str();
    const char *fsSource;
    if ( isYCoCg ) {
        fsSource = YCoCg_FRAGMENT_SHADER_SOURCE.c_str();
    } else {
        fsSource = FRAGMENT_SHADER_SOURCE.c_str();
    }
    GLuint vsHandle, fsHandle;
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];
    GLuint programHandle;

    vsHandle = glCreateShader( GL_VERTEX_SHADER );
    fsHandle = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vsHandle, 1, &vsSource, 0 );
    glCompileShader( vsHandle );
    glGetShaderiv( vsHandle, GL_COMPILE_STATUS, &compileSuccess );
    glGetShaderInfoLog( vsHandle, sizeof( compilerSpew ), 0, compilerSpew );
    PezCheckCondition( compileSuccess, compilerSpew );

    glShaderSource( fsHandle, 1, &fsSource, 0 );
    glCompileShader( fsHandle );
    glGetShaderiv( fsHandle, GL_COMPILE_STATUS, &compileSuccess );
    glGetShaderInfoLog( fsHandle, sizeof( compilerSpew ), 0, compilerSpew );
    PezCheckCondition( compileSuccess, compilerSpew );

    programHandle = glCreateProgram();
    glAttachShader( programHandle, vsHandle );
    glAttachShader( programHandle, fsHandle );
    glBindAttribLocation( programHandle, PositionSlot, "aPosition" );
    glBindAttribLocation( programHandle, TexCoordSlot, "aTexCoord" );
    glLinkProgram( programHandle );
    glGetProgramiv( programHandle, GL_LINK_STATUS, &linkSuccess );
    glGetProgramInfoLog( programHandle, sizeof( compilerSpew ), 0, compilerSpew );
    PezCheckCondition( linkSuccess, compilerSpew );

    glUseProgram( programHandle );

    GLint loc = glGetUniformLocation( programHandle, "Sampler" );
    glUniform1i( loc, 0 );

	return programHandle;
}
