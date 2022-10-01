#include <USBComposite.h> // https://github.com/arpruss/USBComposite_stm32f1
/*
 * USBMidiHidPedalBoard
 *
 * Copyright (C)2022 richard (dot) jones (dot ) 1952 ( art ) gmail (drat ) com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
# USB Midi HID Pedalboard

## Introduction
This project was conceived to connect controls to Virtual Pipe Organ software such as Hauptwerk, Grande Orgue, Cecilia etc running on a touch screen laptop. Flexibility is the main motivation as I have little Midi or VPO experience yet. This project may work for you too. Let me know how you get on and whether improvements should be made.

## Features
- Hosted on the STM32 Blue Pill, a $2US development board
- USB Connection to host computer, LED blink on USB send
- Multiple STM32 boards may be deployed to ease wiring or increase capacity
- Up to 156 contacts arranged as a 14 out by 12 in diode switch matrix
- Contacts may send Midi Note On/Off messages for pedalboard, stops, couplers or toe pistons
- Contacts may instead send Keyboard HID messages for turning music pages displayed on a host computer monitor
- Page turning may use a single sustain pedal type contact e.g. short press forward, long press back.
- Contacts and potentiometers are individually debounced for 50ms to reduce message floods
- Tested using Arduino V1.8.19 with STM32 extensions under Ubuntu 22.04 X86 Linux
- Active High Outputs for the best noise immunity connect to inputs via a diode and switch: 
     Output-----|>|----SW----Input
## Current Configuration
-  8 ADC inputs mapped to Channel 5 Midi CC 20-27 values 0-127 for expression pedals and LED dimming
- 36 Pedalboard inputs in 6x6 matrix mapped to Midi Channel 3 Note 36-67 (C2-G4) and 4 HID keybd buttons for page turning
- 96 Button & LED Shift register IO's mapped to Midi Channels 5-8 Notes 1-24 for pistons and stops

### *** RE-PROGRAMMING ***
 * If the firmware is configured to use the reserved gpio pins the Blue Pill tricky to re-program.
 * To re-program if you have problems:
 * 1. Set Boot0 Link to position 1
 * 2. Press reset
 * 3. Upon releasing reset immediately upload new code using ST-Link
 * 4. Repeat step 3 until programming success
 * 5. Move Boot0 link to position 0
 * 6. Disconnect programmer
 * 7. OR Use a USB boot loader
## Enhancements
- Output drive for LEDs and displays possibly midi controlled may be needed
 ## Thanks
The code is based on the excellent examples from: https://github.com/arpruss/USBComposite_stm32f1/tree/master/examples
Also thanks to the Arduino and STM32 support teams
# License
GPL V3 or later 
*/
 
#define LED_BUILT_IN PC13; // LED illuminates on LOW
const int LED_ON  = LOW;
const int LED_OFF = HIGH;
const unsigned long DEBOUNCE_TIME_MS = 50; // milliseconds
// 24 buttons and leds per division
const int SHIFT_REGISTER_BITS = 8*12 ; // 8 bits per shift register, 4 divisions(3 keyboards+pedals), 3 shift registers each division

 
struct GPIOPinAssignment {int portPin ; int func; int ccCommand; } ;
enum PinFunction {
 IO_SPARE=0,
 IO_RESERVED, /* Used by Blue pill board for debug or boot, available for use as GPIO with some complications */
 OP_SCAN,
 IP_SCAN,
 IP_ADC,
 IP_ADC_LED_DIM,
 OP_SR_CLOCK,
 OP_SR_DATA,
 OP_SR_ENABLE,
 IP_SR_DATA,
 OP_DISPLAY_CLOCK,
 OP_DISPLAY_DATA,
 IP_CONTACT
};
// enum CHANNEL { CH1=1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15,CH16 };
const struct GPIOPinAssignment pin[]  = { 
// ordered in sequence around the PCB to assist wiring up
  // STM32 IC Pins 1-22,7
  { PC13, IO_RESERVED },    // PCB LED active low. Sink max 3mA, Source 3mA, Not 5V Tolerant
  { PC14, IP_SCAN  },       // IP1  Sink max 3mA, Source 3mA, Not 5V Tolerant 
  { PC15, IP_SCAN  },       // IP2  Sink max 3mA, Source 3mA, Not 5V Tolerant 
  { PA0,  IP_ADC }, // Expression Shoe Swell / Crescendo, 3rd param is midi cc command number 
  { PA1,  IP_ADC }, // Expression Shoe Swell / Crescendo 
  { PA2,  IP_ADC }, // Expression Shoe Swell / Crescendo 
  { PA3,  IP_ADC }, // Expression Shoe Swell / Crescendo
  { PA4,  IP_ADC }, // LED Brightness
  { PA5,  IP_ADC },
  { PA6,  IP_ADC },
  { PA7,  IP_ADC },
  { PB0,  IP_SCAN  }, // IP3
  { PB1,  IP_SCAN  }, // IP4
  { PB10, IP_SCAN  }, // IP5
  { PB11, IP_SCAN  }, // IP6
  // STM32 IC Pins 25-46
  { PB12, OP_SCAN  }, // OP1
  { PB13, OP_SCAN  }, // OP2 
  { PB14, IO_RESERVED  }, // Broken
  { PB15, OP_SCAN  }, // OP3 */
  { PA8,  OP_SCAN  }, // OP4 */
  { PA9,  OP_SCAN  }, // OP5 */
  { PA10, OP_SCAN  }, // OP6 
  { PA11,IO_RESERVED }, // Used by USB
  { PA12,IO_RESERVED }, // Used by USB
  { PA15, IO_SPARE }, // 
  { PB3,  IO_SPARE }, //
  { PB4,  OP_SR_CLOCK, SHIFT_REGISTER_BITS }, // 4094 Pin3
  { PB5,  OP_SR_DATA   }, // 4094 Pin 2 Shift registers are used for button input and associated LED indicator output 
  { PB6,  OP_SR_ENABLE }, // 4094 Pin 15
  { PB7,  IP_SR_DATA   }, // Blue Pill 
  { PB8,  OP_DISPLAY_CLOCK }, // TM1637 6 Digit Display
  { PB9,  OP_DISPLAY_DATA  }, // NB SR & Display data lines could be combined
  { PA13,IO_RESERVED }, // SWDIO - Located on Debug connector - Using for GPIO may make programming interesting
  { PA14,IO_RESERVED }, // SWCLK - Located on Debug connector - Using for GPIO makes programming interesting
  { PB2, IO_RESERVED }, // Boot1 - To use for GPIO remove Boot 1 link, and short R10 (100k)

  };
const int NUM_GPIO_PINS = sizeof(pin)/sizeof( pin[0] );

enum Command { 
 NONE = 0 , 
 MIDI_NOTE, // Midi/note on, Midi note off command == 0x80
 MIDI_NOTE_TOGGLE,
 MIDI_CC, // midi control change messages sent by adc
 MIDI_PC, // Unused for now
 HID_PAGE_TURNER
 };

enum Channel { CH1 = 0, CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15,CH16 };

struct Image { int LastInput; unsigned long LastTime; int LastOutput; };
const int MAX_SCANNED_GPIO = 6 * 7 ; // 6 inputs , 7 outputs
const int NUM_ADC_INPUTS = 8;
const int TOTAL_INPUTS = NUM_ADC_INPUTS + MAX_SCANNED_GPIO  + SHIFT_REGISTER_BITS;
struct Image ScanImage[TOTAL_INPUTS];
struct ScanAssociations { uint8_t command ; uint8_t channel; uint8_t value ; };
const struct ScanAssociations ScanParams [ TOTAL_INPUTS ] = {

 { MIDI_CC, CH5, 20 } , // ADC
 { MIDI_CC, CH5, 21 } , // ADC
 { MIDI_CC, CH5, 22 } , // ADC
 { MIDI_CC, CH5, 23 } , // ADC
 { MIDI_CC, CH5, 24 } , // ADC
 { MIDI_CC, CH5, 25 } , // ADC
 { MIDI_CC, CH5, 26 } , // ADC
 { MIDI_CC, CH5, 27 } , // ADC
 

 // Scan OP 1, 6 inputs
 { MIDI_NOTE, CH3, 36 } , /*  1 C1 Pedal */
 { MIDI_NOTE, CH3, 37 } , /*  2 */ 
 { MIDI_NOTE, CH3, 38 } , /*  3 */ 
 { MIDI_NOTE, CH3, 39 } , /*  4 */ 
 { MIDI_NOTE, CH3, 40 } , /*  5 */ 
 { MIDI_NOTE, CH3, 41 } , /*  6 */ 
 // Scan Op 2
 { MIDI_NOTE, CH3, 42 } , /*  7 */ 
 { MIDI_NOTE, CH3, 43 } , /*  8 */ 
 { MIDI_NOTE, CH3, 44 } , /*  9 */ 
 { MIDI_NOTE, CH3, 45 } , /* 10 */ 
 { MIDI_NOTE, CH3, 46 } , /* 11 */ 
 { MIDI_NOTE, CH3, 47 } , /* 12 */ 
 // Scan OP 3
 { MIDI_NOTE, CH3, 48 } , /* 13 */ 
 { MIDI_NOTE, CH3, 49 } , /* 14 */ 
 { MIDI_NOTE, CH3, 50 } , /* 15 */ 
 { MIDI_NOTE, CH3, 51 } , /* 16 */ 
 { MIDI_NOTE, CH3, 52 } , /* 17 */ 
 { MIDI_NOTE, CH3, 53 } , /* 18 */ 
 // Scan OP 4
 { MIDI_NOTE, CH3, 54 } , /* 19 */ 
 { MIDI_NOTE, CH3, 55 } , /* 20 */ 
 { MIDI_NOTE, CH3, 56 } , /* 21 */ 
 { MIDI_NOTE, CH3, 57 } , /* 22 */ 
 { MIDI_NOTE, CH3, 58 } , /* 23 */ 
 { MIDI_NOTE, CH3, 59 } , /* 24 */ 
 // Scan OP 5
 { MIDI_NOTE, CH3, 60 } , /* 25 */ 
 { MIDI_NOTE, CH3, 61 } , /* 26 */ 
 { MIDI_NOTE, CH3, 62 } , /* 27 */ 
 { MIDI_NOTE, CH3, 63 } , /* 28 */ 
 { MIDI_NOTE, CH3, 64 } , /* 29 */ 
 { MIDI_NOTE, CH3, 65 } , /* 30 */ 
 // Scan OP 6
 { MIDI_NOTE, CH3, 66 } , /* 31 */ 
 { MIDI_NOTE, CH3, 67 } , /* 32 G3 Pedal */  
 { HID_PAGE_TURNER, KEY_UP_ARROW    } , /* Note Pedal board uses 32 switches, leaving these 4 entries orphaned */
 { HID_PAGE_TURNER, KEY_END } , // Long press up arrow sends End
 { HID_PAGE_TURNER, KEY_DOWN_ARROW  } , // Long press Down arrow sends next command eg HOME
 { HID_PAGE_TURNER, KEY_HOME  } ,
 // Shift register config for 3 shift registers per division
 // Division 1 Great
 { MIDI_NOTE,  CH5, 1 } , // Stops and thumb pistons
 { MIDI_NOTE,  CH5, 2 } , 
 { MIDI_NOTE,  CH5, 3 } , 
 { MIDI_NOTE,  CH5, 4 } , 
 { MIDI_NOTE,  CH5, 5 } ,
 { MIDI_NOTE,  CH5, 6 } , 
 { MIDI_NOTE,  CH5, 7 } , 
 { MIDI_NOTE,  CH5, 8 } ,
  
 { MIDI_NOTE,  CH5, 9 } ,
 { MIDI_NOTE,  CH5, 10 } , 
 { MIDI_NOTE,  CH5, 11 } , 
 { MIDI_NOTE,  CH5, 12 } , 
 { MIDI_NOTE,  CH5, 13 } , 
 { MIDI_NOTE,  CH5, 14 } , 
 { MIDI_NOTE,  CH5, 15 } , 
 { MIDI_NOTE,  CH5, 16 } , 
 
 { MIDI_NOTE_TOGGLE,  CH5, 17 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 18 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 19 } ,
 { MIDI_NOTE_TOGGLE,  CH5, 20 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 21 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 22 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 23 } , 
 { MIDI_NOTE_TOGGLE,  CH5, 24 } , // Next midi note above the highest on 61 key keyboards
 // Division 2 Swell
 { MIDI_NOTE_TOGGLE,  CH6, 1 } ,
 { MIDI_NOTE_TOGGLE,  CH6, 2 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 3 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 4 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 5 } ,
 { MIDI_NOTE_TOGGLE,  CH6, 6 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 7 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 8 } , 
 
 { MIDI_NOTE_TOGGLE,  CH6, 9 } ,
 { MIDI_NOTE_TOGGLE,  CH6, 10 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 11 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 12 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 13 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 14 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 15 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 16 } , 
 
 { MIDI_NOTE_TOGGLE,  CH6, 17 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 18 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 19 } ,
 { MIDI_NOTE_TOGGLE,  CH6, 20 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 21 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 22 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 23 } , 
 { MIDI_NOTE_TOGGLE,  CH6, 24 } , 
 // Division 3 Choir
 { MIDI_NOTE_TOGGLE,  CH7, 1 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 2 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 3 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 4 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 5 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 6 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 7 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 8 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 9 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 10 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 11 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 12 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 13 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 14 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 15 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 16 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 17 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 18 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 19 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 20 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 21 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 22 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 23 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 24 } , 
 // Division 4 pedalboard 
 { MIDI_NOTE_TOGGLE,  CH7, 1 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 2 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 3 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 4 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 5 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 6 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 7 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 8 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 9 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 10 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 11 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 12 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 13 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 14 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 15 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 16 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 17 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 18 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 19 } ,
 { MIDI_NOTE_TOGGLE,  CH7, 20 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 21 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 22 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 23 } , 
 { MIDI_NOTE_TOGGLE,  CH7, 24 } , 

// { MIDI_NOTE_TOGGLE,  CH7, 56 } , // Should Fail

 };

class myMidi : public USBMIDI {
 virtual void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
   digitalWrite(LED_BUILTIN, LED_ON ); // Turn on LED briefly to indicate USB input
   int scanIndex = getChannelNoteIndex( channel, note ) ;
   if ( scanIndex >= 0 ) {
     ScanImage[scanIndex].LastOutput = 0;
   }
   delay(10); // DEBUG
   digitalWrite(LED_BUILTIN, LED_OFF );
 }
 virtual void handleNoteOn( unsigned int channel, unsigned int note, unsigned int velocity ) {
   digitalWrite(LED_BUILTIN, LED_ON ); // Turn on LED briefly to indicate USB input
   int scanIndex = getChannelNoteIndex( channel, note ) ;
   if ( scanIndex >= 0 ) {
    ScanImage[scanIndex].LastOutput = velocity;
   }
   delay(20); // DEBUG
   digitalWrite(LED_BUILTIN, LED_OFF );
 }
 int getChannelNoteIndex( unsigned int channel, unsigned int note ) {
     for ( int i = 0 ; i < TOTAL_INPUTS; i++ ) {
       switch ( ScanParams[i].command ) {
        case MIDI_NOTE :
          if ( ScanParams[i].channel == channel )
            if ( ScanParams[i].value == note )
              return i;
        break;
        case MIDI_NOTE_TOGGLE :
          if ( ScanParams[i].channel == channel )
            if ( ScanParams[i].value == note )
              return i;
        break;
       }
     }
     return -1 ;
 }
};
//USBMIDI midi;
myMidi midi;
USBHID HID;
HIDKeyboard Keyboard(HID);
USBMultiSerial<1> ms; // One multiserial object

void setup() {   
    int i=0;
    // USBComposite.setProductId(0x0031);
    USBComposite.setProductString("Midi and HID Keyboard");
    USBComposite.setManufacturerString("Organ Pedal Board");
    //midi.begin();
    //HID.begin(HID_KEYBOARD);
    midi.registerComponent(); 
    HID.registerComponent();
    HID.setReportDescriptor(HID_KEYBOARD);
    ms.registerComponent();
    USBComposite.begin();

    pinMode ( LED_BUILTIN, OUTPUT );      
    while (!USBComposite) digitalWrite(LED_BUILTIN, ++i&1); // Super fast flash while waiting for USB to register
    Keyboard.begin(); // useful to detect host capslock state and LEDs

    // afio_cfg_debug_ports(AFIO_DEBUG_NONE); // Remove leading comment to Allow application use of PB3, PA14
    digitalWrite(LED_BUILTIN, LED_ON );
    for ( int gpio_id = 0 ; gpio_id < NUM_GPIO_PINS ; gpio_id++ ) {
      switch ( pin[gpio_id].func ) {
        case IO_RESERVED :
        break;
        case IP_SR_DATA :
        case IP_SCAN :
        case IP_CONTACT :
          pinMode( pin[gpio_id].portPin, INPUT_PULLDOWN );
        break;
        case IP_ADC :
          pinMode( pin[gpio_id].portPin, INPUT );
          // Todo: Trigger pot value send at power up?
        break;
        case OP_SCAN :
        case OP_SR_CLOCK :
        case OP_SR_DATA :
          pinMode( pin[gpio_id].portPin, OUTPUT );
          digitalWrite( pin[gpio_id].portPin, LOW );
        break;
      }
  }
  // Test Shift register driven LED background glow
  for ( int i = 0 ; i <  sizeof( MAX_SCANNED_GPIO  + SHIFT_REGISTER_BITS ) ; i++ ) {
     ScanImage[ i ].LastOutput = 1 ;
  }
  digitalWrite(LED_BUILTIN, LED_OFF ); // Exit with defined LED behaviour
}

//const int MAX_ADC_INPUTS = 10; // for STM32F013
//struct adcStoredFields { int value ; unsigned long timeStamp; } adcHistory[MAX_ADC_INPUTS];

// the loop function runs over and over again forever
void loop() {
  IOScan();
  midi.poll();

  while(ms.ports[0].available()) { 
    ms.ports[0].write(ms.ports[0].read()); // serial port is provisioned to assist reset when installing code using a boot loader
  }
}

void IOScan (void ) {
  static int gpioId = 0;
  static int scanId = 0;
  //static int adcId  = 0;
  static unsigned long lastTimestamp;
  const int threshold = 32;
  int row = 0;
  static int num_sr_clocks = 0;
  static int op_sr_clock = 0;
  static int op_sr_data  = 0;
  static int ip_sr_data  = 0;
  
  unsigned long time_now = millis();
  if ( time_now == lastTimestamp  ) return; // allow active scan inputs time to discharge to 0V 
  lastTimestamp = time_now ;
  do {
    switch ( pin[gpioId].func ) {
      case IP_SCAN : // All inputs are scanned by their associated output lines
      break;
      case IP_ADC :
      {
        /* if ( ( time_now % 100 ) == 0 ) */ { // 100ms interval sampling for 50Hz/60Hz hum rejection
         int new_value = analogRead( pin[gpioId].portPin ); // a value between 0-4095
         // If difference between new_value and old_value is grater than threshold
         if ( abs ( new_value - ScanImage[scanId].LastInput ) > threshold ) {
          if ( ( time_now - ScanImage[scanId].LastTime ) > DEBOUNCE_TIME_MS ) {
            ScanImage[scanId].LastInput = new_value;
            ScanImage[scanId].LastTime  = time_now;
            int midi_channel      = ScanParams[scanId].channel ;
            int cc_command_number = ScanParams[scanId].value;
            digitalWrite(LED_BUILTIN, LED_ON ); // Indicate USB signalling PCB LED
            midi.sendControlChange(midi_channel,cc_command_number, new_value/32 ); // Range 0-127. May need adjustment depending on swell pedal shoe geometry
            delay(3); // DEBUG
            digitalWrite(LED_BUILTIN, LED_OFF );
          }
         }
        }
        //adcId++;
        scanId++;
      }
      break;
      case IP_CONTACT :
      {
         int input = digitalRead( pin[gpioId].portPin );
         if ( ScanImage[scanId].LastInput != input ){
           InputChange( scanId , input );
         }
      }
      break;
      case OP_SCAN :
        {
          pinMode( pin[gpioId].portPin, OUTPUT );
          digitalWrite( pin[gpioId].portPin, HIGH ); // Redundant
          // Scan all digital inputs on this output line
          int col = 0;
          for ( int ip_id = 0 ; ip_id < NUM_GPIO_PINS ; ip_id++ ) {
            if ( pin[ip_id].func == IP_SCAN ) {
              int input  = digitalRead( pin[ip_id].portPin );              
              if ( ScanImage[scanId].LastInput != input ) {
                InputChange( scanId, input );
              }
              scanId++;
            }
          }
          digitalWrite( pin[gpioId].portPin, LOW ); // Finished inputs on this row
        }        
      break;
      case IO_RESERVED :
      break;
      case  OP_SR_CLOCK :
        op_sr_clock = pin[gpioId].portPin;
        num_sr_clocks = pin[gpioId].ccCommand;
      break;
      case OP_SR_DATA :
        op_sr_data = pin[gpioId].portPin;
        break;
      case IP_SR_DATA :
        static int sr_skip = 0;
        if ( sr_skip++ >= 3 ) { // Perform shift registerscan at modest rate
          sr_skip=0;
          // Triple scan of 96 shift register outputs measured as 740us without any optimisation
          ip_sr_data = pin[gpioId].portPin;
          if ( ip_sr_data && op_sr_data && op_sr_clock ) { // All 3 connections present?
            // clear all shift register outputs
            digitalWrite( op_sr_data, LOW );
            for ( int i = 0 ; i <  num_sr_clocks ; i++ ) { // NB: for loop adds 200us, 1.64us clock period, 700us clock hi 
               digitalWrite( op_sr_clock, HIGH );
               digitalWrite( op_sr_clock, LOW );
            }
            // search for changes in button status & update
            int change = 0;
            int input;
            digitalWrite( op_sr_data, HIGH );
            for ( int i = 0 ; i <  num_sr_clocks ; i++ ) {
               digitalWrite( op_sr_clock, HIGH );
               digitalWrite( op_sr_clock, LOW );
               digitalWrite( op_sr_data , LOW ); // only required after 1st clock
               input = digitalRead( ip_sr_data );
               if ( ScanImage[scanId+i].LastInput != input ){
                 change = scanId+i; // record change for later update to avoid SR LEDs blinking out during USB update
                 break;
               }
            }
            // load shift registers with LED status
            digitalWrite( op_sr_data, ScanImage[ scanId + num_sr_clocks - 1 ].LastOutput ); // improve data delay before rising clock
            for ( int i = 0 ; i <  num_sr_clocks ; i++ ) {
               digitalWrite( op_sr_clock, HIGH );
               digitalWrite( op_sr_clock, LOW );
               digitalWrite( op_sr_data, ScanImage[ scanId + num_sr_clocks - i - 2 ].LastOutput );
            }
            if ( change ) {
                 InputChange( change , input );
            }
          }
        }
        scanId += num_sr_clocks;
        break;
    }
    if ( ++ gpioId >= NUM_GPIO_PINS ) {
      gpioId = 0 ;
      scanId = 0 ;
      //adcId  = 0;
    }
  } while ( pin[gpioId].func != OP_SCAN  );
  digitalWrite( pin[ gpioId ].portPin, HIGH ); // Activate next scan line allowing time to charge long cables
}

void InputChange( int scanId, int input ) {
  unsigned long time_now = millis();
  if ( ( time_now - ScanImage[scanId].LastTime ) > DEBOUNCE_TIME_MS ) {
    ScanImage[scanId].LastTime  = time_now;
    ScanImage[scanId].LastInput = input;
    switch ( ScanParams[scanId].command ) {
      case MIDI_NOTE_TOGGLE:
        if ( input == 0 ) 
          return;
        input = ! ScanImage[scanId].LastOutput ; 
        ScanImage[scanId].LastOutput = input ;
      case MIDI_NOTE:
        {    
          digitalWrite(LED_BUILTIN, LED_ON ); // Indicate USB signalling PCB LED ON
          int  midi_channel = ScanParams[scanId].channel ;
          if ( input ) {
            midi.sendNoteOn ( midi_channel , ScanParams[scanId].value, 64 );
          } else {
            midi.sendNoteOff( midi_channel , ScanParams[scanId].value, 0 );
          }
          delay(5); // DEBUG Duration shows where we are on oscilloscope
          digitalWrite(LED_BUILTIN, LED_OFF ); // Indicate USB signalling end PCB LED OFF
        }
        break;
      case HID_PAGE_TURNER :
        digitalWrite(LED_BUILTIN, LED_ON ); // Indicate USB signalling PCB LED ON
        if ( input ) {
          Keyboard.press  ( ScanParams[scanId].value );
          Keyboard.release( ScanParams[scanId].value ); // inhibit auto repeat
        } else {
          if ( time_now - ScanImage[scanId].LastTime > 2000 ) { // Long press?
             if ( ScanParams[scanId+1].command == HID_PAGE_TURNER ) {
              int long_press_command = ScanParams[scanId+1].value; // Simulate next switch contact
              Keyboard.press  ( long_press_command );
              Keyboard.release( long_press_command );
            }
          }
        }
        delay(6); // DEBUG Duration shows where we are on oscilloscope
        digitalWrite(LED_BUILTIN, LED_OFF ); // Indicate USB signalling end PCB LED ON
      break;
    }
  }  
}
