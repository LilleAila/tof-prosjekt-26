/*
 * IMNP441 pin connections:
 * L/R -> GND (GND og VCC to choose between L/R channel. Using left here)
 * WS -> D3 (world select)
 * SCK -> D2 (serial clock)
 * GND -> GND
 * VDD -> VCC (3.3V)
 * SD -> A6 (serial data)
* */

/*
 * BME280 pin connections:
 * GND -> GND
 * 3V3 -> VCC
 * SDA -> 11 / SDA
 * SCL -> 12 / SCL
* */

#include <SPI.h>
#include <SdFat.h>
#include <I2S.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SD_CS_PIN 4

Adafruit_BME280 bme280;

SdFat sd;
File file;
File counterFile;

char filename[20];
unsigned long lastWrite = 0;
const unsigned long writeInterval = 3000;

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

int nextFileNumber() {
  int num = 0;

  if (sd.exists("counter.txt")) {
    if (counterFile.open("counter.txt", O_READ)) {
      counterFile.fgets(filename, sizeof(filename));
      num = atoi(filename);
      counterFile.close();
    }
  }

  num++;

  if (counterFile.open("counter.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    counterFile.println(num);
    counterFile.close();
  }

  return num;
}

void setup() {
  // Small delay
  delay(500);

  // Begin serial
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000);

  // Init I2S
  if (I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S ready");
  } else {
    Serial.println("I2S failed");
    while (1);
  }

  // Init SD card
  Serial.print("Initializing SD... ");
  if (sd.begin(SD_CS_PIN, SD_SCK_MHZ(12))) {
    Serial.println("Initialized SD");
  } else {
    Serial.println("Failed to initialize SD");
    sd.initErrorPrint(&Serial);
    while (1);
  }

  // Create file
  sprintf(filename, "data_%03d.csv", nextFileNumber());
  Serial.print("Creating file: ");
  Serial.println(filename);

  // Write CSV header
  if (file.open(filename, O_WRITE | O_CREAT)) {
    file.println("Timestamp,Temperature (C),Humidity (%),Sound");
    file.close();
    Serial.println("Created file and wrote header");
  } else {
    Serial.println("Failed to create file");
    while (1);
  }

  // Open file
  if (file.open(filename, O_WRITE | O_APPEND)) {
    Serial.print("Opened file ");
    Serial.println(filename);
  } else {
    Serial.print("Failed to open file ");
    Serial.println(filename);
    while (1);
  };

  if (bme280.begin()) {
    Serial.println("Initialized BME280");
  } else {
    Serial.println("Failed to init BME280");
    while (1);
  }
}

void loop() {
  unsigned long now = millis();

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

  if (now - lastWrite >= writeInterval) {
    lastWrite = now;

    float temp = bme280.readTemperature();
    float humidity = bme280.readHumidity();

    file.print(now);
    file.print(",");
    file.print(temp);
    file.print(",");
    file.print(humidity);
    file.print(",");
    file.println(sound_smoothed);
    file.println();

    file.flush();
  }
}
