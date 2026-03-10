#include <I2S.h>

/*
 * IMNP441 pin connections:
 * L/R -> GND (GND og VCC to choose between L/R channel. Using left here)
 * WS -> D3 (world select)
 * SCK -> D2 (serial clock)
 * GND -> GND
 * VDD -> VCC (3.3V)
 * SD -> A6 (serial data)
* */

const int MIC_VDD_PIN = 6;


// loop() runs at approx. 16 kHz
const int threshold = 50000;
const float alpha = 0.01;
const float attack = 0.002;
const float release = 0.00005;
const float maxDiff = 10;
const int hold_samples = 1000;

const unsigned long stall_delay_ms = 500;
unsigned long startTime = 0;
unsigned long lastCheck = 0;
int stalls = 0;
float last_sound_smoothed = 0;

float sound_smoothed = 0;
float envelope = 0;
int hold = 0;

void init_imnp441() {
  // Init I2S
  pinMode(MIC_VDD_PIN, OUTPUT);
  digitalWrite(MIC_VDD_PIN, HIGH);
  delay(50);

  start_i2s();
}

void start_i2s() {
  if (I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S ready");
  } else {
    Serial.println("I2S failed");
    while (1);
  }

  startTime = millis();
  lastCheck = startTime;
}

void sample_imnp441() {
  if (I2S.available()) {
    int32_t sample = I2S.read();

    if (sample != 0 && sample != -1) {
      sample >>= 8; // IMNP441 uses 24-bit I2S, arduino reads 32-bit
      float s = abs(sample);
      if (s > threshold) {
        s = threshold;
      }
      float target;
      if (s > envelope) {
        hold = hold_samples;
        target = attack * s + (1 - attack) * envelope;
      }
      else {
        if (hold <= 0) target = release * s + (1 - release) * envelope;
        else target = envelope;
      };
      if (target > envelope + maxDiff) envelope += maxDiff;
      else envelope = target;

      sound_smoothed = alpha * envelope + (1 - alpha) * sound_smoothed;

      if (hold > 0) hold--;
    }
  }

  if (millis() - startTime > stall_delay_ms) {
    if (millis() - lastCheck > 1000) {
      if (abs(sound_smoothed - last_sound_smoothed) < 1) {
        stalls++;
        if (stalls > 10) {
          Serial.println("Stalled! Resetting MCU");
          update_now();
          NVIC_SystemReset();
          stalls = 0;
        }
      } else {
        stalls = 0;
      }
      lastCheck = millis();
      last_sound_smoothed = sound_smoothed;
    }
  }
}

float read_imnp441() {
  return sound_smoothed;
}
