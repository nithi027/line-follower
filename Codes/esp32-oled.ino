#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Screen Dimensions ---
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels (Change to 32 if using a smaller screen)

// --- Custom I2C Pins ---
#define I2C_SDA 13
#define I2C_SCL 22

// Declaration for an SSD1306 display connected to I2C
// We pass -1 because standard ESP32 OLEDs usually don't share a hardware reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Initialize custom I2C bus with your specific pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the OLED display. 
  // 0x3C is the most common I2C address for these modules. If it fails, try 0x3D.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(f("SSD1306 allocation failed. Check your connections or address!"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println(f("OLED Initialized Successfully!"));
  
  // Clear the buffer (removes the default Adafruit splash screen)
  display.clearDisplay();

  // Draw a basic frame box around the screen edge
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);

  // Configure text properties
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(15, 20);          // Start at X=15, Y=20
  display.println("ESP32 WROOM TEST");
  
  display.setTextSize(2);             // Double-size text
  display.setCursor(15, 35);
  display.println("OLED: OK!");

  // Push the internal memory buffer to the physical screen hardware
  display.display(); 
}

void loop() {
  // Nothing to do in the loop for a static test
}
