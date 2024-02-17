#define SPEAKER_PIN 8  // Pin connected to the speaker

const int analogPin = A0; // Analog pin to read from
const int thresholdValue = 2; // Specified threshold value
unsigned long previousMillis = 0; // will store last time LED was updated
const int hitCooldown = 1000;

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(analogPin); // Read voltage from analog pin
  float voltage = sensorValue * (3.3 / 1023.0); //True voltage
  unsigned long currentMillis = millis();

  
  // Check if sensor value exceeds the threshold
  if (voltage >= thresholdValue && currentMillis - previousMillis >= hitCooldown) {
    previousMillis = currentMillis;
    tone(SPEAKER_PIN, 1000);
    Serial.println("Hit!"); // Print a message indicating threshold reached
    delay(500);
    tone(SPEAKER_PIN, 1000);
    delay(200);
    tone(SPEAKER_PIN, 500);



  }
  
 
}