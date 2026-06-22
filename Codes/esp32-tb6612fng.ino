// --- Pin Definitions ---
// Motor A (Left)
const int AIN1 = 25;
const int AIN2 = 26;
const int PWMA = 32;

// Motor B (Right)
const int BIN1 = 27;
const int BIN2 = 14;
const int PWMB = 33;

// Standby Pin
const int STBY = 12;

// --- PWM Properties ---
const int PWM_FREQ = 5000; // 5 kHz PWM frequency
const int PWM_RES  = 8;    // 8-bit resolution (0-255 speed range)

void setup() {
  Serial.begin(115200);

  // Initialize all control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Configure ESP32 PWM functional properties
  ledcAttach(PWMA, PWM_FREQ, PWM_RES);
  ledcAttach(PWMB, PWM_FREQ, PWM_RES);

  // Enable the motor driver by pulling STBY HIGH
  digitalWrite(STBY, HIGH);
  
  Serial.println("TB6612FNG Motor Test Initialized!");
}

void loop() {
  // 1. Move Forward with gradually increasing speed
  Serial.println("Moving Forward (Accelerating)...");
  for (int speed = 50; speed <= 255; speed += 5) {
    moveMotors(1, 1, speed, speed); // 1 = Forward
    delay(50);
  }
  delay(1000);

  // 2. Stop
  Serial.println("Stopping...");
  stopMotors();
  delay(1000);

  // 3. Move Backward with gradually increasing speed
  Serial.println("Moving Backward (Accelerating)...");
  for (int speed = 50; speed <= 255; speed += 5) {
    moveMotors(-1, -1, speed, speed); // -1 = Backward
    delay(50);
  }
  delay(1000);

  // 4. Stop
  stopMotors();
  delay(1000);

  // 5. Spin Left at medium speed
  Serial.println("Spinning Left...");
  moveMotors(-1, 1, 150, 150); 
  delay(2000);

  // 6. Spin Right at medium speed
  Serial.println("Spinning Right...");
  moveMotors(1, -1, 150, 150); 
  delay(2000);

  // 7. Stop before repeating the loop
  stopMotors();
  delay(2000);
}

// --- Helper Functions ---

/**
 * Controls both motors
 * @param dirA:  1 for Forward, -1 for Backward, 0 for Stop
 * @param dirB:  1 for Forward, -1 for Backward, 0 for Stop
 * @param speedA: PWM value (0 to 255)
 * @param speedB: PWM value (0 to 255)
 */
void moveMotors(int dirA, int dirB, int speedA, int speedB) {
  // --- Control Motor A ---
  if (dirA == 1) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (dirA == -1) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
  ledcWrite(PWMA, speedA);

  // --- Control Motor B ---
  if (dirB == 1) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else if (dirB == -1) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }
  ledcWrite(PWMB, speedB);
}

// Shortcut function to cut power to both motors smoothly
void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}
