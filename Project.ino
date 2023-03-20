#include <LiquidCrystal.h>

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("What drink would you like? ")
}

void loop() {
  // put your main code here, to run repeatedly:

}
