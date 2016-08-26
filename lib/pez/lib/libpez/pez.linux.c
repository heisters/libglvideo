// Pez was developed by Philip Rideout and released under the MIT License.

#include <sys/time.h>
#include <pez.h>
#include <glxew.h>
#include <glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

typedef struct PlatformContextRec
{
    Display* MainDisplay;
    Window MainWindow;
} PlatformContext;

unsigned int GetMilliseconds()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char** argv)
{
    int attrib[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
#if PEZ_ENABLE_MULTISAMPLING
        GLX_SAMPLE_BUFFERS, 1,
        GLX_SAMPLES, 4,
#endif
        None
    };
    
    PlatformContext context;

    context.MainDisplay = XOpenDisplay(NULL);
    int screen = DefaultScreen(context.MainDisplay);
    Window root = RootWindow(context.MainDisplay, screen);

    int fbcount;
    PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddress((GLubyte*)"glXChooseFBConfig");
    GLXFBConfig *fbc = glXChooseFBConfig(context.MainDisplay, screen, attrib, &fbcount);
    if (!fbc)
        PezFatalError("Failed to retrieve a framebuffer config\n");;

    PFNGLXGETVISUALFROMFBCONFIGPROC glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)glXGetProcAddress((GLubyte*)"glXGetVisualFromFBConfig");
    XVisualInfo *visinfo = glXGetVisualFromFBConfig(context.MainDisplay, fbc[0]);
    if (!visinfo)
        PezFatalError("Error: couldn't create OpenGL window with this pixel format.\n");

    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(context.MainDisplay, root, visinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask |
                      PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

    context.MainWindow = XCreateWindow(
        context.MainDisplay,
        root,
        0, 0,
        PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT, 0,
        visinfo->depth,
        InputOutput,
        visinfo->visual,
        CWBackPixel | CWBorderPixel | CWColormap | CWEventMask,
        &attr
    );
    XMapWindow(context.MainDisplay, context.MainWindow);

    GLXContext glcontext;
    if (PEZ_FORWARD_COMPATIBLE_GL) {
        GLXContext tempContext = glXCreateContext(context.MainDisplay, visinfo, NULL, True);
        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");
        if (!glXCreateContextAttribs) {
            PezFatalError("Your platform does not support OpenGL 3.0.\n"
                          "Try changing PEZ_FORWARD_COMPATIBLE_GL to 0.\n");
        }
        int fbcount = 0;
        GLXFBConfig *framebufferConfig = glXChooseFBConfig(context.MainDisplay, screen, 0, &fbcount);
        if (!framebufferConfig) {
            PezFatalError("Can't create a framebuffer for OpenGL 3.0.\n");
        } else {
            int attribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                0
            }; 
            glcontext = glXCreateContextAttribs(context.MainDisplay, framebufferConfig[0], NULL, True, attribs);
            glXMakeCurrent(context.MainDisplay, 0, 0);
            glXDestroyContext(context.MainDisplay, tempContext);
        } 
    } else {
        glcontext = glXCreateContext(context.MainDisplay, visinfo, NULL, True);
    }

    glXMakeCurrent(context.MainDisplay, context.MainWindow, glcontext);
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
        PezFatalError("GLEW Error: %s\n", glewGetErrorString(err));
        
    // Reset OpenGL error state:
    glGetError();

    PezDebugString("OpenGL Version: %s\n", glGetString(GL_VERSION));
    
    const char* windowTitle = PezInitialize(PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);
    XStoreName(context.MainDisplay, context.MainWindow, windowTitle);
    
    // -------------------
    // Start the Game Loop
    // -------------------

    unsigned int previousTime = GetMilliseconds();
    int done = 0;
    while (!done) {
        
        if (glGetError() != GL_NO_ERROR)
            PezFatalError("OpenGL error.\n");

        if (XPending(context.MainDisplay)) {
            XEvent event;
    
            XNextEvent(context.MainDisplay, &event);
            switch (event.type)
            {
                case Expose:
                    //redraw(display, event.xany.window);
                    break;
                
                case ConfigureNotify:
                    //resize(event.xconfigure.width, event.xconfigure.height);
                    break;
                
                case ButtonPress:
                    PezHandleMouse(event.xbutton.x, event.xbutton.y, PEZ_DOWN);
                    break;

                case ButtonRelease:
                    PezHandleMouse(event.xbutton.x, event.xbutton.y, PEZ_UP);
                    break;

                case MotionNotify:
                    PezHandleMouse(event.xmotion.x, event.xmotion.y, PEZ_MOVE);
                    break;

                case KeyRelease:
                case KeyPress: {
                    XComposeStatus composeStatus;
                    char asciiCode[32];
                    KeySym keySym;
                    int len;
                    
                    len = XLookupString(&event.xkey, asciiCode, sizeof(asciiCode), &keySym, &composeStatus);
                    switch (asciiCode[0]) {
                        case 'x': case 'X': case 'q': case 'Q':
                        case 0x1b:
                            done = 1;
                            break;
                    }
                }
            }
        }

        unsigned int currentTime = GetMilliseconds();
        unsigned int deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        
        PezUpdate(deltaTime);

        PezRender();
        glXSwapBuffers(context.MainDisplay, context.MainWindow);
    }

    return 0;
}

void PezDebugStringW(const wchar_t* pStr, ...)
{
    va_list a;
    va_start(a, pStr);

    wchar_t msg[1024] = {0};
    vswprintf(msg, countof(msg), pStr, a);
    fputws(msg, stderr);
}

void PezDebugString(const char* pStr, ...)
{
    va_list a;
    va_start(a, pStr);

    char msg[1024] = {0};
    vsnprintf(msg, countof(msg), pStr, a);
    fputs(msg, stderr);
}

void PezFatalErrorW(const wchar_t* pStr, ...)
{
    fwide(stderr, 1);

    va_list a;
    va_start(a, pStr);

    wchar_t msg[1024] = {0};
    vswprintf(msg, countof(msg), pStr, a);
    fputws(msg, stderr);
    exit(1);
}

void PezFatalError(const char* pStr, ...)
{
    va_list a;
    va_start(a, pStr);

    char msg[1024] = {0};
    vsnprintf(msg, countof(msg), pStr, a);
    fputs(msg, stderr);
    exit(1);
}
