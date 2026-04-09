# Tof prosjekt vår 2026 - rapport

Denne repositoryen inneholder koden og notater tilknyttet prosjektet vårt i faget Teknologi og Forskningslære våren 2026. I denne filen finner du rapporten vår fra forsøket.

## Hypotese

## Kravspesifikasjoner

Vi skal lage et produkt som kan samle inn empirisk data rundt støynivået og klimaet i et klasserom. Hovedfokuset er at den skal gi en verdi for temperatur, lysstyrke og støy der det skal være mulig å se relative forskjeller i verdiene. Disse skal behandles på arduinoen, og samples hvert 3. sekund for å skrive til en CSV-fil. I skal vi ha en knapp på arduinoen for å starte en ny måling. Det vil si at hvis arduinoen mister strøm eller må startes på nytt underveis i en innsamling, vil den fortsette å skrive til det samme datasettet til vi trykker på knappen for at den skal lage en ny fil.

## Komponenter

Her er en liste over komponentene som kreves for å bygge én arduino for innsamlingen:

- Arduino MKR NB 1500
- Sd-kort (formatert til FAT32) + MKR MEM Shield
- TEMT6000 lys-sensor
- BME280 temperatur og luftfuktighet
- IMNP441 mikrofon
- 5V mobillader + micro-usb kabel

### Bibliotaker

- `SdFat` for å interagere med SD-kortet
- `Adafruit BME280 Library` for kompatibilitet med temperatursensoren
