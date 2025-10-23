#include "Chai/Inter/Display.hpp"
using namespace Chai::Internal;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ch_window window("Chai Example", 800, 600, DBG_ON);

    // make drawables
    auto rect = std::make_unique<ch_rect>(100, 100, 300, 200, D2D1::ColorF(D2D1::ColorF::Yellow));
    auto text = std::make_unique<ch_text>(
        L"tait is cool", 
        ch_text_conf{ L"Inter", 24.0f, D2D1::ColorF(D2D1::ColorF::Green), L"en-us" }, 
        D2D1::RectF(50, 50, 400, 100)
    );

    window.draw(std::move(rect));
    window.draw(std::move(text));

    window.run();
    
}
