/*
    STARLIGHT live debug
    ---------------------
    This application creates a resizable window that displays a live capture of a specified screen area.
    The captured area is defined as a 150x150 pixel region centered on the screen.
    The live image is updated frequently using a timer and scaled uniformly when the window is resized.
    The window has a minimum size enforced via WM_GETMINMAXINFO.
*/

#include <windows.h>
#include <tchar.h>

// Global variables for the screen capture region and bitmap
static HBITMAP g_hBitmap = NULL;
static int g_captureX = 0;
static int g_captureY = 0;
static int g_captureWidth = 150;
static int g_captureHeight = 150;

const TCHAR* WINDOW_CLASS_NAME = _T("Debug");
const UINT_PTR IDT_TIMER = 1;

/*
    WindowProc:
    ------------
    The main window procedure that handles messages.
    - WM_GETMINMAXINFO: Sets the minimum window size to prevent the window from being resized too small.
    - WM_PAINT: Renders the captured bitmap scaled uniformly to fit the current client area.
    - WM_TIMER: Updates the screen capture at regular intervals.
    - WM_DESTROY: Performs cleanup before the application exits.
*/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*
    CaptureScreenArea:
    ------------------
    Captures a rectangular area of the screen specified by (x, y, width, height)
    and stores the resulting bitmap in the global variable g_hBitmap.
*/
void CaptureScreenArea(int x, int y, int width, int height);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Obtain the screen dimensions
    int ScreenW = GetSystemMetrics(SM_CXSCREEN);
    int ScreenH = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the center of the screen
    int centerX = ScreenW / 2;
    int centerY = ScreenH / 2;

    // Define the capture region (150x150) centered on the screen
    g_captureWidth = 150;
    g_captureHeight = 150;
    g_captureX = centerX - (g_captureWidth / 2);
    g_captureY = centerY - (g_captureHeight / 2);

    // Register the window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("Error registering the window class"), _T("Error"), MB_ICONERROR);
        return 0;
    }

    // Create the window with an initial size based on the capture region plus margins
    int winWidth = g_captureWidth + 20;
    int winHeight = g_captureHeight + 40;

    HWND hwnd = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME,
        _T("STARLIGHT live debug"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winWidth, winHeight,
        NULL, NULL, hInstance, NULL);

    if (!hwnd)
    {
        MessageBox(NULL, _T("Could not create window"), _T("Error"), MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Set a timer to update the capture every 4 milliseconds (adjust as needed)
    SetTimer(hwnd, IDT_TIMER, 4, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_GETMINMAXINFO:
    {
        // Enforce a minimum window size based on the capture dimensions plus margins
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = g_captureWidth + 20;
        mmi->ptMinTrackSize.y = g_captureHeight + 40;
    }
    return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (g_hBitmap)
        {
            // Get current client area dimensions
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;

            // Calculate uniform scaling factor to maintain aspect ratio
            double scaleX = (double)clientWidth / g_captureWidth;
            double scaleY = (double)clientHeight / g_captureHeight;
            double scale = (scaleX < scaleY) ? scaleX : scaleY;

            // Calculate new dimensions and centering offsets
            int newWidth = (int)(g_captureWidth * scale);
            int newHeight = (int)(g_captureHeight * scale);
            int offsetX = (clientWidth - newWidth) / 2;
            int offsetY = (clientHeight - newHeight) / 2;

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, g_hBitmap);

            // Use StretchBlt to draw the captured image scaled to the new dimensions
            StretchBlt(hdc, offsetX, offsetY, newWidth, newHeight,
                memDC, 0, 0, g_captureWidth, g_captureHeight, SRCCOPY);

            SelectObject(memDC, oldBmp);
            DeleteDC(memDC);
        }

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_TIMER:
    {
        if (wParam == IDT_TIMER)
        {
            // Update the captured area and request a repaint
            CaptureScreenArea(g_captureX, g_captureY, g_captureWidth, g_captureHeight);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    return 0;

    case WM_DESTROY:
        // Clean up the bitmap and quit the application
        if (g_hBitmap)
        {
            DeleteObject(g_hBitmap);
            g_hBitmap = NULL;
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void CaptureScreenArea(int x, int y, int width, int height)
{
    // Delete any previous bitmap
    if (g_hBitmap)
    {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }

    // Capture the specified area of the screen
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    g_hBitmap = CreateCompatibleBitmap(screenDC, width, height);

    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, g_hBitmap);

    BitBlt(memDC, 0, 0, width, height, screenDC, x, y, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}
