#ifndef LECTEUR_H
#define LECTEUR_H

#include <Arduino.h>
#include <PN532_I2C.h>
#include <PN532.h>

class Lecteur {
private:
  uint8_t channel;
  uint8_t expectedUID[7]; // Support jusqu'à 7 bytes UID
  uint8_t expectedUIDLength;
  bool state;
  static int8_t currentChannel; // Canal global actuellement sélectionné
  static PN532* nfc; // Pointeur vers l'instance NFC globale
  
public:
  // Constructeur
  Lecteur(uint8_t ch, uint8_t* uid, uint8_t uidLength);
  
  // Méthodes publiques
  bool readCard();
  bool isValid() const;
  uint8_t getChannel() const;
  void printUID(uint8_t* uid, uint8_t length) const;
  
  // Méthodes statiques
  static void setNFCInstance(PN532* nfcInstance);
  static bool tcaSelect(uint8_t channel);
};

#endif