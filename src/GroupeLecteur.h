#ifndef GROUPELECTEUR_H
#define GROUPELECTEUR_H

#include <Arduino.h>
#include "Lecteur.h"

class GroupeLecteur {
private:
  Lecteur** lecteurs;
  uint8_t nbLecteurs;
  bool lastState;
  void (*onActivateCallback)();
  void (*onDeactivateCallback)();
  
public:
  // Constructeurs
  GroupeLecteur(Lecteur* lecteursArray[], uint8_t nb);
  
  // Méthodes publiques
  bool isAllValid();
  void update();
  void setOnActivate(void (*callback)());
  void setOnDeactivate(void (*callback)());
  
  // Méthodes utilitaires
  void printState();
  uint8_t getValidCount();
};

#endif