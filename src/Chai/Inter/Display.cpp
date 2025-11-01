/****************************************************************************/
/*                       Chai-ui - A small c++ ui library                   */
/*                                                                          */
/*  Licensed under the MIT License. See LICENSE file in the project root.   */
/*                                                                          */
/****************************************************************************/

#include "Chai/Inter/Display.hpp"
#include "Display.hpp"

namespace Chai {
namespace Internal {

void ch_rect::draw(ch_window& win) {
    ID2D1SolidColorBrush* brush = win.brush_manager.poof(this->color);
    win.m_pRenderTarget->FillRectangle(this->rect, brush);
}

void ch_rect::setpos(int x, int y) {
    this->rect.left = static_cast<FLOAT>(x);
    this->rect.top = static_cast<FLOAT>(y);
    this->rect.right = static_cast<FLOAT>(x) + (this->rect.right - this->rect.left);
    this->rect.bottom = static_cast<FLOAT>(y) + (this->rect.bottom - this->rect.top);

    if (owner) owner->invalidate();
}

void ch_rect::move(int offx, int offy){
    FLOAT width = rect.right - rect.left;
    FLOAT height = rect.bottom - rect.top;

    rect.left += offx;
    rect.top += offy;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    if (owner) owner->invalidate();
}

void ch_circle::draw(ch_window& win) {
    ID2D1SolidColorBrush* brush = win.brush_manager.poof(this->color);
    win.m_pRenderTarget->FillEllipse(this->circle, brush);
}

void ch_circle::setpos(int x, int y) {
    circle.point.x = static_cast<FLOAT>(x);
    circle.point.y = static_cast<FLOAT>(y);

    if (owner) owner->invalidate();
}

void ch_circle::move(int offx, int offy){
    circle.point.x += static_cast<FLOAT>(offx);
    circle.point.y += static_cast<FLOAT>(offy); // Offy sounds so cute ngl. i kinda want to make a pet beaver unicorn called offy.

    if (owner) owner->invalidate();
}

void ch_line::draw(ch_window &win) {
    ID2D1SolidColorBrush* brush = win.brush_manager.poof(this->color);
    win.m_pRenderTarget->DrawLine(
        this->head,
        this->tail,
        brush
    );
}

void ch_text::draw(ch_window& win) {
    ID2D1SolidColorBrush* brush = win.brush_manager.poof(this->conf.color);

    win.m_pRenderTarget->DrawText(
        reinterpret_cast<const WCHAR*>(this->cont),
        static_cast<UINT32>(this->contLen),
        pTextFormat_,
        rect,
        brush
    );
}

void ch_text::setpos(int x, int y) {
    this->rect.left = static_cast<FLOAT>(x);
    this->rect.top = static_cast<FLOAT>(y);
    this->rect.right = static_cast<FLOAT>(x) + (this->rect.right - this->rect.left);
    this->rect.bottom = static_cast<FLOAT>(y) + (this->rect.bottom - this->rect.top);

    if (owner) owner->invalidate();
}

void ch_text::move(int offx, int offy){
    FLOAT width = rect.right - rect.left;
    FLOAT height = rect.bottom - rect.top;

    rect.left += offx;
    rect.top += offy;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    if (owner) owner->invalidate();
}

HRESULT ch_text::CreateDWriteResources() {
    // Note: Since when ch_text is made it takes
    // a ch_window, the window already has the factory
    // and all the other resources. so we'll just use those

    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory_)
    );

    if (SUCCEEDED(hr)) {
        hr = pDWriteFactory_->CreateTextFormat(
            conf.fontFamily,                // Font family name
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            conf.fontSize,                      // Font size
            conf.locale,                   // Locale
            &pTextFormat_
        );
    }

    return hr;
}

void ch_window::dbg_out(const std::string& msg, int level) {
    if (!dbgEnabled) return;

    // The level just changes the colour
    // It's as simple as that.
    if (level == DBG_INFO) {
        std::string output = "[Chai-ui] " + msg + "\n";
        std::cout << output;
        OutputDebugStringA(output.c_str());
    } else if (level == DBG_WARN) {
        std::string output = "\033[0;33m[Chai-ui] " + msg + "\n";
        std::string cleaned = output.substr(7); // Remove the ANSI code for the windows debug output
        std::cout << output;
        OutputDebugStringA(cleaned.c_str());
    } else if (level == DBG_ERROR) {
        std::string output = "\033[0;31m[Chai-ui] " + msg + "\n";
        std::string cleaned = output.substr(7); // Remove the ANSI code for the windows debug output
        std::cerr << output;
        OutputDebugStringA(cleaned.c_str());
    }
}

HRESULT ch_window::create()
{
    HRESULT hr;

    hr = CreateDeviceIndependentResources();

    // This just enables the debugging requirements (just ANSI for now)
    if (dbgEnabled) dbg_setup();
    
    if (SUCCEEDED(hr)) {
        sprintf(className, "Chai-ui-%lu", GetCurrentProcessId());

        wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (!RegisterClassA(&wc)) {
            DWORD err = GetLastError();
            throw std::runtime_error("RegisterClassA failed, code: " + std::to_string(err));
        }
        dbg_out("Window class registered: " + std::string(className));

        hwnd = CreateWindowExA(
            0, className, title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, wc.hInstance, this
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
    drawable->owner = this;
    drawList.push_back(std::move(drawable));
    InvalidateRect(hwnd, NULL, FALSE); // Trigger a repaint
}

bool ch_window::poll_event(ch_event& event) {
    event = {};
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // check the full queue
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        switch (msg.message) {
            case WM_CLOSE: event.type = ch_event::type::closed; return true;
            case WM_KEYDOWN:
                event.type = ch_event::type::key_down;
                event.key.keycode = static_cast<int>(msg.wParam);
                event.key.repeat = false;
                return true;
            case WM_KEYUP:
                event.type = ch_event::type::key_up;
                event.key.keycode = static_cast<int>(msg.wParam);
                return true;
            case WM_MOUSEMOVE:
                event.type = ch_event::type::mouse_move;
                event.mouse_move.x = (int)(short)LOWORD(msg.lParam);
                event.mouse_move.y = (int)(short)HIWORD(msg.lParam);
                return true;
            case WM_LBUTTONDOWN: case WM_RBUTTONDOWN:
                event.type = ch_event::type::mouse_down;
                event.mouse_button.button = (msg.message == WM_LBUTTONDOWN ? 0 : 1);
                return true;
            case WM_LBUTTONUP: case WM_RBUTTONUP:
                event.type = ch_event::type::mouse_up;
                event.mouse_button.button = (msg.message == WM_LBUTTONUP ? 0 : 1);
                return true;
        }
    }
    event.type = ch_event::type::none;
    return false;
}


void ch_window::invalidate() {
    if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
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

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        ch_window* window = reinterpret_cast<ch_window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    ch_window* window = reinterpret_cast<ch_window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));


    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            if (!window->m_pRenderTarget) {
                window->CreateDeviceDependentResources();
            }
            if (window && window->m_pRenderTarget) {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);

                window->m_pRenderTarget->BeginDraw();
                window->m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
                window->m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

                for (auto& drawable : window->drawList) {
                    if(drawable) drawable->draw(*window);
                    if (window) window->dbg_out("Drew drawable in window: " + window->title + ", type: " + typeid(*drawable).name());
                }

                window->m_pRenderTarget->EndDraw();
                EndPaint(hwnd, &ps);
            }
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

void ch_window::dbg_setup() {
    // This basically just enables ANSI codes
    // for the console. But not for OutputDebugStringA
    // because why not, that makes complete sense.
    // Window's is totally not inconsistent at all.
    // At least the function naming is consistent.

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD outMode;
    GetConsoleMode(hOut, &outMode);
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, outMode);
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

void ch_clock::tick() {
    if (paused) return;

    auto now = std::chrono::steady_clock::now();
    float delta = std::chrono::duration<float, std::milli>(now - last).count();
    elapsed += delta;
    last = now;

}

void ch_clock::reset() {
    last = std::chrono::steady_clock::now();
    delta = 0.0f;
    elapsed = 0.0f;
}



}

// Internal
} // Chai