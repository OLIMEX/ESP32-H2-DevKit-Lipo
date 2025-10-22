// ==========================================================
// ESP32-H2-DevKit-LiPo Battery and Power Sense Demo
// ----------------------------------------------------------
// First make sure jumpers BAT_SENS_E1 and 
// PWR_SENS_E1 are closed (pads soldered together)!
//
// In Tools -> Board select ESP32H2 Dev Module and Enable CDC on boot!
//
// External Power Sense: GPIO25
// Battery Measurement:  GPIO2
// User Button (BUT1):   GPIO9
// LED:                  GPIO8
//
// Resistor divider coefficient = (R5 + R1) / R5 = 5.7
// Maximum battery voltage = 4.2V
//
// ➤ Press BUT1 for 2–6 seconds → enter ADC calibration mode
// ➤ Press BUT1 for >6 seconds → reset stored calibration factor
// ➤ Measure the battery with a multimeter *while still connected*
//    (at battery contacts on the bottom of the board)
// ➤ LED blinks fast in calibration mode and flashes 3× on success
// ➤ LED flashes 5× quickly on calibration reset
// ==========================================================

#include <Preferences.h>

#define POWER_SENSE_PIN   25
#define BATTERY_PIN       2
#define BUTTON_PIN        9
#define LED_PIN           8

// Divider ratio (R5+R1)/R5 = 5.7
float resistorCoeff = 5.7;

// ADC calibration coefficient (stored in flash)
Preferences prefs;
float calibrationFactor = 1.0;

// LED control
unsigned long lastBlink = 0;
bool ledState = false;

// Calibration mode flag
bool calibrating = false;

// Button press tracking
unsigned long buttonPressStart = 0;
bool buttonPressed = false;

// ----------------------------------------------------------
// Setup
// ----------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(POWER_SENSE_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  prefs.begin("adc-calib", false);
  calibrationFactor = prefs.getFloat("factor", 1.0);

  Serial.println("\nESP32-H2-DevKit-LiPo Power & Battery Sense");
  Serial.println("------------------------------------------------");
  Serial.print("Resistor divider coefficient: ");
  Serial.println(resistorCoeff);
  Serial.print("Calibration factor: ");
  Serial.print(calibrationFactor);
  if (prefs.isKey("factor"))
    Serial.println("  (loaded from flash)");
  else
    Serial.println("  (using default)");
  Serial.println("------------------------------------------------\n");
}

// ----------------------------------------------------------
// Main loop
// ----------------------------------------------------------
void loop() {
  unsigned long now = millis();

  // LED blinking pattern
  unsigned long interval = calibrating ? 150 : 800;
  if (now - lastBlink > interval) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    lastBlink = now;
  }

  // Button press detection
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressStart = now;
      buttonPressed = true;
    }
  } else {
    if (buttonPressed) {
      unsigned long pressDuration = now - buttonPressStart;

      if (pressDuration >= 2000 && pressDuration < 6000) {
        // Enter calibration mode
        startCalibration();
      } else if (pressDuration >= 6000) {
        // Reset calibration
        calibrationFactor = 1.0;
        prefs.clear();
        Serial.println("\n*** Calibration data reset! Using default = 1.0 ***");

        // Flash LED 5× to confirm reset
        for (int i = 0; i < 5; i++) {
          digitalWrite(LED_PIN, HIGH);
          delay(150);
          digitalWrite(LED_PIN, LOW);
          delay(150);
        }
      }

      buttonPressed = false;
    }
  }

  // Regular measurement mode
  if (!calibrating) {
    Serial.print("External Power Sense: ");
    Serial.println(digitalRead(POWER_SENSE_PIN));

    int mv = analogReadMilliVolts(BATTERY_PIN);
    float actualMv = mv * resistorCoeff * calibrationFactor;

    Serial.print("Battery Voltage: ");
    Serial.print(actualMv);
    Serial.println(" mV\n");
    delay(500);
  }
}

// ----------------------------------------------------------
// Calibration mode
// ----------------------------------------------------------
void startCalibration() {
  calibrating = true;
  Serial.println("\n=== ADC Calibration Mode ===");
  Serial.println("Measure the battery with a multimeter (while connected).");
  Serial.println("Enter the measured voltage in millivolts (e.g., 4120):");

  while (true) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      float measuredMv = input.toFloat();

      if (measuredMv > 0) {
        int rawMv = analogReadMilliVolts(BATTERY_PIN) * resistorCoeff;
        calibrationFactor = measuredMv / rawMv;

        prefs.putFloat("factor", calibrationFactor);
        Serial.print("Calibration complete! Factor saved: ");
        Serial.println(calibrationFactor);

        // Flash LED 3× to confirm success
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED_PIN, HIGH);
          delay(150);
          digitalWrite(LED_PIN, LOW);
          delay(150);
        }

        calibrating = false;
        Serial.println("Exiting calibration mode.\n");
        break;
      } else {
        Serial.println("Invalid input. Please enter numeric millivolts (e.g., 4120):");
      }
    }
  }
}
