
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin = 2;
const int buzzerPin = 9;

String morse = "";
String result = "";

bool lastButtonState = HIGH;
unsigned long pressStart = 0;
unsigned long lastInputTime = 0;

bool isPressed = false;
bool hasInput = false;

const char* morseCode[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.",
  "--.", "....", "..", ".---", "-.-", ".-..",
  "--", "-.", "---", ".--.", "--.-", ".-.",
  "...", "-", "..-", "...-", ".--", "-..-",
  "-.--", "--.."
};

char decodeMorse(String code) {
  for (int i = 0; i < 26; i++) {
    if (code == morseCode[i]) {
      return 'A' + i;
    }
  }
  return '?';
}

void playSignal(bool dash) {
  int duration = dash ? 600 : 200;

  tone(buzzerPin, 800);
  delay(duration);
  noTone(buzzerPin);
}

void updateLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Morse: ");
  lcd.print(morse);

  lcd.setCursor(0, 1);
  lcd.print("Text: ");
  lcd.print(result);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Morse Decoder");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");
  delay(1500);

  updateLCD();
}

void loop() {
  bool buttonState = digitalRead(buttonPin);

  // Phat hien luc bat dau bam
  if (lastButtonState == HIGH && buttonState == LOW) {
    pressStart = millis();
    isPressed = true;
  }

  // Phat hien luc nha nut
  if (lastButtonState == LOW && buttonState == HIGH && isPressed) {
    unsigned long duration = millis() - pressStart;

    if (morse.length() < 8) {
      bool dash = duration >= 350;

      if (dash) {
        morse += "-";
      } else {
        morse += ".";
      }

      playSignal(dash);
      lastInputTime = millis();
      hasInput = true;
      updateLCD();
    }

    isPressed = false;
  }

  // Cho 2 giay khong bam de giai ma
  if (hasInput && !isPressed &&
      millis() - lastInputTime >= 2000) {

    char letter = decodeMorse(morse);

    if (result.length() >= 10) {
      result = "";
    }

    result += letter;
    morse = "";
    hasInput = false;

    updateLCD();
  }

  lastButtonState = buttonState;
}
