const int analogPin = A0; // Analog pin to read from
const int thresholdValue = 3; // Specified threshold value

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
}

void loop() {
  int sensorValue = analogRead(analogPin); // Read voltage from analog pin
  float voltage = sensorValue * (3.3 / 1023.0); 
  
  // Check if sensor value exceeds the threshold
  if (voltage >= thresholdValue) {
    Serial.println("Hit!"); // Print a message indicating threshold reached
    // Additional actions can be taken here
    delay(1000); // Delay for readability, adjust as needed
  }
  
 
}