// --- Pin Definitions ---
const int BUZZER_PIN = 17;

// --- PWM Properties ---
const int PWM_CHANNEL = 0; // Use PWM channel 0
const int PWM_RES     = 8; // 8-bit resolution

// --- Musical Notes (Frequencies in Hz) ---
const int NOTE_C4 = 262;
const int NOTE_E4 = 330;
const int NOTE_G4 = 392;
const int NOTE_C5 = 523;

void setup() {
  Serial.begin(115200);
  
  // Set up the ESP32 PWM channel to control the buzzer pitch
  // We initialize it at 2000Hz, but we will change this dynamically
  ledcAttachChannel(BUZZER_PIN, 2000, PWM_RES, PWM_CHANNEL);

  Serial.println("Buzzer Multi-Sound Test Initialized!");
}

void loop() {
  // --- Sound 1: Arpeggio Scale (Beep.. Beep.. Beep..) ---
  Serial.println("Playing: Rising Scale");
  playTone(NOTE_C4, 200);
  playTone(NOTE_E4, 200);
  playTone(NOTE_G4, 200);
  playTone(NOTE_C5, 400);
  delay(1000);

  // --- Sound 2: Fast Police Siren ---
  Serial.println("Playing: Siren Effect");
  for (int i = 0; i < 5; i++) { // Repeat the oscillation 5 times
    // Ramp frequency up
    for (int freq = 600; freq < 1200; freq += 20) {
      ledcWriteTone(BUZZER_PIN, freq);
      delay(10);
    }
    // Ramp frequency down
    for (int freq = 1200; freq > 600; freq -= 20) {
      ledcWriteTone(BUZZER_PIN, freq);
      delay(10);
    }
  }
  noTone(); // Turn off sound
  delay(1000);

  // --- Sound 3: Quick Success Notification ---
  Serial.println("Playing: Success Beeps");
  playTone(880, 100); // High A note
  delay(50);          // Short pause between notes
  playTone(1320, 150); // Higher E note
  delay(2000);        // Wait 2 seconds before looping the entire sequence again
}

// --- Helper Functions ---

/**
 * Plays a specific frequency for a set duration, then stops
 */
void playTone(int frequency, int duration) {
  ledcWriteTone(BUZZER_PIN, frequency); // Start the tone
  delay(duration);                      // Let it play
  ledcWriteTone(BUZZER_PIN, 0);         // Stop the tone (0 Hz turns it off)
}

/**
 * Explicit command to silence the buzzer completely
 */
void noTone() {
  ledcWriteTone(BUZZER_PIN, 0);
}
