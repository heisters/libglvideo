#include <pez.h>
#include <glew.h>
#include <glsw.h>
#include <pnglite.h>
#include <stdlib.h>

static void BuildGeometry();
static void LoadEffect();
static void LoadTexture();

enum { PositionSlot, TexCoordSlot };

void PezHandleMouse(int x, int y, int action) { }

void PezUpdate(unsigned int elapsedMilliseconds) { }

void PezRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

const char* PezInitialize(int width, int height)
{
    BuildGeometry();
    LoadEffect();
    LoadTexture();
    return "Pez Intro";
}

static void LoadTexture()
{
    png_t gandhi;
    unsigned char* data;
    GLuint textureHandle;

    png_init(0, 0);
    png_open_file_read(&gandhi, "../demo/Gandhi.png");
    data = (unsigned char*) malloc(gandhi.width * gandhi.height * gandhi.bpp);
    png_get_data(&gandhi, data);

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gandhi.width, gandhi.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    png_close_file(&gandhi);
    free(data);
}

static void BuildGeometry()
{
    #define X (0.5f * PEZ_VIEWPORT_HEIGHT / PEZ_VIEWPORT_WIDTH)
    #define Y (0.5f)
    float verts[] = {
        -X, -Y,  0,1,
        -X, +Y,  0,0,
        +X, +Y,  1,0,
        +X, +Y,  1,0,
        +X, -Y,  1,1,
        -X, -Y,  0,1,
    };
    #undef X
    #undef Y

    GLuint vboHandle;
    GLsizeiptr vboSize = sizeof(verts);
    GLsizei stride = 4 * sizeof(float);
    GLenum usage = GL_STATIC_DRAW;
    GLvoid* texCoordOffset = (GLvoid*) (sizeof(float) * 2);

    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, verts, usage);
    glVertexAttribPointer(PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(TexCoordSlot, 2, GL_FLOAT, GL_FALSE, stride, texCoordOffset);
    glEnableVertexAttribArray(PositionSlot);
    glEnableVertexAttribArray(TexCoordSlot);
}

static void LoadEffect()
{
    const char* vsSource, * fsSource;
    GLuint vsHandle, fsHandle;
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];
    GLuint programHandle;

    glswInit();
    glswSetPath("../demo/", ".glsl");
    glswAddDirectiveToken("GL3", "#version 130");

    vsSource = glswGetShader("Simple.Vertex.Textured." PEZ_GL_VERSION_TOKEN);
    fsSource = glswGetShader("Simple.Fragment.Textured." PEZ_GL_VERSION_TOKEN);
    PezCheckCondition(vsSource, "Can't find vertex shader.\n");
    PezCheckCondition(fsSource, "Can't find fragment shader.\n");

    vsHandle = glCreateShader(GL_VERTEX_SHADER);
    fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);

    glShaderSource(fsHandle, 1, &fsSource, 0);
    glCompileShader(fsHandle);
    glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);

    programHandle = glCreateProgram();
    glAttachShader(programHandle, vsHandle);
    glAttachShader(programHandle, fsHandle);
    glBindAttribLocation(programHandle, PositionSlot, "Position");
    glBindAttribLocation(programHandle, TexCoordSlot, "InCoord");
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(programHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, compilerSpew);

    glUseProgram(programHandle);
}
