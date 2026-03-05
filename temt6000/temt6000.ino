#define LIGHTSENSORPIN A1

void setup() {
  pinMode(LIGHTSENSORPIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  float reading = analogRead(LIGHTSENSORPIN);
  float square_ratio = reading / 1023.0; //Get percent of maximum value (1023)
  square_ratio = pow(square_ratio, 2.0);
  Serial.println(reading);
  delay(1000);
}
