#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <IRremote.h>
#include <avr/io.h>

int timeruyg = 1;

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define button pin
const byte buttonPin = 7;

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
  timeruyg = 1;
  // Set button pin as input with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, CHANGE);

  Serial.begin(9600);
  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize IR Reciever
  IrReceiver.begin(IRRecieverPin, ENABLE_LED_FEEDBACK);

  // Set up Timer1 for 4 second interrupt
  cli();
  TCCR1A = 0;
  TCCR1B = 0;  //(1 << WGM12) | (1 << CS12) | (1 << CS10);
  TCNT1 = 0;   //OCR1A = 15624;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);  // Set WGM12 bit to 1, turns on CTC mode:
  TCCR1B |= (1 << CS12);   // Set CS12 bits to 1
  /* Notice that When CS10=0, CS11=0 and CS12=1, pre-scaler is 256 */
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt: sei(); // enable global interrupts after configured interrupt
  sei();
  // Attach interrupt to button pin

  // Set itemDispensed to false at startup
  itemDispensed = false;

  // Ask for user choice on startup
  lcd.print("Choose Drink");
}

ISR(TIMER1_COMPA_vect) {
  timeruyg++;
}

void (*resetFunc)(void) = 0;  //declare reset function @ address 0

void loop() {
  if (IrReceiver.decode()) {
    timeruyg = 0;
    userChoice = int(IrReceiver.decodedIRData.command);
    Serial.println(userChoice);

    // Display user choice on LCD
    lcd.clear();
    lcd.print("Choice: ");

    switch (userChoice) {
      case 12:  // maps to 1
        PORTC = 0b00000001;
        lcd.print("1");
        break;
      case 24:  // maps to 2
        PORTC = 0b00000010;
        lcd.print("2");
        break;
      case 94:  // maps to 3
        PORTC = 0b00000001;
        lcd.print("3");
        break;
      case 8:  // maps to 4
        PORTC = 0b00000010;
        lcd.print("4");
        break;
      case 28:  // maps to 5
        PORTC = 0b00000001;
        lcd.print("5");
        break;
      case 90:  // maps to 6
        PORTC = 0b00000010;
        lcd.print(6);
        break;
      case 66:  // maps to 7
        PORTC = 0b00000001;
        lcd.print("7");
        break;
      case 82:  // maps to 8
        PORTC = 0b00000010;
        lcd.print("8");
        break;
      case 74:  // maps to 9
        PORTC = 0b00000001;
        lcd.print("9");
        break;
      case 22:  // maps to 0
        PORTC = 0b00000010;
        lcd.print("0");
        break;
    }
    timeruyg = 0;
    Serial.println("after time");
    while (timeruyg <= 16) {
      //this is a 4 second timer
      Serial.println("iqdispense");
      dispenseDrink();
    }
  }
  IrReceiver.resume();
}



void dispenseDrink() {
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
    Serial.println(distance);
    // If an object is within 10 cm, set green LED
    if (distance < 10) {
      PORTC = 0b00000100;

      // Wait for 2 seconds
      delay(2000);

      PORTC = 0b00000000;

      // Reset back to step 1
      itemDispensed = false;
      resetFunc();  //call reset
    } else {
      // If item has not been dispensed and 4 seconds have elapsed, set red LED
      PORTC = 0b0001000;
      delay(500);
      PORTC = 0b0000000;
    }
  }
}

void buttonInterrupt() {
  // Read the button
  Serial.println("inside buttoninterrupt");
  resetFunc();  //call reset
}