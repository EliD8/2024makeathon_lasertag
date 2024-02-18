#include <Arduino.h>
#define SPEAKER_PIN 19  // Pin connected to the speaker
#define LASER_SENSOR_PIN 36 // Pin connected to the laser sensor
#define GAME_MODE_PIN 12  // Pin connected to the game mode switch
#define TRIGGER_PIN 14  // Pin connected to the trigger
#define LASER_PIN 27  // Pin connected to the laser 

const int analogPin = LASER_SENSOR_PIN; // Analog pin to read from
const int thresholdValue = 2; // Specified threshold value
unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long previousMillis_Trigger = 0; // will store last time trigger was used
const long interval = 400;
bool triggerActive = false;
int points = 0;
int beenHit = 0;
int timeRemaining = 0;

//cooldown after getting hit 
const int hitCooldown = 1000;
int gameMode = 1;
int lives = 99;

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

//For ESP NOW ******************
#include <esp_now.h>
#include <WiFi.h>

//YELLOW ESP MAC Address
// {0xC8, 0xF0, 0x9E, 0x27, 0x53, 0x4C}; 

//BLACK ESP MAC Address
// {0x40, 0xF5, 0x20, 0x70, 0x48, 0xB8}; 

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Define variables to store to be sent
bool hitSend;

// Define variables to store incoming readings
bool hitRecv;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    bool hit;
} struct_message;

// Create a struct_message called hitTransfer to hold data to send
struct_message msgSend;

// Create a struct_message to hold incoming sensor readings
struct_message msgRecv;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msgRecv, incomingData, sizeof(msgRecv));
  Serial.print("Bytes received: ");
  Serial.println(len);
  hitRecv = msgRecv.hit;

  if (!hitRecv && beenHit){
    beenHit = 0;
    // Set values to send
    msgSend.hit = true;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msgSend, sizeof(msgSend));
  } else if (hitRecv) {
    points += 10;
  }
}

// ******************************

void setup() {
  Serial.begin(115200); // Initialize serial communication at 9600 baud rate

  setup_espnow();
  setup_oled();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  display.setRotation(1);

  // Clear the buffer
  display.clearDisplay();  

  pinMode(LASER_PIN, OUTPUT); // Set the laser pin as an output
  pinMode(SPEAKER_PIN, OUTPUT); // Set the speaker pin as an output
  pinMode(GAME_MODE_PIN, INPUT_PULLUP); // Set the game mode pin as an input with pull-up resistor
  pinMode(TRIGGER_PIN, INPUT_PULLUP); // Set the trigger pin as an input
  delay(50);
  int switchState = digitalRead(GAME_MODE_PIN); // Read the state of the game mode switch
  if (switchState == HIGH) {
    Serial.println("Game Mode 1");
    gameMode1();
  } else {
    Serial.println("Game Mode 2");
    gameMode2();
  }
}
void loop(){
  //
}
void gameMode1(){
  displayLives();
  int prevTrigState = digitalRead(TRIGGER_PIN);
  while(lives > 0){
    int sensorValue = analogRead(analogPin); // Read voltage from analog pin
    float voltage = sensorValue * (3.3 / 1023.0); //True voltage
    unsigned long currentMillis = millis();
    unsigned long currentMillis_Trigger = millis();
    
    // Check if sensor value exceeds the threshold and you can be hit
    if (voltage >= thresholdValue && currentMillis - previousMillis >= hitCooldown) {
      previousMillis = currentMillis;
      Serial.println("Hit!"); // Print a message indicating threshold reached
      Serial.println(sensorValue);
      Serial.println(voltage);
      hit_routine(); //stun for 5s
      hitNoise();
      lives--;
      Serial.println("Lives: " + String(lives));
      displayLives();
    }

    //Check if we're firing 
    int trigState = digitalRead(TRIGGER_PIN);
    if (trigState != prevTrigState){
      prevTrigState = trigState;
      if (trigState == LOW && !triggerActive){
        triggerActive = true;
        Serial.println("FIRE!");
        shootNoise();
        digitalWrite(LASER_PIN, HIGH);
        previousMillis_Trigger = currentMillis_Trigger;
      } else if (triggerActive && currentMillis_Trigger - previousMillis_Trigger >= interval){
        triggerActive = false;
        digitalWrite(LASER_PIN, LOW);
      }
    } else if (triggerActive && currentMillis_Trigger - previousMillis_Trigger >= interval){
      triggerActive = false;
      digitalWrite(LASER_PIN, LOW);
    }
  }

  Serial.println("Game Over");
  gameOverNoise();
  }


void gameMode2(){
  int minutes = 2;
  int countdownStart = millis();
  int prevTrigState = digitalRead(TRIGGER_PIN);
  int currentMillis_Timer = millis();
  int previousMillis_Timer = millis();

  while(minutes * 60000 > (millis() - countdownStart)){
    //Dislpay game countdown and points
    currentMillis_Timer = millis();
    if (currentMillis_Timer - previousMillis_Timer >= 1000){
      previousMillis_Timer = currentMillis_Timer;
      timeRemaining = (minutes * 60) - ((millis() - countdownStart) / 1000);
      displayTimeAndScore();
    }

    int sensorValue = analogRead(analogPin); // Read voltage from analog pin
    float voltage = sensorValue * (3.3 / 1023.0); //True voltage
    unsigned long currentMillis = millis();
    unsigned long currentMillis_Trigger = millis();
    
    // Check if sensor value exceeds the threshold and you can be hit
    if (voltage >= thresholdValue && currentMillis - previousMillis >= hitCooldown) {
      previousMillis = currentMillis;
      Serial.println("Hit!"); // Print a message indicating threshold reached
      Serial.println(sensorValue);
      Serial.println(voltage);
      hit_routine(); //stun for 5s
      hitNoise();
      beenHit = 1;
    }

    //Check if we're firing 
    int trigState = digitalRead(TRIGGER_PIN);
    if (trigState != prevTrigState){
      prevTrigState = trigState;
      if (trigState == LOW && !triggerActive){
        // Set values to send
        msgSend.hit = false;

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msgSend, sizeof(msgSend));
        
        if (result == ESP_OK) {
          Serial.println("Sent with success");
        }
        else {
          Serial.println("Error sending the data");
        }

        triggerActive = true;
        Serial.println("FIRE!");
        shootNoise();
        digitalWrite(LASER_PIN, HIGH);
        previousMillis_Trigger = currentMillis_Trigger;
      } else if (triggerActive && currentMillis_Trigger - previousMillis_Trigger >= interval){
        triggerActive = false;
        digitalWrite(LASER_PIN, LOW);
      }
    } else if (triggerActive && currentMillis_Trigger - previousMillis_Trigger >= interval){
      triggerActive = false;
      digitalWrite(LASER_PIN, LOW);
    }
  }

  Serial.println("Game Over");
  gameOverNoise();
  displayFinalScore();
}

void hitNoise(){
  tone(SPEAKER_PIN, 1000, 2000); // Play tone at 1000 Hz for 500 ms
}

void shootNoise(){
  tone(SPEAKER_PIN, 800, 200);
}

void gameOverNoise(){
  tone(SPEAKER_PIN, 1000, 5000);
}

void hit_routine(){
  //on hit, get stunned for 10 s and count down 
  //HIT HIT HITfor 2seconds
  for (int i = 10; i > 0; i--){
    display.clearDisplay();
  display.setTextSize(3); 
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("HIT");
  display.display();
  delay(100);

  //ALL WHITE
  display.fillScreen(WHITE);
  display.display();
  tone(SPEAKER_PIN,500, 200);
  delay(50);
  noTone(SPEAKER_PIN);
  delay(100);
  }

  //COUNT DOWN FROM 8
    for (int i = 8; i > 3; i--){
    display.clearDisplay();
    display.setTextSize(5); 
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.println(i);
    display.display();
    tone(SPEAKER_PIN, i*100, 200);
    delay(50);
    noTone(SPEAKER_PIN);
    delay(1000);
  }


  //Last 3
  for (int i = 3; i > 0; i--){
    display.clearDisplay();
    display.setTextSize(5); 
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0,0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.println(i);
    display.display();
    tone(SPEAKER_PIN, 500, 1000);
    delay(1000);
  }
}

void displayLives(){
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("Lives");
  display.setTextSize(4); 
  display.println(String(lives));
  display.display();
}

void displayTimeAndScore(){
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("Time  " + String(timeRemaining));
  display.println("Score  " + String(points));
  display.display();
}

void displayFinalScore(){
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("FinalScore " + String(points));
  display.display();

}

void setup_espnow(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void setup_oled(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}