/*
Mega Passthrough
22-9-18 RT V1.0

le HM-10 (BLE) est connecté au Serial1
Serial3 sur les PIN 19-18

le Arduino de la maison est connecté au Serial2
Serial2 sur les PIN 16-17
*/

char charMaison = 0;
char charConsole = 0;
char charHM10 = 0;


void setup() {


  Serial.begin(9600);   // Console
  Serial2.begin(9600);  // Maison
  Serial1.begin(9600);  // HM-10
}

void loop() {

  if (Serial2.available()) {
    charMaison = Serial2.read();
    Serial.print("de la maison: ");
    Serial.println(charMaison);
  }
  if (Serial.available()) {
    charConsole = Serial.read();
    Serial.print("de la console: ");
    Serial.println(charConsole);
    Serial2.print(charConsole);
  }
  if (Serial1.available()) {
    charHM10 = Serial1.read();
    Serial.print("de Bluetooth: ");
    Serial.println(charHM10);
    Serial2.print(charHM10);
  }
}