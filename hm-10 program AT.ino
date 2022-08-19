/* programme utilisé pour préparer le module Bluetooth HM-10
* Il est important de connaitre la version du firmware
* il est important de connaitre la vitesse de transmission
* NE PAS CHANGER LE NOM DE VOTRE MODULE *****
* NE PAS CHANGER LE MOT DE PASSE *****

Robert Turenne
22-8-19 Version 1.0
*/

#include "SoftwareSerial.h" //pour créer un port série virtuel

SoftwareSerial hm10(3, 2);  // (Read, Write).
char c = ' ';
boolean NL = true;

void setup() {
  Serial.begin(9600); //pour la console
  hm10.begin(9600); //pour le HM-10
}

void loop() {
  // Lire du module Bluetooth et envoyer sur la console
  if (hm10.available()) {
    c = hm10.read();
    Serial.write(c);
  }


  // Lire de la console et envoyer au Bluetooth
  if (Serial.available()) {
    c = Serial.read();

    // Pas besoin d'enoyer les fins de ligne. Le Bluetooth lit et interprête un charactere à la fois
    if (c != 10 & c != 13) {
      hm10.write(c);
    }

    // Echo the user input to the main window.
    // If there is a new line print the ">" character.
    if (NL) {
      Serial.print("\r\n>");
      NL = false;
    }
    Serial.write(c);
    if (c == 10) { NL = true; }
  }
}

/* Provenant du manufacturier

AT	Test Command or Disconnect Command	If the module is not connected to a remote device it will reply: “OK”
If the module has a connection then the connection will be closed. If the notification setting is active, the module will reply with “OK+LOST”
AT+NAME?	Query the name	Returns the name the module broadcasts such as HMsoft.
AT+NAMEnewname	Change the name of the module	Changes the name broadcast by the module. For example
AT+NAMEmyBTmodule changes the name to myBTmodule. 
The maximum length for a new name is 12 characters.
AT+ADDR?	Queries the HM-10s mac address	Returns the address as a 12 digit hexidecimal number. For example, OK+ADDR:606405D138A3
AT+VERS?
AT+VERR?	Queries the firmware version number	For example: HMSoft V540
AT+RESET	Restarts the module.	Returns OK+RESET 
Will close an active connection while restarting.
AT+RENEW	Restores the default factory settings.	A quick and easy way to reset all settings.
AT+BAUD?	Query the baud rate used for UART serial communication. This is the speed a host device like an Arduino uses to talk to the BT module. It is not the the speed used to send wireless signals between different modules.	Returns the value 0-8, for example, OK+Get:0 
0 – 9600
1 – 19200
2 – 38400
3 – 57600
4 – 115200
5 – 4800
6 – 2400
7 – 1200
8 – 230400
The default setting is 0 – 9600.
Remember that both devices, the Arduino and the HM-10 need to use the same baud rate. Garbage characters are usually a sign of mismatched baud rates.
AT+BAUDx	Set the baud rate used for UART serial communication.	x is a value from 0 to 8. See the above for wwhat value represents which baud rate.
Take care when using with an Arduino. The maximum baud rate the Arduino serial monitor allows is 115200. If you set the baud rate to 230400 with AT+BAUD8 you wont be able to talk to the module.
AT+NOTI	Set the notification status	If notifications are turned on, the HM-10 will reply to commands with a confirmation message or send out a message when certain events take place, like “OK” for the AT command and “OK+LOST” when a connection is broken.
AT+NOTI0 – turn off notifications
AT+NOTI1 – turn on notifications

AT+NOTI?	Query the notification status	Returns either 0 or 1:
0 – notifications are off
1 – notifications are on
AT+PASS?	Query the password used for pairing.	Replies with a 6 digit number like “OK+Get:123456” or whatever the current password is.
AT+PASS	Set a new password.	The password must be 6 characters long.. 
AT+PASS123456 sets the new password to 123456
AT+ROLE?	Query the current Role; Master or Slave	AT+ROLE? returns either 0 or 1.
0 = Slave or Peripheral
1 = Master or Central.
The default setting is 0 (Slave).
AT+ROLEx	Set the device role. x is 0 or 1.	To change to Slave/Peripheral mode use AT+ROLE0. This will return OK+Set:0
To change to Master/Central mode use AT+ROLE1. This will return OK+Set:1
AT+ROLEx mat require a reset before the changes take place.
AT+IMME?	Query the start mode	AT+IMME? returns either 0 or 1.
0 = Connect immediately (assuming a previous connection has been applied
1 = Wait for a connection command before connecting (AT+START, AT+CONN, AT+CONL
The default setting is 0 (connect on start).
AT+IMMEx	Set the start up mode	AT+IMME0 sets auto connect on start*
AT+IMME1 sets manual connection mode
AT+IMMEx is often used together with AT+ROLEx
AT+IMMEx mat require a reset before the changes take place.
AT+RESET	Restarts the module	
AT+RENEW	Resets the module to the factory settings
*/