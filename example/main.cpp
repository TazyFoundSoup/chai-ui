#include "include/Chai/Inter/Display.hpp"

int main(void) {
    chai::internal::ch_window win("Hello my fellas", 800, 600, DBG_ON);

    win.create();
    win.run();
}