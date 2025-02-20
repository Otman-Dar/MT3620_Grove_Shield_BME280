# Projet Azure Sphere avec Grove Shield

## Description
Ce projet utilise une carte **MT3620 Azure Sphere** et le **Grove Shield** de Seeed Studio pour collecter des données environnementales (température, humidité et pression) via le capteur **BME280**. Les données sont envoyées périodiquement à un serveur distant via une requête HTTP POST.

## Matériel Utilisé
- **Carte MT3620 Azure Sphere**
- **Grove Shield pour MT3620**
- **Capteur BME280 (Température, Humidité, Pression)**

## Bibliothèques et Dépendances
Le projet utilise la bibliothèques suivante :
- [MT3620 Grove Shield Library](https://github.com/Seeed-Studio/MT3620_Grove_Shield)

### Bibliothèques à inclure dans `main.c`
Ajoutez les bibliothèques suivantes au début de votre fichier `main.c` :
```c
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <curl/curl.h>
#include <applibs/log.h>
#include <applibs/gpio.h>
#include <hw/template_appliance.h>

#include "./MT3620_Grove_Shield_Library/Grove.h"
#include "./MT3620_Grove_Shield_Library/Sensors/GroveTempHumiBaroBME280.h"
```

## Installation
1. Cloner le dépôt :
   ```sh
   git clone https://github.com/Otman-Dar/MT3620_Grove_Shield_BME280
   cd ./MT3620_Grove_Shield_BME280
   ```
2. Ajouter la bibliothèque **MT3620_Grove_Shield_Library** dans le projet.
3. Modifier le fichier `CMakeLists.txt` pour inclure la bibliothèque.
4. Vérifier et ajuster le fichier `app_manifest.json` pour inclure les permissions nécessaires.

## Compilation et Déploiement
1. Ouvrir le projet dans **Visual Studio**.
2. Compiler et flasher l'application sur la carte **MT3620 Azure Sphere**.
3. Lancer le programme.
4. lancer le serveur 
 ```sh
python app.py
```
5. copier le site que vous aller recevoir, coller dans un moteur de recherche 
 ```sh
Running on http://172.20.10.7:9999
```

## Fonctionnalités du Code
- Lecture des données du capteur **BME280** via le protocole **I2C**.
- Affichage des valeurs sur le terminal **Debug**.
- Envoi des données (JSON) vers un serveur via **cURL**.
- Gestion des signaux pour une extinction propre de l'application.

## Exemple de Requête JSON envoyée
```json
Sending JSON: {"temperature": 19.13, "humidity": 21.97, "pressure": 997.94}
```

## Remarque
Assurez-vous de modifier l'URL du serveur dans `main.c` :
```c
#define SERVER_URL "http://172.20.10.7:9999" // Remplacez par l'adresse de votre serveur
```

Développé par **DARIR Otman, OUMEILA Sara**.
