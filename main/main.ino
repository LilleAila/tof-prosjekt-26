/*
 * BME280 pin connections:
 * GND -> GND
 * 3V3 -> VCC
 * SDA -> 11 / SDA
 * SCL -> 12 / SCL
* */

/*
 * TEMT6000 pin connections:
 * VCC -> VCC
 * GND -> GND
 * OUT -> A1
* */

#include <SdFat.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define TEMT6000_PIN A1
#define BUTTON_PIN 6
#define LED_PIN 4

Adafruit_BME280 bme280;

extern File file;
unsigned long lastWrite = 0;
const unsigned long writeInterval = 3000;
extern unsigned long startupTime;
unsigned long ledStart = 0;

bool lastButton = HIGH;

void setup() {
  // Small delay
  delay(500);

  // Begin serial
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000);
  Serial.println("---------- BOOT ----------");

  init_sd();
  init_imnp441();

  if (bme280.begin()) {
    Serial.println("Initialized BME280");
  } else {
    Serial.println("Failed to init BME280");
    while (1);
  }

  pinMode(TEMT6000_PIN, INPUT);
  Serial.println("Initialized TEMT6000");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  ledStart = millis();
}

void loop() {
  unsigned long now = millis();
  sample_imnp441();

  if (now - ledStart > 3000) {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }

  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButton == HIGH && buttonState == LOW) {
    incrementFile();
  }
  lastButton = buttonState;

  if (now - lastWrite >= writeInterval) {
    lastWrite = now;

    float temp = bme280.readTemperature();
    float humidity = bme280.readHumidity();
    float light = analogRead(TEMT6000_PIN);
    float sound = read_imnp441();

    Serial.print(startupTime + now);
    Serial.print(",");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(humidity);
    Serial.print(",");
    Serial.print(sound);
    Serial.print(",");
    Serial.print(light);
    Serial.println();

    file.print(startupTime + now);
    file.print(",");
    file.print(temp);
    file.print(",");
    file.print(humidity);
    file.print(",");
    file.print(sound);
    file.print(",");
    file.print(light);
    file.println();

    file.flush();
  }
}
