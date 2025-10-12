/****************************************************************************/
/*                       Chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#include "Chai/Inter/Display.hpp"

namespace Chai {
namespace Internal {

void ch_rect::draw(HDC hdc) {
    // TODO: d2d1 conv
    //HBRUSH brush = CreateSolidBrush(color);
    //FillRect(hdc, &rect, brush);
    //DeleteObject(brush);
}

void ch_text::draw(HDC hdc) {
    // TODO: dwrite conv
    //COLORREF oldColor = SetTextColor(hdc, RGB(0, 0, 0)); // Black text
    //TextOutA(hdc, x, y, str.c_str(), (int)str.length());
    //SetTextColor(hdc, oldColor); // Restore old color
}



void ch_window::dbg_out(const std::string& msg) {
    if (!dbgEnabled) return;

    std::string output = "[Chai-ui] " + msg + "\n";
    std::cout << output;                 // Console if attached
    OutputDebugStringA(output.c_str());  // IDE/DebugView if no console
}

HRESULT ch_window::create() {
    HRESULT hr;

    hr = CreateDeviceIndependentResources();
    
    if (SUCCEEDED(hr)) {
        sprintf(className, "Chai-ui-%lu", GetCurrentProcessId());

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

        // From here we can assume hwnd exists so we don't need
        // checking if the window exists before getting the DPI
        // and other weird crap.

        float dpi = GetDpiForWindow(hwnd);

        SetWindowPos(
            hwnd, NULL, NULL, NULL,
            static_cast<int>(ceil(640.f * dpi / 96.f)),
            static_cast<int>(ceil(480.f * dpi / 96.f)),
            SWP_NOMOVE
        );

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    } else {
        throw std::runtime_error("Failed to create device independent resources");
    }

    return hr;
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

HRESULT ch_window::CreateDeviceIndependentResources() {
    return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
}

HRESULT ch_window::CreateDeviceDependentResources() {
    // Stupid scope things
    HRESULT hr = S_OK;
    // We actually need to check if the render target has already been
    // made so it doesn't freak out.
    if (!m_pRenderTarget) {
        // This is just to get the size of the window so we can create
        // the render target.
        // kinda curious what happens if i do another size...
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hwnd,
                size
            ),
            &m_pRenderTarget
        );

        if (SUCCEEDED(hr)) {
            // Now we can create the brush manager
            // And I'm not going to rant about why
            // windows works like it's supposed to
            // but hey, i'm grateful i get to eat
            // food every night under a roof.
            // thanks mum and dad <3
            brush_manager = ch_brush_manager(m_pRenderTarget);
        }
    } 

    return hr;
}

void ch_window::DiscardDeviceResource() {
    SafeRelease(&m_pRenderTarget);
    // Brush manager will just explode itself
    // boom 💥
}

HRESULT ch_window::OnRender() {
    return E_NOTIMPL;
}

void ch_window::OnResize(UINT width, UINT height) {

}



ID2D1SolidColorBrush* ch_window::ch_brush_manager::poof(D2D1_COLOR_F color) {
    uint32_t key = hashclr(color);

    auto it = brush_cache.find(key);
    if (it != brush_cache.end()) return it->second;

    // From this point, we know the brush is not in the cache
    // hence we create a new one
    ID2D1SolidColorBrush* brush = nullptr;
    HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(color, &brush);
    if (FAILED(hr)) throw std::runtime_error("Failed to create SolidColorBrush");

    // Then add the new brush to the cache
    brush_cache[key] = brush;
    return brush;
}

uint32_t ch_window::ch_brush_manager::hashclr(D2D1_COLOR_F c) {
    // D2D1_COLOR_F is just a rgba struct
    const uint32_t r = (uint32_t)(c.r * 255.0f);
    const uint32_t g = (uint32_t)(c.g * 255.0f);
    const uint32_t b = (uint32_t)(c.b * 255.0f);
    const uint32_t a = (uint32_t)(c.a * 255.0f);

    // Return in hash format
    return (r << 24) | (g << 16) | (b << 8) | a;
}
} // Internal
} // Chai