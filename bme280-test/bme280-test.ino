// https://randomnerdtutorials.com/bme280-sensor-arduino-pressure-temperature-humidity
/* dependencies:
 * adafruit bme280
 */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Temperature (C),Humidity (%)");

  bool status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}


void loop() {
  Serial.print(bme.readTemperature());
  Serial.print(",");
  Serial.print(bme.readHumidity());
  Serial.println();
  delay(1000);
}
