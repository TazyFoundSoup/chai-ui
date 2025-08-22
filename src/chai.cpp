/****************************************************************************/
/*                       chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#include "chai.h"


void ch_rect::draw(HDC hdc) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
}

void ch_text::draw(HDC hdc) {
    COLORREF oldColor = SetTextColor(hdc, RGB(0, 0, 0)); // Black text
    TextOutA(hdc, x, y, str.c_str(), (int)str.length());
    SetTextColor(hdc, oldColor); // Restore old color
}



void ch_window::dbg_out(const std::string& msg) {
    if (!dbgEnabled) return;

    std::string output = "[chai-ui] " + msg + "\n";
    std::cout << output;                 // Console if attached
    OutputDebugStringA(output.c_str());  // IDE/DebugView if no console
}

void ch_window::create() {
    sprintf(className, "chai-ui-%lu", GetCurrentProcessId());

        wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = className;

        if (!GetClassInfoA(wc.hInstance, className, &wc)) {
            if (!RegisterClassA(&wc)) {
                throw std::runtime_error("Failed to register window class");
            }
        }
        dbg_out("Window class registered: " + std::string(className));

        hwnd = CreateWindowExA(
            0, className, title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, wc.hInstance, NULL
        );

        dbg_out("Window created with properties: " + title + 
               " (" + std::to_string(width) + "x" + std::to_string(height) + ")");

        if (!hwnd) throw std::runtime_error("Failed to create window");

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
}

void ch_window::destroy() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
        dbg_out("Window destroyed: " + title);
    }
}

void ch_window::run() {
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    dbg_out("Message loop exited for window: " + title);
}

void ch_window::draw(std::unique_ptr<ch_drawable> drawable) {
    drawList.push_back(std::move(drawable));
}

bool ch_window::poll_event(ch_event& event) {
    MSG msg;
    event = {}; // Zero out event in case of weird bugs
    if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
            case WM_CLOSE:
                event.type = ch_event::type::closed;
                break;
            case WM_KEYDOWN:
                event.type = ch_event::type::key_down;
                event.key.keycode = static_cast<int>(msg.wParam);
                // TODO: Implement repeat logic
                event.key.repeat = false;
                break;
            case WM_KEYUP:
                event.type = ch_event::type::key_up;
                event.key.keycode = static_cast<int>(msg.wParam);
                break;
            case WM_MOUSEMOVE:
                event.type = ch_event::type::mouse_move;
                event.mouse_move.x = (int)(short)LOWORD(msg.lParam); // Extract
                event.mouse_move.y = (int)(short)HIWORD(msg.lParam);
                break;
            case WM_LBUTTONDOWN:
                event.type = ch_event::type::mouse_down;
                event.mouse_button.button = 0;
                break;
            case WM_LBUTTONUP:
                event.type = ch_event::type::mouse_up;
                event.mouse_button.button = 0;
                break;
            case WM_RBUTTONDOWN:
                event.type = ch_event::type::mouse_down;
                event.mouse_button.button = 1;
                break;
            case WM_RBUTTONUP:
                event.type = ch_event::type::mouse_up;
                event.mouse_button.button = 1;
                break;
        }
        return true;

    } else {
        event.type = ch_event::type::none;
        return false;
    }
}

void ch_window::set_icon(std::string iconPath) {
    std::wstring iconPathW = std::wstring(iconPath.begin(), iconPath.end());
    
    HICON hIcon = (HICON)LoadImageW(
        NULL, 
        iconPathW.c_str(), 
        IMAGE_ICON, 
        0, 
        0, 
        LR_LOADFROMFILE
    );
    
    if (!hIcon) {
        throw std::runtime_error("Failed to load icon from path: " + iconPath);
    }

    SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    dbg_out("Icon set for window: " + title + " from path: " + iconPath);
}

LRESULT CALLBACK ch_window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // TODO: Iterate over drawList to draw objects

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}