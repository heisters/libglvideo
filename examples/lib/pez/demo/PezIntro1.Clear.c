#include <pez.h>
#include <glew.h>
#include <math.h>

static float time = 0;
static int pressing = 0;
static float speed = 0.01f;

const char* PezInitialize(int width, int height)
{
    return "Pez Intro";
}

void PezRender()
{
    if (pressing) {
        glClearColor(1, 1, 0.75f, 1);
    } else {
        float blue = 0.5f * (1.0f + sinf(time));
        glClearColor(0, 0.25f, blue, 1);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void PezUpdate(unsigned int elapsedMilliseconds)
{
    time += elapsedMilliseconds * speed;
}

void PezHandleMouse(int x, int y, int action)
{
    if (action == PEZ_DOWN)
        pressing = 1;
    else if (action == PEZ_UP)
        pressing = 0;
}
