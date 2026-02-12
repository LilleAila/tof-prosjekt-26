#include <SPI.h>
#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.print("Initializing SD... ");

  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(12))) {
    Serial.println("Failed to initialize SD");
    sd.initErrorPrint(&Serial);
    while (1);
  }

  Serial.println("Initialized SD");

  file.open("test2.txt", O_WRITE | O_CREAT | O_APPEND);
  file.println("Hello from SdFat!");
  file.close();
}

void loop() {}