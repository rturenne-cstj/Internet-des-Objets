/*
* Programme de dgestion de la maison connectée
* cours 420-5C4 A22


Robert Turenne
* 22-8-22 Version 1.0
* 22-9-2 Version 1.1
*/
// Charger les librairires requises
#include <Servo.h>              // Gestion des servo-moteurs
#include <Wire.h>               // Gestion de la connection I2C
#include <LiquidCrystal_I2C.h>  // Gestion de l'écran LCD avec I2C
//Adresse de communication avec l'écran I2C est 0x27,  16 characteres par line, 2 lignes in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);  //definir l'instance I2C

//Créer les instances
Servo servoPorte;    //porte
Servo servoFenetre;  //Fenêtre

int bouton1Val = 0;  //set variable bouton1Val
int bouton2Val = 0;
int btn2_num = 0;          //set variable btn2_num
int boutton1 = 0;          //set variable o
int boutonDroiteEtat = 0;  //set variable boutonDroiteEtat
int boutonGaucheEtat = 0;
String fans_char;      //string type variable fans_char
int fans_val;          //set variable fans_char

int pluieFlag = 0;  //set variable flag2
int flag3;          //set variable flag3
int gas = 0;        //set variable gas
String led2;        //string type variable led2
String pass;        //string type variable pass
String passwd;      //string type variable passwd

String servo1;     //string type variable servo1
int servo1_angle;  //set variable light
String servo2;     //string type variable servo2
int servo2_angle;  //set variable servo2_angle

int soil;        //set variable soil
int val;         //set variable val
int value_led2;  //set variable value_led2


int longueurDeLaToune = 0;  //Avec valeur initiale
const int tonePin = 3;      //set the signal end of passive buzzer to digital 3
const int servoFenetrePin = 9;
const int servoPortePin = 10;
const int gazPin = A0;  // pin pour le détecteur de Gaz carbonique et autres
const int lumierePin = A1;
const int gazMax = 700;    //valeur nécessaire pour déclancher l'alarme de gas
const int gazMin = 100;    //valeur nécessaire pour arrêter l'alarme de gas
int gazFlag = 0;           //indicateur de présence de gaz
const int lightMin = 300;  // Probablement le soir?
int lumiereFlag = 0;
int light = 0;                   //valeur de la lumiere
const int infraPin = 2;          // Pin du capteur d'infrarouges
int infrarEtat = 0;              // Etat du detecteur de mouvement
const int lumierePortePin = 13;  // fun fact : la LED du Arduino UNO est aussi connecter a la pin 13

// Vincent :)
const int pluiePin = A3;
int pluie = 0;  // Detecte la quantite d'eau sur le toit
const int pluieMax = 800;
const int pluieMin = 30;


//JS :3
const int fleurPin = A2;
const int fleurMin = 50;
const int fleurMax = 10;
int humiditeFleur = 0;
int fleurFlag = 0;

//Oli
const int boutonGauchePin = 4;
const int boutonDroitPin = 8;


void setup() {
  Serial.begin(9600);  //Activer la console

  mylcd.init();       //Initialiser l'écran LCD
  mylcd.backlight();  //initialize LCD
  //LCD shows "password:" at first row and column
  mylcd.setCursor(1 - 1, 1 - 1);  //*************todo
  mylcd.print("password:");

  servoFenetre.attach(servoFenetrePin);  //make servo connect to
  servoPorte.attach(servoPortePin);      //make servo connect to
  servoFenetre.write(0);                 //set servo fenetre à 0°    //****************todo
  servoPorte.write(0);                   //set servo connected digital 10 to 0°
  delay(300);                            // *********************************a tester

  pinMode(7, OUTPUT);     //set digital 7 to output
  pinMode(6, OUTPUT);     //set digital 6 to output
  digitalWrite(7, HIGH);  //set digital 7 to high level
  digitalWrite(6, HIGH);  //set digital 6 to high level

  pinMode(4, INPUT);           //set digital 4 to input
  pinMode(8, INPUT);           //set digital 8 to input
  pinMode(2, INPUT);           //set digital 2 to input
  pinMode(3, OUTPUT);          //set digital 3 to output
  pinMode(gazPin, INPUT);      //set A0 to input
  pinMode(lumierePin, INPUT);  //set A1 to input
  pinMode(13, OUTPUT);         //set digital 13 to input
  pinMode(pluiePin, INPUT);    //set A3 to input
  pinMode(A2, INPUT);          //set A2 to input

  pinMode(12, OUTPUT);  //set digital 12 to output
  pinMode(5, OUTPUT);   //set digital 5 to output
  pinMode(3, OUTPUT);   //set digital 3 to output
                        // longueurDeLaToune = sizeof(tune) / sizeof(tune[0]);  //Avec valeur initiale

  Serial.println(F("Maison Corrigée - V1.0 - Fin du Setup"));
}

void loop() {
  auto_sensor();
  if (Serial.available() > 0)  //serial reads the characters
  {
    val = Serial.read();  //set val to character read by serial    Serial.println(val);//output val character in new lines
    pwm_control();
  }
  switch (val) {
    case 'a':                  //if val is character 'a'，program will circulate
      digitalWrite(13, HIGH);  //set digital 13 to high level，LED 	lights up
      break;                   //exit loop
    case 'b':                  //if val is character 'b'，program will circulate
      digitalWrite(13, LOW);   //Set digital 13 to low level, LED is off
      break;                   //exit loop
    case 'c':                  //if val is character 'c'，program will circulate
      digitalWrite(12, HIGH);  //set digital 12 to high level，NO of relay is connected to COM
      break;                   //exit loop
    case 'd':                  //if val is character 'd'，program will circulate
      digitalWrite(12, LOW);   //set digital 12 to low level，NO of relay is disconnected to COM

      break;                  //exit loop
    case 'e':                 //if val is character 'e'，program will circulate
      //music1();               //play birthday song
      break;                  //exit loop
    case 'f':                 //if val is character 'f'，program will circulate
      //music2();               //play ode to joy song
      break;                  //exit loop
    case 'g':                 //if val is character 'g'，program will circulate
      noTone(3);              //set digital 3 to stop playing music
      break;                  //exit loop
    case 'h':                 //if val is character 'h'，program will circulate
      Serial.println(light);  //output the value of variable light in new lines
      delay(100);
      break;                //exit loop
    case 'i':               //if val is character 'i'，program will circulate
      Serial.println(gas);  //output the value of variable gas in new lines
      delay(100);
      break;                 //exit loop
    case 'j':                //if val is character 'j'，program will circulate
      Serial.println(soil);  //output the value of variable soil in new lines
      delay(100);
      break;                  //exit loop
    case 'k':                 //if val is character 'k'，program will circulate
      Serial.println(pluie);  //output the value of variable pluie in new lines
      delay(100);
      break;                    //exit loop
    case 'l':                   //if val is character 'l'，program will circulate
      servoFenetre.write(180);  //set servo connected to digital 9 to 180°
      delay(500);
      break;   //exit loop
    case 'm':  //if val is character 'm'，program will circulate
      servoFenetre.write(0);
      ;  //set servo connected to digital 9 to 0°
      delay(500);
      break;                  //exit loop
    case 'n':                 //if val is character 'n'，program will circulate
      servoPorte.write(180);  //set servo connected to digital 10 to 180°
      delay(500);
      break;                //exit loop
    case 'o':               //if val is character 'o'，program will circulate
      servoPorte.write(0);  //set servo connected to digital 10 to 0°
      delay(500);
      break;                  //exit loop
    case 'p':                 //if val is character 'p'，program will circulate
      digitalWrite(5, HIGH);  //set digital 5 to high level, LED is on
      break;                  //exit loop
    case 'q':                 //if val is character 'q'，program will circulate
      digitalWrite(5, LOW);   // set digital 5 to low level, LED is off
      break;                  //exit loop
    case 'r':                 //if val is character 'r'，program will circulate
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);  //fan rotates anticlockwise at the fastest speed
      break;                  //exit loop
    case 's':                 //if val is character 's'，program will circulate
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);  //fan stops rotating
      break;                 //exit loop
  }
}



//detect gas
void auto_sensor() {
  gas = analogRead(gazPin);  //assign the analog value of A0 to gas
  if (gas > gazMax) {

    gazFlag = 1;  //indicateur de présence de gaz
    // while (flag == 1)
    // //if flag is 1, program will circulate
    // {
    //   Serial.println("danger");  //output "danger" in new lines
    //   tone(3, 440);
    //   delay(125);
    //   delay(100);
    //   noTone(3);
    //   delay(100);
    //   tone(3, 440);
    //   delay(125);
    //   delay(100);
    //   noTone(3);
    //   delay(300);
    //   gas = analogRead(A0);  //gas analog the value of A0 to gas
  } else if (gas < gazMin) {
    gazFlag = 0;  //set variable flag to 0
    //  break;     //exit loop exist to loop
  }


  // {
  //   noTone(3);  // digital 3 stops playing music
  // }
  light = analogRead(lumierePin);  // Lecture de la lumiere (dehors)
  if (light < lightMin)            // SI y fait NOIR (dehors)
  {
    lumiereFlag = 1;
    infrarEtat = digitalRead(infraPin);  //assign the value of digital 2 to infrar
    Serial.println(infrarEtat);          //output the value of variable infrar in new lines
    // TODO : Le mettre quelque part d'autre
    if (infrarEtat == 1) {
      digitalWrite(lumierePortePin, HIGH);  //set digital 13 to high level, LED is on
    } else                                  //Otherwise
    {
      digitalWrite(lumierePortePin, LOW);  //set digital 13 to low level, LED is off
    }
  } else {
    lumiereFlag = 0;
  }

  pluie = analogRead(pluiePin);  //assign the analog value of A3 to variable pluie
  if (pluie > pluieMax)
  // if variable pluie is larger than 800
  {
    pluieFlag = 1;  //if variable flag 2 to 1
    // while (pluieFlag == 1) // A déplacer AVEC le traitement des alarmes !
    // // if flag2 is 1, program will circulate
    // {
    Serial.println("SAUVE QUI PLEUT");  //output "rain" in new lines
    servoPorte.write(180);              // set the servo connected to digital 10 to 180°
    pluie = analogRead(pluiePin);
    // }
  } else if (pluie < pluieMin)  // if variable pluie is less than 30
  {
    pluieFlag = 0;  // set flag2 to 0
  }


  //{
  // Á étudier ***********
  //if (val != 'u' && val != 'n')
  //if val is not equivalent 'u' either 'n'
  //{
  servoPorte.write(0);  //set servo connected to digital 10 to 0°
                        //delay(10);
  //}
  //}



  humiditeFleur = analogRead(fleurPin);  //assign the analog value of A2 to variable soil
  if (humiditeFleur > fleurMin)          // if variable soil is greater than 50
  {
    fleurFlag = 1;  //set flag3 to 1
    //If set flag3 to 1, program will circulate
    {
      // Serial.println("hydropenia");  //output "hydropenia " in new lines
      // tone(tonePin, 440);
      // delay(125);
      // delay(100);
      // noTone(tonePin);
      // delay(100);
      // tone(tonePin, 440);
      // delay(125);
      // delay(100);
      // noTone(tonePin);  //digital 3 stops playing sound
      // delay(300);
      ;
    }
  } else if (humiditeFleur < fleurMax)  //If variable soil<10
  {
    fleurFlag = 0;  //set flag3 to 0
  }

  door();  //run subroutine todo changer l'appel de la fonction
}

void door() {
  boutonGaucheEtat = digitalRead(boutonGauchePin);  // assign the value of digital 4 to o
  boutonDroiteEtat = digitalRead(boutonDroitPin);  //assign the value of digital 8 to boutonDroiteEtat

  // # Gestion password # Á determiner
  if (boutonGaucheEtat == 0)  //if variableboutonGaucheEtat is 0
  {
    delay(10);                     //delay in 10ms
    while (boutonGaucheEtat == 0)  //if variableboutonGaucheEtat is 0，program will circulate
    {
      boutonGaucheEtat = digitalRead(boutonGauchePin);  // assign the value of digital 4 to o
      bouton1Val = bouton1Val + 1;                      //variable bouton1Val plus 1
      delay(100);                                       // delay in 100ms
    }
  }
  if (bouton1Val >= 1 && bouton1Val < 5)  //1≤if variablebtn1_num<5
  {
    Serial.print(".");
    Serial.print("");
    passwd = String(passwd) + String(".");  //set passwd
    pass = String(pass) + String(".");      //set pass
    //LCD shows pass at the first row and column
    mylcd.setCursor(0, 1);
    mylcd.print(pass);
  }
  if (bouton1Val >= 5)
  //if variablebtn1_num ≥5
  {
    Serial.print("-");
    passwd = String(passwd) + String("-");  //Set passwd
    pass = String(pass) + String("-");      //set pass
    //LCD shows pass at the first row and column
    mylcd.setCursor(1 - 1, 2 - 1);
    mylcd.print(pass);
  }
  if (boutonDroiteEtat == 0)  //if variableboutonDroiteEtat is 0
  {
    delay(10);
    if (boutonDroiteEtat == 0)  //if variableboutonDroiteEtat is 0
    {
      if (passwd == ".--.-.")  //if passwd is ".--.-."
      {
        mylcd.clear();  //clear LCD screen
        //LCD shows "open!" at first character on second row
        mylcd.setCursor(1 - 1, 2 - 1);
        mylcd.print("open!");
        servoFenetre.write(100);  //set servo connected to digital 9 to 100°
        delay(300);
        delay(5000);
        passwd = "";
        pass = "";
        mylcd.clear();  //clear LCD screen
        //LCD shows "password:"at first character on first row
        mylcd.setCursor(1 - 1, 1 - 1);
        mylcd.print("password:");

      } else  //Otherwise
      {
        mylcd.clear();  //clear LCD screen
        //LCD shows "error!"at first character on first row
        mylcd.setCursor(1 - 1, 1 - 1);
        mylcd.print("error!");
        passwd = "";
        pass = "";
        delay(2000);
        //LCD shows "again" at first character on first row
        mylcd.setCursor(1 - 1, 1 - 1);
        mylcd.print("again");
      }
    }
  }
  infrarEtat = digitalRead(infraPin);  //assign the value of digital 2 to infrar
  if (infrarEtat == 0 && (val != 'l' && val != 't'))
  //if variable infrar is 0 and val is not 'l' either 't'
  {
    servoFenetre.write(0);  //set servo connected to digital 9 to 0°
    delay(50);
  }
  if (boutonDroiteEtat == 0)  //if variableboutonDroiteEtat is 0
  {
    delay(10);
    while (boutonDroiteEtat == 0)  //if variableboutonDroiteEtat is 0，program will circulate
    {
      boutonDroiteEtat = digitalRead(boutonDroitPin);  //assign the value of digital 8 to boutonDroiteEtat
      btn2_num = btn2_num + 1;                         //variable btn2_num plus 1
      delay(100);
      if (btn2_num >= 15)  //if variablebtn2_num ≥15
      {
        tone(3, 532);
        delay(125);
        mylcd.clear();  //clear LCD screen
        //LCD shows "password:" at the first character on first row
        mylcd.setCursor(0, 0);
        mylcd.print("password:");
        //LCD shows "wait" at the first character on first row
        mylcd.setCursor(0, 0);
        mylcd.print("wait");
      } else  //Otherwise
      {
        noTone(3);  //digital 3 stops playing music
      }
    }
  }
  // bouton1Val = 0;  //set bouton1Val to 0
  // btn2_num = 0;    //set btn2_num to 0
}

// // Birthday song
// void music1() {
//   birthday();
// }
// //Ode to joy
// void music2() {
//   Ode_to_Joy();
// }
// void Ode_to_Joy()  //play Ode to joy song
// {
//   for (int x = 0; x < longueurDeLaToune =0; //Avec valeur initiale x++) 
//   {
//     tone(tonePin, tune[x]);
//     delay(300 * durt[x]);
// }
// }

//PWM control
void pwm_control() {
  switch (val) {
    case 't':  //if val is 't'，program will circulate
      servo1 = Serial.readStringUntil('#');
      servo1_angle = String(servo1).toInt();
      servoFenetre.write(servo1_angle);  //set the angle of servo connected to digital 9 to servo1_angle
      delay(300);
      break;   //exit loop
    case 'u':  //if val is 'u'，program will circulate
      servo2 = Serial.readStringUntil('#');
      servo2_angle = String(servo2).toInt();
      servoPorte.write(servo2_angle);  //set the angle of servo connected to digital 10 to servo2_angle
      delay(300);
      break;   //exit loop
    case 'v':  //if val is 'v'，program will circulate
      led2 = Serial.readStringUntil('#');
      value_led2 = String(led2).toInt();
      analogWrite(5, value_led2);  //PWM value of digital 5 is value_led2
      break;                       //exit loop
    case 'w':                      //if val is 'w'，program will circulate
      fans_char = Serial.readStringUntil('#');
      fans_val = String(fans_char).toInt();
      digitalWrite(7, LOW);
      analogWrite(6, fans_val);  //set PWM value of digital 6 to fans_val，the larger the value, the faster the fan
      break;                     //exit loop
  }
}