#include "Chai/Inter/Display.hpp"

int main(void) {
    Chai::Internal::ch_window win("Test window", 800, 600, DBG_ON);
    Chai::Internal::ch_rect rectangle(100, 100, 300, 300, D2D1::ColorF(D2D1::ColorF::Blue));
    win.draw(std::make_unique<Chai::Internal::ch_rect>(rectangle));
    win.run();
}