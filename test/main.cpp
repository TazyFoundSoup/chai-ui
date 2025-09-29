#include "../src/Display.h"

int main() {
    chai::internal::ch_window win("hello", 640, 480, true);
    win.dbg_out("This is a debug message from main.cpp");
    win.set_icon("C:/Programs/chai-ui/test/icon.ico");
}