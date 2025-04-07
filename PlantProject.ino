#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
#define SOIL_SENSOR_PIN A0
#define RED_PIN 14     // D5
#define GREEN_PIN 2    // D4
#define BUZZER_PIN 15  // D8

// Melody for happy condition
int melody[] = {392, 330, 262}; // G4, E4, C4
int noteDurations[] = {400, 400, 800};

bool wasDryBefore = false;
unsigned long lastBeepTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED initialization failed"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BLDEACET EC Department");
  display.println("Soil Monitor Booting...");
  display.display();
  delay(2000);
}

void playMelody() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, melody[i], noteDurations[i]);
    delay(noteDurations[i] + 50);
  }
  noTone(BUZZER_PIN);
}

void drySoilBeep() {
  tone(BUZZER_PIN, 1000, 300); // from 100 → 300 ms
  delay(320);
  noTone(BUZZER_PIN);
}

void loop() {
  int sensorValue = analogRead(SOIL_SENSOR_PIN); // 0 (wet) to 1023 (dry)
  int moisturePercent = map(sensorValue, 1023, 0, 0, 100); // Map to percentage

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("---------------------");
  display.println("--BLDEACET EC Dept.--");
  display.println("---------------------");
  display.println("----Plant Project----");
  display.println("---------------------");
  display.print("Moisture: ");
  display.print(moisturePercent);
  display.println("%");

  if (moisturePercent < 50) {
    display.println("Soil is dry! :( ");
    display.println("Please water me!");

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);

    if (millis() - lastBeepTime > 200) {
      drySoilBeep(); // Louder periodic beep
      lastBeepTime = millis();
    }

    wasDryBefore = true;
  } else {
    display.println("Plant is happy :)");

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);

    // Repeat melody every 5 seconds while soil is happy
    if (millis() - lastBeepTime > 2000) {
      playMelody(); // Melody for healthy plant
      lastBeepTime = millis();
    }

    wasDryBefore = false;
  }

  display.display();
  delay(500); // Frequent update
}
