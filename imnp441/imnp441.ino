/*
 * IMNP441 pin connections:
 * L/R -> GND (GND og VCC to choose between L/R channel. Using left here)
 * WS -> D3 (world select)
 * SCK -> D2 (serial clock)
 * GND -> GND
 * VDD -> VCC (3.3V)
 * SD -> A6 (serial data)
* */

#include <I2S.h>

const int sample_duration = 300;

// Sample rate of approx. 16 kHz
const int threshold = 50000;
const float alpha = 0.01;
const float attack = 0.002;
const float release = 0.00005;
const float maxDiff = 10;

float smoothed = 0;
float envelope = 0;
int hold = 0;
const int hold_samples = 1000;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial initialized");

  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S failed");
    while (1);
  }

  Serial.println("I2S ready");
}

void loop() {
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

    smoothed = alpha * envelope + (1 - alpha) * smoothed;

    if (hold > 0) hold--;

    if (millis() - lastPrint > sample_duration) {
      Serial.println(smoothed);
      lastPrint = millis();
    }
  }
}
