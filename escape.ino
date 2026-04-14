#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#define MPU_ADDR 0x68

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo1, servo2, servo3;

// PINS
int servo1Pin = 18;
int servo2Pin = 19;
int servo3Pin = 23;

int ledPin = 2;
int buzzerPin = 15;
int resetPin = 4;

int reed1 = 16; // correct
int reed2 = 17; // wrong
int reed3 = 5;  // wrong

// GAME VARIABLES
unsigned long startTime;
bool gameActive = true;
int gameStage = 1;

String correctCode = "1234#";
String enteredCode = "";

bool screenUpdated = false;

// SCORE
int score = 0;

// MPU VARIABLES
int16_t ax, ay, az;

// KEYPAD
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD RESET
void resetLCD() {
  lcd.clear();
  delay(50);
  lcd.setCursor(0, 0);
}

// RESET GAME
void resetGame() {
  resetLCD();
  lcd.print("GAME 1");

  enteredCode = "";
  score = 0;

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);

  startTime = millis();
  gameActive = true;
  gameStage = 1;
  screenUpdated = false;
}

// READ MPU
void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();

  delay(20);
}

// SETUP
void setup() {
  Wire.begin(21, 22);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  lcd.init();
  lcd.backlight();
  delay(200);
  resetLCD();

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP);

  pinMode(reed1, INPUT_PULLUP);
  pinMode(reed2, INPUT_PULLUP);
  pinMode(reed3, INPUT_PULLUP);

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);

  resetGame();
}

// LOOP
void loop() {

  if (digitalRead(resetPin) == LOW) {
    delay(200);
    resetGame();
  }

  if (!gameActive) return;

  // ===== DYNAMIC TIMER =====
  int limit;
  if (gameStage == 1) limit = 50;
  else if (gameStage == 2) limit = 30;
  else limit = 20;

  unsigned long elapsed = (millis() - startTime) / 1000;
  int remaining = limit - elapsed;
  if (remaining < 0) remaining = 0;

  // DISPLAY SCORE + TIMER
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(score);
  lcd.print("   ");

  lcd.setCursor(10, 0);
  lcd.print("T:");
  lcd.print(remaining);
  lcd.print(" ");

  // TIMEOUT
  if (remaining <= 0) {
    resetLCD();
    lcd.print("TIME OVER");

    lcd.setCursor(0, 1);
    lcd.print("Final:");
    lcd.print(score);

    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);

    delay(3000);
    gameActive = false;
    return;
  }

  // ===== GAME 1 =====
  if (gameStage == 1) {

    char key = keypad.getKey();

    if (key) {
      enteredCode += key;

      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(enteredCode);
    }

    if (enteredCode.endsWith("#")) {

      int timeTaken = (millis() - startTime) / 1000;

      if (enteredCode == correctCode) {

        if (timeTaken <= 25) score += 10;

        resetLCD();
        lcd.print("GAME 2");

        digitalWrite(ledPin, HIGH);
        servo1.write(90);
        delay(700);
        digitalWrite(ledPin, LOW);

        enteredCode = "";
        gameStage = 2;
        startTime = millis();
        screenUpdated = false;
      }
      else {
        score -= 5;

        resetLCD();
        lcd.print("WRONG");

        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(buzzerPin, LOW);

        enteredCode = "";
      }
    }
  }

  // ===== GAME 2 =====
  else if (gameStage == 2) {

    if (!screenUpdated) {
      resetLCD();
      lcd.print("FIND MAGNET");
      screenUpdated = true;
    }

    if (digitalRead(reed1) == LOW) {

      int timeTaken = (millis() - startTime) / 1000;
      if (timeTaken <= 15) score += 10;

      resetLCD();
      lcd.print("GAME 3");

      digitalWrite(ledPin, HIGH);
      servo2.write(90);
      delay(700);
      digitalWrite(ledPin, LOW);

      gameStage = 3;
      startTime = millis();
      screenUpdated = false;
    }

    if (digitalRead(reed2) == LOW || digitalRead(reed3) == LOW) {
      score -= 5;

      resetLCD();
      lcd.print("WRONG");

      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
    }
  }

  // ===== GAME 3 =====
  else if (gameStage == 3) {

    if (!screenUpdated) {
      resetLCD();
      lcd.print("TILT DEVICE");
      screenUpdated = true;
    }

    readMPU();

    if (abs(ax) > 15000 || abs(ay) > 15000) {

      int timeTaken = (millis() - startTime) / 1000;
      if (timeTaken <= 10) score += 10;

      resetLCD();
      lcd.print("YOU WON!");

      lcd.setCursor(0, 1);
      lcd.print("Final:");
      lcd.print(score);

      digitalWrite(ledPin, HIGH);
      servo3.write(90);
      delay(700);

      delay(3000);
      gameActive = false;
    }
  }
}