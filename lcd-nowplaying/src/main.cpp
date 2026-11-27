#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t COLS = 16;
const unsigned long SCROLL_INTERVAL = 2000;  // ms pro Scrollschritt

String lines[2] = {"Bereit", ""};
uint16_t offsets[2] = {0, 0};
unsigned long lastScroll = 0;

void showLine(uint8_t row) {
  String text = lines[row];
  String out;

  if (text.length() <= COLS) {
    out = text;
  } else {
    String padded = text + "   ";  // Lücke zwischen Ende und Anfang
    for (uint8_t i = 0; i < COLS; i++) {
      out += padded[(offsets[row] + i) % padded.length()];
    }
  }

  while (out.length() < COLS) out += ' ';  // mit Leerzeichen auffüllen
  lcd.setCursor(0, row);
  lcd.print(out);
}

void readSerial() {
  if (Serial.available() > 0) {
    String text = Serial.readStringUntil('\n');
    text.trim();

    int sep = text.indexOf('|');
    if (sep >= 0) {
      lines[0] = text.substring(0, sep);
      lines[1] = text.substring(sep + 1);
    } else {
      lines[0] = text;
      lines[1] = "";
    }

    offsets[0] = 0;
    offsets[1] = 0;
    showLine(0);
    showLine(1);
    lastScroll = millis();
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  showLine(0);
  showLine(1);
}

void loop() {
  readSerial();

  if (millis() - lastScroll >= SCROLL_INTERVAL) {
    lastScroll = millis();
    for (uint8_t row = 0; row < 2; row++) {
      if (lines[row].length() > COLS) {
        offsets[row] = (offsets[row] + 1) % (lines[row].length() + 3);
        showLine(row);
      }
    }
  }
}