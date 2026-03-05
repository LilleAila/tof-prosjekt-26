#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file;
File counterFile;

char filename[20];

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

void init_sd() {
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
    file.println("Timestamp,Temperature (C),Humidity (%),Sound,Light");
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
}
