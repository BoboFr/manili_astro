#ifndef GROUPEMANAGER_H
#define GROUPEMANAGER_H

#include <Arduino.h>
#include "GroupeLecteur.h"

class GroupeManager {
private:
  GroupeLecteur** groupes;
  uint8_t nbGroupes;
  bool lastAllValidState;
  void (*onAllGroupsValidCallback)();
  void (*onAnyGroupInvalidCallback)();
  
public:
  // Constructeur
  GroupeManager(GroupeLecteur* groupesArray[], uint8_t nb);
  
  // Méthodes principales
  bool areAllGroupsValid();
  void updateAll();
  
  // Callbacks
  void setOnAllGroupsValid(void (*callback)());
  void setOnAnyGroupInvalid(void (*callback)());
  
  // Utilitaires
  void printAllStates();
  uint8_t getValidGroupsCount();
};

#endif