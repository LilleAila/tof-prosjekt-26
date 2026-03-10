#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file;
File counterFile;

char filename[20];

int fileNumber = 0;

int getFileNumber() {
  if (sd.exists("counter.txt")) {
    if (counterFile.open("counter.txt", O_READ)) {
      counterFile.fgets(filename, sizeof(filename));
      fileNumber = atoi(filename);
      counterFile.close();
    }
  }
}

void incrementFile() {
  num++;
  if (counterFile.open("counter.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    counterFile.println(num);
    counterFile.close();
  }
  NVIC_SystemReset();
}

File init_sd() {
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
  getFileNumber();
  sprintf(filename, "data_%03d.csv", fileNumber);
  Serial.print("Creating file: ");
  Serial.println(filename);

  // Write CSV header
  if (!sd.exists(filename)) {
    if (file.open(filename, O_WRITE | O_CREAT)) {
      file.println("Timestamp,Temperature (C),Humidity (%),Sound,Light");
      file.close();
      Serial.println("Created file and wrote header");
    } else {
      Serial.println("Failed to create file");
      while (1);
    }
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
