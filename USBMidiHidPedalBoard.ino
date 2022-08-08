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
- Midi Control Change messages sent from 4 potentiometers for swell shoe and crescendo controls
- Contacts and potentiometers are individually debounced for 50ms to reduce message floods
- Midi Channel 1 is notionally assigned to foot controls, Midi Ch 2 to 24 buttons on each of up to 4 manuals.
- Midi channel numbers may be offset by adding configuration diodes or a hex switch
- Tested using Arduino V1.8.19 with STM32 extensions under Ubuntu 22.04 X86 Linux
- Active High Outputs for the best noise immunity connect to inputs via a diode and switch: 
     Output-----|>|----SW----Input
### *** RE-PROGRAMMING ***
 * If the firmware is configured to use the reserved gpio pins the Blue Pill tricky to re-program.
 * To re-program if you have problems:
 * 1. Set Boot0 Link to position 1
 * 2. Press reset
 * 3. Upon releasing reset immediately upload new code using ST-Link
 * 4. Repeat step 3 until programming success
 * 5. Move Boot0 link to position 0
 * 6. Disconnect programmer
## Enhancements
- Output drive for LEDs and displays possibly midi controlled may be needed
 ## Thanks
The code is based on the excellent examples from: https://github.com/arpruss/USBComposite_stm32f1/tree/master/examples
Also thanks to the Arduino and STM32 support teams
# License
GPL V3 or later */
 
const unsigned long DEBOUNCE_TIME_MS = 50;

USBMIDI midi;
USBHID HID;
HIDKeyboard Keyboard(HID);

#define LED_BUILTIN PC13
  
struct GPIOPinAssignment {int number ; int func;} ;
enum PinFunction {
 IO_SPARE=0,
 IO_RESERVED, /* Used by Blue pill board for debug or boot, available for use as GPIO with some complications */
 OP_SCAN,
 IP_SCAN,
 IP_ADC
};
enum CHANNEL { CH1=1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15,CH16 };
const struct GPIOPinAssignment pin[]  = { // ordered mostly in sequence around the PCB to assist wiring up
  /* STM32 IC Pins 1-22,7 */
  { PC14, IP_SCAN  }, /* IP1  Sink max 3mA, Source 0mA, Not 5V Tolerant */
  { PC15, IP_SCAN  }, /* IP2  Sink max 3mA, Source 0mA, Not 5V Tolerant */
  { PA0,  IP_ADC   }, /* Expression Shoe Swell / Crescendo */
  { PA1,  IP_ADC   }, /* Expression Shoe Swell / Crescendo */
  { PA2,  IP_ADC   }, /* Expression Shoe Swell / Crescendo */
  { PA3,  IP_ADC   }, /* Expression Shoe Swell / Crescendo */
  { PA4,  IP_SCAN  }, /* IP3  */
  { PA5,  IP_SCAN  }, /* IP4  */
  { PA6,  IP_SCAN  }, /* IP5  */
  { PA7,  IP_SCAN  }, /* IP6  */
  { PB0,  IP_SCAN  }, /* IP7  */
  { PB1,  IP_SCAN  }, /* IP8  */
  { PB10, IP_SCAN  }, /* IP9  */
  { PB11, IP_SCAN  }, /* IP10 */
  /* STM32 IC Pins 25-46 */
  { PB12, IP_SCAN  }, /* IP11 */
  { PB13, IP_SCAN  }, /* IP12 */
  { PB14, OP_SCAN  }, /* OP1  1-12 Pedal C1-B1 */
  { PB15, OP_SCAN  }, /* OP2 13-24 Pedal C2-B2 */
  { PA8,  OP_SCAN  }, /* OP3 25-32 Pedal C3-G3 */
  { PA9,  OP_SCAN  }, /* OP4 33-48 Toe Pistons / Stops / Couplers */
  { PA10, OP_SCAN  }, /* OP5 49-60 Toe Pistons / Stops / Couplers */
  { PA11,IO_RESERVED }, /* Used by USB */
  { PA12,IO_RESERVED }, /* Used by USB */
  { PA15, OP_SCAN  }, /* OP6  Manual 1 / Stops / Couplers */
  { PB3,  OP_SCAN  }, /* OP7  Manual 1 / Stops / Couplers */
  { PB4,  OP_SCAN  }, /* OP8  Manaul 2 / Stops / Couplers */
  { PB5,  OP_SCAN  }, /* OP9  Manual 2 / Stops / Couplers */
  { PB6,  OP_SCAN  }, /* OP10 Manual 3 / Stops / Couplers */
  { PB7,  OP_SCAN  }, /* OP11 Manual 3 / Stops / Couplers */
  { PB8,  OP_SCAN  }, /* OP12 Manual 4 / Stops / Couplers */
  { PB9,  OP_SCAN  }, /* OP13 Manual 4 / Stops / Couplers */
  { PA13, IO_RESERVED }, /* SWDIO - Located on Debug connector - Using for GPIO makes programming interesting */
  { PA14, IO_RESERVED }, /* SWCLK - Located on Debug connector - Using for GPIO makes programming interesting */ 
  { PB2,  IO_RESERVED }, /* Boot1 - To use for GPIO remove Boot 1 link, and short R10 (100k) */
  { PC13, OP_SCAN  }, /* OP14 Config also PCB LED active low. Sink max 3mA, Source 0mA, Not 5V Tolerant */

  };
const int NUM_GPIO_PINS = sizeof(pin)/sizeof( pin[0] );

enum Command { 
 NONE = 0 , 
 MIDI_NOTE_CH1=1, // Midi note on, Midi note off command == 0x80
 MIDI_NOTE_CH2=2,
 MIDI_CC, // Unused for now
 MIDI_PC, // Unused for now
 HID_PAGE_TURNER,
 CONFIG, // Used for midi channel offset configuration
 };
  
struct Image { int LastInput; unsigned long LastTime; };
const int MAX_SCANNED_GPIO = 12 * 14 ; // 12 inputs , 14 outputs Unless some reserved pins are pressed into service
struct Image ScanImage[MAX_SCANNED_GPIO];
struct ScanAssociations { int command ; int value ; };
const struct ScanAssociations ScanParams [MAX_SCANNED_GPIO] = {

 /* Scan OP 1, 12 inputs */
 { MIDI_NOTE_CH1, 24 } , /*  1 C1 Pedal */
 { MIDI_NOTE_CH1, 25 } , /*  2 */ 
 { MIDI_NOTE_CH1, 26 } , /*  3 */ 
 { MIDI_NOTE_CH1, 27 } , /*  4 */ 
 { MIDI_NOTE_CH1, 28 } , /*  5 */ 
 { MIDI_NOTE_CH1, 29 } , /*  6 */ 
 { MIDI_NOTE_CH1, 30 } , /*  7 */ 
 { MIDI_NOTE_CH1, 31 } , /*  8 */ 
 { MIDI_NOTE_CH1, 32 } , /*  9 */ 
 { MIDI_NOTE_CH1, 33 } , /* 10 */ 
 { MIDI_NOTE_CH1, 34 } , /* 11 */ 
 { MIDI_NOTE_CH1, 35 } , /* 12 */ 
 /* Scan OP 2, 12 Inputs */
 { MIDI_NOTE_CH1, 36 } , /* 13 */ 
 { MIDI_NOTE_CH1, 37 } , /* 14 */ 
 { MIDI_NOTE_CH1, 38 } , /* 15 */ 
 { MIDI_NOTE_CH1, 39 } , /* 16 */ 
 { MIDI_NOTE_CH1, 40 } , /* 17 */ 
 { MIDI_NOTE_CH1, 41 } , /* 18 */ 
 { MIDI_NOTE_CH1, 42 } , /* 19 */ 
 { MIDI_NOTE_CH1, 43 } , /* 20 */ 
 { MIDI_NOTE_CH1, 44 } , /* 21 */ 
 { MIDI_NOTE_CH1, 45 } , /* 22 */ 
 { MIDI_NOTE_CH1, 46 } , /* 23 */ 
 { MIDI_NOTE_CH1, 47 } , /* 24 */ 
 /* Scan OP 3, 12 Inputs */
 { MIDI_NOTE_CH1, 48 } , /* 25 */ 
 { MIDI_NOTE_CH1, 49 } , /* 26 */ 
 { MIDI_NOTE_CH1, 50 } , /* 27 */ 
 { MIDI_NOTE_CH1, 51 } , /* 28 */ 
 { MIDI_NOTE_CH1, 52 } , /* 29 */ 
 { MIDI_NOTE_CH1, 53 } , /* 30 */ 
 { MIDI_NOTE_CH1, 54 } , /* 31 */ 
 { MIDI_NOTE_CH1, 55 } , /* 32 G3 Pedal */  
 { HID_PAGE_TURNER, KEY_UP_ARROW    } , /* Note Pedal board uses 32 switches, leaving these 4 entries orphaned */
 { HID_PAGE_TURNER, KEY_END } , // Long press up arrow sends End
 { HID_PAGE_TURNER, KEY_DOWN_ARROW  } , // Long press Down arrow sends next command eg HOME
 { HID_PAGE_TURNER, KEY_HOME  } , 
 /* Scan OP 4, 12 Inputs */
 { MIDI_NOTE_CH1, 60 } , /* 37 Toe Pistons / Stops / Couplers */ 
 { MIDI_NOTE_CH1, 61 } , /* 38 */ 
 { MIDI_NOTE_CH1, 62 } , /* 39 */ 
 { MIDI_NOTE_CH1, 63 } , /* 40 */ 
 { MIDI_NOTE_CH1, 64 } , /* 41 */ 
 { MIDI_NOTE_CH1, 65 } , /* 42 */ 
 { MIDI_NOTE_CH1, 66 } , /* 43 */ 
 { MIDI_NOTE_CH1, 67 } , /* 44 */ 
 { MIDI_NOTE_CH1, 68 } , /* 45 */ 
 { MIDI_NOTE_CH1, 69 } , /* 46 */ 
 { MIDI_NOTE_CH1, 70 } , /* 47 */ 
 { MIDI_NOTE_CH1, 71 } , /* 48 */ 
 /* Scan OP 5, 12 Inputs */
 { MIDI_NOTE_CH1, 72 } , /* 49 */ 
 { MIDI_NOTE_CH1, 73 } , /* 50 */ 
 { MIDI_NOTE_CH1, 74 } , /* 51 */ 
 { MIDI_NOTE_CH1, 75 } , /* 52 */ 
 { MIDI_NOTE_CH1, 76 } , /* 53 */ 
 { MIDI_NOTE_CH1, 77 } , /* 54 */ 
 { MIDI_NOTE_CH1, 78 } , /* 55 */ 
 { MIDI_NOTE_CH1, 79 } , /* 56 */ 
 { MIDI_NOTE_CH1, 80 } , /* 57 */ 
 { MIDI_NOTE_CH1, 81 } , /* 58 */ 
 { MIDI_NOTE_CH1, 82 } , /* 59*/ 
 { MIDI_NOTE_CH1, 83 } , /* 60 */ 

 /* Scan OP 6, 12 Inputs */
 { MIDI_NOTE_CH2,  1 } , /* Toe Pistons / stops / manual buttons / */
 { MIDI_NOTE_CH2,  2 } , 
 { MIDI_NOTE_CH2,  3 } , 
 { MIDI_NOTE_CH2,  4 } , 
 { MIDI_NOTE_CH2,  5 } ,
 { MIDI_NOTE_CH2,  6 } , 
 { MIDI_NOTE_CH2,  7 } , 
 { MIDI_NOTE_CH2,  8 } , 
 { MIDI_NOTE_CH2,  9 } ,
 { MIDI_NOTE_CH2, 10 } , 
 { MIDI_NOTE_CH2, 11 } , 
 { MIDI_NOTE_CH2, 12 } , 
 /* Scan OP 7, 12 Inputs */
 { MIDI_NOTE_CH2, 13 } , /* Toe Pistons / stops / manual buttons */
 { MIDI_NOTE_CH2, 14 } , 
 { MIDI_NOTE_CH2, 15 } , 
 { MIDI_NOTE_CH2, 16 } , 
 { MIDI_NOTE_CH2, 17 } ,
 { MIDI_NOTE_CH2, 18 } , 
 { MIDI_NOTE_CH2, 19 } , 
 { MIDI_NOTE_CH2, 20 } , 
 { MIDI_NOTE_CH2, 21 } ,
 { MIDI_NOTE_CH2, 22 } , 
 { MIDI_NOTE_CH2, 23 } , 
 { MIDI_NOTE_CH2, 24 } , 
 /* Scan OP 8, 12 Inputs */
 { MIDI_NOTE_CH2, 25 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 26 } , 
 { MIDI_NOTE_CH2, 27 } , 
 { MIDI_NOTE_CH2, 28 } , 
 { MIDI_NOTE_CH2, 29 } ,
 { MIDI_NOTE_CH2, 30 } , 
 { MIDI_NOTE_CH2, 31 } , 
 { MIDI_NOTE_CH2, 32 } , 
 { MIDI_NOTE_CH2, 33 } ,
 { MIDI_NOTE_CH2, 34 } , 
 { MIDI_NOTE_CH2, 35 } , 
 { MIDI_NOTE_CH2, 36 } , 
 /* Scan OP 9, 12 Inputs */
 { MIDI_NOTE_CH2, 37 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 38 } , 
 { MIDI_NOTE_CH2, 39 } , 
 { MIDI_NOTE_CH2, 40 } , 
 { MIDI_NOTE_CH2, 41 } ,
 { MIDI_NOTE_CH2, 42 } , 
 { MIDI_NOTE_CH2, 43 } , 
 { MIDI_NOTE_CH2, 44 } , 
 { MIDI_NOTE_CH2, 45 } ,
 { MIDI_NOTE_CH2, 46 } , 
 { MIDI_NOTE_CH2, 47 } , 
 { MIDI_NOTE_CH2, 48 } , 
 /* Scan OP 10, 12 Inputs */
 { MIDI_NOTE_CH2, 49 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 50 } , 
 { MIDI_NOTE_CH2, 51 } , 
 { MIDI_NOTE_CH2, 52 } , 
 { MIDI_NOTE_CH2, 53 } ,
 { MIDI_NOTE_CH2, 54 } , 
 { MIDI_NOTE_CH2, 55 } , 
 { MIDI_NOTE_CH2, 56 } , 
 { MIDI_NOTE_CH2, 57 } ,
 { MIDI_NOTE_CH2, 58 } , 
 { MIDI_NOTE_CH2, 59 } , 
 { MIDI_NOTE_CH2, 60 } , 
 /* Scan OP 11, 12 Inputs */
 { MIDI_NOTE_CH2, 61 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 62 } , 
 { MIDI_NOTE_CH2, 63 } , 
 { MIDI_NOTE_CH2, 64 } , 
 { MIDI_NOTE_CH2, 65 } ,
 { MIDI_NOTE_CH2, 66 } , 
 { MIDI_NOTE_CH2, 67 } , 
 { MIDI_NOTE_CH2, 68 } , 
 { MIDI_NOTE_CH2, 69 } ,
 { MIDI_NOTE_CH2, 70 } , 
 { MIDI_NOTE_CH2, 71 } , 
 { MIDI_NOTE_CH2, 72 } , 
 /* Scan OP 12, 12 Inputs */
 { MIDI_NOTE_CH2, 73 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 74 } , 
 { MIDI_NOTE_CH2, 75 } , 
 { MIDI_NOTE_CH2, 76 } , 
 { MIDI_NOTE_CH2, 77 } ,
 { MIDI_NOTE_CH2, 78 } , 
 { MIDI_NOTE_CH2, 79 } , 
 { MIDI_NOTE_CH2, 80 } , 
 { MIDI_NOTE_CH2, 81 } ,
 { MIDI_NOTE_CH2, 82 } , 
 { MIDI_NOTE_CH2, 83 } , 
 { MIDI_NOTE_CH2, 84 } , 
 /* Scan OP 13, 12 Inputs */
 { MIDI_NOTE_CH2, 85 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 86 } , 
 { MIDI_NOTE_CH2, 87 } , 
 { MIDI_NOTE_CH2, 88 } , 
 { MIDI_NOTE_CH2, 89 } ,
 { MIDI_NOTE_CH2, 90 } , 
 { MIDI_NOTE_CH2, 91 } , 
 { MIDI_NOTE_CH2, 92 } , 
 { MIDI_NOTE_CH2, 93 } ,
 { MIDI_NOTE_CH2, 94 } , 
 { MIDI_NOTE_CH2, 95 } , 
 { MIDI_NOTE_CH2, 96 } , 

 /* Scan OP 14, 12 Inputs */
 { MIDI_NOTE_CH2, 97 } , /* Couplers / stops / manual buttons */
 { MIDI_NOTE_CH2, 98 } , 
 { MIDI_NOTE_CH2, 99 } , 
 { MIDI_NOTE_CH2, 100} , 
 { MIDI_NOTE_CH2, 101} ,
 { MIDI_NOTE_CH2, 102} , 
 { MIDI_NOTE_CH2, 103} , 
 { MIDI_NOTE_CH2, 104} , 

 { CONFIG, 1 } , // Hex switch for User Configured midi channel offset 0-15
 { CONFIG, 2 } , 
 { CONFIG, 4 } ,
 { CONFIG, 8 } ,

 };

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
    USBComposite.begin();

    pinMode ( LED_BUILTIN, OUTPUT );      
    while (!USBComposite) digitalWrite(LED_BUILTIN, ++i&1); // Super fast flash while waiting for USB to register
    Keyboard.begin(); // useful to detect host capslock state and LEDs

    // afio_cfg_debug_ports(AFIO_DEBUG_NONE); // Remove leading comment to Allow application use of PB3, PA14
    digitalWrite(LED_BUILTIN, LOW );
    for ( int gpio_id = 0 ; gpio_id < NUM_GPIO_PINS ; gpio_id++ ) {
      switch ( pin[gpio_id].func ) {
        case IO_RESERVED :
        break;
        case IP_SCAN :
          pinMode( pin[gpio_id].number, INPUT_PULLDOWN );
        break;
        case IP_ADC :
          pinMode( pin[gpio_id].number, INPUT );
          // Todo: Trigger pot value send at power up?
        break;
        case OP_SCAN :
          pinMode( pin[gpio_id].number, OUTPUT );
          digitalWrite( pin[gpio_id].number, LOW );
        break;
      }
  }
  digitalWrite(LED_BUILTIN, HIGH ); // Exit with defined LED behaviour
}

const int MAX_ADC_INPUTS = 10; // for STM32F013
struct adcStoredFields { int value ; unsigned long timeStamp; } adcHistory[MAX_ADC_INPUTS];

// the loop function runs over and over again forever
void loop() {
  IOScan();
}

void IOScan (void ) {
  static int gpioId = 0;
  static int scanId = 0;
  static int adcId  = 0;
  static int UserConfig;
  static unsigned long lastTimestamp;
  const int threshold = 32;
  int row = 0;
  
  unsigned long time_now = millis();
  if ( time_now == lastTimestamp  ) return; // allow active scan inputs time to discharge to 0V 
  lastTimestamp = time_now ;
  do {
    switch ( pin[gpioId].func ) {
      case IP_SCAN : // All inputs are scanned by their associated output lines
      break;
      case IP_ADC :
      {
        int new_value = analogRead( pin[adcId].number ); // a value between 0-4095
        // If difference between new_value and old_value is grater than threshold
        // if ((new_value > adcHistory[adcId].value && new_value - adcHistory[adcId].value > threshold) ||
        //    (new_value < adcHistory[adcId].value && adcHistory[adcId].value - new_value > threshold)) {
        if ( abs ( new_value - adcHistory[adcId].value ) > threshold ) {
          if ( ( time_now - adcHistory[adcId].timeStamp ) > DEBOUNCE_TIME_MS ) {
            adcHistory[adcId].value = new_value;
            adcHistory[adcId].timeStamp = time_now;
            int midi_channel = ( ScanParams[0].command - 1 + UserConfig ) & 0xF ; // Borrow channel number from first contact definition
            int cc_command = 10+adcId; // Arbitrary command numbers until we read how to configure Hauptwerk
            digitalWrite(LED_BUILTIN, HIGH ); // Indicate USB signalling PCB LED OFF
            midi.sendControlChange(midi_channel,cc_command, new_value/32 ); // Range 0-127. May need adjustment depending on swell pedal shoe geometry
            digitalWrite(LED_BUILTIN, LOW );
            adcHistory[adcId].value = new_value;
          }
        }
        adcId++;
      }
      break;
      case OP_SCAN :
        {
          pinMode( pin[gpioId].number, OUTPUT );
          digitalWrite( pin[gpioId].number, HIGH ); // Redundant
          // Scan all digital inputs on this output line
          int col = 0;
          for ( int ip_id = 0 ; ip_id < NUM_GPIO_PINS ; ip_id++ ) {
            if ( pin[ip_id].func == IP_SCAN ) {
              int input  = digitalRead( pin[ip_id].number );              
              if ( ScanImage[scanId].LastInput != input ) {
                if ( ( time_now - ScanImage[scanId].LastTime ) > DEBOUNCE_TIME_MS ) {
                  switch ( ScanParams[scanId].command ) {
                    case MIDI_NOTE_CH1 :
                    case MIDI_NOTE_CH2 : 
                      {    
                        digitalWrite(LED_BUILTIN, HIGH ); // Indicate USB signalling PCB LED OFF
                        int midi_channel = ( ScanParams[scanId].command - 1 + UserConfig ) & 0xF ;
                        if ( input ) {
                          midi.sendNoteOn ( midi_channel , ScanParams[scanId].value, 64 );
                        } else {
                          midi.sendNoteOff( midi_channel , ScanParams[scanId].value, 0 );
                        }
                        digitalWrite(LED_BUILTIN, LOW ); // Indicate USB signalling PCB LED ON
                      }
                      break;
                    case HID_PAGE_TURNER :
                      digitalWrite(LED_BUILTIN, HIGH ); // Indicate USB signalling PCB LED OFF
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
                      digitalWrite(LED_BUILTIN, LOW ); // Indicate USB signalling PCB LED ON
                    break;
                  }
                  ScanImage[scanId].LastTime  = time_now;
                  ScanImage[scanId].LastInput = input;
                }  
              }
              if ( ScanParams[scanId].command == CONFIG ) {
                UserConfig &= ScanParams[scanId].value ^ 0xFFFFFFFF ;
                //input  = digitalRead( pin[ip_id].number );
                if ( input )
                  UserConfig |= ScanParams[scanId].value;
              }
              scanId++;
            }
          }
          digitalWrite( pin[gpioId].number, LOW ); // Finished inputs on this row
        }        
      break;
      case IO_RESERVED :
      break;
    }
    if ( ++ gpioId >= NUM_GPIO_PINS ) {
      gpioId = 0 ;
      scanId = 0 ;
      adcId  = 0;
    }
  } while ( pin[gpioId].func != OP_SCAN  );
  digitalWrite( pin[ gpioId ].number, HIGH ); // Activate next scan line allowing time to charge long cables
}
