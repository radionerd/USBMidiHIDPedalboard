//  led.h 

//  Copyright (C)2022 richard (dot) jones (dot ) 1952 ( at ) gmail (dot ) com

// License MIT

class LED {
  private:
    const int ON  = 0;
    const int OFF = 1;
    const unsigned long HOLDOFF_US = 10000000L;
    int ledpin;
    unsigned long holdoff;
  public:
   LED ( int pin = LED_BUILTIN ) {
     ledpin = pin;
     pinMode ( ledpin ,  OUTPUT );
     holdoff = micros();
   }
   
   void on ( void )   { digitalWrite( ledpin , ON ); holdoff = micros() + HOLDOFF_US; }
   
   void off( void )   { digitalWrite( ledpin , OFF); }
   
   void toggle (void) { digitalWrite( ledpin , ! digitalRead(ledpin) ); holdoff = micros() + HOLDOFF_US; }
   
   void service(void) {
     unsigned long time = micros();
     if ( time > holdoff ) {
       holdoff = 0; // Guard against wrap around
       int pulse_width = ( time & 0x1FFFFF ) >> 6 ; // 2097152us or 0x200000us period
       if ( pulse_width > 16383 )
         pulse_width = 32768 - pulse_width;
       int led_state = HIGH;
       if ( pulse_width > (time & 16383 ) )  
         led_state = LOW;
       digitalWrite(LED_BUILTIN, led_state );   // update the LED    
     }
   }
};

LED led; // or led(PC13); choose your own LED gpio pin
