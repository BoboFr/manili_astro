#include "GroupeManager.h"

// Constructeur
GroupeManager::GroupeManager(GroupeLecteur* groupesArray[], uint8_t nb) 
  : nbGroupes(nb), lastAllValidState(false), onAllGroupsValidCallback(nullptr), onAnyGroupInvalidCallback(nullptr) {
  
  // Allouer mémoire pour les pointeurs vers groupes
  groupes = new GroupeLecteur*[nbGroupes];
  
  // Copier les pointeurs
  for (uint8_t i = 0; i < nbGroupes; i++) {
    groupes[i] = groupesArray[i];
  }
}

// Vérifier si tous les groupes sont valides
bool GroupeManager::areAllGroupsValid() {
  for (uint8_t i = 0; i < nbGroupes; i++) {
    if (!groupes[i]->isAllValid()) {
      return false;
    }
  }
  return true;
}

// Mettre à jour tous les groupes et déclencher les callbacks si nécessaire
void GroupeManager::updateAll() {
  // Mettre à jour chaque groupe individuellement
  for (uint8_t i = 0; i < nbGroupes; i++) {
    groupes[i]->update();
  }
  
  bool currentAllValidState = areAllGroupsValid();
  
  // Détection du passage de faux à vrai (tous les groupes deviennent valides)
  if (currentAllValidState && !lastAllValidState && onAllGroupsValidCallback) {
    onAllGroupsValidCallback();
  }
  
  // Détection du passage de vrai à faux (au moins un groupe devient invalide)
  if (!currentAllValidState && lastAllValidState && onAnyGroupInvalidCallback) {
    onAnyGroupInvalidCallback();
  }
  
  lastAllValidState = currentAllValidState;
}

// Définir callback pour quand tous les groupes sont valides
void GroupeManager::setOnAllGroupsValid(void (*callback)()) {
  onAllGroupsValidCallback = callback;
}

// Définir callback pour quand un groupe devient invalide
void GroupeManager::setOnAnyGroupInvalid(void (*callback)()) {
  onAnyGroupInvalidCallback = callback;
}

// Afficher l'état de tous les groupes
void GroupeManager::printAllStates() {
  Serial.println("=== État des groupes ===");
  for (uint8_t i = 0; i < nbGroupes; i++) {
    Serial.print("Groupe ");
    Serial.print(i + 1);
    Serial.print(": ");
    groupes[i]->printState();
  }
  Serial.print("TOUS LES GROUPES: ");
  Serial.println(areAllGroupsValid() ? "VALIDES ✓" : "INVALIDES ✗");
  Serial.println("=====================");
}

// Obtenir le nombre de groupes valides
uint8_t GroupeManager::getValidGroupsCount() {
  uint8_t count = 0;
  for (uint8_t i = 0; i < nbGroupes; i++) {
    if (groupes[i]->isAllValid()) {
      count++;
    }
  }
  return count;
}