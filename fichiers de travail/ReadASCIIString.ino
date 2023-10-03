

/*
  Reading a serial ASCII-encoded string.

  This sketch demonstrates the Serial parseInt() function.
  It looks for an ASCII string of comma-separated values.
 
  created 13 Apr 2012
  by Tom Igoe
  modified 14 Mar 2016
  by Arturo Guadalupi
  modifié 23-9-24
  par Robert Turenne

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ReadASCIIString
*/
#include "SoftwareSerial.h"

SoftwareSerial mega(2, 3);  //Rx,Tx


void setup() {
  // initialize serial:
  Serial.begin(9600);
  mega.begin(9600);
}

void loop() {

  if (mega.available() > 0) {
    int commande = mega.read(); //On pourrait utiliser un Serial.peek() sans délais
    if (commande == 'r') {
      //Serial.print(mega.available());
      //Serial.print(' ');
      delay(5); //délais nécessaire si on utilise la deuxième méthode (plusieurs print) pour envoyer les donnés

      while (mega.available() > 0) {
        // trouver un INT dans le stream dans le buffer du port série
        // Parse int laisse tomber ce qui précède le INT, et les délimiteeurs.
        long temperature = mega.parseInt();
        // Encore
        long humid = mega.parseInt();
        
        long response = 0;

        // look for the newline. That's the end of your sentence:
        if (mega.read() == '\n') {
         
          Serial.print(temperature);
          Serial.print(' ');
          Serial.print(humid);
          Serial.print(" Réponse: ");
          float temp = temperature / 100.0;
          float hum = humid / 100.0;
          Serial.print(temp);
          Serial.print(" ");
          Serial.println(hum);
        }
      }
    } else {
      while (mega.available() > 0) { //Commande inconnue: on vide le buffer, mais on risque de perdre des commandes
        mega.read();
      }
    }
  }
}
