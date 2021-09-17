// 4pr2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "4pr2.h"
#include <stdio.h>
#include <exception>
#include <string>
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

RECT rcClient = { 0, 0, 1100, 800 };

struct PR2FRAME
{
    HWND hwnd;
    PROCESS_INFORMATION pi;
};
PR2FRAME frames[4] = { 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CreatePR2Frame(HWND hwnd_parent, PR2FRAME* frame);
HWND FindTopWindow(DWORD pid);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    ZeroMemory(&frames, sizeof(PR2FRAME) * 4);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY4PR2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY4PR2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY4PR2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_MY4PR2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   RECT adjusted = rcClient;
   AdjustWindowRect(&adjusted, WS_OVERLAPPEDWINDOW, false);
   MoveWindow(hWnd, 50, 50, adjusted.right - adjusted.left, adjusted.bottom - adjusted.top, true);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   CreatePR2Frame(hWnd, &frames[0]);
   if (!frames[0].hwnd) throw std::exception("failed to create pr2 child");
   MoveWindow(frames[0].hwnd, 0, 0, 550, 400, true);

   CreatePR2Frame(hWnd, &frames[1]);
   if (!frames[1].hwnd) throw std::exception("failed to create pr2 child");
   MoveWindow(frames[1].hwnd, 550, 0, 550, 400, true);

   CreatePR2Frame(hWnd, &frames[2]);
   if (!frames[2].hwnd) throw std::exception("failed to create pr2 child");
   MoveWindow(frames[2].hwnd, 0, 400, 550, 400, true);

   CreatePR2Frame(hWnd, &frames[3]);
   if (!frames[3].hwnd) throw std::exception("failed to create pr2 child");
   MoveWindow(frames[3].hwnd, 550, 400, 550, 400, true);

   return TRUE;
}

BOOL CreatePR2Frame(HWND hwnd_parent, PR2FRAME* frame)
{
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = { 0 };
    std::wstring cmd = L"Platform Racing 2.exe";
    // CreateProcessW can modify lpCommandLine so we need a non-const copy
    std::wstring cmd_cpy(cmd.length(), L' ');
    std::copy(cmd.begin(), cmd.end(), cmd_cpy.begin());
    if (!CreateProcess(NULL, const_cast<wchar_t*>(cmd_cpy.c_str()),
        NULL, NULL, true, 0, NULL, NULL, &si, &pi))
    {
        MessageBox(NULL, L"Couldn't find 'Platform Racing 2.exe' in this directory.",
            L"Error", MB_ICONERROR);
        exit(1);
    }
    
    WaitForInputIdle(pi.hProcess, INFINITE);

    HWND hwnd_pr2 = FindTopWindow(pi.dwProcessId);
    if (!hwnd_pr2)
        return FALSE;
    SetParent(hwnd_pr2, hwnd_parent);
    SetWindowLongPtr(hwnd_pr2, GWL_STYLE, WS_VISIBLE);
    SetWindowPos(hwnd_pr2, HWND_TOPMOST, 0, 0, 550, 400, SWP_SHOWWINDOW);
    SetMenu(hwnd_pr2, NULL);
    frame->hwnd = hwnd_pr2;
    frame->pi = pi;
    return TRUE;
}

HWND FindTopWindow(DWORD pid)
{
    std::pair<HWND, DWORD> params = { 0, pid };

    // Enumerate the windows using a lambda to process each window
    BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
    {
        auto pParams = (std::pair<HWND, DWORD>*)(lParam);

        DWORD processId;
        if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
        {
            // Stop enumerating
            SetLastError(-1);
            pParams->first = hwnd;
            return FALSE;
        }

        // Continue enumerating
        return TRUE;
    }, (LPARAM)&params);

    if (!bResult && GetLastError() == -1 && params.first)
    {
        return params.first;
    }

    return 0;
}

BOOL ResizeWindow(HWND hwnd, int width, int height)
{
    if (!hwnd)
        return FALSE;

    RECT orig;
    GetClientRect(hwnd, &orig);
    RECT orig_win;
    GetWindowRect(hwnd, &orig_win);
    RECT desired = { orig.left, orig.top, width - orig.left, height - orig.top };
    AdjustWindowRect(&desired, WS_OVERLAPPEDWINDOW, false);
    MoveWindow(hwnd, orig_win.left, orig_win.top, desired.right - desired.left, desired.bottom - desired.top, true);
    
    int frame_width = width / 2;
    int frame_height = height / 2;
    MoveWindow(frames[0].hwnd, 0, 0, frame_width, frame_height, true);
    MoveWindow(frames[1].hwnd, frame_width, 0, frame_width, frame_height, true);
    MoveWindow(frames[2].hwnd, 0, frame_height, frame_width, frame_height, true);
    MoveWindow(frames[3].hwnd, frame_width, frame_height, frame_width, frame_height, true);
    
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_F1:
            ResizeWindow(hWnd, 1100, 800);
            break;
        case VK_F2:
            ResizeWindow(hWnd, 704, 512);
            break;
        case VK_F3:
            ResizeWindow(hWnd, 275, 200);
            break;
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        for (int i = 0; i < 4; i++)
        {
            if (frames[i].hwnd != NULL)
            {
                PostMessage(frames[i].hwnd, WM_CLOSE, NULL, NULL);
                DestroyWindow(frames[i].hwnd);
                frames[i].hwnd = NULL;
            }
            if (frames[i].pi.hProcess != NULL)
            {
                if (!TerminateProcess(frames[i].pi.hProcess, 1))
                    continue;
                if (WaitForSingleObject(frames[i].pi.hProcess, INFINITE))
                    continue;
                if (!CloseHandle(frames[i].pi.hProcess))
                    continue;
                frames[i].pi.hProcess = NULL;
                if (!CloseHandle(frames[i].pi.hThread))
                    continue;
                frames[i].pi.hThread = NULL;
            }
            
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}