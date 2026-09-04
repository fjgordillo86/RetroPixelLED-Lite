# ✨ Retro Pixel LED Lite v3.1.2
**[🇪🇸 Español](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md) | [🇫🇷 Français](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README.md)**

### **[✈️ Rejoindre le Groupe Telegram : Retro Pixel LED pour rester informé des mises à jour](https://t.me/RetroPixelLed)**

## 💡 Description du Projet

**Retro Pixel LED Lite** est la version haute performance conçue pour ceux qui recherchent une stabilité absolue, une vitesse instantanée et un système sans maintenance. Contrairement à la version standard, le firmware LITE élimine la charge du serveur web et la connectivité permanente pour consacrer 100% de la puissance de l'ESP32 au rendu des GIFs.

Si la branche 2.x.x a introduit le Menu OSD, la nouvelle **v3.0.0** représente la transition définitive vers l'indépendance matérielle. Cette version transforme le panneau LED en un appareil intelligent autonome, éliminant complètement le besoin de connecter l'ESP32 à un ordinateur pour les tâches de maintenance ou de configuration.
Pour la première fois, le système permet la modification des fichiers de configuration (`config.ini`) et la gestion des bibliothèques de playlists directement depuis l'Explorateur Windows ou des clients FTP, transformant la carte SD en un lecteur réseau sans fil.
L'assistance native pour télécommandes est intégrée, vous permettant de naviguer dans le Menu OSD, d'ajuster la luminosité dynamique et de contrôler la mise sous/hors tension depuis votre canapé. **À partir de la version 3.1.0, nous pouvons contrôler le panneau LED depuis l'APPLICATION !!!**


Vous souhaitez créer vos propres GIFs ? Voici trois magnifiques outils :
- [DMD GIF converter](https://github.com/shan-aya/DMD_GIF_converter) créé par **shan-aya**.
- [dmd gif converter](https://github.com/red77290/dmd_gif_converter) créé par **red77290**.
- [Video à GIF](https://p4blogc.github.io/dmdos-converter/) créé par **p4bloGC**.


## 🆕 Nouveautés de la Version v3.1.2 Lite

#### 🚀 Nouvelles Fonctionnalités (Features)

* **🔤 Sélection du style de police dans la PWA:** Il est posible de choisir entre les styles Bold, SemiBold, Regular et Light.
* **🏠 Intégration avec Home Assistant :** S'intègre avec Home Assistant via l'intégration REST. **Plus d'infos au point 10. 🏠 Intégration avec Home Assistant**

---

## 🕹️ Intégration Spéciale : Mode Arcade (Batocera, Recalbox & RePlayOS)

Cette version Lite introduit un support avancé pour les écosystèmes de retrogaming, permettant deux méthodes de synchronisation : via des scripts locaux (**Batocera / Recalbox**) ou par surveillance native sur réseau local (**RePlayOS**). 

Grâce à une hiérarchie de fichiers intelligente et optimisée pour le matériel de l'ESP32, le panneau gère le changement d'état et affiche :

1. **Marquise du Jeu :** Image `.bmp` 24 bits chargée instantanément, ou un **GIF animé** (Batocera et Recalbox) s'il en existe un pour ce jeu — incluant des séquences de plusieurs GIFs lus les uns après les autres en boucle.
2. **Logo du Système :** Image `.bmp` 24 bits chargée instantanément lors de la navigation à travers les systèmes.


Pour plus d'informations, rendez-vous au point `9. 🕹️ Intégration avec Batocera, Recalbox ou ReplayOS (Arcade)`

---

## 📜 Historique Détaillé des Changements (v3.0.0 -> v3.1.2)

| Fonctionnalité | Détail Technique | Bénéfice |
| :--- | :--- | :--- |
| **🏠 Intégration Home Assistant** | Exposition des points d'accès REST API (GET `/status`, POST `/control`, `/playlist`, `/texto`, `/timer/toggle`) et package YAML complet pour une intégration domotique native. | **Automatisation et contrôle domotique.** Contrôlez l'allumage, l'extinction, changez de mode, de playlist et envoyez des notifications textuelles depuis l'interface ou les automatisations de Home Assistant. |
| **🔤 Sélection de police du texte** | Intégration de 4 typographies sélectionnables par paramètre (`Bold`, `SemiBold`, `Regular`, `Light`) sur les points d'accès HTTP, la PWA et l'intégration REST. | **Personnalisation visuelle.** Permet d'adapter le style visuel des messages défilants selon le type de notification ou les préférences esthétiques. |
| **🎛️ Panneau de Contrôle PWA** | Interface web progressive (Progressive Web App) avec 5 modules de contrôle indépendants et édition à distance de `config.ini`. | **Contrôle total depuis n'importe quel appareil.** Indépendant de la géolocalisation, fonctionne sur le réseau local sans serveur externe. |
| **☀️ Contrôle de Luminosité** | Curseur glissant en temps réel (0-100%) avec application instantanée, sans redémarrage. | **Ajustement fluide.** Adapte la luminosité à l'éclairage ambiant en un instant. |
| **🔤 Texte Défilant avec UTF-8** | Moteur de défilement de texte avec décodeur UTF-8→Latin-1 en temps réel, support des caractères polonais et accentués. | **Internationalisation complète.** Messages avec ñ, á, ł, ą sans limitations. |
| **🎨 Mode GIF / Horloge / Texte** | Sélecteur de mode sur l'accueil de la PWA ; changement instantané sans redémarrage. | **Expérience immédiate.** Changements visibles sur le panneau instantanément. |
| **🎞️ Playlists Dynamiques** | Changement de playlist en temps réel depuis la PWA ; rechargement des index à la volée. | **Flexibilité maximale.** Alternez entre les collections sans interrompre la lecture. |
| **⏰ Minuteur Intelligent** | Mise sous/hors tension programmée avec interface de sélection d'horaire ; dérogation manuelle par bouton ou PWA. | **Automatisation complète.** Allume le panneau à une heure, l'éteint à une autre ; un bouton annule le programme. |
| **🔄 Mise à Jour Distante (OTA + Langues)** | Page de Mise à Jour indépendante dans la PWA ; téléchargement du firmware et des langues depuis GitHub sans extraction de la SD. | **Maintenance sans fil.** Mettez à jour le panneau entièrement sans fil, y compris les fichiers de langue JSON. |
| **⚙️ Réglages Complets Distants** | Édition à distance des sections de `config.ini` : WiFi, Matériel (panneau, vitesse I2S, rafraîchissement), Lecture (arcade, horloge), Météo, Langue. | **Configuration sans fil.** Modifiez tout le comportement du panneau depuis la PWA ; redémarrage automatique si nécessaire. |
| **💬 Contrôle du Texte Défilant** | Endpoints HTTP POST et PWA web app sur le réseau local pour envoyer des chaînes personnalisées, sélectionner la couleur, la palette et la vitesse de défilement. | **Interactivité distante.** Affichez des messages et notifications à la volée depuis n'importe quel appareil mobile ou ordinateur connecté au réseau sans reprogrammer. |
| **💥 Transition de Particules** | Moteur de particules dynamiques intégré pour les effets d'entrée et de sortie de l'heure. | **Fluidité visuelle.** Élimine les coupures statiques au profit d'un effet fluide et professionnel. |
| **🎨 Sélection de Couleur OSD** | Menu interactif à l'écran associé au récepteur IR et à la mémoire EEPROM/SD. | **Personnalisation.** Changez la couleur de l'horloge à la volée depuis la télécommande sans modifier le `config.ini`. |
| **⚡ Horloge Sans Scintillement** | Refactorisation de la logique de rendu utilisant un mode *Single Buffer* optimisé pour les interfaces. | **Image nette.** Élimination totale du scintillement (*flicker*) lors de la mise à jour rapide des données. |
| **🧠 Optimisation de la RAM** | Refactorisation des objets `String` en `char[]` et utilisation massive de `PSTR()` / `F()`. | **Zéro fragmentation.** Les textes sont stockés dans la Flash, libérant le Heap pour le Double Buffer. |
| **🛡️ Anti-Panic System** | Vérification de `display->begin()` avec basculement en Single Buffer en cas d'échec d'allocation RAM. | **Stabilité totale.** Évite les blocages (`StoreProhibited`) si la mémoire se fragmente après l'utilisation du WiFi. |
| **🖱️ Confirmation Sécurisée** | Logique de détection basée sur la durée de pression (*Long Press*) pour le bouton physique. | **Navigation Précise.** Évite les entrées accidentelles dans les menus ; vous confirmez désormais en maintenant enfoncé. |
| **📂 Serveur FTP Intégré** | Protocole de transfert de fichiers sans fil direct vers la carte SD de l'ESP32. | **Confort.** Gérez vos playlists, fichiers `.ini` et `.json` sans avoir à retirer la MicroSD. |
| **📡 Télécommande IR** | Cartographie dynamique des fonctions et navigation dans les menus via récepteur infrarouge. | **Contrôle à distance.** Gérez la luminosité, éteignez ou allumez le panneau et naviguez dans le menu confortablement depuis une télécommande. |
| **🎨 Configuration des Couleurs** | Paramètre `colorOrder` (RGB/RBG/GBR) traité dynamiquement depuis le `config.ini`. | **Polyvalence.** Compatibilité avec n'importe quel panneau HUB75 du marché sans reprogrammation. |
---
### 🖥️ Structure du Menu OSD (Navigation Intelligente)

Le système se contrôle à l'aide d'un **bouton unique**. Il utilise une logique de pression avancée qui s'adapte selon le menu dans lequel vous vous trouvez :
* **Pression Rapide :**
    * **Dans les Menus :** Déplacer le curseur / Naviguer vers le bas.
    * **En Mode Veille :** Réveille immédiatement le panneau (Wake-up).
* **Pression Maintenue :**
    * **Action Générale :** Entrer dans les sous-menus ou confirmer la sélection.
    * **Dans la Configuration de l'Heure (Minuteur) :** Soustrait **-5 minutes** à la valeur actuelle pour un ajustement rapide vers l'arrière.
* **Pression Très Longue (> 4 sec) :**
    * **Dérogation Manuelle (Manual Override) :** Force l'extinction (Mode Veille), en bloquant l'automatisme du minuteur jusqu'au prochain cycle.
* **Maintenir la Pression Continue :**
    * **Dans la Configuration de l'Heure (Minuteur) :** Augmente automatiquement **+5 minutes** de manière cyclique tant que vous maintenez la pression.

```text
🏠 MENU PRINCIPAL
├── 📂 Playlists
│   ├── 📄 Favoris
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Retour
├── 📂 Lecture
│   └── 🖼️ Mode : [GIFs / Horloge]
│   └── 🔀 Aléatoire : [OUI / NON]
│   └── 🕹️ Arcade : [OFF / Batocera / Recalbox / ReplayOS]
│   └── 💬 Texte : [OUI / NON]
│   └── 🔙 Retour
├── ☀️ Luminosité
│   └──   Luminosité : [5% - 100%]
├── 📶 WiFi : [ON / OFF]
│   ├── 🔄 Activer : [OUI / NON]
│   ├── 🔎 Afficher IP : [OUI / NON]
│   ├── 🏷️ IP : [192.169.1.117]
│   ├── 📱 Contrôle APP : [OUI / NON]
│   └── 🔙 Retour
├── 🕒 Horloge : [ON / OFF]
│   ├── 🔄 Activar : [OUI / NON]
│   ├── 🖼️ Tous les : [1...20] GIFs
│   ├── ⏳ Afficher : [5...30] sec
│   └── 🎨 Style Horloge : [Matrix, Solid, Rainbow, Pulse, Gradient]
│   └── 🎨 Couleur : [Blanc, Rouge, Vert, Bleu, Jaune, Cyan, Magenta, Orange et Rose]
│   ├── 🔄 Transition : [OUI / NON]
│   └── 🔙 Retour
├── 🌡️ Météo : [ON / OFF]
│   └── 🔄 Activer : [OUI / NON]
│   └── 🔙 Retour
├── 🕒 Minuteur : [ON / OFF]
│   ├── 🔄 Activer : [OUI / NON]
│   ├── ⏳ ON : [00:00 à 24:00]
│   ├── ⏳ OFF : [00:00 à 24:00]
│   └── 🔙 Retour
├── ⚙️ Réglages Avancés
│   ├── ⚡ Vitesse I2S : [8, 10, 16, 20MHz]
│   ├── 🔄 Rafraîchissement : [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer : [OUI / NON]
│   ├── 👻 AntiGhot : [1, 2, 3, 4]
│   ├── 🎮 Mappage Télécommande IR : [On, Off, Menu, Valider, Monter, Descendre, Luminosité+, Luminosité-]
│   ├── ⚠️ Réinitialiser :
│   └── 🔙 Retour
├── 🚀 Mise à Jour
│   └── 🔄 Rechercher OTA
│   ├── 🔤 Télécharger langues
│   └── 🔙 Retour
├── 📂 Explorateur SD
│   └── 🔄 Démarrer FTP
│   └── 🔙 Retour
├── 🌐 Langue
│   └── [ES] Espagnol
│   ├── [EN] Anglais
│   ├── [FR] Français
│   ├── ...
│   └── 🔙 Retour
├── 💾 Enregistrer
└── 🔙 Quitter
```

## 📱 PWA - Progressive Web App (Télécommande Complète)

### **[👉 Installer ou Essayer Retro Pixel LED Control](https://fjgordillo86.github.io/RetroPixelLED-Lite/control/)**

La PWA est une application web moderne, installable sur n'importe quel appareil (smartphone, tablette, ordinateur) connecté au même réseau local que le panneau. Elle ne nécessite aucun serveur externe, fonctionne entièrement sur le réseau local et reste accessible hors ligne une fois installée.

https://github.com/user-attachments/assets/f5231448-7862-4476-901e-ac25ac7f4248

#### 🎯 Caractéristiques Principales

L'interface est divisée en **5 sections indépendantes** accessibles depuis la barre de navigation :

**1️⃣ Page d'Accueil (Home)**
- **☀️ Contrôle de Luminosité :** Curseur 0-100% avec application instantanée, sans redémarrage.
- **🎛️ Sélecteur de Mode :** Choisissez entre GIF, Horloge ou Texte en temps réel.
  - **Mode GIF :** Affiche la playlist active + bascule de lecture aléatoire.
  - **Mode Horloge :** Choisissez parmi 5 styles (Matrix, Solid, Rainbow, Pulse, Gradient) et 9 couleurs personnalisées.
  - **Mode Texte :** Aperçu en direct de la matrice LED pendant que vous tapez, avec sélecteur de couleur et vitesse de défilement.
- **🔌 État de la Connexion :** Indicateur visuel (vert/rouge) du statut WiFi.

**2️⃣ Minuteur (⏰)**
- **Activer/Désactiver le minuteur** avec un interrupteur.
- **Sélecteur d'heure d'allumage** (format 24h, plage 00:00 - 23:59).
- **Sélecteur d'heure d'extinction** (même format).
- **Bouton d'allumage/extinction manuel immédiat** (dérogation).
- État actuel : panneau allumé (✓ vert) ou en veille (● gris).

**3️⃣ Mode Texte**
- **Aperçu de matrice 26×7 :** Affiche le texte en temps réel pendant que vous l'écrivez, simulant exactement le rendu sur le panneau.
- **Sélecteur de couleur :** Palette de 9 couleurs prédéfinies + sélecteur personnalisé hexadécimal.
- **Contrôle de vitesse :** Curseur 5-200ms/pas avec aperçu en direct.
- **Boutons d'action :** "▶ ENVOYER" (envoie le texte au panneau) et "■ STOP" (annule le défilement).

**4️⃣ Mise à Jour (🔄)**
- **OTA du Firmware :** Vérifie GitHub et télécharge/installe automatiquement si une nouvelle version est disponible.
- **Téléchargement des Langues :** Récupère tous les fichiers `.json` depuis le dossier des langues du dépôt GitHub et les enregistre dans `/idiomas` sur la SD, en remplaçant les précédents.

**5️⃣ Réglages (🛠)**
- **Édition distante complète de config.ini**, organisée en 7 sections :
  - **WiFi :** SSID, mot de passe, fuseau horaire.
  - **Matériel :** Nombre de panneaux, ordre des couleurs (RGB/RBG/GBR), luminosité, vitesse I2S, rafraîchissement minimum, buffering, anti-ghosting.
  - **Arcade :** Activer Batocera, Recalbox, ReplayOS, ou aucun.
  - **Texte Défilant :** Activer le défilement de texte à distance.
  - **Horloge :** Actif/inactif, transition avec particules, intervalle, durée, style, couleur.
  - **Météo :** Activer, ville, clé API OpenWeatherMap, intervalle de mise à jour, texte au-dessus de l'horloge.
  - **Langue :** Sélecteur (ES, EN, FR, ...).
  - **Redémarrage automatique** après enregistrement si des modifications le nécessitent.

#### ⚙️ Installation et Configuration

1. **Accès Web :** Ouvrez https://fjgordillo86.github.io/RetroPixelLED-Lite/control/ depuis le navigateur de votre appareil (smartphone, tablette ou ordinateur).
2. **Configurer l'IP :** Sur l'écran, appuyez sur l'icône de connexion (⚙) et entrez l'IP locale de votre panneau ESP32 (ex. 192.168.1.117).
3. **Installer comme Application (Optionnel) :**
   - Chrome/Edge : Une invite d'installation automatique devrait apparaître. Si ce n'est pas le cas, appuyez sur le menu (⋮) et sélectionnez "Installer l'application".
   - Firefox/Safari : Appuyez sur le menu de partage (↗) et choisissez "Sur l'écran d'accueil".
4. **Utilisation :** Une fois installée, elle apparaîtra comme une application normale sur votre appareil — accès rapide sans saisir d'URL.

#### 📝 Prérequis

- Le panneau doit être connecté au même réseau WiFi que votre appareil.
- Il doit être **activé** sur le panneau (`CONFI_APP_ENABLE=1`) pour pouvoir contrôler le Panneau LED depuis l'APPLICATION.
- Le mode **Texte Défilant** doit être **activé** sur le panneau (`config.ini: TEXT_ENABLE=1`) pour que les commandes d'envoi de messages fonctionnent.
- Pour mettre à jour le firmware ou les langues, le panneau doit avoir accès à Internet (accès à GitHub).
- Les fichiers de langue sont téléchargés une seule fois ; une fois enregistrés sur la carte SD, ils fonctionnent hors ligne.

---

### 📖 Comment utiliser le Script Générateur de Playlists (Windows)

Le script `Generador de Playlist v1.0.1.bat` facilite la création de collections personnalisées sans toucher à une seule ligne de code. Vous le trouverez dans le dossier "Contenido SD" [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD).

1. **Préparation :** Placez le fichier `.bat` à la **racine de votre carte SD**, à côté du dossier `gifs`.
2. **Exécution :** Double-cliquez sur le fichier. Une fenêtre de commande s'ouvrira.
3. **Sélection :** - Le script listera tous les sous-dossiers contenus dans `/gifs`.
   - Entrez les numéros des dossiers que vous souhaitez inclure dans la liste, séparés par des virgules (ex : `3,4,10`) ou tapez `TODO`.
4. **Nom :** Écrivez le nom que vous souhaitez donner à votre liste (ex : `MesFavoris`). 
5. **Résultat :** Le script créera automatiquement un dossier nommé `playlists` et y enregistrera le fichier `MesFavoris.txt` avec les chemins corrigés pour l'ESP32.
6. **Chargement :** Insérez la carte SD dans votre Retro Pixel LED, il lira la première playlist trouvée dans le dossier. Si vous souhaitez changer de playlist, entrez dans le menu OSD et sélectionnez-la dans "Playlists".
<img width="514" height="565" alt="Script PlayList" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />

### ⚙️ Fichier de Configuration (config.ini)
Remplace complètement l'interface web de la version standard. Il permet d'ajuster le comportement du matériel de manière persistante.
* **Emplacement dans le repo :** `/Contenido SD/`
* **Destination :** Le fichier `config.ini` doit être copié à la **racine de la Micro SD**.
* **Fonction :** Définit les identifiants WiFi pour la synchronisation horaire, la luminosité des LEDs, le style de l'horloge et la fréquence à laquelle la galerie est interrompue pour afficher l'heure.
---

## ⚙️ Installation et Configuration

### 1. 🚀 Programmer l'ESP32 (Web Installer)
Vous pouvez installer cette version sans rien installer sur votre PC en utilisant notre installateur basé sur Chrome/Edge :

### **[👉 Ouvrir l'Installateur Web Retro Pixel LED Lite](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

**Étapes pour l'installation :**
1. Utilisez un navigateur compatible (**Google Chrome** ou **Microsoft Edge**).
2. Connectez votre ESP32 au port USB de l'ordinateur.
3. Cliquez sur le bouton **"Install"** sur le site et sélectionnez le port COM correspondant.
4. **IMPORTANT :** Veillez à cocher la case **"Erase device"** dans l'assistant pour effectuer un nettoyage complet de la mémoire et éviter les erreurs de fragmentation.

> 💡 **Votre ESP32 n'est pas reconnu ?**
> Si aucun port COM n'apparaît lorsque vous cliquez sur "Install", il est probable que vous deviez installer les pilotes de la puce USB de votre carte :
> * **Puce CP2102 :** [Télécharger les Pilotes Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> * **Puce CH340/CH341 :** [Télécharger les Pilotes SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 Préparation de la Carte SD
Formatez votre MicroSD en **FAT32** et ajoutez tout le contenu du dossier [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) à la racine. La carte Micro SD aura la structure suivante :

```text
/ (Racine de la SD)
├── gifs/                        <-- Vos dossiers avec GIFs (Arcade, Consoles, etc.)
├── idioma/                      <-- Ici se trouveront les fichiers .json avec les textes traduits.
│   ├── ES.json                  <-- Dictionnaire ES.json.
│   ├── EN.json                  <-- Dictionnaire EN.json.
│   └── FR.json                  <-- Dictionnaire FR.json.
├── playlists/                   <-- Ici se trouveront les listes générées par le script "Generador de Playlists".
│   ├── Arcade.txt               <-- Liste .txt.
│   ├── Computers.txt            <-- Liste .txt.
│   ├── Consolas.txt             <-- Liste .txt.
│   └── Todos.txt                <-- Liste .txt.
├── config.ini                   <-- Configuration du WiFi et du Panneau.
└── Generador de Playlists.bat   <-- Script pour générer les Playlists.
```

>[!IMPORTANT]
>Si vous ajoutez, supprimez ou déplacez des GIFs dans le dossier `/gifs/`, veillez à exécuter à nouveau le script **Generador de Playlists.bat** pour mettre à jour l'index.

### 3. 📝 Configuration via `config.ini`
Le fichier nommé `config.ini` que vous trouverez dans le dossier "Contenido SD" [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) doit être ajouté à la racine de la carte SD et modifié pour personnaliser Retro Pixel LED Lite selon vos préférences :

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.1.2 - FICHIER DE CONFIGURATION
# ============================================================
# Remarque : Ne laissez pas d'espaces autour du symbole '='.
# Exemple correct : BRIGHTNESS=40

[WIFI_NTP]
# Configurez votre réseau WiFi
WIFI_ENABLE=1
SSID=Nom_De_Votre_Reseau
PASS=MotDePasse_De_Votre_Reseau
# Configurez votre fuseau horaire
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Nombre de panneaux
PANEL_CHAIN=2
# Ordre des couleurs du Panneau : RGB, RBG ou GBR
COLOR_ORDER=RGB
# Luminosité (0 à 255)
BRIGHTNESS=38
# Vitesse I2S : 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Rafraîchissement Minimum (Hz) : 30 à 120
REFRESH_MIN=120
# Double Buffer : 0=OFF, 1=ON (Élimine le scintillement)
DOUBLE_BUFF=0
# Anti-Ghosting : 1 à 4 (Augmentez si vous voyez de la rémanence lumineuse)
LATCH_BLANK=1

[LOGIC]
# Mode d'affichage : 0=GIFs, 1=Horloge Seule
PLAY_MODE=0
# Active ou désactive la configuration via l'APPLICATION : 0=OFF, 1=ON (Nécessite le WiFi)
CONFI_APP_ENABLE=1
# Sélectionnez votre système Arcade : 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS
ARCADE_ENABLE=0
# Active ou désactive le texte défilant : 0=OFF, 1=ON (Nécessite le WiFi)
TEXT_ENABLE=1
# Active ou désactive l'horloge : 0=OFF, 1=ON (Nécessite le WiFi)
CLOCK_ENABLE=1
# Mode de lecture : 0=Séquentiel, 1=Aléatoire
RANDOM_MODE=1
# Intervalle : Tous les combien de GIFs l'horloge apparaît
AUTO_CLOCK_INT=6
# Durée : Combien de secondes l'horloge s'affiche
CLOCK_DURATION=10
# Styles : 0=Matrix, 1=Solid, 2=Rainbow, 3=Pulse, 4=Gradient
CLOCK_STYLE=2
# Active la transition de l'horloge vers les GIFs avec une explosion de particules : 0=OFF, 1=ON
TRANSITION_ENABLE=1
# Couleur de l'horloge (0=Blanc, 1=Rouge, 2=Vert, 3=Bleu, 4=Jaune, 5=Cyan, 6=Magenta, 7=Orange, 8=Rose)
CLOCK_COLOR=4

[WEATHER]
# Active la météo : 0=OFF, 1=ON (Nécessite le WiFi)
WEATHER_ENABLE=1
# Votre ville (Sans espaces, utilisez '+' si nécessaire : Madrid,ES ou Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Votre clé API gratuite OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalle de mise à jour de la météo en MINUTES
WEATHER_INT=60
# Texte affiché au-dessus de l'horloge
WEATHER_MSG=Game Room

[LANGUAGE]
# Indique la Langue (Nom du fichier sans .json : ES, EN, FR...)
LANGUAGE=ES

[IR_REMOTE]
# Codes HEX de la télécommande IR (Inutile d'indiquer quoi que ce soit, Retro Pixel LED les enregistrera automatiquement)
BTN_ON=F20DFF00
BTN_OFF=E01FFF00
BTN_BRILLO_UP=F609FF00
BTN_BRILLO_DOWN=E21DFF00
BTN_MENU=EA15FF00
BTN_OK=ED12FF00
BTN_SUBIR=E41BFF00
BTN_BAJAR=B34CFF00

[REPLAY_OS]
# Adresse IP attribuée à ReplayOS
IP=192.168.1.101
# Token ReplayOS : SYSTEM > INFORMATION > NET CONTROL CODE
TOKEN=xxxxxx

[END]
```
---

### 4. 🌍 Configuration du Fuseau Horaire (TZ)

Pour que l'**Horloge** et le **Minuteur** fonctionnent correctement, le paramètre `timezone` dans le fichier `config.ini` doit suivre le format POSIX.

Exemple pour l'**Espagne (Péninsule et Baléares) / France / Italie** :
`timezone=CET-1CEST,M3.5.0,M10.5.0/3`
Exemple pour les **Canaries / Portugal / Royaume-Uni** :
`timezone=WET0WEST,M3.5.0/1,M10.5.0`

### Comment obtenir votre code TZ ?
Si vous habitez dans une autre région, vous pouvez obtenir le code exact de votre ville ici :
👉 **[ESP32 TZ Tool / Database](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)**

### Explication du format :
* **CET-1CEST** : Nom de la zone (Central European Time) et décalage de base (UTC+1).
* **M3.5.0** : Passage à l'heure d'été (Mars, semaine 5, Dimanche).
* **M10.5.0/3** : Passage à l'heure d'hiver (Octobre, semaine 5, Dimanche à 03:00).
  
### 5. ☁️ Comment obtenir votre API KEY Météo

Pour que la barre de notifications affiche la température et l'icône de la météo, vous avez besoin d'une clé gratuite de **OpenWeatherMap** :

1. Rendez-vous sur [OpenWeatherMap.org](https://openweathermap.org/) et créez un compte gratuit.
2. Une fois connecté, allez dans votre profil et cliquez sur **"My API Keys"**.
3. Générez une nouvelle Key (vous pouvez la nommer "RetroPixel").
4. **IMPORTANT :** La Key peut prendre entre **30 minutes et 2 heures** pour s'activer après sa création. Si le panneau affiche "0.0C", patientez simplement un moment.
5. Copiez cette clé dans la section `API_KEY=` de votre fichier `config.ini`.

### 🔍 Comment vérifier si le code de la ville est correct ?

Si vous souhaitez être 100% certain qu'**OpenWeatherMap** reconnaît votre ville avant d'enregistrer le fichier sur la Micro SD, vous pouvez effectuer ce test rapide dans votre navigateur :

1. Copiez l'adresse suivante dans la barre de votre navigateur.
2. Remplacez `Navalmoral de la Mata` par votre **Ville** réelle.
3. Remplacez `XXXXX` par votre **Clé API** réelle.

`http://api.openweathermap.org/data/2.5/weather?q=Navalmoral de la Mata,ES&appid=XXXXX`

* **Si le résultat est un texte avec des données (JSON) :** Le nom est parfait et l'ESP32 le lira sans problème !
* **Si le résultat est une erreur (401 ou 404) :** Vérifiez que votre Clé API est active (rappelez-vous qu'elle peut mettre jusqu'à 2 heures à s'activer) ou que le nom de la ville ne contient pas de coquilles.

### 6. ☁️ Mise à Jour du Système (OTA)
Il n'est plus nécessaire de connecter le panneau au PC pour le mettre à jour. Si une nouvelle version est disponible sur le dépôt :

1. Vérifiez que le WiFi est configuré et actif dans votre `config.ini`.
2. Accédez au menu OSD du panneau.
3. Naviguez jusqu'à **Mise à Jour > Rechercher OTA**.
4. Le système téléchargera le nouveau firmware depuis GitHub et redémarrera tout seul.

> [!WARNING]
> Ne débranchez pas l'alimentation du panneau pendant le processus de mise à jour.

### 7. 🌐 Guide du Système Multilingue (Fichiers .json)

La version v2.1.0 utilise un système de **Dictionnaires Dynamiques**. Contrairement à d'autres systèmes, le dictionnaire NE réside PAS en permanence dans la mémoire RAM ; il ne se charge que lorsque l'utilisateur entre dans le menu et se libère à la sortie. Cela garantit que le moteur de GIFs dispose de toute la mémoire disponible pour les animations.

#### 📂 Emplacement et Nomenclature
Les fichiers doivent se trouver dans le dossier `/idioma/` de la carte SD. Le nom du fichier (sans l'extension) est celui qui apparaîtra dans le menu de sélection.

- `/idioma/ES.json` -> Apparaîtra comme "ES"
- `/idioma/EN.json` -> Apparaîtra comme "EN"

#### 🛠️ Structure du Fichier JSON
Si vous souhaitez créer une nouvelle traduction, vous pouvez copier le fichier `ES.json` et le renommer. Les champs sont organisés par blocs :

1. **`MENU`** : Étiquettes du menu principal.
2. **`SUBMENU_XXX`** : Étiquettes spécifiques à chaque section.
3. **`ESTADOS`** : Mots courts d'état (ON, OFF, OUI, NON, RETOUR).
4. **`CONFIG_INI`** : Commentaires qui seront écrits dans le fichier de configuration physique de la SD.

#### ⚠️ Règles Critiques pour l'Édition
Afin d'éviter que le système ne subisse de blocages (*Kernel Panic*) ou d'erreurs visuelles, suivez ces règles :

* **🚫 Sans Accents ni Ñ :** La police actuelle du système ne prend pas en charge les caractères Unicode étendus. Utilisez `n` au lieu de `ñ` et évitez les accents (ex : `Actualizacion` au lieu de `Actualización`).
* **📏 Limite de Caractères :** Les étiquettes des sous-menus ne doivent pas dépasser **21 caractères** pour garantir un centrage parfait dans la zone de 128px sans dépasser des marges.
* **🔡 Format des Étiquettes :** Dans les sections de sous-menu, incluez le double-point et l'espace si vous souhaitez qu'ils apparaissent (ex : `"modo": "Mode : "`).
* **💾 Format UTF-8 :** Assurez-vous d'enregistrer le fichier au format **UTF-8 (sans BOM)** pour éviter l'apparition de caractères étranges au début de la lecture.

#### 🔄 Flux de Chargement
Lorsque vous changez de langue dans l'OSD :
1. Le système met à jour la valeur `LANGUAGE` dans le `config.ini`.
2. Le pointeur du dictionnaire est réinitialisé.
3. La prochaine fois que vous ouvrirez le menu, le système recherchera le fichier correspondant à la nouvelle configuration.

### 8. 📂 Explorateur SD (FTP)
Cette fonction active un serveur de fichiers sans fil sur votre Retro Pixel LED. Son objectif principal est de faciliter la maintenance du système sans devoir retirer la carte MicroSD.

> [!IMPORTANT]
> **Utilisation recommandée :** Cette fonction a été conçue spécifiquement pour gérer **les fichiers de configuration (`config.ini`)**, **les fichiers de langue (`ES.json`)**, l'édition de **playlists (`.txt`)** et les fichiers de petite taille. En raison des limitations de bande passante du matériel ESP32, **elle n'est pas recommandée pour le transfert massif de collections de GIFs**, car le processus serait extrêmement lent par rapport à un lecteur de carte conventionnel.

#### 🚀 Comment activer le serveur FTP
1. Naviguez dans le menu OSD jusqu'à **Explorateur SD**.
2. Sélectionnez l'option **Démarrer FTP**.
3. Le panneau arrêtera la lecture des GIFs et affichera :
   * **Adresse IP :** (ex. `192.168.1.109`)

#### 💻 Configuration de la connexion
Il est recommandé d'utiliser un client comme **FileZilla** avec les données suivantes :

* **Protocole :** Protocole de transfert de fichiers FTP.
* **Serveur/Hôte :** L'adresse IP qui apparaît sur votre panneau LED.
* **Chiffrement :** Utiliser uniquement un FTP simple.
* **Type d'authentification :** Normal
* **Identifiant :** `admin`
* **Mot de passe :** `admin`
* **Port :** `21`
* **Mode de transfert :** Par défaut
* **Limiter le nombre de connexions simultanées :** Activé
* **Nombre maximum de connexions :** 1
  
<img width="545" height="227" alt="image" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />

<img width="544" height="193" alt="image" src="https://github.com/user-attachments/assets/ba4c85bc-920a-48c9-83d8-99b96ecbc57f" />

**Dans Édition -> Options -> Transferts**
* **Nombre maximal de transferts simultanés :** 1
* **Activer les limites de vitesse :** Activé
* **Limite de téléchargement :** 20 KiB/s
* **Limite d'envoi :** 20 KiB/s
<img width="841" height="522" alt="image" src="https://github.com/user-attachments/assets/e90d3e84-9c93-45c0-b942-8b601db40041" />

---
Si vous ne souhaitez pas installer de logiciel supplémentaire tel que FileZilla, vous pouvez intégrer la carte SD du panneau directement sur votre ordinateur comme s'il s'agissait d'un dossier en utilisant l'**Explorateur de Fichiers** :

`(Cette option n'est pas recommandée, lors des tests il est arrivé que les fichiers ne soient pas chargés complètement, provoquant des erreurs)`

1. **Ouvrir l'Explorateur :** Allez dans **Ce PC** sur votre ordinateur.
2. **Ajouter un emplacement :** Faites un clic droit sur un espace blanc de la fenêtre et sélectionnez **"Ajouter un emplacement réseau"**.
3. **Configurer l'adresse :** Lorsque l'assistant demande l'adresse réseau, entrez l'IP affichée par votre panneau avec le préfixe FTP.
   * Exemple : `ftp://192.168.1.109`
4. **Identifiants :** Décochez la case "Ouvrir une session anonyme" et renseignez l'utilisateur : `admin`.
5. **Terminer :** Donnez un nom descriptif au lecteur (ex : `Retro Pixel LED`) pour l'identifier facilement à l'avenir.

#### ⚠️ Remarques de sécurité et d'utilisation
* **Verrouillage de l'écran :** Tant que le FTP est actif, le panneau ne lira pas de GIFs afin de consacrer tout le CPU au transfert de données.
* **Sortie sécurisée :** Pour fermer le serveur et revenir au mode normal, appuyez sur le bouton physique ou utilisez la touche "Valider" de votre télécommande IR.
* **Attention à la mise hors tension :** Ne débranchez pas l'alimentation pendant l'édition d'un fichier via FTP, car le fichier pourrait être corrompu.

### 9. 🕹️ Intégration avec Batocera, Recalbox ou ReplayOS (Arcade)
Si vous souhaitez activer l'affichage par Retro Pixel LED Lite des marquees du jeu lancé ou du système parcouru, vous devez activer l'option **Arcade** dans le menu.
```
🏠 MENU PRINCIPAL
├── 📂 Lecture
│   └── 🖼️ Mode : [GIFs / Horloge]
│   └── 🔀 Aléatoire : [OUI / NON]
│   └── 🕹️ Arcade : [OFF / Batocera / Recalbox / ReplayOS]   <-- ICI
│   └── 🔙 Retour
```
> [!IMPORTANT]
> ### 🕹️ Configuration de Batocera, Recalbox ou ReplayOS
> Pour apprendre à synchroniser vos ROMs, utiliser le script PC et installer les scripts de communication, consultez notre guide détaillé :
> 
> **[👉 CLIQUEZ ICI POUR VOIR LES INSTRUCTIONS POUR BATOCERA](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README_BATOCERA.md)**
> 
> **[👉 CLIQUEZ ICI POUR VOIR LES INSTRUCTIONS POUR RECALBOX](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README_RECALBOX.md)**
> 
> **[👉 CLIQUEZ ICI POUR VOIR LES INSTRUCTIONS POUR REPLAYOS](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_REPLAYOS.md)**
---
### 10. 🏠 Intégration avec Home Assistant

Vous pouvez intégrer et contrôler totalement **RetroPixel LED Lite** depuis **Home Assistant** via l'API REST locale, sans dépendre du cloud. 

Cette intégration vous permet de :
- 🟢 **Allumer / Éteindre** le panneau à l'aide d'un interrupteur (*switch*).
- 📊 **Consulter l'état actuel** (mode actif, playlist en cours de lecture, etc.).
- 🔄 **Changer de mode** (Horloge / GIF) et de **Playlist** instantanément.
- 💬 **Envoyer des messages texte défilants** en choisissant la couleur, la vitesse et la police depuis le Dashboard.

---

### 📦 1. Ajouter la configuration à Home Assistant

Si vous utilisez la structure de dossiers par packages (`packages`), enregistrez le fichier nommé `retropixel.yaml` disponible **[ICI](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Home%20Asisstant)** dans le dossier `/config/packages/`. Si vous n'utilisez pas les packages, collez le contenu dans votre fichier `configuration.yaml`.

> ⚠️ **IMPORTANT :** Remplacez l'adresse IP `192.168.31.210` par l'adresse IP attribuée à votre ESP32 et adaptez le nom des playlists avec les vôtres.

Une carte `entities.yaml` est également disponible pour l'ajouter à votre tableau de bord.
<img width="822" height="1005" alt="Captura HA" src="https://github.com/user-attachments/assets/9294b479-f428-4c68-9fcc-c871ad2e88e4" />


## 🧠 Fonctionnalités Core LITE

* **📡 Contrôle IR & Mappage Dynamique :** Prise en charge complète des télécommandes infrarouges avec mappage des fonctions depuis le menu OSD (Luminosité, Navigation, Power Toggle et Confirmation).
* **📂 Serveur FTP de Maintenance :** Permet la gestion sans fil du fichier `config.ini` et des listes de lecture. Idéal pour des ajustements rapides sans retirer la carte MicroSD.
* **Gestion RAM Anti-Panique :** Système de surveillance du *heap*. Si le DMA ne peut pas allouer de mémoire après l'activité WiFi, le système bascule en Single Buffer pour garantir une stabilité totale.
* **Moteur de Recherche Binaire (Arcade) :** Capacité à localiser des marquees parmi des milliers de fichiers en quelques millisecondes. Le système ne "scanne" pas les dossiers, mais saute directement à la position du fichier sur la SD grâce à des index triés par ordre alphabétique.
* **Mémoire Adaptative (Single/Double Buffer) :** Gestion intelligente de la RAM. Le système utilise le *Double Buffer* pour une fluidité totale dans les GIFs, mais bascule automatiquement en *Single Buffer* en mode Arcade afin de garantir une stabilité absolue lors du chargement de bitmaps haute définition.
* **API HTTP en Temps Réel :** Récepteur de commandes intégré permettant la synchronisation avec des systèmes externes tels que Batocera ou RetroPie pour le changement dynamique des marquees.
* **Smart Text Centering :** Moteur dynamique qui aligne automatiquement les menus et les états au centre de la matrice (`offset + 64px`) en calculant la largeur de chaque chaîne de texte.
* **WiFi Stealth Mode :** L'ESP32 n'active le WiFi que brièvement pour synchroniser l'heure et la météo. Le reste du temps, le système reste **100% hors ligne**, garantissant **0 latence** lors de la lecture des GIFs.
* **Barre de Notifications Dynamique :** Si vous activez la météo, l'horloge abaisse automatiquement sa position (`startY=9`) pour afficher le message personnalisé (`WEATHER_MSG`), l'icône météo et la température.
* **Icônes en Bitmap :** Inclut des icônes optimisées de 8x8 pixels dessinées à la main pour représenter : Soleil, Nuages, Pluie, Neige, Orage et Brouillard.
* **Iconographie Avancée (Jour/Nuit) :** Inclut des icônes de 8x8 pixels dessinées à la main représentant : Soleil, Lune (Nuit), Nuages, Pluie, Neige, Orage et Brouillard, s'adaptant dynamiquement en fonction de la tranche horaire.
* **Système de Playlists Dynamiques :** Remplace l'ancien moteur à liste unique. Désormais, le système peut gérer plusieurs fichiers `.txt` dans le dossier `/playlists/`, permettant de passer d'une collection thématique à une autre (Arcade, Consoles, Favoris, etc.) depuis le menu OSD.
* **Horloge à Auto-Interruption :** Le panneau interrompt la galerie tous les "x" GIFs pour afficher l'heure pendant "x" secondes (tous deux configurables depuis le menu OSD et dans config.ini), puis reprend la lecture exactement là où elle s'était arrêtée.
* **Résilience Hors Ligne :** Si aucun réseau WiFi n'est disponible, le système ignore la synchronisation et commence à lire immédiatement les GIFs en utilisant l'horloge interne de la puce.
* **Moteur de Rendu Double Buffer :** Exploite le DMA de l'ESP32 pour afficher les images de manière invisible, obtenant une fluidité absolue et éliminant toute trace de scintillement dans les animations.

## 🛒 Liste du Matériel

Pour garantir la compatibilité, il est recommandé d'utiliser les composants testés lors du développement :

* **Microcontrôleur :** [ESP32 DevKit V1 (30 broches) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
* **Panneau LED Matrix (HUB75) :** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)
* **Lecteur de Carte :** [Module Adaptateur Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
* **Carte de connexion ESP32-Panneau LED :** [DMDos Board V3 - Mortaca ](https://www.mortaca.com/) (Optionnel, aucune soudure requise et intègre un lecteur SD)
* **Récepteur IR :** [Capteur récepteur infrarouge Universel - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)
* **Bouton Poussoir :** [Interrupteur momentané au choix DS-316 - AliExpress](https://es.aliexpress.com/item/4000888761296.html)
* **Alimentation :** Bloc d'alimentation 5V (Minimum 2A recommandé pour les panneaux de 64x32).

---
## ⚙️ Installation

### 1. 🔌 Connexions 
Si vous utilisez la DMDos Board V3, cette partie est déjà traitée, passez au point suivant.

#### 📂 Lecteur de Carte Micro SD (Interface SPI)
| Broche SD | Broche ESP32 | Fonction |
| :--- | :--- | :--- |
| **CS** | GPIO 5 | Chip Select |
| **CLK** | GPIO 18 | Clock |
| **MOSI** | GPIO 23 | Master Out Slave In |
| **MISO** | GPIO 19 | Master In Slave Out |
| **VCC** | 3.3V | Alimentation |
| **GND** | GND | Masse |

#### 🖼️ Panneau LED RGB (Interface HUB75)
| Broche Panneau | Broche ESP32 | Fonction |
| :--- | :--- | :--- |
| **R1** | GPIO 25 | Données Rouge (Supérieur) |
| **G1** | GPIO 26 | Données Vert (Supérieur) |
| **B1** | GPIO 27 | Données Bleu (Supérieur) |
| **R2** | GPIO 14 | Données Rouge (Inférieur) |
| **G2** | GPIO 12 | Données Vert (Inférieur) |
| **B2** | GPIO 13 | Données Bleu (Inférieur) |
| **A** | GPIO 33 | Sélection de Ligne A |
| **B** | GPIO 32 | Sélection de Ligne B |
| **C** | GPIO 22 | Sélection de Ligne C |
| **D** | GPIO 17 | Sélection de Ligne D |
| **E** | GND | Masse |
| **CLK** | GPIO 16 | Clock |
| **LAT** | GPIO 4 | Latch |
| **OE** | GPIO 15 | Output Enable (Luminosité) |

#### 🕹️ Contrôle Utilisateur Menu OSD (Physique et Infrarouge)

Le système permet un contrôle total via un bouton physique (avec logique de pression longue) et un récepteur IR pour la manipulation à distance.

| Composant | Broche ESP32 | Fonction |
| :--- | :--- | :--- |
| **Bouton (PIN)** | GPIO 21 | **Multifonction :** Clic (Naviguer) / Pression Longue (Confirmer - Power Toggle). |
| **Bouton (GND)** | GND | Référence masse. |
| **Récepteur IR (Data)** | GPIO 34 | Entrée signal (Protocole NEC/etc). |
| **Récepteur IR (VCC)** | 3.3V | Alimentation du capteur. |
| **Récepteur IR (GND)** | GND | Référence masse. |

<img width="769" height="716" alt="image" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />


---

## 🛠️ Feuille de Route (Roadmap LITE)

### ⚡ Optimisation & Fonctionnalités

### 🎨 Esthétique & Connectivité

---

## ⚖️ Licence et Remerciements

Ce projet est publié sous **Licence MIT**.

Remerciements particuliers aux développeurs des bibliothèques de base :
* **Bitbank2** pour la formidable bibliothèque `AnimatedGIF`.
* **Mrfaptastic** pour le moteur DMA haute performance dédié aux matrices.
* **Communauté Telegram DMDos** : en la découvrant et en voyant ce dont DMDos était capable, j'ai été encouragé à développer **Retro Pixel LED**.
* **RpiTe@m** pour le partage **gratuit** du pack de 600 GIFs et sa collection incroyable de 11000 GIFs disponible [ici.](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065)
* **shan-aya** pour la traduction en français et son magnifique logiciel de création de [GIFs.](https://github.com/shan-aya/DMD_GIF_converter)
* **joseAveleira** pour l'effet de particules sur l'Horloge. [GitHub](https://github.com/joseAveleira/RelojPixel/tree/main)
