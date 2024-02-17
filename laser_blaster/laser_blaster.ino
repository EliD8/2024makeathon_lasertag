//For OLED Display *************
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
//******************************

#define SPEAKER_PIN 8  // Pin connected to the speaker
#define LASER_SENSOR_PIN A0  // Pin connected to the laser sensor
#define GAME_MODE_PIN 2  // Pin connected to the game mode switch
#define TRIGGER_PIN 13  // Pin connected to the laser

const int analogPin = LASER_SENSOR_PIN; // Analog pin to read from
const int thresholdValue = 2; // Specified threshold value
unsigned long previousMillis = 0; // will store last time LED was updated
const int hitCooldown = 1000;
int gameMode = 1;
int lives = 5;



void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  pinMode(SPEAKER_PIN, OUTPUT); // Set the speaker pin as an output
  pinMode(GAME_MODE_PIN, INPUT_PULLUP); // Set the game mode pin as an input with pull-up resistor
  pinMode(TRIGGER_PIN, INPUT_PULLUP); // Set the trigger pin as an input
  int switchState = digitalRead(GAME_MODE_PIN); // Read the state of the game mode switch
  if (switchState == HIGH) {
    Serial.println("Game Mode 1");
    gameMode1();
  } else {
    Serial.println("Game Mode 2");
    //gameMode2();
  }
}

void gameMode1(){
  int prevTrigState = digitalRead(TRIGGER_PIN);
  while(lives > 0){
    int sensorValue = analogRead(analogPin); // Read voltage from analog pin
    float voltage = sensorValue * (3.3 / 1023.0); //True voltage
    unsigned long currentMillis = millis();
    
    // Check if sensor value exceeds the threshold and you can be hit
    if (voltage >= thresholdValue && currentMillis - previousMillis >= hitCooldown) {
      previousMillis = currentMillis;
      Serial.println("Hit!"); // Print a message indicating threshold reached
      hitNoise();
      lives--;
      Serial.println("Lives: " + String(lives));
    }

  }

  Serial.println("Game Over");
  gameOverNoise();
  }


//void gameMode2(){
//  
//}

void hitNoise(){
  tone(SPEAKER_PIN, 1000);
  delay(500);
  tone(SPEAKER_PIN, 700);
  delay(200);
  noTone(SPEAKER_PIN);
}

void gameOverNoise(){
  tone(SPEAKER_PIN, 1000);
  delay(2000);
  tone(SPEAKER_PIN, 1000);
  delay(2000);
}
```