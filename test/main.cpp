#include "../src/chai.h"

int main() {
    ch_window win("hello", 640, 480, true);
    win.dbg_out("This is a debug message from main.cpp");
    win.set_icon("C:/Programs/chai-ui/test/icon.ico");

    win.draw(ch_rect(50, 50, 100, 100, RGB(50, 50, 100)));
    win.run();
}