//Sharon Chrysler
//Final Project - Door Lock
//August 5th 2024
//Keypad Library
#include <Keypad.h>
//Lcd library
#include <LiquidCrystal.h>
//Servo Library
#include <Servo.h>
//NeoPixel Library
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define PIN 10
Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

const int PASSWORD_LENGTH = 5;//Length of the password array
const int SERVO_PIN = 9;//Servo control pin
const int MAX_ATTEMPTS = 3;//max wrong attempts
const char MASTER_PASSWORD[PASSWORD_LENGTH] = "1234";//Master password
int failedAttempts = 0;//Storing failed attempts
char enteredPassword[PASSWORD_LENGTH];
int dataCount = 0;
bool doorIsOpen = true;

Servo myServo;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

//Keypad config
const byte ROWS = 4;//setting the rows
const byte COLS = 4;//setting the colums
char keys[ROWS][COLS] = {
  {'1', '2', '3', ' '},
  {'4', '5', '6', ' '},
  {'7', '8', '9', ' '},
  {'*', '0', '#', ' '}
};//setting the keypad
byte rowPins[ROWS] = {1, 2, 3, 4}; //row pins
byte colPins[COLS] = {5, 6, 7, 8}; //column pins
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//NeoPixel different colors
const uint32_t colors[] = {
  ring.Color(255, 0, 0),   // Red
  ring.Color(0, 255, 0),   // Green
  ring.Color(0, 0, 255),   // Blue
  ring.Color(255, 255, 0), // Yellow
  ring.Color(255, 0, 255), // Magenta
  ring.Color(0, 255, 255)  // Cyan
};
//using the functions in the setup
void setup() {
  myServo.attach(SERVO_PIN);
  ring.begin();
  ring.show();
  closeDoor();
  lcd.begin(16, 2);
  lcd.print("Final Project");
  lcd.setCursor(0, 1);
  lcd.print("Door Lock");
  delay(2000);
  lcd.clear();
}
//main loop, printing the door status
void loop() {
  if (!doorIsOpen) {
    char key = keypad.getKey();
    if (key == '#') {
      closeDoor();
      lcd.clear();
      lcd.print("  Door is closed");
      delay(3000);
      doorIsOpen = true;
    }
  } else {
    handlePasswordEntry();
  }
}
//function to handle wrong attempts.
void handlePasswordEntry() {
  if (failedAttempts >= MAX_ATTEMPTS) {
    lcd.clear();
    lcd.print("3 Wrong Attempts!!");
    lcd.setCursor(0, 1);
    lcd.print("Go Away");
    delay(5000); //display message for 5 seconds
    return;
  }
  lcd.setCursor(0, 0);
  lcd.print(" Enter Password");
   //keypad input
  char key = keypad.getKey();
  if (key) {
    if (dataCount < PASSWORD_LENGTH - 1) {
      enteredPassword[dataCount++] = key;
      lcd.setCursor(dataCount - 1, 1);
      lcd.print(key);
      //should light up NeoPixel ring in green for each correct key press
      lightUpNeoPixelGreen();
    }
    if (dataCount == PASSWORD_LENGTH - 1) {
      enteredPassword[dataCount] = '\0'; //0 terminate the entered password
      if (strcmp(enteredPassword, MASTER_PASSWORD) == 0) {
        lcd.clear();
        lcd.print("  Door is Open");
        openDoor();
        doorIsOpen = false;
        failedAttempts = 0; //reset failed attempts on success
      } else {
        lcd.clear();
        lcd.print("  Wrong Password");
        failedAttempts++;
        updateNeoPixelRing();
        delay(1000);
      }
      dataCount = 0; //Reset password entry
    }
  }
}
//servo motor action - open
void openDoor() {
  for (int pos = 180; pos >= 0; pos -= 5) {
    myServo.write(pos);
    delay(15);
  }
}
//servo motor action - closed
void closeDoor() {
  for (int pos = 0; pos <= 180; pos += 5) {
    myServo.write(pos);
    delay(15);
  }
}
//using a video, the int variable is taken from there and the uint32.
void updateNeoPixelRing() {
  int numPixelsToLight = failedAttempts * 8; //33% of 24 pixels = 8 pixels
  uint32_t color = colors[failedAttempts % (sizeof(colors) / sizeof(colors[0]))]; // Cycle through colors
  for (int i = 0; i < 24; i++) {
    if (i < numPixelsToLight) {
      ring.setPixelColor(i, ring.Color(255, 0, 0)); //Green color for correct attempt
    } else {
      ring.setPixelColor(i, ring.Color(0, 0, 0)); //Turn off the rest
    }
  }
  ring.show();
  delay(200);
}
void lightUpNeoPixelGreen() {
  //should light up the NeoPixel ring in green for each correct key press
  int numPixelsToLight = (24 / 4); //25% of 24 pixels = 6 pixels
  for (int i = 0; i < 24; i++) {
    if (i < numPixelsToLight) {
      ring.setPixelColor(i, ring.Color(0, 255, 0)); //Green color for correct attempt
    } else {
      ring.setPixelColor(i, ring.Color(0, 0, 0)); //Turn off the rest
    }
  }
  ring.show();
  delay(200); //short delay to make the color change visible
}