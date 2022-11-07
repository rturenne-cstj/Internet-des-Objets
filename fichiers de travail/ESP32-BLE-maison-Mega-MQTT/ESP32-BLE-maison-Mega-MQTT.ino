/**
 * client BLE
 Connecté à la maison KEYSTUDIO
 par un HM-10 sur un Arduino Mega
 avec un ESP32 - écran OLED
 
 Robert Turenne
 2022-10-16 Version 2.0 Nettoyage du code. BLE complet
 2022-11-6  Version 3.0 Ajouter MQTT
 */

#include "BLEDevice.h"      //pour le Bluetooth
#include "heltec.h"         //pour le display OLED
#include "EspMQTTClient.h"  //Pour MQTT vers Nodered ou autre

// Attention à la version!!!!!
const char* version = "BLE ESP32-HM10 V3.0";
char* displayMessage = "                    ";
std::string newValue;             //utilisé pour le display OLED du ESP32
const char* nomBLE = "ESP32-00";  //Votre identifiant UNIQUE correspond à votre numéro du HM-10 BLE et par conséquent votre ESP32
String noderedPayload;
bool noderedReceived = false;

//Identification du serveur MQTT
//Notre accès MQTT passe par le WIFI
EspMQTTClient client(
  "rtls",
  "leborddulac",
  "192.168.2.172",  // Adresse IP du serveur (broker) MQTT
  //"MQTTUsername",   // Pas obligatoire
  //"MQTTPassword",   // Pas obligatoire
  nomBLE,  // Votre nom de client UNIQUE
  1883     // Le port MQTT, default to 1883. Ligne pas obligatoire
);


// Le firmware du MH-10 défini les services et charactéristiques BLE
// Le service REMOTE qui nous intéresse
static BLEUUID serviceUUID("FFE0");
// La charactéristique REMOTE qui nous intéresse
static BLEUUID charUUID("FFE1");
// Notre HM-10 a un nom unique dans notre environnement
// Ça sert à nous assurer que nous avons le bon HM-10
std::string myDeviceName = "00-HM10-IDO";  //ÉCRIRE ICI le nom de votre HM-10

static boolean doConnect = false;
static boolean connected = false;
static boolean disconnected = true;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


// Callback losqu'on reçoit du data sur BLE du ESP32 (dans notre cas initié par le HM-10)
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.println(F("notifyCallback- Notification de rappel -callback- du ESP32 "));

  //Serial.print(F("Notify callback for characteristic "));
  //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(F(" -longueur de data "));
  Serial.print(length);
  Serial.print(F(" -data: "));
  Serial.println((char*)pData);
  newValue = (char*)pData;
  displayMessage = "Data reçue de BlueTooth";
  //Publier la charactéristique sur  MQTT
  client.publish(nomBLE + String("/CB_maison"), (char*)pData);
}

//Automatique lors du connect ou disconnect BLE
class MyClientCallback : public BLEClientCallbacks {

  void onConnect(BLEClient* pclient) {
    disconnected = false;
    Serial.println(F("MyClientCallback"));
    Serial.println(F("  - onConnect() -Connecté au serveur"));
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    disconnected = true;
    Serial.println(F("MyClientCallback"));
    Serial.println(F("  - onDisconnect() -DÉCONNECTÉ du serveur"));
  }
};  //fin class MyClientCallback (le ; est important pour une classe)

// Fonction complète pour se connecter au serveur (ici HM-10)
// avec validations
bool connectToServer() {
  Serial.println(F("connectToServer"));
  Serial.print(F(" - Initiation d'une Connexion vers "));
  Serial.print(myDevice->getAddress().toString().c_str());
  //RT
  Serial.print(F(" qui a le nom "));
  Serial.println(myDevice->getName().c_str());
  // RT - Validation: Est-ce le bon nom pour notre HM-10?
  if (myDevice->getName() != myDeviceName) {
    Serial.println(F("********** MAUVAIS NOM *********"));
    delay(1000);
    return false;
  }

  Serial.println(F(" - Création de mon client"));
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(F(" - Client Créé"));

  // Préparer les callBacks pour ce client
  pClient->setClientCallbacks(new MyClientCallback());
  //Va faire un appel à MyClientCallback en créant l'instance
  Serial.print(F(" -Appel de "));

  // Connecter au client REMOTE
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of
  // address, it will be recognized type of peer device address (public or
  // private) RT-Ben oui??!

  if (disconnected == false) {
    Serial.println(F(" - Maintenant Connecté au serveur"));
  } else {
    Serial.println(F("***** NON connecté su serveur *****"));
    return false;
  }

  // Obtenir la référence au service. On avait déjà le service
  // Obtenu lors du scan (setup) mais on vvérifie
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print(F("Incapable de trouver le servcice associé au UUID: "));
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();  //va provoquer un tentative de reConnexion
    return false;
  }
  Serial.print(F(" - Service trouvé: "));
  Serial.println(serviceUUID.toString().c_str());

  // Obtenir la référence à la charactéristique du service du serveur REMOTE BLE
  // On a besoin du service avant de pouvoir chercher la charctéristique
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print(F("Incapable de trouver la charactéristique associée au UUID: "));
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();  //va provoquer un tentative de reConnexion
    return false;
  }
  Serial.print(F(" - Charactéristique trouvée: "));
  Serial.println(charUUID.toString().c_str());

  // Est-ce que la charactéristique est lisible?
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    //Serial.print(F(" - La valeur de la charactéristique est: "));
    //Serial.println(value.c_str());
  }
  // Est-ce qu'on peut modifier (notify) la charactéristique?
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println(F(" - La charactéristique peut être ré-écrite"));
  } else {
    Serial.println(F(" - La characteristique NE PEUT PAS être ré-écrite --ERREUR"));
    return false;
  }
  connected = true;  //On est vraiment sur le bon serveur!
  displayMessage = "Connecté au serveur";
}  //connecToServer



/**
 * Scan pour les serveur BLE et trouver le premier qui a le service que nous désirons accéder
 RT- On vérifie aussi le nom du module HM-10. Ici on sauve du temps
    lors du scan. Malheureusement, la charactéristique n'est pas
    disponible à ce niveau.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Appelé pour chacun des serveur BLE trouvé
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print(F("Objet BLE trouvé: "));
    // On imprime son contenu
    Serial.println(advertisedDevice.toString().c_str());

    // On a trouvé un objet BLE. Regardons le service qu'il publie
    // pour savoir si c'est le bon. Ensuite, est-ce qu'il publie un nom?
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID) /*RT*/ && advertisedDevice.haveName()) {

      myDevice = new BLEAdvertisedDevice(advertisedDevice);  //Connexion au
      //nouveau serveur
      // ==>On doit se connecter pour avoir le nom
      Serial.println(myDevice->getName().c_str());
      // RT - Est-ce le bon nom pour notre HM-10?
      if (myDevice->getName() != myDeviceName) {
        Serial.println(F("********** MAUVAIS NOM *********"));
        delay(1000);
        ;
      } else {
        doConnect = true;
        Serial.println(F("Objet a un service compatible avec le bon nom"));
        displayMessage = "Serveur compatible trouvé";
        BLEDevice::getScan()->stop();  // IMPORTANT: On arrête le scan (lancé dans le setup)
        Serial.println(F("Fin du Scan"));
      }  // Trouvé notre Serveur
    }    // Vérifier résultats du scan
  }      // Regarder les objets trouvés
};       // Callback des BLE publiés

// MQTT: cette fonction est appelée dès que la connection WIFI et MQTT est complétée
// Dans notre cas, on s'abonne à une TOPIC et on affiche un message sur la console pour indiquer que la connection est complétée
// WARNING : IMPLANTATION OBLIGATOIRE si on utilise le client MQTT
void onConnectionEstablished() {
  // S'abonner à la TOPIC esp32/commande et afficher tout message reçu sur la console
  client.subscribe(nomBLE + String("/commande"), [](const String& payload) {
    Serial.print("De node-red ");
    Serial.println(payload);
    client.publish(nomBLE + String("/echo"), payload);
    noderedPayload = payload;
    noderedReceived = true;
  });

  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  //client.subscribe("mytopic/wildcardtest/#", [](const String& topic, const String& payload) {
  //  Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  //});

  // Publier un message pour indiquer que la connection est complétée
  client.publish(nomBLE + String("/init"), "MQTT: connection prête!");  // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  //client.executeDelayed(5 * 1000, []() {
  //  client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  //});
}


void setup() {
  Serial.begin(115200);  //ATTENTION à la vitesse de la console
  Serial.println(F("Début de l'application BLE..."));
  BLEDevice::init("");
  // Pour initialiser l'écran et la console du ESP32
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  //Le display du ESP32 a 80 X 120 pixels
  delay(1000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, version);
  Heltec.display->display();  //Toujours faire un display

  // Déclancher un scanner et instancier le callback pour être informé lorsque
  // on détecte un nouveau device (BLE). Spécifier que le mode de scan doit être
  // "Active Scanning" et débuter le scan
  // On continue le scan jusqu'à ce qu'on trouve notre serveur BLE
  // Il peut y avoir beaucoup d'obets BLE...
  displayMessage = "Scanning";
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  // Parametrres optionels pour espMQTT client:
  client.enableDebuggingMessages(); // Enable debugging (sur la console)

  Serial.println(F("Fin du SETUP"));
}  // Fin du setup.


void loop() {

  //MQTT - Obligatoire
  client.loop();

  Heltec.display->clear();
  Heltec.display->drawString(0, 0, version);  //Attention à la version
  Heltec.display->drawString(0, 12, "Connecté à: ");
  Heltec.display->drawString(59, 12, myDevice->getName().c_str());
  Heltec.display->drawString(0, 24, "Dernière Commande: ");
  Heltec.display->drawString(100, 24, newValue.c_str());
  Heltec.display->drawString(0, 36, displayMessage);
  Heltec.display->drawString(0, 48, "Adresse IP: ");
  Heltec.display->drawString(55, 48, WiFi.localIP().toString());
  Heltec.display->display();

  // Si le flag "doConnect" est VRAI, alors on a trouvé le serveur BLE avec lequel on
  // voulait connecter. Maintenant on se connecte au serveur
  if (doConnect == true) {
    // On démarre la connecttion en appeelant la fonction et on teste le résultat
    if (connectToServer()) {
      Serial.print(F("Connexion réussie au serveur BLE: "));
      Serial.println(myDevice->getName().c_str());
      Heltec.display->drawString(0, 12, "Connecté à: ");
      Heltec.display->drawString(59, 12, myDevice->getName().c_str());
      Heltec.display->display();

    } else {
      Serial.println(F("La Connexion au serveur BLE n'a pas fonctionné"));
      Heltec.display->clear();
      Heltec.display->drawString(0, 12, "Pas connecté à BLE");
      Heltec.display->display();
    }
    doConnect = false;
  }

  // Si la Connexion à un serveur BLE est OK, update de la Charactéristique
  //  à chaque fois qu'on nous contacte (par BLE)
  if (connected) {
    //Les données peuvent provenir du port série à travers la console
    if (Serial.available())  //RT
    {
      Serial.print("Premier chacractère reçu: ");
      char firstChar = Serial.read();  //lire le premier charactere pour préparer le CASE
      Serial.print(firstChar);
      Serial.print(" ");
      //std::string newValue = std::string(String(firstChar).c_str());
      newValue = std::string(String(firstChar).c_str());

      //Serial.print(newValue.c_str());  //pourrait utiliser s.toCharArray(newValue, sizeof(newValue));
      std::string numValue = "";
      char digit;
      int value;
      //Certaines commandes sont accompagnées de nombres (0-255) et on doit les
      //joindre à la valeur de la caractérique avant de l'envoyer
      switch (firstChar) {
        case 't':
        case 'u':
        case 'v':
        case 'w':
          do {
            digit = Serial.read();
            Serial.print(" In the While: Digit: ");
            Serial.print(digit);
            Serial.print(" : ");
            if (digit != 35) {              //35 = '#'
              numValue = numValue + digit;  //numValue est une string
              Serial.print(" numValue: ");
              Serial.print(numValue.c_str());
            }
          } while (digit != 35);

          Serial.print(numValue.c_str());
          newValue = newValue + numValue + "#";  //All strings
          break;
        case 'z':
          //pServer->disconnectClient();//Pas encore fait************
          delay(5000);
          break;
        default:
          break;
      }
      Serial.print("Valeur affectée à la charactéristique: ");
      Serial.println(newValue.c_str());
      displayMessage = "Data send BlueTooth";
      // Écrire la valeur pour la charactéristique
      pRemoteCharacteristic->writeValue(newValue.c_str());
      client.publish(nomBLE + String("/commande_cons_to_maison"), newValue.c_str());
      //Heltec.display->drawString(0, 0, newValue.c_str());
      //Heltec.display->display();
    }  // Fin du traitemennt des commandes de la console

    if (noderedReceived) {  //par MQTT
      noderedReceived = false;
      newValue = noderedPayload.c_str();
      Serial.print("Setting characteristic value to ");
      Serial.println(newValue.c_str());
      // Set the characteristic's value to be the array of bytes that is actually a string.
      //pRemoteCharacteristic->writeValue(String(newValue).c_str(), newValue.length());
      //pRemoteCharacteristic->writeValue(String(newValue), newValue.length());

      pRemoteCharacteristic->writeValue(newValue.c_str());
      client.publish(nomBLE + String("/commande_NR_to_maison"), newValue.c_str());
      //Heltec.display->drawString(0, 0, newValue.c_str());
      //Heltec.display->display();
    }

  } else {
    connected = false;
    doConnect = true;
    displayMessage = "Connection Perdue";
    Serial.println(F("Connexion perdue - On recommence!!!!"));
  }
}  // End of loop