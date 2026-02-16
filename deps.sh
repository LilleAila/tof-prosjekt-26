# installs globaly

arduino-cli core update-index
arduino-cli core install arduino:samd

arduino-cli lib update-index
arduino-cli lib install "SdFat"
arduino-cli lib install "RTCZero"
arduino-cli lib install "Adafruit BME280 Library"
