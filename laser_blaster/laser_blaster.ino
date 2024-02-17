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