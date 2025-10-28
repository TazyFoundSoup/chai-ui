#include "Chai/Inter/Display.hpp"
using namespace Chai::Internal;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ch_window window("Chai Example", 800, 600, DBG_ON);

    // make drawables
    auto rect = std::make_unique<ch_rect>(100, 100, 300, 200, D2D1::ColorF(D2D1::ColorF::Yellow));
    auto text = std::make_unique<ch_text>(
        L"this is some text. it's cool ig ngl idk :3", 
        ch_text_conf{ L"Inter", 24.0f, D2D1::ColorF(D2D1::ColorF::Green), L"en-us" }, 
        D2D1::RectF(50, 50, 400, 100)
    );
    auto circle = std::make_unique<ch_circle>(
       500, 400, 75, 75,
        D2D1::ColorF(D2D1::ColorF::Blue)
    );

    window.draw(std::move(rect));
    window.draw(std::move(text));
    window.draw(std::move(circle));

    window.dbg_out("Test warning message", ch_window::DBG_WARN);
    window.dbg_out("Test error message", ch_window::DBG_ERROR);

    window.run();
    
}
