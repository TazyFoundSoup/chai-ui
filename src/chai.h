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
        none,
        closed,
        key_down,
        key_up,
        mouse_move,
        mouse_down,
        mouse_up,
        mouse_wheel,
        resized,
    } type = type::none;

    union {
        struct { int x, y; } mouse_move;
        struct { int button; } mouse_button;
        struct { int delta; } mouse_wheel;
        struct { int keycode; bool repeat; } key;
        struct { int width, height; } size;
    };
};


class ch_window {
public:
    ch_window(const std::string& t, int w, int h, bool d) : title(t), width(w), height(h), dbgEnabled(d) {
        create();
        if (dbgEnabled) {
            dbg_out("Debugging enabled for window");
        }
    }
    ~ch_window() {
        UnregisterClassA(className, wc.hInstance);
    }

    // Functions for debugging
    void dbg_out(const std::string& msg);

    void create();
    void destroy();
    void run();
    bool poll_event(ch_event& event);

    // Setters for window properties
    void set_icon(std::string iconPath);

private:
    bool dbgEnabled = false;

    char className[64];
    std::string title;
    int width, height;

    WNDCLASSA wc;
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};