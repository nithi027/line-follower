// --- Pin Definitions ---
// Address Pins (S0 - S3)
const int S0 = 5;
const int S1 = 18;
const int S2 = 19;
const int S3 = 21;

// Signal Pin (Analog/Digital Input)
const int SIG_PIN = 34; 

void setup() {
  // Initialize Serial Monitor at 115200 baud
  Serial.begin(115200);
  delay(1000); // Give the serial monitor a moment to connect

  // Configure address control pins as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Note: GPIO34 doesn't need explicit pinMode for analogRead, 
  // but it's good practice for tracking inputs.
  pinMode(SIG_PIN, INPUT);

  Serial.println("CD74HC4067 16-Channel Multiplexer Test Started");
  Serial.println("------------------------------------------------");
}

void loop() {
  // Loop through all 16 channels (0 to 15)
  for (int channel = 0; channel < 16; channel++) {
    // Select the channel on the multiplexer
    selectChannel(channel);
    
    // Small delay to let the voltage stabilize after switching channels
    delayMicroseconds(50); 
    
    // Read the sensor value (0 to 4095 on ESP32's 12-bit ADC)
    int sensorValue = analogRead(SIG_PIN);
    
    // Print the data to the Serial Monitor
    Serial.print("CH");
    Serial.print(channel);
    Serial.print(": ");
    Serial.print(sensorValue);
    
    // Formatting: Print a tab space between values, or a newline at the end
    if (channel < 15) {
      Serial.print("\t"); // Spaces out data horizontally
    } else {
      Serial.println();   // Moves to a new line after the 16th channel
    }
  }

  // Wait 500ms before reading the entire array again
  delay(500);
}

/**
 * Sets the 4 control pins (S0-S3) to match the binary representation 
 * of the desired channel number (0-15).
 */
void selectChannel(int channel) {
  // bitRead extracts the specific binary bit (0 or 1) of the channel number
  digitalWrite(S0, bitRead(channel, 0));
  digitalWrite(S1, bitRead(channel, 1));
  digitalWrite(S2, bitRead(channel, 2));
  digitalWrite(S3, bitRead(channel, 3));
}
