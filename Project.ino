#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <IRremote.h>
#include <avr/io.h>

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define button pin
const int buttonPin = 6;

// Define ultrasonic sensor pins
const int trigPin = 9;   // Analog Input 0
const int echoPin = 10;  // Analog Input 1

// Define IR Remote Reciever Pin
const int IRRecieverPin = 8;

volatile int userChoice;
volatile boolean itemDispensed;

void setup() {
  // Set PORT C as 00000000
  DDRC = 0b00000000;
  PORTC = 0b00000000;

  // Set button pin as input with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize IR Reciever
  IrReceiver.begin(IRRecieverPin, ENABLE_LED_FEEDBACK);

  // Set up Timer1 for 4 second interrupt
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
  OCR1A = 15624;
  TIMSK1 = (1 << OCIE1A);

  // Attach interrupt to button pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING);

  // Set itemDispensed to false at startup
  itemDispensed = false;

  // Ask for user choice on startup
  lcd.print("Choose Drink");
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop() {
  if (IrReceiver.decode()) {
    userChoice = int(IrReceiver.decodedIRData.command);
    Serial.println(userChoice);

    // Display user choice on LCD
    lcd.clear();
    lcd.print("Choice: ");

    switch (userChoice) {
      case 12:  // maps to 1
        // digitalWrite(yellowLedPin, HIGH);
        // digitalWrite(blueLedPin, LOW);
        PORTC = 0b00000001;
        lcd.print(1);
        break;
      case 24:  // maps to 2
        // digitalWrite(blueLedPin, HIGH);
        // digitalWrite(yellowLedPin, LOW);
        PORTC = 0b00000010;
        lcd.print(2);
        break;
      case 94:  // maps to 3
        // digitalWrite(yellowLedPin, HIGH);
        // digitalWrite(blueLedPin, LOW);
        PORTC = 0b00000001;
        lcd.print(3);
        break;
      case 8:  // maps to 4
        // digitalWrite(blueLedPin, HIGH);
        // digitalWrite(yellowLedPin, LOW);
        PORTC = 0b00000010;
        lcd.print(4);
        break;
      case 28:  // maps to 5
        // digitalWrite(yellowLedPin, HIGH);
        // digitalWrite(blueLedPin, LOW);
        PORTC = 0b00000001;
        lcd.print(5);
        break;
      case 90:  // maps to 6
        // digitalWrite(blueLedPin, HIGH);
        // digitalWrite(yellowLedPin, LOW);
        PORTC = 0b00000010;
        lcd.print(6);
        break;
      case 66:  // maps to 7
        // digitalWrite(yellowLedPin, HIGH);
        // digitalWrite(blueLedPin, LOW);
        PORTC = 0b00000001;
        lcd.print(7);
        break;
      case 82:  // maps to 8
        // digitalWrite(blueLedPin, HIGH);
        // digitalWrite(yellowLedPin, LOW);
        PORTC = 0b00000010;
        lcd.print(8);
        break;
      case 74:  // maps to 9
        // digitalWrite(yellowLedPin, HIGH);
        // digitalWrite(blueLedPin, LOW);
        PORTC = 0b00000001;
        lcd.print(9);
        break;
      case 22:  // maps to 0
        // digitalWrite(blueLedPin, HIGH);
        // digitalWrite(yellowLedPin, LOW);
        PORTC = 0b00000010;
        lcd.print(0);
        break;
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  // If 4 seconds have elapsed and item has not been dispensed
  if (!itemDispensed) {
    // Measure distance using ultrasonic sensor
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // If an object is within 10 cm, set green LED
    if (distance < 10) {
      // digitalWrite(greenLedPin, HIGH);
      // digitalWrite(yellowLedPin, LOW);
      // digitalWrite(blueLedPin, LOW);
      PORTC = 0b00000100;

      // Wait for 2 seconds
      delay(2000);

      PORTC = 0b00000000;

      // Reset back to step 1
      itemDispensed = false;
      resetFunc();  //call reset
    } else {
      // If item has not been dispensed and 4 seconds have elapsed, set red LED
      // digitalWrite(redLedPin, !digitalRead(redLedPin));
      PORTC = 0b0001000;
      delay(500);
      PORTC = 0b0000000;      
    }
  }
}

void buttonInterrupt() {
  // Read the button input
  resetFunc();  //call reset
}