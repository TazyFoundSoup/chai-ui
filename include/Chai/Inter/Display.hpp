/****************************************************************************/
/*                       chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#ifdef _WIN32

#ifndef CHAI_INTERNAL_DISPLAY_H_
#define CHAI_INTERNAL_DISPLAY_H_

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>
#include <math.h>
#include <unordered_map>

#pragma once

// I know I'm copying from the windows tutorial
template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

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
    ch_window(const std::string& t, int w, int h, bool d)
        : title(t), width(w), height(h), dbgEnabled(d),
        m_pDirect2dFactory(nullptr),
        m_pRenderTarget(nullptr),
        brush_manager(m_pRenderTarget)
    {
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

    HRESULT create();
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

    // Inner classes have to go ahead because C++ is stupid and is top to bottom
    class ch_brush_manager {
    // Here's a quick explanation
    // In windows you need to make a brush for every colour.
    // And since chai-ui is a header, it needs to have lots of colours.
    // So there's a cache for brushes.
    public:
        ch_brush_manager(ID2D1HwndRenderTarget* rt) : m_pRenderTarget(rt){}
        ~ch_brush_manager() {
            // TODO: Release all brushes in the cache
        }
        // This is just get it from the cache if it exists
        // or create it if it doesn't
        ID2D1SolidColorBrush* poof(D2D1_COLOR_F color); // idk what else to call it

    private:
        // Helper tool for hashing color
        uint32_t hashclr(D2D1_COLOR_F c);

        // uint32_t -> colour (use the hashclr function for hashing a D2D color into uint32_t)
        // ID2D1SolidColorBrush* -> brush
        std::unordered_map<uint32_t, ID2D1SolidColorBrush*> brush_cache;

        ID2D1HwndRenderTarget* m_pRenderTarget;
    };

    bool dbgEnabled = false;

    char className[64];
    std::string title;
    int width, height;

    std::vector<std::unique_ptr<ch_drawable>> drawList;

    WNDCLASSA wc;
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // D2D1 requirements
    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ch_brush_manager brush_manager;

    // Resource related functions
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceDependentResources();

    void DiscardDeviceResource();


    HRESULT OnRender();

    void OnResize(UINT width, UINT height);
};

} // internal
} // chai

#endif // CHAI_INTERNAL_DISPLAY_H_

#else // _WIN32

#error "Chai-ui is only compatible with Windows machines"

#endif // _WIN32