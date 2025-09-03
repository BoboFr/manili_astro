#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "Lecteur.h"
#include "GroupeLecteur.h"
#include "GroupeManager.h"

#define TCA_ADDR 0x70 // Adresse I2C par défaut du TCA9548A

#define OUTPUT_PIN DD5

// Interface I2C pour PN532
PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);

// Création des lecteurs avec leurs canaux et UIDs attendus
uint8_t uid1[] = {0x29, 0x38, 0x45, 0x06};
uint8_t uid2[] = {0xDA, 0x1C, 0x44, 0x06};
uint8_t uid3[] = {0x07, 0x1A, 0x3F, 0x06};
uint8_t uid4[] = {0x3A, 0x07, 0x49, 0x06};

Lecteur lecteurs[] = {
    Lecteur(0, uid1, 4), // Canal 0, carte 1
    Lecteur(1, uid2, 4), // Canal 1, carte 2
    Lecteur(2, uid3, 4), // Canal 2, carte 3
    Lecteur(3, uid4, 4)  // Canal 3, carte 4
};

const uint8_t numLecteurs = sizeof(lecteurs) / sizeof(lecteurs[0]);

// Callbacks pour les différents niveaux
void onAllCardsValid()
{
  Serial.println("SUCCÈS: Toutes les cartes sont valides!");
}

void onAllGroupesValid()
{
  Serial.println("MISSION ACCOMPLIE: Tous les groupes sont valides!");
  // Actions spéciales quand TOUT est validé
}

// Créer différents groupes
Lecteur *groupe1Lecteurs[] = {&lecteurs[0], &lecteurs[1], &lecteurs[2]};
GroupeLecteur groupe1(groupe1Lecteurs, 3);

Lecteur *groupe2Lecteurs[] = {&lecteurs[3]};
GroupeLecteur groupe2(groupe2Lecteurs, 1);

// Manager pour gérer tous les groupes
GroupeLecteur *tousGroupes[] = {&groupe1, &groupe2};
GroupeManager manager(tousGroupes, 2);

// Fonction pour vérifier si tous les lecteurs ont lu la bonne carte
bool allReadersValid()
{
  for (uint8_t i = 0; i < numLecteurs; i++)
  {
    if (!lecteurs[i].isValid())
    {
      return false;
    }
  }
  return true;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  Wire.begin();
  delay(500);
  Serial.println("Initialisation du système avec TCA9548A");

  // Initialiser la classe Lecteur avec l'instance NFC
  Lecteur::setNFCInstance(&nfc);

  // Configurer les callbacks des groupes individuels
  groupe1.setOnActivate(onAllCardsValid);
  groupe2.setOnActivate(onAllCardsValid);

  // Configurer les callbacks du manager global
  manager.setOnAllGroupsValid(onAllGroupesValid);

  // Test de connexion au TCA9548A
  Wire.beginTransmission(TCA_ADDR);
  if (Wire.endTransmission() == 0)
  {
    Serial.println("TCA9548A détecté !");
  }
  else
  {
    Serial.println("Erreur: TCA9548A non détecté");
    while (1)
      ;
  }

  for (uint8_t i = 0; i < numLecteurs; i++)
  {
    uint8_t channel = lecteurs[i].getChannel();

    Lecteur::tcaSelect(channel);
    delay(100); // Délai pour la stabilisation après begin()

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
      Serial.print("NFC #");
      Serial.print(channel);
      Serial.println(" non détecté !");
      continue;
    }

    nfc.SAMConfig();
    delay(50); // Délai après configuration SAM
    Serial.print("NFC #");
    Serial.print(channel);
    Serial.println(" prêt !");
  }

  Serial.println("En attente de cartes NFC...");
}

bool output = false;

void loop()
{
  // Lire tous les lecteurs
  for (uint8_t i = 0; i < numLecteurs; i++)
  {
    lecteurs[i].readCard();
  }

  // Mettre à jour tous les groupes via le manager
  manager.updateAll();

  // Définir l'état de sortie basé sur TOUS les groupes
  output = manager.areAllGroupsValid();
  digitalWrite(OUTPUT_PIN, output ? HIGH : LOW);

  digitalWrite(LED_BUILTIN, groupe1.isAllValid() ? HIGH : LOW);

  // Affichage de l'état des lecteurs
  Serial.print("États: ");
  for (uint8_t i = 0; i < numLecteurs; i++)
  {
    Serial.print("L");
    Serial.print(i + 1);
    Serial.print(":");
    Serial.print(lecteurs[i].isValid() ? "1" : "0");
    Serial.print(" ");
  }
  Serial.print(" - Tous groupes: ");
  Serial.println(manager.areAllGroupsValid() ? "✓" : "✗");
}
