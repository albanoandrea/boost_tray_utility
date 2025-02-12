#include <windows.h>
#include <shellapi.h>

// Tray icon active options
// TODO: Move in some build option 
#define ENABLE_PREVENT_LOCK     1

#if ENABLE_PREVENT_LOCK
#include "prevent_lock.h"
#endif

#define CLOSE_TRAY_ID       9999

#define WM_TRAY_ICON (WM_USER + 1)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

NOTIFYICONDATA nid;
HMENU hPopupMenu;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TrayIconApp";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(0, L"TrayIconApp", L"Tray Icon App", 0, 0, 0, 0, 0, HWND_DESKTOP, NULL, hInstance, NULL);

    // Initialize tray icon data
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1; // Unique ID for the tray icon
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(L"logo.ico")); // Load your icon.  IDI_APPLICATION is a placeholder. *Important*
    nid.uCallbackMessage = WM_TRAY_ICON;
    wcscpy_s(nid.szTip, L"My Tray Icon App");  // Tooltip text

    Shell_NotifyIcon(NIM_ADD, &nid);  // Add the tray icon

    // Create the popup menu
    hPopupMenu = CreatePopupMenu();
#if ENABLE_PREVENT_LOCK
    AppendMenu(hPopupMenu, MF_STRING, PREVENT_LOCK_ID, PREVENT_LOCK_START_STRING); // Example menu item
#endif

    AppendMenu(hPopupMenu, MF_STRING, CLOSE_TRAY_ID, L"Exit");     // Exit menu item

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove the tray icon when the application exits
    Shell_NotifyIcon(NIM_DELETE, &nid);

    return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_TRAY_ICON:
        if (lParam == WM_RBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd); // Important for menu to appear correctly
            TrackPopupMenu(hPopupMenu, TPM_RIGHTALIGN, pt.x, pt.y, 0, hWnd, NULL);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == CLOSE_TRAY_ID)
        {
            // Handle Exit click
            DestroyWindow(hWnd); // This will trigger WM_DESTROY
        }
#if ENABLE_PREVENT_LOCK
        else  if (LOWORD(wParam) == PREVENT_LOCK_ID)
        {
            if (prevent_lock_is_running())
            {
                prevent_lock_stop();
                ModifyMenu(hPopupMenu, 1001, MF_BYCOMMAND | MF_STRING, PREVENT_LOCK_ID, PREVENT_LOCK_START_STRING);
            }
            else
            {
                prevent_lock_start();
                ModifyMenu(hPopupMenu, 1001, MF_BYCOMMAND | MF_STRING, PREVENT_LOCK_ID, PREVENT_LOCK_STOP_STRING);
            }
            DrawMenuBar(hWnd);
        }
#endif
        break;
    case WM_DESTROY:
#if ENABLE_PREVENT_LOCK
        prevent_lock_stop();
#endif
        PostQuitMessage(0); // Important to exit the message loop
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}