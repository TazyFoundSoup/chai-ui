/****************************************************************************/
/*                       chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#include "chai.h"

void ch_window::enableDbg() {
    dbgEnabled = true;
}

void ch_window::dbgOut(const std::string& msg) {
    if (dbgEnabled) {
        std::cout << "[chai-dbg] " << msg << std::endl; // TODO: add timestamps
    }
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

        hwnd = CreateWindowExA(
            0, className, title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, wc.hInstance, NULL
        );

        if (!hwnd) throw std::runtime_error("Failed to create window");

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
}

void ch_window::destroy() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

void ch_window::run() {
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK ch_window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}