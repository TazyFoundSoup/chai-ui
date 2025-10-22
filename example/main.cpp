#include "Chai/Inter/Display.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    Chai::Internal::ch_window win("Test window", 800, 600, DBG_ON);
    Chai::Internal::ch_rect rectangle(100, 100, 300, 300, D2D1::ColorF(D2D1::ColorF::Blue));
    Chai::Internal::ch_text_conf textConf;
    textConf.fontSize = 24.0f;
    textConf.color = D2D1::ColorF(D2D1::ColorF::Red);
    Chai::Internal::ch_text text(L"676767", textConf, D2D1::RectF(50.0f, 50.0f, 400.0f, 200.0f));
    win.draw(std::make_unique<Chai::Internal::ch_text>(text));
    win.draw(std::make_unique<Chai::Internal::ch_rect>(rectangle));
    win.run();
    text.move(200, 200);
}     