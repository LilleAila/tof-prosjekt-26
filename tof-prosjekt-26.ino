#include <SPI.h>
#include <SdFat.h>

#define SD_CS_PIN 4

SdFat sd;
File file;
File counterFile;

char filename[20];
unsigned long lastWrite = 0;
const unsigned long writeInterval = 5000;

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

  // Init SD card
  Serial.print("Initializing SD... ");
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(12))) {
    Serial.println("Failed to initialize SD");
    sd.initErrorPrint(&Serial);
    while (1);
  }
  Serial.println("Initialized SD");

  // Create file
  sprintf(filename, "data_%03d.csv", nextFileNumber());
  Serial.print("Creating file: ");
  Serial.println(filename);

  // Write CSV header
  if (file.open(filename, O_WRITE | O_CREAT)) {
    file.println("Timestamp,SomeValue");
    file.close();
    Serial.println("Created file and wrote header");
  } else {
    Serial.println("Failed to create file");
  }
}

void loop() {
  const int now = millis();
  if (now - lastWrite >= writeInterval) {
    lastWrite = now;

    if (file.open(filename, O_WRITE | O_APPEND)) {
      file.print(now);
      file.print(",");
      file.print("helloworld");
      file.println();
      file.close();
    } else {
      Serial.println("Failed to open file");
    }
  }
}
