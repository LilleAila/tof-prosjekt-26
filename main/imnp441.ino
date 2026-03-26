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


// Define various parameters used in the envelope
// This is a method used to clean up the incoming microphone samples
// , remove noise and generally improve the readings by taking a rolling envelope
// which we then sample every 3000ms.
// loop() runs at approx. 16 kHz, for which we can use these parameters:
const int threshold = 50000;
const float alpha = 0.01;
const float attack = 0.002;
const float release = 0.00005;
const float maxDiff = 10;
const int holdSamples = 1000;

// Stall parameters. This is used to detect when the microphone stops working and safely restart the arduino to keep going
const unsigned long stallDelayMs = 500;
unsigned long startTime = 0;
unsigned long lastCheck = 0;
int stalls = 0;
float lastSoundSmoothed = 0;

// These are the resulting values from the computations.
float soundSmoothed = 0;
float envelope = 0;
int hold = 0;

void initImnp441() {
  // Initialize the pins used
  pinMode(MIC_VDD_PIN, OUTPUT);
  digitalWrite(MIC_VDD_PIN, HIGH);
  delay(50);

  startI2s();
}

void startI2s() {
  // Start I2S, which is used to communicate with the microphone.
  if (I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S ready");
  } else {
    Serial.println("I2S failed");
    while (1);
  }

  startTime = millis();
  lastCheck = startTime;
}

void sampleImnp441() {
  // Sample the microphone. This is run every single loop and updates the rolling envelope.

  if (I2S.available()) {
    // Only read if the sensor has samples in the buffer.
    int32_t sample = I2S.read();

    if (sample != 0 && sample != -1) {
      sample >>= 8; // IMNP441 uses 24-bit I2S, arduino reads 32-bit, so we discard the first 8
      float s = abs(sample); // The sample can be negative, so we use the absolute value
      if (s > threshold) { // Clamp all samples to a define threshold. This reduces the impact of sudden loud noises to the dataset
        s = threshold;
      }

      // Compute the value for the envelope.
      float target;
      if (s > envelope) {
        hold = holdSamples;
        target = attack * s + (1 - attack) * envelope;
      }
      else {
        if (hold <= 0) target = release * s + (1 - release) * envelope;
        else target = envelope;
      };
      if (target > envelope + maxDiff) envelope += maxDiff;
      else envelope = target;

      soundSmoothed = alpha * envelope + (1 - alpha) * soundSmoothed;

      if (hold > 0) hold--;
    }
  }

  // Check whether the sensor has stalled. If so, we will save the current progress and safely restart the arduino to keep going when the sensor is working again.
  if (millis() - startTime > stallDelayMs) {
    if (millis() - lastCheck > 1000) {
      if (abs(soundSmoothed - lastSoundSmoothed) < 1) {
        stalls++;
        if (stalls > 10) {
          Serial.println("Stalled! Resetting MCU");
          updateNow();
          NVIC_SystemReset();
          stalls = 0;
        }
      } else {
        stalls = 0;
      }
      lastCheck = millis();
      lastSoundSmoothed = soundSmoothed;
    }
  }
}

float readImnp441() {
  // Sample the envelope. This is what is written to the CSV file.
  return soundSmoothed;
}
