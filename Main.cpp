//
// Main.cpp
//

#include "pch.h"
#include "Game.h"
#include <fcntl.h>

using namespace DirectX;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void BindStdHandlesToConsole();
// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;
	BindStdHandlesToConsole();
    auto game = std::make_unique<Game>();

    // Register class and create window
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"Procedural_RPGWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassEx(&wcex))
            return 1;

        // Create window
        int w, h;
        game->GetDefaultSize(w, h);

        RECT rc;
        rc.top = 0;
        rc.left = 0;
        rc.right = static_cast<LONG>(w); 
        rc.bottom = static_cast<LONG>(h);

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        HWND hwnd = CreateWindowEx(0, L"Procedural_RPGWindowClass", L"Procedural-RPG", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
        // TODO: Change to CreateWindowEx(WS_EX_TOPMOST, L"Procedural_RPGWindowClass", L"Procedural-RPG", WS_POPUP,
        // to default to fullscreen.

        if (!hwnd)
            return 1;

        ShowWindow(hwnd, nCmdShow);
        // TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(game.get()) );

        GetClientRect(hwnd, &rc);

        game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }
    // Main message loop
    MSG msg = { 0 };
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (WM_QUIT == msg.message) break;
		game->Tick();
	}
    game.reset();

    CoUninitialize();

    return (int) msg.wParam;
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
    // TODO: Set s_fullscreen to true if defaulting to fullscreen.

    auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && game)
                    game->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && game)
                game->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && game)
        {
            game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (game)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        if (game)
        {
            if (wParam)
            {
                game->OnActivated();
            }
            else
            {
                game->OnDeactivated();
            }
        }
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && game)
                game->OnSuspending();
            s_in_suspend = true;
            return true;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && game)
                    game->OnResuming();
                s_in_suspend = false;
            }
            return true;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(message, wParam, lParam);
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                if (game)
                    game->GetDefaultSize(width, height);

                ShowWindow(hWnd, SW_SHOWNORMAL);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            s_fullscreen = !s_fullscreen;
        }
        break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;
void BindStdHandlesToConsole()
{
	//AllocConsole();
	////Redirect unbuffered STDIN to the console
	//HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	//if (stdInHandle != INVALID_HANDLE_VALUE)
	//{
	//	int fileDescriptor = _open_osfhandle((intptr_t)stdInHandle, _O_TEXT);
	//	if (fileDescriptor != -1)
	//	{
	//		FILE* file = _fdopen(fileDescriptor, "r");
	//		if (file != NULL)
	//		{
	//			*stdin = *file;
	//			setvbuf(stdin, NULL, _IONBF, 0);
	//		}
	//	}
	//}

	////Redirect unbuffered STDOUT to the console
	//HANDLE stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	//if (stdOutHandle != INVALID_HANDLE_VALUE)
	//{
	//	int fileDescriptor = _open_osfhandle((intptr_t)stdOutHandle, _O_TEXT);
	//	if (fileDescriptor != -1)
	//	{
	//		FILE* file = _fdopen(fileDescriptor, "w");
	//		if (file != NULL)
	//		{
	//			*stdout = *file;
	//			setvbuf(stdout, NULL, _IONBF, 0);
	//		}
	//	}
	//}

	////Redirect unbuffered STDERR to the console
	//HANDLE stdErrHandle = GetStdHandle(STD_ERROR_HANDLE);
	//if (stdErrHandle != INVALID_HANDLE_VALUE)
	//{
	//	int fileDescriptor = _open_osfhandle((intptr_t)stdErrHandle, _O_TEXT);
	//	if (fileDescriptor != -1)
	//	{
	//		FILE* file = _fdopen(fileDescriptor, "w");
	//		if (file != NULL)
	//		{
	//			*stderr = *file;
	//			setvbuf(stderr, NULL, _IONBF, 0);
	//		}
	//	}
	//}

	////Clear the error state for each of the C++ standard stream objects. We need to do this, as
	////attempts to access the standard streams before they refer to a valid target will cause the
	////iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	////to always occur during startup regardless of whether anything has been read from or written to
	////the console or not.
	//std::wcout.clear();
	//std::cout.clear();
	//std::wcerr.clear();
	//std::cerr.clear();
	//std::wcin.clear();
	//std::cin.clear();
	//printf("%5d", "YOW");
}
