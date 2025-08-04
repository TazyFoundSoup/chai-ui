#include "../src/chai.h"

int main() {
    ch_window win("hello", 640, 480, true);
    win.dbgOut("This is a debug message from main.cpp");
    win.setIcon("C:/Programs/chai-ui/test/icon.ico");
    win.run();
}