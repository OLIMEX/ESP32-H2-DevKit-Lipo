// Olimex ESP32-H2-DevKit-LiPo demo
// LED blinks
// Button toggles LED blinking ON/OFF

#define LED_PIN     8
#define BUTTON_PIN  9

bool blinkingEnabled = true;
bool lastButtonState = HIGH;

unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, HIGH); // LED OFF (active LOW)
}

void loop() {

  // ----- BUTTON -----
  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    blinkingEnabled = !blinkingEnabled;
    delay(50); // debounce
  }

  lastButtonState = buttonState;


  // ----- LED CONTROL -----
  if (blinkingEnabled) {

    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      ledState = !ledState;

      if (ledState)
        digitalWrite(LED_PIN, LOW);   // ON
      else
        digitalWrite(LED_PIN, HIGH);  // OFF
    }

  } else {
    digitalWrite(LED_PIN, HIGH); // force OFF
  }

}
