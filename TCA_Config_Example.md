# Configuration Multi-TCA9548A

## Adresses I2C TCA9548A supportées

Le système supporte maintenant plusieurs multiplexeurs TCA9548A avec des adresses différentes :

- **TCA_ADDR** : 0x70 (adresse par défaut)
- **TCA_ADDR2** : 0x71 (exemple d'adresse alternative)

## Configuration des adresses TCA9548A

Pour modifier les adresses I2C des TCA9548A, vous devez configurer les cavaliers (jumpers) sur chaque module :

| Cavaliers A2 A1 A0 | Adresse I2C |
|--------------------|-------------|
| 0 0 0             | 0x70        |
| 0 0 1             | 0x71        |
| 0 1 0             | 0x72        |
| 0 1 1             | 0x73        |
| 1 0 0             | 0x74        |
| 1 0 1             | 0x75        |
| 1 1 0             | 0x76        |
| 1 1 1             | 0x77        |

## Exemple de configuration

```cpp
// Définir les adresses TCA
#define TCA_ADDR  0x70  // Premier TCA9548A
#define TCA_ADDR2 0x71  // Deuxième TCA9548A
#define TCA_ADDR3 0x72  // Troisième TCA9548A (optionnel)

// UIDs des cartes NFC
uint8_t uid1[] = {0x29, 0x38, 0x45, 0x06};
uint8_t uid2[] = {0xDA, 0x1C, 0x44, 0x06};
uint8_t uid3[] = {0x07, 0x1A, 0x3F, 0x06};

// Créer des lecteurs sur différents TCA
Lecteur lecteurs[] = {
    Lecteur(TCA_ADDR, 0, uid1, 4),   // TCA 0x70, Canal 0
    Lecteur(TCA_ADDR, 1, uid2, 4),   // TCA 0x70, Canal 1  
    Lecteur(TCA_ADDR2, 0, uid3, 4),  // TCA 0x71, Canal 0
    // Ou utiliser le constructeur simplifié (TCA par défaut 0x70)
    Lecteur(2, uid1, 4)              // TCA 0x70, Canal 2
};
```

## Constructeurs disponibles

1. **Constructeur avec TCA par défaut** : `Lecteur(canal, uid, taille_uid)`
2. **Constructeur avec TCA spécifique** : `Lecteur(adresse_tca, canal, uid, taille_uid)`

## Optimisations

- Le système évite les commutations inutiles en gardant en mémoire le TCA et canal actuellement sélectionnés
- Messages d'erreur détaillés incluent l'adresse TCA et le canal
- Support de jusqu'à 8 TCA9548A différents (adresses 0x70-0x77)
- Chaque TCA peut gérer jusqu'à 8 canaux (0-7)

## Total possible : 64 lecteurs NFC

Avec 8 TCA9548A × 8 canaux = 64 lecteurs NFC maximum