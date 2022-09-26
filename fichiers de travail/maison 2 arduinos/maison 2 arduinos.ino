//call the relevant library file
#include <Servo.h>
//#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
//Set the communication address of I2C to 0x27, display 16 characters every line, two lines in total
LiquidCrystal_I2C mylcd(0x27, 16, 2);

//On utilise un autre port série pour séparer les communications
SoftwareSerial mega(3, 2);  //RX,TX

//set ports of two servos to digital 9 and 10
Servo servo_10;  //Servo pour la fenetre
Servo servo_9;

int btn1_num;      //set variable btn1_num
int btn2_num;      //set variable btn2_num
int button1;       //set variable button1
int button2;       //set variable button2
String fans_char;  //string type variable fans_char
int fans_val;      //set variable fans_char
int flag;          //set variable flag
int flag2;         //set variable flag2
int flag3;         //set variable flag3
int gas;           //set variable gas
int infrar;        //set variable infrar
String led2;       //string type variable led2
int light;         //set variable light
String pass;       //string type variable pass
String passwd;     //string type variable passwd

String servo1;     //string type variable servo1
int servo1_angle;  //set variable light
String servo2;     //string type variable servo2
int servo2_angle;  //set variable servo2_angle

int soil;  //set variable soil
int val;   //set variable val
int valchar;
String commande;  //Quel type de commande
int commandeVal;  //Valeur du parametre pour les commandes PWM
int value_led2;   //set variable value_led2
int water;        //set variable water

int tonepin = 30;                //set the signal end of passive buzzer to digital 3
const int pluiePin = A3;         //Attention: A désigne un pin analogue
const int fenetrePin = 10;       //Pour le servo de la fenetre
const int fenetreOuverte = 180;  //degrés pour placer la fenetre en position ouverte
const int fenetreFermee = 60;    //degtrés pour placer la fenetre en position fermée
bool fenetreEtat = 0;            //Fenetre si ouverte = 1
const int gasPin = A0;
const int lightPin = A1;
const int mouvPin = 240;
const int DELExtPin = 31;  //DEL placée à l'extérieur
const int button1Pin = 4;  //bouton jaune de gauche (marqué 3)
const int button2Pin = 8;  //bouton jaune de droite (marqué 4)



void setup() {
  Serial.begin(9600);  //set baud rate to 9600
  mega.begin(9600);

  mylcd.init();
  mylcd.backlight();  //initialize LCD
  //LCD shows "passcord:" at first row and column
  mylcd.setCursor(1 - 1, 1 - 1);
  mylcd.print("passcord:");

  servo_9.attach(9);               //make servo connect to digital 9
  servo_10.attach(fenetrePin);     //make servo connect to digital 10
  servo_9.write(0);                //set servo connected digital 9 to 0°
  servo_10.write(fenetreOuverte);  //set servo connected digital 10 to 0°
  fenetreEtat = 1;
  delay(300);

  pinMode(7, OUTPUT);     //set digital 7 to output
  pinMode(6, OUTPUT);     //set digital 6 to output
  digitalWrite(7, HIGH);  //set digital 7 to high level
  digitalWrite(6, HIGH);  //set digital 6 to high level

  pinMode(4, INPUT);  //set digital 4 to input
  pinMode(8, INPUT);  //set digital 8 to input
  //pinMode(2, INPUT);    //set digital 2 to input
  //pinMode(3, OUTPUT);   //set digital 3 to output
  pinMode(A0, INPUT);   //set A0 to input
  pinMode(A1, INPUT);   //set A1 to input
  pinMode(13, OUTPUT);  //set digital 13 to input
  pinMode(A3, INPUT);   //set A3 to input
  pinMode(A2, INPUT);   //set A2 to input

  pinMode(12, OUTPUT);  //set digital 12 to output
  pinMode(5, OUTPUT);   //set digital 5 to output
  //pinMode(3, OUTPUT);   //set digital 3 to output
  //mega.print(F("Maison OK "));
  Serial.println(F("Maison OK "));
}

void loop() {
  //analyser les données provenant des senseurs
  //Pas besoin d'attendre une entrée de commande
  auto_sensor();

  //Données reçues du Mega ou de la console

  if (mega.available() > 0)  //serial reads the characters
  {
    valchar = mega.read();
    Serial.print("de MEGA ");
    //Traitement du premier char pour le CASE plus bas
    //output val character in new lines
    Serial.print("Valeur recue: ");
    Serial.write(valchar);
    mylcd.print("commande:");
    //pwm_control();
    val = valchar;
    Serial.print(" numerique ");
    Serial.println(val);
    mylcd.clear();
    mylcd.setCursor(0, 0);
    //mylcd.print("commande: ");
    //mylcd.print(val);
  }
  //Données reçues directementt sur la console
  if (Serial.available() > 0) {
    valchar = Serial.read();
    Serial.print("de console ");
    mega.write(valchar);
    //Traitement du premier char pour le CASE plus bas
    //output val character in new lines
    Serial.print("Valeur recue: ");
    Serial.write(valchar);
    //pwm_control();
    val = valchar;
    Serial.print(" numerique ");
    Serial.println(val);
    mylcd.clear();
    mylcd.setCursor(0, 0);
    mylcd.print("commande: ");
    mylcd.print(String(val));
  }


  switch (val) {
    case 'a':  //if val is character 'a'，program will circulate
      commande = "LED ON";
      digitalWrite(13, HIGH);  //set digital 13 to high level，LED 	lights up
      break;                   //exit loop
    case 'b':                  //if val is character 'b'，program will circulate
      commande = "LED OFF";
      digitalWrite(13, LOW);  //Set digital 13 to low level, LED is off
      break;                  //exit loop
    case 'c':                 //if val is character 'c'，program will circulate
      commande = "RELAY ON";
      digitalWrite(12, HIGH);  //set digital 12 to high level，NO of relay is connected to COM
      break;                   //exit loop
    case 'd':                  //if val is character 'd'，program will circulate
      commande = "RELAY OFF";
      digitalWrite(12, LOW);  //set digital 12 to low level，NO of relay is disconnected to COM
      break;                  //exit loop
    case 'e':                 //if val is character 'e'，program will circulate
      //music1();
      mega.write('e');  //play birthday song
      break;            //exit loop
    case 'f':           //if val is character 'f'，program will circulate
      //music2();
      mega.write('f');  //play ode to joy song
      break;            //exit loop
    case 'g':           //if val is character 'g'，program will circulate
      //noTone(3);
      mega.write('g');        //set digital 3 to stop playing music
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
      Serial.println(water);  //output the value of variable water in new lines
      delay(100);
      break;               //exit loop
    case 'l':              //if val is character 'l'，program will circulate
      servo_9.write(180);  //set servo connected to digital 9 to 180°
      delay(500);
      break;   //exit loop
    case 'm':  //if val is character 'm'，program will circulate
      servo_9.write(0);
      ;  //set servo connected to digital 9 to 0°
      delay(500);
      break;                //exit loop
    case 'n':               //if val is character 'n'，program will circulate
      servo_10.write(180);  //set servo connected to digital 10 to 180°
      delay(500);
      break;              //exit loop
    case 'o':             //if val is character 'o'，program will circulate
      servo_10.write(0);  //set servo connected to digital 10 to 0°
      delay(500);
      break;   //exit loop
    case 'p':  //if val is character 'p'，program will circulate
      commande = "LED ext ON";
      digitalWrite(5, HIGH);  //set digital 5 to high level, LED is on
      break;                  //exit loop
    case 'q':                 //if val is character 'q'，program will circulate
      commande = "LED ext OFF";
      digitalWrite(5, LOW);  // set digital 5 to low level, LED is off
      break;                 //exit loop
    case 'r':                //if val is character 'r'，program will circulate
      commande = "Fan Max";
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);  //fan rotates anticlockwise at the fastest speed
      break;                  //exit loop
    case 's':                 //if val is character 's'，program will circulate
      commande = "Fan Off";
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);  //fan stops rotating
      break;                 //exit loop
    default:
      pwm_control();  //plus efficace ici!
  }
  mylcd.setCursor(0, 1);
  mylcd.print(String(commande));
  if (commandeVal != -1) {
    mylcd.print(commandeVal);
    commandeVal = -1;
  }
  //remettre val à 0 après l'avoir traité pour éviter de répéter
  val = 0;
}


//PWM control
void pwm_control() {
  switch (val) {
    case 't':  //if val is 't'，program will circulate
      servo1 = Serial.readStringUntil('#');
      servo1_angle = String(servo1).toInt();
      servo_9.write(servo1_angle);  //set the angle of servo connected to digital 9 to servo1_angle
      delay(300);
      break;   //exit loop
    case 'u':  //if val is 'u'，program will circulate
      servo2 = Serial.readStringUntil('#');
      servo2_angle = String(servo2).toInt();
      servo_10.write(servo2_angle);  //set the angle of servo connected to digital 10 to servo2_angle
      delay(300);
      break;   //exit loop
    case 'v':  //if val is 'v'，program will circulate
      led2 = Serial.readStringUntil('#');
      value_led2 = String(led2).toInt();
      analogWrite(5, value_led2);  //PWM value of digital 5 is value_led2
      break;                       //exit loop
    case 'w':                      //if val is 'w'，program will circulate
      if (mega.available() > 0) {
        Serial.print("BLE ");
        fans_char = mega.readStringUntil('#');
      } else if (Serial.available() > 0) {
        Serial.print("Serial ");
        fans_char = Serial.readStringUntil('#');
      }
      commandeVal = -1;
      Serial.print("fans_char ");
      Serial.print(fans_char);
      fans_val = String(fans_char).toInt();
      Serial.print(" fans_val ");
      Serial.println(fans_val);
      digitalWrite(7, LOW);
      analogWrite(6, fans_val);  //set PWM value of digital 6 to fans_val，the larger the value, the faster the fan
      commande = "FAN ON VALUE ";
      commandeVal = fans_val;
      break;  //exit loop
    default:
      break;
  }
}

void auto_sensor() {

  //detect gas
  gas = analogRead(gasPin);  //assign the analog value of A0 to gas
  if (gas > 700) {
    //if variable gas>700
    flag = 1;  //set variable flag to 1
    while (flag == 1)
    //if flag is 1, program will circulate
    {
      Serial.println("danger");  //output "danger" in new lines

      gas = analogRead(A0);  //gas analog the value of A0 to gas
      if (gas < 100)         //if variable gas is less than 100
      {
        flag = 0;  //set variable flag to 0
        break;     //exit loop exist to loop
      }
    }

  } else
  //otherwise
  {
    //noTone(3);  // digital 3 stops playing music
  }

  //Est-ce qu'il fait noir?
  light = analogRead(lightPin);  ////Assign the analog value of A1 to light
  if (light < 300)               //plutot noir
  {
    //Est-ce qu'il y a du mouvement (détecté par le détecteur de mouvement infrarouge)
    //infrar = digitalRead(mouvPin);  //assign the value of digital 2 to infrar
    //Serial.println(infrar);
    //if (infrar)  //mouvement et il fait noir
    //{
    digitalWrite(DELExtPin, HIGH);  //Allumer la LED externe
    //}
  } else {
    digitalWrite(DELExtPin, LOW);  //Étiendre la lumière de toute façon s'il fait assez clair
  }

  //Y'a t'il de la pluie?
  water = analogRead(pluiePin);    //assign the analog value of A3 to variable water
  if (water > 800 && fenetreEtat)  //
  {

    Serial.println("rain");  //output "rain" in new lines
    mega.write("rain");

    servo_10.write(fenetreFermee);  // set the servo connected to digital 10 to 180°
    fenetreEtat = 0;                //Indique que la fenetre est fermee

  } else  //Otherwise
  {
    if (val != 'u' && val != 'n')  //Communication Série par le BLE
    {
      if (water < 300 && !fenetreEtat)  //
      {
        servo_10.write(fenetreOuverte);  //set servo connected to digital 10 to 0°
        fenetreEtat = 1;                 //Indique que la fenetre est ouverte
      }
    }
  }

  soil = analogRead(A2);  //assign the analog value of A2 to variable soil
  if (soil > 50)
  // if variable soil is greater than 50
  {
    flag3 = 1;  //set flag3 to 1
    while (flag3 == 1)
    //If set flag3 to 1, program will circulate
    {
      Serial.println("hydropenia ");  //output "hydropenia " in new lines

      soil = analogRead(A2);  //Assign the analog value of A2 to variable soil
      if (soil < 10)          //If variable soil<10
      {
        flag3 = 0;  //set flag3 to 0
        break;      //exit loop
      }
    }

  } else  //Otherwise
  {
    // noTone(3);  //set digital 3 to stop playing music
  }
  //door();  //run subroutine
}