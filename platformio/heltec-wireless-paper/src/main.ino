#include "heltec-eink-modules.h"

EInkDisplay_WirelessPaperV1_1 display;

void setup() {
    display.print("Hello, World!");
    display.update();
}

void loop() {}


