/****************************************************************************/
/*                       chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#include <windows.h>
#include <iostream>
#include <string>
#include <stdexcept>

#pragma once

struct ch_event {
    enum class type {
        None,
        Closed,
        KeyDown,
        KeyUp,
        MouseMove,
        MouseDown,
        MouseUp,
        MouseWheel,
        Resized,
    } type = type::None;

    union {
        struct { int x, y; } mouseMove;
        struct { int button; } mouseButton;
        struct { int delta; } mouseWheel;
        struct { int keycode; bool repeat; } key;
        struct { int width, height; } size;
    };
};


class ch_window {
public:
    ch_window(const std::string& t, int w, int h) : title(t), width(w), height(h) {
        create();
    }
    ~ch_window() {
        UnregisterClassA(className, wc.hInstance);
    }

    // Debug functions
    void enableDbg();
    void dbgOut(const std::string& msg);

    void create();
    void destroy();
    void run();

    // Setters for window properties
    void setIcon(std::string iconPath);

private:
    bool dbgEnabled = false;

    char className[64];
    std::string title;
    int width, height;

    WNDCLASSA wc;
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};