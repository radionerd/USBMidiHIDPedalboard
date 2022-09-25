#line 1 "/home/richard/Projects/PedalBoard/Firmware/USBMidiHidPedalBoard/libraries/USBComposite_stm32f1-master/Consumer.cpp"
#include "USBComposite.h" 

void HIDConsumer::begin(void) {}
void HIDConsumer::end(void) {}
void HIDConsumer::press(uint16_t button) {
    report.button = button;
    sendReport();
}

void HIDConsumer::release() {
    report.button = 0;
    sendReport();
}
