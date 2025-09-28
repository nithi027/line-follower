// SmartFlex 8 + TB6612FNG line follower (Arduino Nano)
// Pins must match your circuit exactly.

const int pwma = 6;
const int ain1 = 2;
const int ain2 = 3;
const int pwmb = 9;
const int bin1 = 4;
const int bin2 = 5;
const int stby = 7;

const int sensorPins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int NUM_SENSORS = 8;

const int button_calibration = 11; // D12 -> to GND when pressed
const int button_start = 12;       // D13 -> to GND when pressed

// PID tuning (start with these; tune later)
float Kp = 0.09;
float Ki = 0.0001;
float Kd = 4.0;

float right_motor_correction = 0.90;

int MAX_SPEED = 220;
int BASE_SPEED = 180;
int TURN_SPEED = 180;

int sensorMin[NUM_SENSORS];
int sensorMax[NUM_SENSORS];

long lastError = 0;
long integralTerm = 0;

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(pwma, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwmb, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(stby, OUTPUT);
  digitalWrite(stby, HIGH); // Wake TB6612

  // Buttons (use pullups)
  pinMode(button_calibration, INPUT_PULLUP);
  pinMode(button_start, INPUT_PULLUP);

  // Init sensor min/max
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  // Wait for calibration button press
  Serial.println("Ready. Press CALIBRATION button (D12) to start calibration.");
  while (digitalRead(button_calibration) == HIGH) {
    delay(10);
  }
  delay(200); // debounce

  Serial.println("Calibrating sensors... Robot will spin slowly for calibration.");
  calibrateSensors();
  Serial.println("Calibration complete.");

  // Wait for start button press
  Serial.println("Press START button (D13) to begin line following.");
  while (digitalRead(button_start) == HIGH) {
    delay(10);
  }
  delay(200); // debounce
  Serial.println("Starting line following.");
}

void loop() {
  uint16_t position;
  bool lost = readSensorsAndCheckIfLineLost(&position);

  if (lost) {
    // Simple recovery: pivot toward last error side
    if (lastError < 0) {
      // Last seen on left -> pivot left (left reverse, right forward)
      driveMotors(-TURN_SPEED, TURN_SPEED);
    } else {
      // Last seen on right -> pivot right
      driveMotors(TURN_SPEED, -TURN_SPEED);
    }
    // Try for up to 400 ms to find line, then resume normal
    unsigned long start = millis();
    while (millis() - start < 400) {
      if (!readSensorsAndCheckIfLineLost(&position)) break;
      delay(5);
    }
    // stop briefly
    driveMotors(0, 0);
    delay(30);
    return;
  }

  // Normal PID line following
  int error = (int)position - 3500; // center is ~3500 for 0..7000 scale
  long P = error;
  integralTerm += error;
  // Anti-windup
  if (integralTerm > 100000) integralTerm = 100000;
  if (integralTerm < -100000) integralTerm = -100000;
  long D = error - lastError;
  lastError = error;

  float correction = P * Kp + integralTerm * Ki + D * Kd;

  int leftSpeed = (int)round(BASE_SPEED + correction);
  int rightSpeed = (int)round(BASE_SPEED - correction);

  // Limit speeds
  leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);

  // Apply right motor correction factor when positive (scale only when forward)
  if (rightSpeed > 0) {
    rightSpeed = (int)(rightSpeed * right_motor_correction);
  } else {
    // if reversing, don’t scale (or scale similarly if you prefer)
  }

  driveMotors(leftSpeed, rightSpeed);
}

// -------------------------------------------------
// Calibration: spin left and right while sampling
// -------------------------------------------------
void calibrateSensors() {
  int calSpeed = 120; // reasonable speed to spin
  // initialize arrays
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  // spin left (left reverse, right forward)
  driveMotors(-calSpeed, calSpeed);
  unsigned long t0 = millis();
  while (millis() - t0 < 1200) { // 1.2s
    sampleSensorsForCalibration();
    delay(6);
  }

  // spin right (left forward, right reverse)
  driveMotors(calSpeed, -calSpeed);
  t0 = millis();
  while (millis() - t0 < 1200) { // 1.2s
    sampleSensorsForCalibration();
    delay(6);
  }

  driveMotors(0, 0);
  delay(100);
}

// read sensor values and update min/max
void sampleSensorsForCalibration() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    int v = analogRead(sensorPins[i]);
    if (v < sensorMin[i]) sensorMin[i] = v;
    if (v > sensorMax[i]) sensorMax[i] = v;
  }
}

// -------------------------------------------------
// Read sensors -> compute weighted position (0..7000)
// Returns true if line lost
// -------------------------------------------------
bool readSensorsAndCheckIfLineLost(uint16_t* pPosition) {
  long numerator = 0;
  long denominator = 0;
  bool onLine = false;

  for (int i = 0; i < NUM_SENSORS; i++) {
    int raw = analogRead(sensorPins[i]);
    int scaled = 0;
    int minv = sensorMin[i];
    int maxv = sensorMax[i];
    if (maxv > minv) {
      scaled = map(raw, minv, maxv, 0, 1000);
      if (scaled < 0) scaled = 0;
      if (scaled > 1000) scaled = 1000;
    } else {
      scaled = 0;
    }

    if (scaled > 200) onLine = true; // threshold: adjust if needed

    numerator += (long)scaled * (i * 1000);
    denominator += scaled;
  }

  if (!onLine || denominator == 0) {
    return true;
  }

  *pPosition = (uint16_t)(numerator / denominator);
  return false;
}

// -------------------------------------------------
// Motor driver helper (TB6612 style)
// leftSpeed and rightSpeed accept negative (reverse) values
// range: -255 .. +255
// -------------------------------------------------
void driveMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(stby, HIGH);

  // Left motor (A)
  if (leftSpeed >= 0) {
    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    analogWrite(pwma, leftSpeed);
  } else {
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, HIGH);
    analogWrite(pwma, -leftSpeed);
  }

  // Right motor (B)
  if (rightSpeed >= 0) {
    digitalWrite(bin1, HIGH);
    digitalWrite(bin2, LOW);
    analogWrite(pwmb, rightSpeed);
  } else {
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, HIGH);
    analogWrite(pwmb, -rightSpeed);
  }
}
