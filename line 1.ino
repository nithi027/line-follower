// ========== PIN ASSIGNMENTS ==========
const int pwma = 6;
const int ain1 = 2;
const int ain2 = 3;
const int pwmb = 9;
const int bin1 = 4;
const int bin2 = 5;
const int stby = 7;

const int sensorPins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int NUM_SENSORS = 8;

// ========== TUNING VALUES ==========
float right_motor_correction = 0.90;

float Kp = 0.09;
float Kd = 4.0;

int MAX_SPEED = 220;
int BASE_SPEED = 180;
int TURN_SPEED = 180;

// ========== GLOBAL VARIABLES ==========
int sensorMin[NUM_SENSORS];
int sensorMax[NUM_SENSORS];
int lastError = 0;

// =======================================================
//  SETUP
// =======================================================
void setup() {
  pinMode(pwma, OUTPUT); pinMode(ain1, OUTPUT); pinMode(ain2, OUTPUT);
  pinMode(pwmb, OUTPUT); pinMode(bin1, OUTPUT); pinMode(bin2, OUTPUT);
  pinMode(stby, OUTPUT);

  Serial.begin(9600);
  calibrateSensors();
}

// =======================================================
//  LOOP
// =======================================================
void loop() {
  uint16_t position;
  bool lineLost = readSensorsAndCheckIfLineLost(&position);

  // --- BASIC LINE FOLLOWING ---
  if (!lineLost) {
    int error = position - 3500;
    int derivative = error - lastError;
    int motorSpeedCorrection = Kp * error + Kd * derivative;
    lastError = error; // Update last known error only when line is found

    int leftSpeed = constrain(BASE_SPEED + motorSpeedCorrection, 0, MAX_SPEED);
    int rightSpeed = constrain(BASE_SPEED - motorSpeedCorrection, 0, MAX_SPEED);

    driveMotors(leftSpeed, rightSpeed * right_motor_correction);
  }
  // --- RECOVERY WHEN LINE LOST ---
  else {
    Serial.println("Line Lost. Trying to recover...");

    // *** THIS IS THE CORRECTED LOGIC ***
    if (lastError > 0) { // A positive error means the line was on the RIGHT
      // To find it, PIVOT RIGHT (Left motor forward, Right motor backward)
      driveMotors(TURN_SPEED, -TURN_SPEED);
    } else { // A negative error means the line was on the LEFT
      // To find it, PIVOT LEFT (Left motor backward, Right motor forward)
      driveMotors(-TURN_SPEED, TURN_SPEED);
    }

    // Recovery timeout (prevent endless spin)
    unsigned long start = millis();
    while (millis() - start < 600) {
      uint16_t tempPos;
      bool found = !readSensorsAndCheckIfLineLost(&tempPos);
      if (found) break; // Stop pivoting if line is found
    }

    // Stop briefly before resuming
    driveMotors(0, 0);
    delay(30);
  }
}

// =======================================================
//  HELPER FUNCTIONS
// =======================================================

// --- Check if line is detected ---
bool readSensorsAndCheckIfLineLost(uint16_t* pPosition) {
  long numerator = 0;
  int denominator = 0;
  bool onLine = false;

  for (int i = 0; i < NUM_SENSORS; i++) {
    int rawValue = analogRead(sensorPins[i]);
    int scaledValue = constrain(map(rawValue, sensorMin[i], sensorMax[i], 0, 1000), 0, 1000);

    if (scaledValue > 200) onLine = true;

    numerator += (long)scaledValue * (i * 1000);
    denominator += scaledValue;
  }

  if (!onLine || denominator == 0) return true;

  *pPosition = numerator / denominator;
  return false;
}

// --- Sensor Calibration ---
void calibrateSensors() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Calibrating...");

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  for (int i = 0; i < 250; i++) {
    for (int j = 0; j < NUM_SENSORS; j++) {
      int val = analogRead(sensorPins[j]);
      if (val < sensorMin[j]) sensorMin[j] = val;
      if (val > sensorMax[j]) sensorMax[j] = val;
    }
    delay(20);
  }

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Calibration complete!");
  delay(1000);
}

// --- Motor Control ---
void driveMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(stby, HIGH);

  // Left Motor
  if (leftSpeed >= 0) {
    digitalWrite(ain1, HIGH); digitalWrite(ain2, LOW);
    analogWrite(pwma, leftSpeed);
  } else {
    digitalWrite(ain1, LOW); digitalWrite(ain2, HIGH);
    analogWrite(pwma, -leftSpeed);
  }

  // Right Motor
  if (rightSpeed >= 0) {
    digitalWrite(bin1, HIGH); digitalWrite(bin2, LOW);
    analogWrite(pwmb, rightSpeed);
  } else {
    digitalWrite(bin1, LOW); digitalWrite(bin2, HIGH);
    analogWrite(pwmb, -rightSpeed);
  }
}
