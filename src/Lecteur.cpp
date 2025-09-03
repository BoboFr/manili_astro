#include "Lecteur.h"
#include <Wire.h>

#define TCA_ADDR 0x70

// Variables statiques
int8_t Lecteur::currentChannel = -1;
uint8_t Lecteur::currentTcaAddress = 0xFF; // Valeur invalide pour forcer l'initialisation
PN532* Lecteur::nfc = nullptr;

// Constructeur avec TCA par défaut
Lecteur::Lecteur(uint8_t ch, uint8_t* uid, uint8_t uidLength) 
  : tcaAddress(TCA_ADDR), channel(ch), expectedUIDLength(uidLength), state(false) {
  
  // Copier l'UID attendu
  for (uint8_t i = 0; i < uidLength && i < 7; i++) {
    expectedUID[i] = uid[i];
  }
}

// Constructeur avec TCA personnalisé
Lecteur::Lecteur(uint8_t tcaAddr, uint8_t ch, uint8_t* uid, uint8_t uidLength) 
  : tcaAddress(tcaAddr), channel(ch), expectedUIDLength(uidLength), state(false) {
  
  // Copier l'UID attendu
  for (uint8_t i = 0; i < uidLength && i < 7; i++) {
    expectedUID[i] = uid[i];
  }
}

// Définir l'instance NFC globale
void Lecteur::setNFCInstance(PN532* nfcInstance) {
  nfc = nfcInstance;
}

// Sélection optimisée du canal TCA avec adresse spécifique
bool Lecteur::tcaSelect(uint8_t tcaAddress, uint8_t channel) {
  if (channel > 7) {
    Serial.print("Canal invalide : ");
    Serial.println(channel);
    return false;
  }

  // Éviter la commutation si déjà sur le bon TCA et canal
  if (currentTcaAddress == tcaAddress && currentChannel == channel) {
    return true;
  }

  Wire.beginTransmission(tcaAddress);
  Wire.write(1 << channel);
  uint8_t error = Wire.endTransmission();

  if (error != 0) {
    Serial.print("Erreur sélection TCA 0x");
    Serial.print(tcaAddress, HEX);
    Serial.print(" canal ");
    Serial.print(channel);
    Serial.print(" -> Code I2C: ");
    Serial.println(error);
    currentChannel = -1;
    currentTcaAddress = 0xFF;
    return false;
  }

  currentTcaAddress = tcaAddress;
  currentChannel = channel;
  delay(20);
  return true;
}

// Version compatible avec l'ancien code (utilise TCA par défaut)
bool Lecteur::tcaSelect(uint8_t channel) {
  return tcaSelect(TCA_ADDR, channel);
}

// Comparer deux UIDs
bool compareUID(uint8_t *uid1, uint8_t *uid2, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

// Lire une carte sur ce lecteur
bool Lecteur::readCard() {
  if (!nfc) {
    Serial.println("Erreur: Instance NFC non définie");
    return false;
  }

  // Sélectionner le canal sur le bon TCA
  if (!tcaSelect(tcaAddress, channel)) {
    state = false;
    return false;
  }

  uint8_t uid[7];
  uint8_t uidLength;
  
  // Lecture de la carte
  uint8_t success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    // Vérifier si l'UID correspond
    if (uidLength == expectedUIDLength && compareUID(uid, expectedUID, uidLength)) {
      state = true;
    } else {
      state = false;
    }
  } else {
    state = false;
  }

  nfc->inRelease();
  delay(20);
  
  return state;
}

// Obtenir l'état du lecteur
bool Lecteur::isValid() const {
  return state;
}

// Obtenir le canal du lecteur
uint8_t Lecteur::getChannel() const {
  return channel;
}

// Obtenir l'adresse TCA du lecteur
uint8_t Lecteur::getTcaAddress() const {
  return tcaAddress;
}

// Afficher un UID (utilitaire)
void Lecteur::printUID(uint8_t* uid, uint8_t length) const {
  Serial.print("L");
  Serial.print(channel);
  Serial.print(" UID: ");
  for (uint8_t j = 0; j < length; j++) {
    if (uid[j] < 0x10) Serial.print("0");
    Serial.print(uid[j], HEX);
    if (j < length - 1) Serial.print(" ");
  }
  Serial.print(" ");
}