#include <pez.h>
#include <glew.h>
#include <glsw.h>

static void BuildGeometry();
static void LoadEffect();

enum { PositionSlot, ColorSlot };

void PezHandleMouse(int x, int y, int action) { }

void PezUpdate(unsigned int elapsedMilliseconds) { }

void PezRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

const char* PezInitialize(int width, int height)
{
    BuildGeometry();
    LoadEffect();
    return "Pez Intro";
}

static void BuildGeometry()
{
    float verts[] = {
        -0.5f, -0.5f,  1,1,0, // Yellow
        +0.0f, +0.5f,  1,0,1, // Magenta
        +0.5f, -0.5f,  0,1,1, // Cyan
    };

    GLuint vboHandle;
    GLsizeiptr vboSize = sizeof(verts);
    GLsizei stride = 5 * sizeof(float);
    GLenum usage = GL_STATIC_DRAW;
    GLvoid* colorOffset = (GLvoid*) (sizeof(float) * 2);

    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, verts, usage);
    glVertexAttribPointer(PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(ColorSlot, 3, GL_FLOAT, GL_FALSE, stride, colorOffset);
    glEnableVertexAttribArray(PositionSlot);
    glEnableVertexAttribArray(ColorSlot);
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

    vsSource = glswGetShader("Simple.Vertex." PEZ_GL_VERSION_TOKEN);
    fsSource = glswGetShader("Simple.Fragment." PEZ_GL_VERSION_TOKEN);
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
    glBindAttribLocation(programHandle, ColorSlot, "Color");
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(programHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, compilerSpew);

    glUseProgram(programHandle);
}
