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


// loop() runs at approx. 16 kHz
const int threshold = 50000;
const float alpha = 0.01;
const float attack = 0.002;
const float release = 0.00005;
const float maxDiff = 10;
const int hold_samples = 1000;

float sound_smoothed = 0;
float envelope = 0;
int hold = 0;

float lastValue = -1;
int freezeCounter = 0;
const int freezeThreshold = 5000;

void init_imnp441() {
  // Init I2S
  if (I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S ready");
  } else {
    Serial.println("I2S failed");
    while (1);
  }

}

void sample_imnp441() {
  int maxSamples = 32;
  while (I2S.available() && maxSamples--) {
    int32_t sample = I2S.read();
    if (sample == -1) return;
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

  if (abs(sound_smoothed - lastValue) < 0.001) {
    freezeCounter++;
  } else {
    freezeCounter = 0;
  }
  lastValue = sound_smoothed;

  if (freezeCounter > freezeThreshold) {
    I2S.end();
    delay(150);
    if (I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
      Serial.println("I2S restarted successfully");
      freezeCounter = 0;
      lastValue = -1;
    } else {
      Serial.println("I2S restart failed");
    };
  }
}



float read_imnp441() {
  return sound_smoothed;
}
