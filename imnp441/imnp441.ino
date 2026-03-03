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

const int sample_duration = 500;

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
  int start = millis();

  long long sum = 0;
  int count = 0;

  while (millis() <= start + sample_duration) {
    int32_t sample = I2S.read();

    if (sample != 0 && sample != -1) {
      sample >>= 14;
      sum += abs(sample);
      count++;
    }
  }

  if (count > 0) {
    float avg = (float)sum / count;
    Serial.println(avg);
  }
}
