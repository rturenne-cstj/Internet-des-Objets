/*
Mega Passthrough avec SHT31 pour la temperature
23-10-2 RT V2.0

le HM-10 (BLE) est connecté au
Serial2 sur les PIN 16-17

le Arduino de la maison est connecté
Serial1 sur les PIN 18-19

Le programme envoie une chaine de caractères par le port série vers un ESP32
qui utilisera parseInt()
23-9-24 RT V1.0
*/

//#define DEBUG

#include "SHT31.h"

SHT31 sht31 = SHT31();

char charMaison = 0;
char charConsole = 0;
char charHM10 = 0;


void setup() {


  Serial.begin(9600);   // Console
  Serial1.begin(9600);  // Maison
  Serial2.begin(9600);  // HM-10
  sht31.begin();
}

void loop() {

  if (Serial1.available()) {
    charMaison = Serial2.read();
    Serial.print("de la maison: ");
    Serial.println(charMaison);
  }
  if (Serial.available()) {
    charConsole = Serial.read();
    Serial.print("de la console: ");
    Serial.println(charConsole);
    Serial2.print(charConsole);
    Serial1.print(charConsole);
  }
  if (Serial2.available()) {
    charHM10 = Serial2.read();
    Serial.print("de Bluetooth: ");
    Serial.println(charHM10);
    Serial1.print(charHM10);
  }
  if (millis() % 10000 == 0) {  //à chaque seconde
    float temp = sht31.getTemperature();
    float hum = sht31.getHumidity();
    int temperature = (temp * 100);
    int humid = (hum * 100);
#ifdef DEBUG
    Serial.print("Temp = ");
    Serial.print(temp);
    Serial.print(" C   ");  //The unit for  Celsius because original arduino don't support special symbols
    Serial.print("Hum = ");
    Serial.print(hum);
    Serial.println("%");
//Serial.print(temperature);
//Serial.print(" , ");
//Serial.println(humid);
#endif
    // L'une ou l'autre des méthodes qui suivent fonctionnent
    Serial2.print('x'+ (String)temperature + ',' + (String)humid + '\n'); //on peut concaténer des strings
    // ou
    // Serial2.print('x');
    // Serial2.print(temperature);
    // Serial2.print(',');
    // Serial2.print(humid);
    // Serial2.print('\n');
    // Serial2.flush();  //Juste pour être certain d'avoir enoyé tous les charactères dans la deuxième méthode1,1#
    delay(1);
  }
}