#include "Chai/Inter/Display.hpp"
#include <cmath>

using namespace Chai::Internal;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    ch_window win("Chai Clock Demo", 800, 600, DBG_ON);

    ch_clock clock;
    ch_rect box(100, 100, 200, 200, D2D1::ColorF(D2D1::ColorF::Red));
    ch_line line(
        D2D1::Point2F(300.f, 300.f),
        D2D1::Point2F(500.f, 400.f),
        D2D1::ColorF(D2D1::ColorF::Blue)
    );

    float speed = 0.2f;
    float posX = 100.f;

    MSG msg;
    bool running = true;

    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) running = false;
        }

        clock.tick();

        // move box horizontally using delta time in ms
        posX += clock.dt() * speed;
        if (posX > 600.f || posX < 0.f) speed = -speed;

        box.setpos((int)posX, 250);

        // color pulse using seconds
        float t = fmod(clock.time() / 1000.f, 2.f); // loop 0-2 seconds
        int green = static_cast<int>((t / 2.f) * 255);
        box.color = D2D1::ColorF(1.f, green / 255.f, 0.f, 1.f);

        win.draw(std::make_unique<ch_rect>(box));
    }

    return 0;
}
