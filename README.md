# 🎯 Astro CarteFile - Système de Lecture Multi-Cartes NFC

Système Arduino optimisé pour la lecture simultanée de cartes NFC via multiplexeur TCA9548A, avec gestion de groupes et callbacks personnalisables.

## 🔧 Matériel Requis

- **Arduino Nano** (ATmega328P)
- **TCA9548A** - Multiplexeur I2C 8 canaux
- **Modules PN532** - Lecteurs NFC/RFID
- **Buzzer** (optionnel) - Pin D4
- **LED** - Pin intégrée + sortie personnalisée

## ⚡ Fonctionnalités

### 🏃‍♂️ Performance Optimisée
- **3x plus rapide** que la version originale (~35ms vs ~180ms)
- Cache intelligent des canaux TCA pour éviter les commutations inutiles
- Timeouts optimisés pour la lecture NFC
- Suppression des réinitialisations redondantes

### 📚 Architecture Modulaire
- **Classe Lecteur** : Encapsulation complète de chaque lecteur NFC
- **Classe GroupeLecteur** : Gestion de groupes de lecteurs avec callbacks
- **Classe GroupeManager** : Supervision de tous les groupes

### 🎵 Interactions Utilisateur
- Mélodie Harry Potter au succès global
- Affichage détaillé des UIDs en temps réel
- États visuels via LED et sortie digitale

## 📁 Structure du Projet

```
src/
├── main.cpp           # Programme principal
├── Lecteur.h/cpp      # Classe lecteur individuel
├── GroupeLecteur.h/cpp # Classe groupe de lecteurs
├── GroupeManager.h/cpp # Manager global des groupes
└── pitches.h          # Définitions des notes musicales
```

## 🚀 Installation & Configuration

### 1. Dépendances PlatformIO
```ini
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328
framework = arduino
lib_deps = adafruit/Adafruit PN532@^1.3.4
upload_speed = 115200
monitor_speed = 115200
```

### 2. Connexions Matérielles

#### TCA9548A (Multiplexeur I2C)
- **VCC** → 5V Arduino
- **GND** → GND Arduino  
- **SDA** → A4 Arduino
- **SCL** → A5 Arduino
- **A0-A2** → GND (adresse 0x70)

#### Modules PN532
- Connecter chaque module aux canaux 0-3 du TCA9548A
- Alimenter en 3.3V ou 5V selon le module

#### Sorties
- **Buzzer** → Pin D4
- **LED** → Pin intégrée + Pin D5 (OUTPUT_PIN)

## 🔑 Configuration des Cartes

Modifier les UIDs attendus dans `main.cpp` :

```cpp
uint8_t uid1[] = {0x29, 0x38, 0x45, 0x06}; // Carte 1
uint8_t uid2[] = {0xDA, 0x1C, 0x44, 0x06}; // Carte 2  
uint8_t uid3[] = {0x07, 0x1A, 0x3F, 0x06}; // Carte 3
uint8_t uid4[] = {0x3A, 0x07, 0x49, 0x06}; // Carte 4
```

## 📊 Usage Avancé

### Créer des Lecteurs
```cpp
uint8_t monUID[] = {0xAA, 0xBB, 0xCC, 0xDD};
Lecteur monLecteur(canal, monUID, 4);
```

### Créer des Groupes
```cpp
Lecteur* mesLecteurs[] = {&lecteur1, &lecteur2};
GroupeLecteur monGroupe(mesLecteurs, 2);

// Définir actions
monGroupe.setOnActivate(maFonction);
```

### Manager Global
```cpp
GroupeLecteur* groupes[] = {&groupe1, &groupe2};
GroupeManager manager(groupes, 2);

manager.setOnAllGroupsValid(actionFinale);
```

## 🎯 Exemple d'Utilisation

### Scénario : Salle d'Escape Game
```cpp
// Zone énigme 1 : Lecteurs 0-1
Lecteur* zoneEnigme1[] = {&lecteurs[0], &lecteurs[1]};
GroupeLecteur enigme1(zoneEnigme1, 2);
enigme1.setOnActivate(ouvrirPorte1);

// Zone énigme 2 : Lecteur 2  
Lecteur* zoneEnigme2[] = {&lecteurs[2]};
GroupeLecteur enigme2(zoneEnigme2, 1);
enigme2.setOnActivate(activerIndice);

// Victoire finale : Tous groupes
GroupeLecteur* zones[] = {&enigme1, &enigme2};
GroupeManager jeu(zones, 2);
jeu.setOnAllGroupsValid(victoireFinal);
```

## 🔧 Personnalisation

### Changer la Mélodie
Modifier le tableau `successMelody[]` dans `main.cpp` avec vos notes préférées.

### Ajouter des Lecteurs
1. Créer l'objet `Lecteur`
2. L'ajouter au groupe souhaité
3. Mettre à jour les tableaux de pointeurs

### Callbacks Personnalisés
```cpp
void monAction() {
  Serial.println("Action personnalisée !");
  // Votre code ici
}

monGroupe.setOnActivate(monAction);
```

## 📈 Performances

| Métrique | Avant | Après | Amélioration |
|----------|-------|-------|--------------|
| Cycle complet | ~180ms | ~35ms | **80% plus rapide** |
| Délai TCA | 50ms | 5-20ms | **60-75% réduit** |
| Timeout lecture | 10ms | 5-50ms | **Adaptatif** |
| Réinitialisations | À chaque cycle | Une fois | **100% éliminées** |

## 🐛 Dépannage

### Cartes Non Détectées
- Vérifier les connexions I2C
- Tester individuellement chaque module PN532
- Augmenter les délais si instable

### UIDs Incorrects  
- Utiliser le mode debug pour voir les UIDs lus
- Vérifier la correspondance avec les UIDs attendus
- S'assurer que les cartes sont compatibles ISO14443A

### Multiplexeur Défaillant
- Tester la détection TCA9548A au démarrage
- Vérifier l'adresse I2C (0x70 par défaut)
- Contrôler l'alimentation 5V stable

## 📋 Moniteur Série

Le système affiche en temps réel :
```
Initialisation du système avec TCA9548A
TCA9548A détecté !
NFC #0 détecté, version: 132
NFC #1 détecté, version: 132
...
États: L1:1 L2:0 L3:1 L4:0 - Tous groupes: ✗
SUCCÈS: Toutes les cartes sont valides!
États: L1:1 L2:1 L3:1 L4:1 - Tous groupes: ✓
🎉 MISSION ACCOMPLIE: Tous les groupes sont valides!
```

## 📝 Licence

Projet open-source sous licence MIT. Contributions bienvenues !

## 🙏 Crédits

- **Adafruit** - Bibliothèque PN532
- **hibit-dev** - Bibliothèque Buzzer et mélodies
- **Optimisations** - Développement personnalisé

---
*Développé pour les applications nécessitant une lecture NFC rapide et fiable avec gestion multi-groupes.*