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

// Define the pins used
#define TEMT6000_PIN A1
#define BUTTON_PIN 6
#define LED_PIN 4

Adafruit_BME280 bme280;

extern File file; // The .csv file to log to
unsigned long lastWrite = 0; // When the file was last written to
const unsigned long writeInterval = 3000; // How often to write to the file
extern unsigned long startupTime; // The timestamp from when the arduino was shut down, so that it can continue properly
unsigned long ledStart = 0; // When the LED was lit

bool lastButton = HIGH;

void setup() {
  // Small delay to allow the compmuter to catch up
  delay(500);

  // Initialize serial
  Serial.begin(115200);
  unsigned long start = millis();
  // Try to start serial. If it fails after 3 seconds, ignore it and keep measuring
  // this allows us to test when connected to a computer, without having to change the code
  while (!Serial && millis() - start < 3000);
  Serial.println("---------- BOOT ----------");

  // Initialize SD card and microphone
  initSd();
  initImnp441();

  // Initialize temperature and humidity sensor
  if (bme280.begin()) {
    Serial.println("Initialized BME280");
  } else {
    Serial.println("Failed to init BME280");
    while (1);
  }

  // Initialize light sensor
  pinMode(TEMT6000_PIN, INPUT);
  Serial.println("Initialized TEMT6000");

  // Initialize LED ans button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  ledStart = millis();
}

void loop() {
  unsigned long now = millis();
  sampleImnp441(); // Sample the microphone every iteration. See imnp441.ino for more details

  // Blink the LED to indicate that the arduino has successfully restarted
  if (now - ledStart > 3000) {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }

  // Check the button state and increment the file number if pressed
  // This is a hack to get around the problems with having to restart
  // the arduino sometimes when the microphone stops working
  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButton == HIGH && buttonState == LOW) {
    incrementFile();
  }
  lastButton = buttonState;

  // Limit the number of writes, as it is unnecessary to write every single loop.
  if (now - lastWrite >= writeInterval) {
    lastWrite = now;

    // Sample all the values we want to keep track of
    float temp = bme280.readTemperature();
    float humidity = bme280.readHumidity();
    float light = analogRead(TEMT6000_PIN);
    float sound = readImnp441();

    // Useful for debugging when connected
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

    // Print the data to file
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
