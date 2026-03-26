#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file; // Data file
File counterFile; // Counter file
File timestampFile; // Timestamp file

char filename[20];

int fileNumber = 0;
unsigned long startupTime = 0;

// Get the file number stored in the counter file. This is used to determine which data file to use.
void getFileNumber() {
  if (sd.exists("counter.txt")) {
    if (counterFile.open("counter.txt", O_READ)) {
      counterFile.fgets(filename, sizeof(filename));
      fileNumber = atoi(filename);
      counterFile.close();
    }
  }
}

// Increment the file number to begin measurements in a new file.
void incrementFile() {
  fileNumber++;
  if (counterFile.open("counter.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    counterFile.println(fileNumber);
    counterFile.close();
  }
  // Instead of implementing restart behavior, we take advantage of the existing code by just restarting the arduino.
  resetNow();
  NVIC_SystemReset();
}

// Update the timestamp stored int eh timestamp file so that it will resume from the correct state after a restart
void updateNow() {
  unsigned long now = millis();
  if (timestampFile.open("timestamp.txt", O_WRITE | O_TRUNC | O_CREAT)) {
    timestampFile.println(now + startupTime);
    timestampFile.close();
  }
}

// Reset the timestamp so that the next measurement starts from 0.
void resetNow() {
  sd.remove("timestamp.txt");
}

// Retrieve the stored timestamp to resume.
void getNow() {
  char startup[20];
  if (sd.exists("timestamp.txt")) {
    if (timestampFile.open("timestamp.txt", O_READ)) {
      timestampFile.fgets(startup, sizeof(startup));
      startupTime = strtoul(startup, NULL, 10);
      timestampFile.close();
    }
  }
}

void initSd() {
  // Init SD card
  Serial.print("Initializing SD... ");
  if (sd.begin(SD_CS_PIN, SD_SCK_MHZ(12))) {
    Serial.println("Initialized SD");
  } else {
    Serial.println("Failed to initialize SD");
    sd.initErrorPrint(&Serial);
    while (1);
  }

  // Create data file
  getFileNumber();
  sprintf(filename, "data_%03d.csv", fileNumber);
  Serial.print("Creating file: ");
  Serial.println(filename);

  // Set the timestamp to start from if it exists
  getNow();

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

  // Open file for further writing.
  if (file.open(filename, O_WRITE | O_APPEND)) {
    Serial.print("Opened file ");
    Serial.println(filename);
  } else {
    Serial.print("Failed to open file ");
    Serial.println(filename);
    while (1);
  };
}
