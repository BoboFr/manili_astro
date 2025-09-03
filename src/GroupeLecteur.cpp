#include "GroupeLecteur.h"

// Constructeur
GroupeLecteur::GroupeLecteur(Lecteur* lecteursArray[], uint8_t nb) 
  : nbLecteurs(nb), lastState(false), onActivateCallback(nullptr), onDeactivateCallback(nullptr) {
  
  // Allouer mémoire pour les pointeurs vers lecteurs
  lecteurs = new Lecteur*[nbLecteurs];
  
  // Copier les pointeurs
  for (uint8_t i = 0; i < nbLecteurs; i++) {
    lecteurs[i] = lecteursArray[i];
  }
}

// Vérifier si tous les lecteurs du groupe sont valides
bool GroupeLecteur::isAllValid() {
  for (uint8_t i = 0; i < nbLecteurs; i++) {
    if (!lecteurs[i]->isValid()) {
      return false;
    }
  }
  return true;
}

// Mettre à jour l'état et déclencher les callbacks
void GroupeLecteur::update() {
  bool currentState = isAllValid();
  
  // Détection du passage de faux à vrai (activation)
  if (currentState && !lastState && onActivateCallback) {
    onActivateCallback();
  }
  
  // Détection du passage de vrai à faux (désactivation)
  if (!currentState && lastState && onDeactivateCallback) {
    onDeactivateCallback();
  }
  
  lastState = currentState;
}

// Définir callback d'activation
void GroupeLecteur::setOnActivate(void (*callback)()) {
  onActivateCallback = callback;
}

// Définir callback de désactivation
void GroupeLecteur::setOnDeactivate(void (*callback)()) {
  onDeactivateCallback = callback;
}

// Afficher l'état du groupe
void GroupeLecteur::printState() {
  Serial.print("Groupe [");
  for (uint8_t i = 0; i < nbLecteurs; i++) {
    Serial.print("L");
    Serial.print(lecteurs[i]->getChannel());
    Serial.print(":");
    Serial.print(lecteurs[i]->isValid() ? "1" : "0");
    if (i < nbLecteurs - 1) Serial.print(", ");
  }
  Serial.print("] = ");
  Serial.println(isAllValid() ? "ACTIF" : "INACTIF");
}

// Obtenir le nombre de lecteurs valides
uint8_t GroupeLecteur::getValidCount() {
  uint8_t count = 0;
  for (uint8_t i = 0; i < nbLecteurs; i++) {
    if (lecteurs[i]->isValid()) {
      count++;
    }
  }
  return count;
}