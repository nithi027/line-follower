// --- Pin Definitions ---
const int BTN_DOWN   = 15;
const int BTN_SELECT = 2;
const int BTN_BACK   = 4;
const int BTN_UP     = 16;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(500);

  // Configure button pins as INPUT_PULLUP
  // This holds the pin HIGH normally; pressing the button pulls it to GND (LOW)
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);

  Serial.println("Button Test Initialized. Press any button...");
}

void loop() {
  // Read button states (LOW means pressed)
  bool downPressed   = (digitalRead(BTN_DOWN) == LOW);
  bool selectPressed = (digitalRead(BTN_SELECT) == LOW);
  bool backPressed   = (digitalRead(BTN_BACK) == LOW);
  bool upPressed     = (digitalRead(BTN_UP) == LOW);

  // Check and print which button is active
  if (downPressed) {
    Serial.println("Button: DOWN [GPIO15]");
  }
  if (selectPressed) {
    Serial.println("Button: SELECT [GPIO2]");
  }
  if (backPressed) {
    Serial.println("Button: BACK [GPIO4]");
  }
  if (upPressed) {
    Serial.println("Button: UP [GPIO16]");
  }

  // Small delay to keep the Serial Monitor readable and handle basic bouncing
  if (downPressed || selectPressed || backPressed || upPressed) {
    delay(200); 
  }
}
