/home/richard/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-g++ -Os -Wl,--gc-sections --specs=nano.specs -mcpu=cortex-m3 -T/home/richard/Arduino/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c/ld/bootloader_20.ld -Wl,-Map,./arduino_build_437602/USBMidiHidPedalBoard.ino.map -L/home/richard/Arduino/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c/ld -o ./arduino_build_437602/USBMidiHidPedalBoard.ino.elf -L./arduino_build_437602 -lm -lgcc -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols -Wl,--start-group ./arduino_build_437602/sketch/USBMidiHidPedalBoard.ino.cpp.o ./arduino_build_437602/libraries/USBComposite/MinSysex.c.o ./arduino_build_437602/libraries/USBComposite/usb_audio.c.o ./arduino_build_437602/libraries/USBComposite/usb_composite_serial.c.o ./arduino_build_437602/libraries/USBComposite/usb_generic.c.o ./arduino_build_437602/libraries/USBComposite/usb_hid.c.o ./arduino_build_437602/libraries/USBComposite/usb_mass.c.o ./arduino_build_437602/libraries/USBComposite/usb_mass_mal.c.o ./arduino_build_437602/libraries/USBComposite/usb_midi_device.c.o ./arduino_build_437602/libraries/USBComposite/usb_multi_serial.c.o ./arduino_build_437602/libraries/USBComposite/usb_multi_x360.c.o ./arduino_build_437602/libraries/USBComposite/usb_scsi.c.o ./arduino_build_437602/libraries/USBComposite/usb_scsi_data.c.o ./arduino_build_437602/libraries/USBComposite/usb_x360_generic.c.o ./arduino_build_437602/libraries/USBComposite/usb_x360w.c.o ./arduino_build_437602/libraries/USBComposite/AbsMouse.cpp.o ./arduino_build_437602/libraries/USBComposite/Consumer.cpp.o ./arduino_build_437602/libraries/USBComposite/Digitizer.cpp.o ./arduino_build_437602/libraries/USBComposite/HIDReports.cpp.o ./arduino_build_437602/libraries/USBComposite/Joystick.cpp.o ./arduino_build_437602/libraries/USBComposite/Keyboard.cpp.o ./arduino_build_437602/libraries/USBComposite/Mouse.cpp.o ./arduino_build_437602/libraries/USBComposite/USBAudio.cpp.o ./arduino_build_437602/libraries/USBComposite/USBComposite.cpp.o ./arduino_build_437602/libraries/USBComposite/USBCompositeSerial.cpp.o ./arduino_build_437602/libraries/USBComposite/USBHID.cpp.o ./arduino_build_437602/libraries/USBComposite/USBMIDI.cpp.o ./arduino_build_437602/libraries/USBComposite/USBMassStorage.cpp.o ./arduino_build_437602/libraries/USBComposite/USBMultiSerial.cpp.o ./arduino_build_437602/libraries/USBComposite/USBMultiXBox360.cpp.o ./arduino_build_437602/libraries/USBComposite/USBXBox360Reporter.cpp.o ./arduino_build_437602/libraries/USBComposite/USBXBox360W.cpp.o ./arduino_build_437602/libraries/USBComposite/usb_setup.cpp.o ./arduino_build_437602/core/wirish/start.S.o ./arduino_build_437602/core/wirish/start_c.c.o ./arduino_build_437602/core/wirish/syscalls.c.o ./arduino_build_437602/core/board.cpp.o ./arduino_build_437602/core/wirish/boards.cpp.o ./arduino_build_437602/core/wirish/boards_setup.cpp.o ./arduino_build_437602/core/core.a -Wl,--end-group
/home/richard/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-objcopy -O binary ./arduino_build_437602/USBMidiHidPedalBoard.ino.elf ./arduino_build_437602/USBMidiHidPedalBoard.ino.bin
