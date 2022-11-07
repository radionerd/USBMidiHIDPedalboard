#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
class LCD_N_C32 {
  public:
  LCD_N_C32 ( ) {
    Wire.begin(1);
    Wire.setClock(400000); // fast speed
    for ( int i = 0 ; i < 128 ; i++ ) {
      Wire.beginTransmission(i);
      int error = Wire.endTransmission();
      if ( error == 0 ) {
        char buff[80];
        sprintf ( buff , "VirtualPipeOrganI2C Address 0x%02X",i);
        write(i,buff);
      }
    }
  }
  int write(uint8_t LCDNumberLSB, char *buffer ) {
      Wire.begin(1);
      Wire.setClock(400000); // fast speed
      Wire.beginTransmission(LCDNumberLSB);
      int error = Wire.endTransmission();
      if ( error ) {
         char buff[80];
         sprintf ( buff , "LCD[0x%02X] Error %d",LCDNumberLSB, error );
         CompositeSerial.println(buff);
      } else {
         LiquidCrystal_PCF8574 lcd(LCDNumberLSB);  // set the LCD i2c address for a 16 chars and 2 line display
         lcd.begin(16, 2, Wire);  // 7913us initialize the lcd  
         lcd.setBacklight(255);
         lcd.setCursor(0, 0);
         char temp = buffer[16];
         buffer[16] = 0;
         lcd.print( buffer );   // 2096us
         buffer[16] = temp;
         lcd.setCursor(0, 1);
         lcd.print(buffer+16);
    }
    return error; // 6.3ms 32 characters write Total with delays in library LiquidCrystal_PCF8574.begin() reduced to 200us each
  }
};
