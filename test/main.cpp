#include "../src/chai.h"

int main() {
    ch_window win("hello", 640, 480);
    win.setIcon("C:/Programs/chai-ui/test/icon.ico");
    win.run();
}