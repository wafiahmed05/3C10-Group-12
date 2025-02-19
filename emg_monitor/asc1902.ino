#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels.
#define SCREEN_HEIGHT 64  // OLED display height, in pixels.

// Declaration for SSD1306 display connected using hardware SPI:
#define OLED_COPI 11  // Data input from Arduino to Display.
#define OLED_CLK 13   // Serial Clock input to Display.
#define OLED_DC 9     // Data / Command: 0 = Command, 1 = Data.
#define OLED_CS 10    // Chip select: 0 = Display selected, 1 = Display ignored.
#define OLED_RESET 8  // Reset: Resets when pulled low. Keep high during normal operation.

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//                          &SPI, OLED_DC, OLED_RESET, OLED_CS);
// Comment out above and uncomment below to use software SPI.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
 OLED_COPI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define ANALOG_INPUT A0    // SINE WAVE INPUT FROM OP-AMP
// #define TRIGGER_LEVEL 512  // TO CENTER THE SINE WAVE
int TRIGGER_LEVEL;

int max_analog_dta = 300;   // max analog data
int min_analog_dta = 100;   // min analog data
int static_analog_dta = 0;  // static analog data


int getAnalog(int pin) {
  long sum = 0;
  for (int i = 0; i < 32; i++) {
    sum += analogRead(pin);
  }

  int dta = sum >> 5;

  max_analog_dta = dta > max_analog_dta ? dta : max_analog_dta;  // if max data
  min_analog_dta = min_analog_dta > dta ? dta : min_analog_dta;  // if min data

  return sum >> 5;
}


void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();  // Clears the buffer
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("GROUP 12 EMG Monitor");
  display.display();

    long sum = 0;
  for (int i = 0; i <= 10; i++) {
    for (int j = 0; j < 100; j++) {
      sum += getAnalog(A0);
      delay(1);
    }
  }
  sum /= 1100;
  static_analog_dta = sum;
  Serial.print("static_analog_dta = ");
  Serial.println(static_analog_dta);
  TRIGGER_LEVEL = static_analog_dta;
}

void loop() {
  display.clearDisplay();
  /*
  Using a rising edge trigger prevents false starts or erratic displays. 
  It also centers the waveform, the waveform starts at a consistent point each cycle.
  */
int prevValue = getAnalog(A0);  // get Analog value
  // Serial.println(currentValue);
  int currentValue;
  // Wait for a rising edge crossing TRIGGER_LEVEL
  while (true) {
    currentValue = analogRead(ANALOG_INPUT);
    if (prevValue < TRIGGER_LEVEL && currentValue >= TRIGGER_LEVEL) {
      break;  // Trigger point found
    }
    prevValue = currentValue;
  }
  // Plot waveform after trigger point
  int graphHeight = SCREEN_HEIGHT - 10;
  int lastX = 0;
  int lastY = map(currentValue, 0, 1023, graphHeight, 0);
  int value;
  for (int x = 1; x < SCREEN_WIDTH; x++) {
    value = analogRead(ANALOG_INPUT);
    int y = map(value, 0, 1023, graphHeight, 0);          // maps 'value' from [0, 1023] to [54, 0]
    display.drawLine(lastX, lastY, x, y, SSD1306_WHITE);  // Connect points smoothly
    lastX = x;
    lastY = y;
  }
  Serial.println(value);

  display.display();
  delay(100);
}