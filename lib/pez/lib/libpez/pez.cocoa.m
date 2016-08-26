#import <pez.h>
#import <glew.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <mach/mach_time.h>

@class View;

@interface View : NSOpenGLView <NSWindowDelegate> {
    NSRect m_frameRect;
    BOOL m_didInit;
    uint64_t m_previousTime;
    NSTimer* m_timer;
}

- (void) animate;

@end

@implementation View

-(void)windowWillClose:(NSNotification *)note {
    [[NSApplication sharedApplication] terminate:self];
}

- (void) timerFired:(NSTimer*) timer
{
    [self animate];     
}

- (id) initWithFrame: (NSRect) frame
{
    m_didInit = FALSE;
    
    int attribs[] = {
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFANoRecovery,
#if PEZ_ENABLE_MULTISAMPLING
        kCGLPFASampleBuffers, 1,
        kCGLPFASamples, 4,
#endif
        0
    };

    NSOpenGLPixelFormat *fmt = [[NSOpenGLPixelFormat alloc]
                            initWithAttributes:(NSOpenGLPixelFormatAttribute*) attribs];

    self = [self initWithFrame:frame pixelFormat:fmt];

    [fmt release];

    m_frameRect = frame;
    m_previousTime = mach_absolute_time();

    m_timer = [[NSTimer
                       scheduledTimerWithTimeInterval:1.0f/120.0f
                       target:self 
                       selector:@selector(timerFired:)
                       userInfo:nil
                       repeats:YES] retain];

    return self;
}

- (void) drawRect:(NSRect) theRect
{
    if (!m_didInit) {
            
        int TransparentWindow = 0;
        if (TransparentWindow) {
            int opaque = NO;
            [[self openGLContext]
                setValues:&opaque
                forParameter:NSOpenGLCPSurfaceOpacity];
    
            [[self window] setOpaque:NO];
            [[self window] setAlphaValue:0.99];
        }
        
        glewInit();
        const char* szTitle = PezInitialize(PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);
        m_didInit = YES;
        
        [[self window] setLevel: NSFloatingWindowLevel];
        [[self window] makeKeyAndOrderFront: self];
        [[self window] setTitle: [NSString stringWithUTF8String: szTitle]];
    }

    PezRender();
    [[self openGLContext] flushBuffer]; 
}

- (void) mouseDragged: (NSEvent*) theEvent
{
    NSPoint curPoint = [theEvent locationInWindow];
    PezHandleMouse(curPoint.x, m_frameRect.size.height - curPoint.y, PEZ_MOVE);
}

- (void) mouseUp: (NSEvent*) theEvent
{
    NSPoint curPoint = [theEvent locationInWindow];
    PezHandleMouse(curPoint.x, m_frameRect.size.height - curPoint.y, PEZ_UP);
}

- (void) mouseDown: (NSEvent*) theEvent
{
    NSPoint curPoint = [theEvent locationInWindow];
    PezHandleMouse(curPoint.x, m_frameRect.size.height - curPoint.y, PEZ_DOWN);
}

- (void) animate
{
    uint64_t currentTime = mach_absolute_time();
    uint64_t elapsedTime = currentTime - m_previousTime;
    m_previousTime = currentTime;
    
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    
    elapsedTime *= info.numer;
    elapsedTime /= info.denom;
    
    float timeStep = elapsedTime / 1000000.0f;

    PezUpdate(timeStep);
    
    [self display];
}

- (void) onKey: (unichar) character downEvent: (BOOL) flag
{
    switch(character) {
        case 27:
        case 'q':
            [[NSApplication sharedApplication] terminate:self];
            break;
    }
}

- (void) keyDown:(NSEvent *)theEvent
{
    NSString *characters;
    unsigned int characterIndex, characterCount;
    
    characters = [theEvent charactersIgnoringModifiers];
    characterCount = [characters length];

    for (characterIndex = 0; characterIndex < characterCount; characterIndex++)
        [self onKey:[characters characterAtIndex:characterIndex] downEvent:YES];
}

@end

int main(int argc, const char *argv[])
{
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    NSApplication *NSApp = [NSApplication sharedApplication];
    NSRect frame = NSMakeRect( 100., 100., 300., 300. );
    
    NSRect screenBounds = [[NSScreen mainScreen] frame];
    NSRect viewBounds = NSMakeRect(0, 0, PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);
    
    View* view = [[View alloc] initWithFrame:viewBounds];
    
    NSRect centered = NSMakeRect(NSMidX(screenBounds) - NSMidX(viewBounds),
                                 NSMidY(screenBounds) - NSMidY(viewBounds),
                                 viewBounds.size.width, viewBounds.size.height);
    
    NSWindow *window = [[NSWindow alloc]
        initWithContentRect:centered
        styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask
        backing:NSBackingStoreBuffered
        defer:NO];

    [window setContentView:view];
    [window setDelegate:view];
    [view release];
    
    [NSApp run];
    
    [pool release];
    return EXIT_SUCCESS;
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