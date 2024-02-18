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

#include <Arduino.h>
#define SPEAKER_PIN 19  // Pin connected to the speaker
#define LASER_SENSOR_PIN 36 // Pin connected to the laser sensor
#define GAME_MODE_PIN 13  // Pin connected to the game mode switch
#define TRIGGER_PIN 14  // Pin connected to the trigger
#define LASER_PIN 27  // Pin connected to the laser 

const int analogPin = LASER_SENSOR_PIN; // Analog pin to read from
const int thresholdValue = 2; // Specified threshold value
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 400;
bool triggerActive = false;
const int hitCooldown = 1000;
int gameMode = 1;
int lives = 5;



void setup() {
  Serial.begin(115200); // Initialize serial communication at 9600 baud rate

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();  

  pinMode(LASER_PIN, OUTPUT); // Set the laser pin as an output
  pinMode(SPEAKER_PIN, OUTPUT); // Set the speaker pin as an output
  pinMode(GAME_MODE_PIN, INPUT_PULLUP); // Set the game mode pin as an input with pull-up resistor
  pinMode(TRIGGER_PIN, INPUT_PULLUP); // Set the trigger pin as an input
  int switchState = digitalRead(GAME_MODE_PIN); // Read the state of the game mode switch
  if (switchState == HIGH) {
    Serial.println("Game Mode 1");
    gameMode1();
  } else {
    Serial.println("Game Mode 1");
    gameMode1();
  }
}
void loop(){
  //
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
      displayLives();
    }

    int trigState = digitalRead(TRIGGER_PIN);
    if (trigState != prevTrigState){
      prevTrigState = trigState;
      if (trigState == LOW && !triggerActive){
        triggerActive = true;
        Serial.println("FIRE!");
        shootNoise();
        digitalWrite(LASER_PIN, HIGH);
        previousMillis = currentMillis;
      } else if (triggerActive && currentMillis - previousMillis >= interval){
        triggerActive = false;
        digitalWrite(LASER_PIN, LOW);
      }
    } else if (triggerActive && currentMillis - previousMillis >= interval){
      triggerActive = false;
      digitalWrite(LASER_PIN, LOW);
    }
  }

  Serial.println("Game Over");
  gameOverNoise();
  }


//void gameMode2(){
//  
//}

void hitNoise(){
  tone(SPEAKER_PIN, 1000, 500); // Play tone at 1000 Hz for 500 ms
  tone(SPEAKER_PIN, 700, 200); // Play tone at 700 Hz for 200 ms
}

void shootNoise(){
  tone(SPEAKER_PIN, 800, 200);
}

void gameOverNoise(){
  tone(SPEAKER_PIN, 1000, 5000);
}

void displayLives(){
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("Lives: " + String(lives));
  display.display();
}