#include <SPI.h>
#include <SD.h>

const int sdPin = 4;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.print("Initializing SD...");
  if (!SD.begin(sdPin)) {
    Serial.println("SD card init failed");
    while (1);
  }
  Serial.println("SD initialized");

  File dataFile = SD.open("test.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Hello, world!");
    dataFile.flush();
    dataFile.close();
  }
}

void loop() {}