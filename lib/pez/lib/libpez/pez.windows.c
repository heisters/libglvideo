// Pez was developed by Philip Rideout and released under the MIT License.

#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <pez.h>
#include <glew.h>
#include <wglew.h>

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE ignoreMe0, LPSTR ignoreMe1, INT ignoreMe2)
{
    LPCSTR szName = "Pez App";
    WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(0), 0, 0, 0, 0, szName, 0 };
    DWORD dwStyle = WS_SYSMENU | WS_VISIBLE | WS_POPUP;
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    RECT rect;
    int windowWidth, windowHeight, windowLeft, windowTop;
    HWND hWnd;
    PIXELFORMATDESCRIPTOR pfd;
    HDC hDC;
    HGLRC hRC;
    int pixelFormat;
    GLenum err;
    DWORD previousTime = GetTickCount();
    MSG msg = {0};

    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClassExA(&wc);

    SetRect(&rect, 0, 0, PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    windowLeft = GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2;
    windowTop = GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2;
    hWnd = CreateWindowExA(0, szName, szName, dwStyle, windowLeft, windowTop, windowWidth, windowHeight, 0, 0, 0, 0);

    // Create the GL context.
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    hDC = GetDC(hWnd);
    pixelFormat = ChoosePixelFormat(hDC, &pfd);

    SetPixelFormat(hDC, pixelFormat, &pfd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    if (PEZ_ENABLE_MULTISAMPLING)
    {
        int pixelAttribs[] =
        {
            WGL_SAMPLES_ARB, 16,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_RED_BITS_ARB, 8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB, 8,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            0
        };
        int* sampleCount = pixelAttribs + 1;
        int* useSampleBuffer = pixelAttribs + 3;
        int pixelFormat = -1;
        PROC proc = wglGetProcAddress("wglChoosePixelFormatARB");
        unsigned int numFormats;
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) proc;

        if (!wglChoosePixelFormatARB)
        {
            PezFatalError("Could not load function pointer for 'wglChoosePixelFormatARB'.  Is your driver properly installed?");
        }


        // Try fewer and fewer samples per pixel till we find one that is supported:
        while (pixelFormat <= 0 && *sampleCount >= 0)
        {
            wglChoosePixelFormatARB(hDC, pixelAttribs, 0, 1, &pixelFormat, &numFormats);
            (*sampleCount)--;
            if (*sampleCount <= 1)
            {
                *useSampleBuffer = GL_FALSE;
            }
        }

        // Win32 allows the pixel format to be set only once per app, so destroy and re-create the app:
        DestroyWindow(hWnd);
        hWnd = CreateWindowExA(0, szName, szName, dwStyle, windowLeft, windowTop, windowWidth, windowHeight, 0, 0, 0, 0);
        SetWindowPos(hWnd, HWND_TOP, windowLeft, windowTop, windowWidth, windowHeight, 0);
        hDC = GetDC(hWnd);
        SetPixelFormat(hDC, pixelFormat, &pfd);
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);
    }

#define PEZ_TRANSPARENT_WINDOW 0

    // For transparency, this doesn't seem to work.  I think I'd need to read back from an OpenGL FBO and blit it with GDI.
    if (PEZ_TRANSPARENT_WINDOW)
    {
        long flag = GetWindowLong(hWnd, GWL_EXSTYLE);
        int opacity = 128;
        flag |= WS_EX_LAYERED;
        SetWindowLong(hWnd, GWL_EXSTYLE, flag);
        SetLayeredWindowAttributes(hWnd, 0, opacity, LWA_ALPHA);		
    }

    err = glewInit();
    if (GLEW_OK != err)
    {
        PezFatalError("GLEW Error: %s\n", glewGetErrorString(err));
    }
    PezDebugString("OpenGL Version: %s\n", glGetString(GL_VERSION));

    if (!PEZ_VERTICAL_SYNC)
    {
        wglSwapIntervalEXT(0);
    }

    if (PEZ_FORWARD_COMPATIBLE_GL && glewIsSupported("GL_VERSION_3_0"))
    {
        const int contextAttribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        HGLRC newRC = wglCreateContextAttribsARB(hDC, 0, contextAttribs);
        wglMakeCurrent(0, 0);
        wglDeleteContext(hRC);
        hRC = newRC;
        wglMakeCurrent(hDC, hRC);
    }

    {
        const char* szWindowTitle = PezInitialize(PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);
        SetWindowTextA(hWnd, szWindowTitle);
    }

    // -------------------
    // Start the Game Loop
    // -------------------
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = GetTickCount();
            DWORD deltaTime = currentTime - previousTime;
            previousTime = currentTime;
            PezUpdate(deltaTime);
            PezRender();
            SwapBuffers(hDC);
            if (glGetError() != GL_NO_ERROR)
            {
                PezFatalError("OpenGL error.\n");
            }
        }
    }

    UnregisterClassA(szName, wc.hInstance);

    return 0;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    switch (msg)
    {
        case WM_LBUTTONUP:
            PezHandleMouse(x, y, PEZ_UP);
            break;

        case WM_LBUTTONDOWN:
            PezHandleMouse(x, y, PEZ_DOWN);
            break;

        case WM_MOUSEMOVE:
            PezHandleMouse(x, y, PEZ_MOVE);
            break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                case VK_OEM_2: // Question Mark / Forward Slash for US Keyboards
                    break;
            }
            break;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void PezDebugStringW(const wchar_t* pStr, ...)
{
    wchar_t msg[1024] = {0};

    va_list a;
    va_start(a, pStr);

    _vsnwprintf_s(msg, _countof(msg), _TRUNCATE, pStr, a);
    OutputDebugStringW(msg);
}

void PezDebugString(const char* pStr, ...)
{
    char msg[1024] = {0};

    va_list a;
    va_start(a, pStr);

    _vsnprintf_s(msg, _countof(msg), _TRUNCATE, pStr, a);
    OutputDebugStringA(msg);
}

void PezFatalErrorW(const wchar_t* pStr, ...)
{
    wchar_t msg[1024] = {0};

    va_list a;
    va_start(a, pStr);

    _vsnwprintf_s(msg, _countof(msg), _TRUNCATE, pStr, a);
    OutputDebugStringW(msg);
#ifdef _DEBUG
    __debugbreak();
#endif
    exit(1);
}

void PezFatalError(const char* pStr, ...)
{
    char msg[1024] = {0};

    va_list a;
    va_start(a, pStr);

    _vsnprintf_s(msg, _countof(msg), _TRUNCATE, pStr, a);
    OutputDebugStringA(msg);
#ifdef _DEBUG
    __debugbreak();
#endif
    exit(1);
}
