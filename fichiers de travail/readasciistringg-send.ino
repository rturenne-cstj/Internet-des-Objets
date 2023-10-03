/*
Programme qui envoie une chaines de caractères par le port série vers un arduino
qui utilisera parseInt()

23-9-24 RT V1.0

*/


#include "SHT31.h"

SHT31 sht31 = SHT31();


void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  sht31.begin();
}

void loop() {
  if (Serial.available() > 0) { //on peut aussi envoyer deux int ou long par la console
    String chaine = Serial.readStringUntil('\n');
    Serial.println(chaine);
    Serial1.print(chaine + '\n');
  }

  float temp = sht31.getTemperature();
  float hum = sht31.getHumidity();
  Serial.print("Temp = "); 
  int temperature = (temp * 100);
  int humid = (hum  * 100);
  Serial.print(temp);
  Serial.print(" C   "); //The unit for  Celsius because original arduino don't support special symbols
  Serial.print("Hum = "); 
  Serial.print(hum);
  Serial.println("%"); 
  Serial.print(temperature);
  Serial.print(" , ");
  Serial.println(humid);
  // L'une ou l'autre des méthodes qui suivent fonctionnent
  //Serial1.print((String)temperature + ',' + (String)humid + '\n'); //on peut conténer des strings
  // ou
  Serial1.print('x');
  Serial1.print(temperature);
  Serial1.print(',');
  Serial1.print(humid);
  Serial1.print('\n');
  Serial1.flush(); //Juste pour être certain d'avoir enoyé tous les charactères dans la deuxième méthode1,1#

  delay(1000);
}
