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

private:
    bool dbgEnabled = false;

    char className[64];
    std::string title;
    int width, height;

    WNDCLASSA wc;
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};