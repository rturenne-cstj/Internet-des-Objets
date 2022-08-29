//DFRobot.com
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "WProgram.h"
void setup();
void loop();
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, world!");
}

void loop()
{
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

