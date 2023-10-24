/**
 * client BLE
 Connecté à la maison KEYSTUDIO
 par un HM-10 sur un Arduino Mega
 avec un ESP32 - écran OLED
 
 Robert Turenne
 2022-10-16 Version 2.0
 2023-9-16 Version 3.0 Nettoyage du code. BLE debuggé.
 */

#include "BLEDevice.h"  //pour le Bluetooth
#include "heltec.h"     //pour le display OLED

//#define DEBUG

// ATTTENTION à la version, au service et au nom !***************************!!!!!
const char* version = "BLE ESP32-HM10 V3.0";
char* displayMessage = "                    ";  //utilisé pour le display OLED du ESP32
std::string newValue;                           //utilisé pour le display OLED du ESP32

// Le firmware du MH-10 défini les services et charactéristiques BLE
// Le service REMOTE qui nous intéresse, unique dans notre environnement
static BLEUUID serviceUUID("FF99");  //remplacer le UUID par FFXX où XX est le numéro du HM10
// La charactéristique REMOTE qui nous intéresse (NE PAS CHANGER)
static BLEUUID charUUID("FFE1");  //NE PAS CHANGER
// Notre HM-10 a un nom unique dans notre environnement
// Ça sert à nous assurer que nous avons le bon HM-10
std::string myDeviceName = "99-HM10-IDO";  //ÉCRIRE ICI le nom de votre HM-10, donc remplacer 00 par votre numéro

static boolean doConnect = false;
static boolean connected = false;
static boolean disconnected = true;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;






// Callback losqu'on reçoit du data sur le bluetooth du ESP32 (dans notre cas initié par le HM-10)
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

#ifdef DEBUG
  Serial.println(F("notifyCallback- Notification de rappel -callback- du ESP32 "));
  Serial.print(F("Notify callback pour characteristic "));
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(F(" -longueur de data "));
  Serial.print(length);
  Serial.print(F(" -data: "));
#else
  Serial.print(F("Recu de Bluetooth: "));
#endif

  Serial.println((char*)pData);

  newValue = (char*)pData;
  displayMessage = "Data reçue de BlueTooth";
}

//Automatique lors du connect ou disconnect BLE
//Dans notre cas lorsqu'on est connecté au HM10
//La fonction est appelée plus bas dans le code,
// et lorsque le hm10 se déconnecte tout seul
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

  Serial.println(F("Fonction: connectToServer"));
#ifdef DEBUG
  Serial.print(F(" - Initiation d'une Connexion vers "));
  Serial.print(myDevice->getAddress().toString().c_str());
  //RT
  Serial.print(F(" qui a le nom "));
  Serial.println(myDevice->getName().c_str());
  //RT - Validation : Est - ce le bon nom pour notre HM - 10 ?
  // if (myDevice->getName() != myDeviceName) {
  //   Serial.println(F("********** MAUVAIS NOM *********"));
  //   delay(1000);
  //   return false;
  //}
#endif

  Serial.println(F(" - Création de mon client"));
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(F(" - Client Créé"));

  // Préparer les callBacks pour ce client (fonction ci-haut)
  pClient->setClientCallbacks(new MyClientCallback());
  //Va faire un appel à MyClientCallback en créant l'instance
  Serial.print(F(" -Appel de "));
  // Connecter au client REMOTE
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of
  // address, it will be recognized type of peer device address (public or
  // private) RT-Ben oui!, c'esst clair?????!

//#ifdef DEBUG
  if (disconnected == false) {
    Serial.println(F(" - Maintenant Connecté au serveur"));
  } else {
    Serial.println(F("***** NON connecté su serveur *****"));
    return false;
  }
//#endif

  // Obtenir la référence au service. On avait déjà le service
  // Obtenu lors du scan (setup) mais on vvérifie
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);

#ifdef DEBUG
  if (pRemoteService == nullptr) {
    Serial.print(F("Incapable de trouver le service associé au UUID: "));
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();  //va provoquer un tentative de reConnexion
    return false;
  }
  //La prenière partie (8 octets) contient le nom du service
  Serial.print(F(" - Service trouvé: "));
  Serial.println(serviceUUID.toString().c_str());
#endif


  // Obtenir la référence à la charactéristique du service du serveur REMOTE BLE
  // On a besoin du service avant de pouvoir chercher la charctéristique
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);

#ifdef DEBUG
  if (pRemoteCharacteristic == nullptr) {
    Serial.print(F("Incapable de trouver la charactéristique associée au UUID: "));
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();  //va provoquer un tentative de reConnexion
    return false;
  }
  //La prenière partie (8 octets) contient le nom du service
  Serial.print(F(" - Charactéristique trouvée: "));
  Serial.println(charUUID.toString().c_str());

  //Est - ce que la charactéristique est lisible ?
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    //Serial.print(F(" - La valeur de la charactéristique est: "));
    //Serial.println(value.c_str());
  }
#endif

  // Est-ce qu'on peut modifier (notify) la charactéristique?
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println(F(" - La charactéristique peut être ré-écrite"));
  } else {
    Serial.println(F(" - La characteristique NE PEUT PAS être ré-écrite --ERREUR"));
    return false;
  }

  connected = true;  //On est vraiment sur le bon serveur!
  Serial.println(F("Tout est beau!"));

  /* RT 23-9-15
  Le compilateur C++ va freeker (mais sans erreur) si une fonction non-void ne retourne rien */
  return true;

}  //connecToServer



/**
 * Scan pour les serveur BLE et trouver le premier qui a le service que nous désirons accéder
 RT- On peut aussi vérifier aussi le nom du module HM-10. Ici on sauve du temps
    lors du scan. Malheureusement, la charactéristique n'est pas
    disponible à ce niveau.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Appelé pour chacun des serveur BLE trouvé
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
#ifdef DEBUG
    Serial.print(F("Objet BLE trouvé: "));
    // On imprime son contenu
    Serial.println(advertisedDevice.toString().c_str());
#endif

    // On a trouvé un objet BLE. Regardons le service qu'il publie
    // pour savoir si c'est le bon service.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID) /*RT && advertisedDevice.haveName()*/) {

      myDevice = new BLEAdvertisedDevice(advertisedDevice);  //Connexion au nouveau serveur BLE

      //Maintenant on va chercher le nom
      //SEULEMENT pour du déboggage profond!
      //Serial.println(myDevice->getName().c_str());
      // RT - Est-ce le bon nom pour notre HM-10?
      // if (myDevice->getName() != myDeviceName) {
      //   Serial.println(F("********** MAUVAIS NOM *********"));
      //   delay(1000);
      //   ;
      // } else {
      doConnect = true;  //Si on a le bon serveur, on n'est pas encore connecté. Va falloir appeler ConnectToServer()
      Serial.println(F("SCAN: l'objet a un service compatible!"));
      //displayMessage = "Serveur compatible trouvé";
      BLEDevice::getScan()->stop();  // IMPORTANT: On arrête le scan (lancé dans le setup)
      Serial.println(F("Fin du Scan car Objet trouvé!"));
      // }  // Trouvé notre Serveur
    }  // Vérifier résultats du scan
  }    // Regarder les objets trouvés
};     // Callback des BLE publiés (NE PAS oublier le ;)


void setup() {
  Serial.begin(115200);  //ATTENTION à la vitesse de la console
  delay(1000);
  Serial.println("  ");
  Serial.println(F("Début de l'application BLE..."));
  Serial.print("Version: ");
  Serial.println(version);

  // Pour initialiser l'écran du ESP32
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
  //Voir la classe "class MyAdvertisedDeviceCallbacks" ci-haut
  Serial.println(F("Début du Scan pour le service"));
  Serial.println(F("Voir classe MyAdvertisedDeviceCallbacks"));

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();  //Creer un nouveau scan
  //Préparer le lien avec les CallBacks
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //pBLEScan->setInterval(1349);
  pBLEScan->setInterval(100);
  //pBLEScan->setWindow(449);
  pBLEScan->setWindow(99);
  pBLEScan->setActiveScan(true);  //Active scan utilise plus de puissance, mais esst plus rapide
  BLEScanResults foundDevices = pBLEScan->start(30, false); //commencer le scan. 30 secondes en laboratoire
  //on revient ici après le scan
  Serial.print("Objets trouvés: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan complété!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);


  Serial.println(F("Fin du SETUP"));
}  // Fin du setup.


void loop() {


  Heltec.display->clear();
  Heltec.display->drawString(0, 0, version);  //Attention à la version
  Heltec.display->drawString(0, 12, "Connecté à: ");
  Heltec.display->drawString(59, 12, myDevice->getName().c_str());
  Heltec.display->drawString(0, 24, "Dernière Commande: ");
  Heltec.display->drawString(100, 24, newValue.c_str());
  Heltec.display->drawString(0, 36, displayMessage);
  Heltec.display->display();

  // Si le flag "doConnect" est VRAI, alors on a trouvé le serveur BLE avec lequel on
  // voulait connecter. Maintenant on se connecte au serveur
  if (doConnect == true) {
    // On démarre la connecttion en appeelant la fonction et on teste le résultat
    connectToServer();
    doConnect = false;

    // #ifdef DEBUG
    //     if (connectToServer()) {
    //       Serial.print(F("Connexion réussie au serveur BLE: "));
    //       Serial.println(myDevice->getName().c_str());
    //       Heltec.display->drawString(0, 12, "Connecté à: ");
    //       Heltec.display->drawString(59, 12, myDevice->getName().c_str());
    //       Heltec.display->display();

    //     } else {
    //       Serial.println(F("La Connexion au serveur BLE n'a pas fonctionné"));
    //       Heltec.display->clear();
    //       Heltec.display->drawString(0, 12, "Pas connecté à BLE");
    //       Heltec.display->display();
    //     }
    //#endif

  }  //if doConnect

  // Si la Connexion à un serveur BLE est OK, update de la Charactéristique
  //  à chaque fois qu'on nous contacte (par BLE ou la console)
  if (connected) {
    //Les données peuvent provenir du port série à travers la console
    if (Serial.available())  //RT
    {
      Serial.print("Premier chacractère reçu: (");
      char firstChar = Serial.read();  //lire le premier charactere pour préparer le CASE
      Serial.print(firstChar);
      Serial.print(") ");
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

          Serial.println(numValue.c_str());
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
      displayMessage = "Data to BlueTooth";
      // Écrire la valeur pour la charactéristique
      pRemoteCharacteristic->writeValue(newValue.c_str());
      //Heltec.display->drawString(0, 0, newValue.c_str());
      //Heltec.display->display();
    }  // Fin du traitemennt des commandes de la console

  } else {
    connected = false;
    doConnect = true;
    displayMessage = "Connection Perdue";
    Serial.println(F("Connexion perdue - On recommence!!!!"));
  }
}  // End of loop
