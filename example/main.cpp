#include "Chai/Inter/Display.hpp"

int main(void) {
    Chai::Internal::ch_window win("Hello my fellas", 800, 600, DBG_ON);
    
    win.run();
}