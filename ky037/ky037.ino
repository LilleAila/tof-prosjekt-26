#define KY_037_PIN A1

void setup() {
  pinMode(KY_037_PIN, INPUT);

  Serial.begin(115200);
}

void loop() {
  // Serial.println(analogRead(KY_037_PIN));

  int minVal = 1023;
  int maxVal = 0;
  unsigned long start = millis();
  while (millis() - start < 500) {
    int val = analogRead(KY_037_PIN);
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
  }

  // int d = maxVal - minVal;
  Serial.println(maxVal);
}
