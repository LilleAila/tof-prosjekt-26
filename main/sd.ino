#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file;
File counterFile;
File timestampFile;

char filename[20];

int fileNumber = 0;
unsigned long startupTime = 0;

void getFileNumber() {
  if (sd.exists("counter.txt")) {
    if (counterFile.open("counter.txt", O_READ)) {
      counterFile.fgets(filename, sizeof(filename));
      fileNumber = atoi(filename);
      counterFile.close();
    }
  }
}

// NOTE: this part of the code has not been tested yet!
// We need to add a physical button to call this function, otherwise it never gets called
void incrementFile() {
  fileNumber++;
  if (counterFile.open("counter.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    counterFile.println(fileNumber);
    counterFile.close();
  }
  reset_now();
  NVIC_SystemReset();
}

void update_now() {
  unsigned long now = millis();
  if (timestampFile.open("timestamp.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    timestampFile.println(now + startupTime);
    timestampFile.close();
  }
}

void reset_now() {
  sd.remove("timestamp.txt");
}

void get_now() {
  char startup[20];
  if (sd.exists("timestamp.txt")) {
    if (timestampFile.open("timestamp.txt", O_READ)) {
      timestampFile.fgets(startup, sizeof(startup));
      startupTime = strtoul(startup, NULL, 10);
      timestampFile.close();
    }
  }
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
  getFileNumber();
  sprintf(filename, "data_%03d.csv", fileNumber);
  Serial.print("Creating file: ");
  Serial.println(filename);

  get_now();

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
