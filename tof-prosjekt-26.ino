#include <SPI.h>
#include <SdFat.h>
#include <RTCZero.h>

#define SD_CS_PIN 4

SdFat sd;
File file;
RTCZero rtc;

char filename[40];
unsigned long lastWrite = 0;
const unsigned long writeInterval = 5000; // 5 seconds

void setup() {
  // Begin serial
  Serial.begin(115200);

  // Init SD card
  Serial.print("Initializing SD... ");
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(12))) {
    Serial.println("Failed to initialize SD");
    sd.initErrorPrint(&Serial);
    while (1);
  }
  Serial.println("Initialized SD");

  // Begin RTC
  rtc.begin();
  Serial.println("Initialized RTC");
  // rtc.setTime(9, 10, 40);
  // rtc.setDate(12, 2, 2026);
  // Serial.println("Set RTC time");


  // Create file
  sprintf(filename,
          "data-%04d-%02d-%02dT%02d-%02d-%02d.csv",
          rtc.getYear() + 2000,
          rtc.getMonth(),
          rtc.getDay(),
          rtc.getHours(),
          rtc.getMinutes(),
          rtc.getSeconds());
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
      file.print(getISO8601Timestamp());
      file.print(",");
      file.print("helloworld");
      file.println();
      file.close();
    } else {
      Serial.println("Failed to open file");
    }
  }
}

String getISO8601Timestamp() {
  char buffer[30];

  sprintf(buffer,
          "%04d-%02d-%02dT%02d:%02d:%02d",
          rtc.getYear() + 2000,
          rtc.getMonth(),
          rtc.getDay(),
          rtc.getHours(),
          rtc.getMinutes(),
          rtc.getSeconds());

  return String(buffer);
}
