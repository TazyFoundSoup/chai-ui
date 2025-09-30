/****************************************************************************/
/*                       chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#ifndef CHAI_INTERNAL_DISPLAY_H_
#define CHAI_INTERNAL_DISPLAY_H_

#include <windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

#pragma once

namespace chai {
namespace internal {

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

struct ch_drawable {
    virtual void draw(HDC hdc) = 0;
    virtual ~ch_drawable() {}
};

struct ch_rect : public ch_drawable {
    RECT rect;
    COLORREF color;

    ch_rect(int x1, int y1, int x2, int y2, COLORREF c) {
    rect = {x1, y1, x2, y2};
    color = c;
    }

    void draw(HDC hdc) override;
};

struct ch_text : public ch_drawable {
    std::string str;
    int x, y;
    
    // Most unreable code award goes to the line below this comment
    ch_text(std::string c, int x, int y) : str(c), x(x), y(y) {};
    void draw(HDC hdc) override;
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
    void draw(std::unique_ptr<ch_drawable> drawable);
    bool poll_event(ch_event& event);

    // Setters for window properties
    void set_icon(std::string iconPath);

private:
    // Note for contributers
    // Private is going to be in camelCase but public is going to be in snake_case
    // This is just for debugging, but try to keep in case
    bool dbgEnabled = false;

    char className[64];
    std::string title;
    int width, height;

    std::vector<std::unique_ptr<ch_drawable>> drawList;

    WNDCLASSA wc;
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

} // internal
} // chai

#endif // CHAI_INTERNAL_DISPLAY_H_

