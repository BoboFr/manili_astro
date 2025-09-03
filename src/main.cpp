#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "Lecteur.h"
#include "GroupeLecteur.h"
#include "GroupeManager.h"

#define TCA_ADDR 0x70 // Adresse I2C par défaut du premier TCA9548A
#define TCA_ADDR2 0x71 // Adresse I2C du deuxième TCA9548A (exemple)

#define OUTPUT_PIN DD5

// Interface I2C pour PN532
PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);

// Création des lecteurs avec leurs TCA, canaux et UIDs attendus
uint8_t uid1[] = {0x29, 0x38, 0x45, 0x06};
uint8_t uid2[] = {0xDA, 0x1C, 0x44, 0x06};
uint8_t uid3[] = {0x07, 0x1A, 0x3F, 0x06};
uint8_t uid4[] = {0x3A, 0x07, 0x49, 0x06};
uint8_t uid5[] = {0x5B, 0x2C, 0x67, 0x06};
uint8_t uid6[] = {0x8E, 0x45, 0x23, 0x06};

Lecteur lecteurs[] = {
    Lecteur(TCA_ADDR, 0, uid1, 4),  // TCA 0x70, Canal 0, carte 1
    Lecteur(TCA_ADDR, 1, uid2, 4),  // TCA 0x70, Canal 1, carte 2
    Lecteur(TCA_ADDR, 2, uid3, 4),  // TCA 0x70, Canal 2, carte 3
    Lecteur(TCA_ADDR, 3, uid4, 4),  // TCA 0x70, Canal 3, carte 4
    Lecteur(TCA_ADDR2, 0, uid5, 4), // TCA 0x71, Canal 0, carte 5
    Lecteur(TCA_ADDR2, 1, uid6, 4)  // TCA 0x71, Canal 1, carte 6
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
Lecteur *groupe1Lecteurs[] = {&lecteurs[0], &lecteurs[1], &lecteurs[2], &lecteurs[3]}; // TCA 0x70
GroupeLecteur groupe1(groupe1Lecteurs, 4);

Lecteur *groupe2Lecteurs[] = {&lecteurs[4], &lecteurs[5]}; // TCA 0x71
GroupeLecteur groupe2(groupe2Lecteurs, 2);

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
  Serial.println("Initialisation du système avec multiple TCA9548A");

  // Initialiser la classe Lecteur avec l'instance NFC
  Lecteur::setNFCInstance(&nfc);

  // Configurer les callbacks des groupes individuels
  groupe1.setOnActivate(onAllCardsValid);
  groupe2.setOnActivate(onAllCardsValid);

  // Configurer les callbacks du manager global
  manager.setOnAllGroupsValid(onAllGroupesValid);

  // Test de connexion aux TCA9548A
  uint8_t tcaAddresses[] = {TCA_ADDR, TCA_ADDR2};
  uint8_t numTcas = sizeof(tcaAddresses) / sizeof(tcaAddresses[0]);
  
  for (uint8_t i = 0; i < numTcas; i++) {
    Wire.beginTransmission(tcaAddresses[i]);
    if (Wire.endTransmission() == 0) {
      Serial.print("TCA9548A 0x");
      Serial.print(tcaAddresses[i], HEX);
      Serial.println(" détecté !");
    } else {
      Serial.print("Erreur: TCA9548A 0x");
      Serial.print(tcaAddresses[i], HEX);
      Serial.println(" non détecté");
      // Ne pas arrêter le système, continuer avec les autres TCA
    }
  }

  for (uint8_t i = 0; i < numLecteurs; i++)
  {
    uint8_t tcaAddr = lecteurs[i].getTcaAddress();
    uint8_t channel = lecteurs[i].getChannel();

    Lecteur::tcaSelect(tcaAddr, channel);
    delay(100); // Délai pour la stabilisation après begin()

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
      Serial.print("NFC TCA:0x");
      Serial.print(tcaAddr, HEX);
      Serial.print(" CH:");
      Serial.print(channel);
      Serial.println(" non détecté !");
      continue;
    }

    nfc.SAMConfig();
    delay(50); // Délai après configuration SAM
    Serial.print("NFC TCA:0x");
    Serial.print(tcaAddr, HEX);
    Serial.print(" CH:");
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
