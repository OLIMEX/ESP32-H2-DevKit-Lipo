// Olimex ESP32-H2-DevKit-LiPo demo
// LED blinks
// Button toggles LED blinking ON/OFF
// Serial reports button status
// If using USB1 remember to have
// USB CDC On Boot: "Enabled"

#define LED_PIN     8
#define BUTTON_PIN  9

bool blinkingEnabled = true;

bool buttonState;
bool lastButtonReading;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 25;

unsigned long lastBlink = 0;
bool ledState = false;

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, HIGH); // LED OFF (active LOW)

  buttonState = digitalRead(BUTTON_PIN);
  lastButtonReading = buttonState;

  Serial.println("ESP32-H2 Button/LED demo started");
}

void loop() {

  // -------- BUTTON READ --------
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == LOW) {
        Serial.println("Button PRESSED");
        blinkingEnabled = !blinkingEnabled;   // toggle LED blinking
      } else {
        Serial.println("Button RELEASED");
      }

    }
  }

  lastButtonReading = reading;


  // -------- LED BLINK --------
  if (blinkingEnabled) {

    if (millis() - lastBlink > 500) {

      lastBlink = millis();
      ledState = !ledState;

      digitalWrite(LED_PIN, ledState ? LOW : HIGH); // active LOW LED
    }

  } else {

    digitalWrite(LED_PIN, HIGH); // LED OFF
  }

}
