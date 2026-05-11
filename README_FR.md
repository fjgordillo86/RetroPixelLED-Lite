# ✨ Retro Pixel LED Lite v2.1.4
**[🇪🇸 Español](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md) | [🇫🇷 Français](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_FR.md)**

### **[✈️ Rejoindre le Groupe Telegram : Retro Pixel LED pour rester informé des mises à jour](https://t.me/RetroPixelLed)**


## 💡 Description du Projet

**Retro Pixel LED Lite** est la version haute performance conçue pour ceux qui recherchent une stabilité absolue, une vitesse instantanée et un système sans maintenance. Contrairement à la version standard, le firmware LITE élimine la charge du serveur web et la connectivité permanente pour consacrer 100 % de la puissance de l’ESP32 au rendu des GIFs.

Si la branche 2.x.x a introduit le Menu OSD, la nouvelle v3.0.0 représente le saut définitif vers l’indépendance matérielle. Cette version transforme le panneau LED en un dispositif intelligent autonome, supprimant complètement la nécessité de connecter l’ESP32 à l’ordinateur pour les tâches de maintenance ou de configuration.
Pour la première fois, le système permet l’édition des fichiers de configuration (config.ini) et la gestion des bibliothèques de playlists directement depuis l’Explorateur Windows ou les clients FTP, transformant la carte SD en une unité réseau sans fil.
Un support natif pour les télécommandes est intégré, permettant de naviguer dans le Menu OSD, d’ajuster la luminosité dynamique et de contrôler la mise sous tension/hors tension depuis le canapé.

> [!TIP]
> **🚀 Philosophie Lite :** Moins, c’est plus. En éteignant le WiFi après avoir synchronisé l’heure et la météo, le système élimine le lag, réduit la chaleur du chip et évite les blocages dus à la saturation du réseau, permettant une lecture fluide de collections massives.

Si vous voulez essayer la version standard, voici le lien vers le **[GitHub.](https://github.com/fjgordillo86/RetroPixelLED)**

Voulez-vous créer vos propres GIFs ? Voici deux outils magnifiques.

- [DMD GIF converter](https://github.com/shan-aya/DMD_GIF_converter) créé par **shan-aya**.
- [Video a GIF](https://p4blogc.github.io/dmdos-converter/) créé par **p4bloGC**.
---

## 🆕 Nouveautés de la Version v3.0.0 Lite

| Caractéristique           | Détail Technique                                                                                  | Bénéfice                                                                                       |
| :---------------------- | :----------------------------------------------------------------------------------------------- | :--------------------------------------------------------------------------------------------- |
| **🛡️ Système Anti-Panique** | Vérification de `display->begin()` avec redémarrage automatique en cas d’échec d’allocation RAM.      | **Stabilité totale.** Évite les plantages (`StoreProhibited`) si la mémoire se fragmente après utilisation du WiFi.             |
| **🖱️ Confirmation Sécurisée** | Logique de détection basée sur la durée d’appui (*Long Press*) pour le bouton physique.          | **Navigation précise.** Évite les entrées accidentelles dans les menus ; la confirmation se fait désormais via un appui prolongé.               |
| **📂 Serveur FTP Intégré** | Protocole de transfert de fichiers sans fil directement vers la carte SD de l’ESP32.         | **Confort.** Gérez vos playlists, fichiers `.ini` et `.json` sans avoir à extraire la MicroSD.                 |
| **📡 Télécommande IR** | Mappage dynamique des fonctions et navigation des menus via récepteur infrarouge.               | **Contrôle à distance.** Gérez la luminosité, allumez ou éteignez le panneau et naviguez dans le menu confortablement depuis une télécommande. |
| **🎨 Configuration des Couleurs** | Paramètre `colorOrder` (RGB/RBG/GBR) traité dynamiquement depuis le `config.ini`.             | **Polyvalence.** Compatible avec tous les panneaux HUB75 du marché sans nécessiter de reprogrammation.                 |

---

## 🕹️ Intégration Spéciale : Mode Arcade (Batocera)

Cette version Lite introduit une prise en charge avancée pour les systèmes de retrogaming. Grâce à une hiérarchie de fichiers intelligente, le panneau peut afficher :

1. **Marquise du Jeu :** Récupérée directement depuis ta collection scrapée dans Batocera.  
2. **Logo du Système :** Image de secours si le jeu ne possède pas d’art spécifique.  
3. **Ressource Maître :** Image par défaut si le système n’est pas indexé.

> [!IMPORTANT]  
> Un outil est inclus dans `/Batocera/tools` pour automatiser le redimensionnement en 128×32, la conversion en BMP 24 bits et la génération des index pour une réponse instantanée de l’ESP32.  
> Consulte le [README spécifique de Batocera](/README_BATOCERA_FR.md) pour plus de détails.

---

## 📜 Historique détaillé des changements (v2.1.4 -> v3.0.0)

| Type             | Composant   | Description du changement                                                                                  |
| :--------------- | :---------- | :-------------------------------------------------------------------------------------------------------- |
| **🛡️ Stabilité**  | **Noyau (DMA)**    | **Système Anti-Panique :** Mise en place de la détection de fragmentation de la RAM avec redémarrage préventif automatique.    |
| **✨ Nouveau**    | **Fichiers**       | **Serveur FTP :** Accès distant à la carte SD pour charger des GIFs et éditer la configuration sans fil.                   |
| **✨ Nouveau**    | **Contrôle**       | **Télécommande IR :** Mappage complet des fonctions du menu OSD pour le contrôle à distance par infrarouge.               |
| **⚡ Amélioration** | **Bouton**        | **Appui Long :** Modification de la logique de confirmation ; il faut maintenant maintenir le bouton appuyé pour éviter les erreurs. |
| **⚡ Amélioration** | **Matériel**      | **colorOrder :** Possibilité d’alterner entre RGB, RBG et GBR directement depuis le fichier `config.ini`.                  |
| **⚡ Amélioration** | **Réseau**        | **WiFi à la demande :** Le système ignore la phase de connexion si `wifiEnable` est désactivé, économisant ainsi la RAM critique.         |
| **⚡ Amélioration** | **Timer**         | **Smart Sync :** Ajustement fin des intervalles (+5 min) et resynchronisation automatique après intervention manuelle.         |
| **⚡ Amélioration** | **Mémoire**       | **Nettoyage agressif :** Déconnexion forcée de la radio et vidage des buffers après mise à jour Clima/NTP.                    |

---

### 🖥️ Structure du Menu OSD (Navigation Intelligente)

Le système est contrôlé au moyen d’un **seul bouton**. Il utilise une logique de pression avancée qui s’adapte selon le menu où vous vous trouvez :

* **Pression Rapide (>0,5 sec) :**
    * **Dans les Menus :** Déplacer le curseur / Naviguer vers le bas.
    * **En Mode Sommeil :** Réveille immédiatement le panneau (Wake-up).

* **Pression Longue (1 à 2 sec) :**
    * **Action Générale :** Entrer dans les sous-menus ou confirmer une sélection.
    * **Dans la Configuration du Temps (Minuterie) :** Soustrait **-5 minutes** à la valeur actuelle pour un ajustement rapide vers l’arrière.

* **Pression Extra Longue (> 4 sec) :**
    * **Manual Override :** Force l’extinction (Mode Sommeil), bloquant l’automatisme du temporisateur jusqu’au prochain cycle.

* **Maintenir Pressé en Continu :**
    * **Dans la Configuration du Temps (Minuterie) :** Augmente automatiquement **+5 minutes** de manière cyclique tant que vous maintenez la pression.

---

🏠 **MENU PRINCIPAL**  
├── 📂 Playlists  
│   ├── 📄 Favoris  
│   ├── 📄 Arcade  
│   ├── 📄 ...  
│   └── 🔙 Retour  
├── 📂 Lecture  
│   └── 🖼️ Mode : [GIFs / Horloge]  
│   └── 🔀 Aléatoire : [OUI / NON] 
│   └── 🕹️ Arcade : [OUI / NON] 
│   └── 🔙 Retour  
├── ☀️ Luminosité  
│   └──   Luminosité : [5% - 100%]  
├── 📶 WiFi : [ON / OFF]  
│   ├── 🔄 Activer : [OUI / NON]  
│   └── 🔙 Retour  
├── 🕒 Horloge : [ON / OFF]  
│   ├── 🔄 Activer : [OUI / NON]  
│   ├── 🖼️ Toutes les : [1...20] GIFs  
│   ├── ⏳ Afficher : [5...30] sec  
│   └── 🎨 Style Horloge : [Matrix, Solid, Rainbow, Pulse, Gradient]  
│   └── 🔙 Retour  
├── 🌡️ Météo : [ON / OFF]  
│   └── 🔄 Activer : [OUI / NON]  
│   └── 🔙 Retour  
├── 🕒 Minuterie : [ON / OFF]  
│   ├── 🔄 Activer : [OUI / NON]  
│   ├── ⏳ ON : [00:00 à 24:00]  
│   ├── ⏳ OFF : [00:00 à 24:00]  
│   └── 🔙 Retour  
├── ⚙️ Réglages Avancés  
│   ├── ⚡ I2sSeep : [8, 10, 16, 20MHz]  
│   ├── 🔄 Balayage : [30, 60, 90, 120Hz]  
│   ├── 🖼️ Buffer : [OUI / NON]  
│   ├── 👻 AntiGhost : [1, 2, 3, 4]
│   ├── 🎮 Mappage télécommande IR : [On, Off, ,Menu, Valider, Monter, Descendre, Luminosité+, Luminosité-]
│   ├── ⚠️ Réinitialiser :  
│   └── 🔙 Retour  
├── 🚀 Mise à jour  
│   └── 🔄 Rechercher OTA  
│   └── 🔙 Retour  
├── 🌐 Langue  
│   └── [ES] Español  
│   ├── [EN] English  
│   ├── [FR] Français  
│   ├── ...  
│   └── 🔙 Retour  
├── 💾 Sauvegarder  
└── 🔙 Quitter


```
⏰ Fonctionnement du Sous-menu Minuterie

Pour faciliter la configuration des heures d’allumage (hon/mon) et d’extinction (hoff/moff), une logique de sauts de 5 minutes a été implémentée :
1.Vous voulez avancer rapidement ? Maintenez le bouton appuyé. Le temps augmentera de 5 en 5 minutes sans que vous ayez à relâcher.
2.Vous avez dépassé l’heure ? Faites une pression longue (une seconde) et le temps reculera de 5 minutes.
   
## 🛠️ Outils Exclusifs Lite

### 📖 Comment utiliser le Script Générateur de Playlists (Windows)

Le script `Generador de Playlist v1.0.1.bat` facilite la création de collections personnalisées sans toucher une seule ligne de code. Vous le trouverez dans le dossier "Contenido SD" [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD).

1. **Préparation :** Placez le fichier `.bat` à la **racine de votre carte SD**, juste à côté du dossier `gifs`.
2. **Exécution :** Double-cliquez sur le fichier. Une fenêtre de commandes s’ouvrira.
3. **Sélection :**
   - Le script listera tous les sous-dossiers dans `/gifs`.
   - Entrez les numéros des dossiers que vous souhaitez inclure dans la liste, séparés par des virgules (ex : `3,4,10`) ou écrivez `TODO`.
4. **Nom :** Écrivez le nom que vous souhaitez pour votre liste (ex : `MesFavoris`).
5. **Résultat :** Le script créera automatiquement un dossier appelé `playlists` et y enregistrera le fichier `MesFavoris.txt` avec les chemins corrigés pour l’ESP32.
6. **Chargement :** Insérez la SD dans votre Retro Pixel LED, il lira la première playlist qu’il trouve dans le dossier. Si vous souhaitez changer de playlist, entrez dans le menu OSD et sélectionnez-la dans "Playlists".

<img width="514" height="565" alt="Script PlayList" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />


### ⚙️ Fichier de Configuration (config.ini)
Il remplace complètement l’interface web de la version standard. Il permet d’ajuster le comportement du matériel de manière persistante.

* **Emplacement dans le dépôt :** `/Contenido SD/`
* **Destination :** Le fichier config.ini doit être copié à la **racine de la Micro SD**.
* **Fonction :** Définit les identifiants WiFi pour la synchronisation horaire, la luminosité des LEDs, le style de l’horloge et la fréquence à laquelle la galerie est interrompue pour afficher l’heure.
---

## ⚙️ Installation et Configuration

### 1. 🚀 Programmer l’ESP32 (Web Installer)
Vous pouvez installer cette version sans rien installer sur votre PC en utilisant notre installateur basé sur Chrome/Edge :

### **[👉 Ouvrir l’Installateur Web Retro Pixel LED Lite](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

**Étapes pour l’installation :**
1. Utilisez un navigateur compatible (**Google Chrome** ou **Microsoft Edge**).
2. Connectez votre ESP32 au port USB de l’ordinateur.
3. Cliquez sur le bouton **"Install"** sur la page web et sélectionnez le port COM correspondant.
4. **IMPORTANT :** Assurez-vous de cocher la case **"Erase device"** dans l’assistant pour effectuer un nettoyage complet de la mémoire et éviter les erreurs de fragmentation.

> 💡 **Votre ESP32 n’est pas reconnu ?**  
> Si en appuyant sur "Install" aucun port COM n’apparaît, il est probable que vous deviez installer les drivers du chip USB de votre carte :
> * **Chip CP2102 :** [Télécharger Drivers Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> * **Chip CH340/CH341 :** [Télécharger Drivers SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 Préparation de la Carte SD
Formatez votre MicroSD en **FAT32**, ajoutez les fichiers Generador de Playlists v1.0.1.bat et config.ini, et organisez la Micro SD de la manière suivante :

```text
/ (Racine de la SD)  
├── gifs/                        <-- Tes dossiers avec les GIFs (Arcade, Consoles, etc.)  
├── idioma/                      <-- Ici se trouveront les fichiers .json avec les textes traduits.  
│   ├── ES.json                  <-- Dictionnaire ES.json.  
│   ├── EN.json                  <-- Dictionnaire EN.json.  
│   └── FR.json                  <-- Dictionnaire FR.json.  
├── playlists/                   <-- Ici se trouveront les listes générées par le script « Générateur de Playlists ».  
│   ├── Mes Favoris.txt          <-- Liste .txt.  
│   ├── Metal Slug.txt           <-- Liste .txt.  
│   └── Tous.txt                 <-- Liste .txt.
├── arcade/                      <-- Intégration avec Batocera Marquesinas. (Uniquement si tu utilises le Mode Arcade)
│   ├── neogeo.txt               <-- Liste des marquesinas.txt. (Uniquement si tu utilises le Mode Arcade)
│   ├── mame.txt                 <-- Liste des marquesinas.txt. (Uniquement si tu utilises le Mode Arcade)
│   ├── neogeo/                  <-- Dossier contenant les marquesinas pour le système neogeo. (Uniquement si tu utilises le Mode Arcade)
│   │   ├── mslug.bmp            <-- Image de marquise. (Uniquement si tu utilises le Mode Arcade)
│   │   └── kof98.bmp            <-- Image de marquise. (Uniquement si tu utilises le Mode Arcade)
│   ├── mame/                    <-- Dossier contenant les marquesinas pour le système mame. (Uniquement si tu utilises le Mode Arcade)
│   │   ├── logo.bmp             <-- Image de marquise. (Uniquement si tu utilises le Mode Arcade)
│   └── └── pacman.bmp           <-- Image de marquise. (Uniquement si tu utilises le Mode Arcade)
├── config.ini                   <-- Configuration du WiFi et du Panneau.  
└── Générateur de Playlists.bat  <-- Script pour générer les Playlists.


```
[!IMPORTANT]
Si vous ajoutez, supprimez ou déplacez des GIFs dans le dossier /gifs/, assurez-vous d’exécuter à nouveau le script Generador de Playlists.bat pour mettre à jour l’index.

### 3. 📝 Configuration via config.ini
Le fichier nommé `config.ini` que vous trouverez dans le dossier "Contenido SD" [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) doit être ajouté à la racine de la carte SD et modifié pour configurer Retro Pixel LED Lite selon vos préférences :

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.0.0 - FICHIER DE CONFIGURATION
# ============================================================
# Remarque : Ne laissez pas d'espaces autour du symbole '='.
# Exemple correct : BRIGHTNESS=40

[WIFI_NTP]
# Configurez votre réseau WiFi
WIFI_ENABLE=1
SSID=Nom_De_Votre_Réseau
PASS=Mot_de_Passe_De_Votre_Réseau
# Configurez votre fuseau horaire
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Nombre de panneaux
PANEL_CHAIN=2
# Ordre des couleurs du panneau : RGB, RBG ou GBR
COLOR_ORDER=RGB
# Luminosité (de 0 à 255)
BRIGHTNESS=43
# Vitesse I2S : 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Rafraîchissement minimum (Hz) : de 30 à 120
REFRESH_MIN=120
# Double buffer : 0=OFF, 1=ON (élimine le clignotement)
DOUBLE_BUFF=1
# Anti-Ghosting : de 1 à 4 (augmentez si vous voyez des "fantômes")
LATCH_BLANK=1

[LOGIC]
# Mode d'affichage : 0=GIFs, 1=Horloge seule
PLAY_MODE=0
# Active la réception de bandeaux depuis Batocera : 0=OFF, 1=ON
ARCADE_ENABLE=0
# Active ou désactive l'horloge : 0=OFF, 1=ON (requiert WiFi)
CLOCK_ENABLE=1
# Mode de lecture : 0=Séquentiel, 1=Aléatoire
RANDOM_MODE=1
# Intervalle : tous les combien de GIFs l'horloge apparaît
AUTO_CLOCK_INT=6
# Durée : nombre de secondes d'affichage de l'horloge
CLOCK_DURATION=10
# Styles : 0=Matrix, 1=Solide, 2=Arc-en-ciel, 3=Pulsation, 4=Gradient
CLOCK_STYLE=2
# Couleur de l'horloge (format HEX)
CLOCK_COLOR=#FF0055

[WEATHER]
# Active la météo : 0=OFF, 1=ON (requiert CLOCK_ENABLE=1)
WEATHER_ENABLE=1
# Votre ville (sans espaces, utilisez '+' si nécessaire : Madrid,ES ou Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Votre clé API gratuite OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalle de mise à jour en MINUTES
WEATHER_INT=60
# Texte affiché au-dessus de l'horloge
WEATHER_MSG=Game Room

[LANGUAGE]
# Indique la langue (nom du fichier sans .json : ES, EN, FR...)
LANGUAGE=ES

[IR_REMOTE]
# Codes HEX de la télécommande IR (aucune saisie nécessaire, Retro Pixel LED les enregistrera automatiquement)
BTN_ON=F20DFF00
BTN_OFF=E01FFF00
BTN_BRILLO_UP=F609FF00
BTN_BRILLO_DOWN=E21DFF00
BTN_MENU=EA15FF00
BTN_OK=ED12FF00
BTN_SUBIR=E41BFF00
BTN_BAJAR=B34CFF00

[END]
```
---

### 4. 🌍 Configuration du Fuseau Horaire (TZ)

Pour que l’**Horloge** et la **Minuterie** fonctionnent correctement, le paramètre `timezone` dans le fichier `config.ini` doit suivre le format POSIX.

Exemple pour **Espagne (Péninsule et Baléares) / France / Italie** :  
`timezone=CET-1CEST,M3.5.0,M10.5.0/3`

Exemple pour **Canaries / Portugal / Royaume-Uni** :  
`timezone=WET0WEST,M3.5.0/1,M10.5.0`

### Comment obtenir votre code TZ ?
Si vous vivez dans une autre région, vous pouvez obtenir le code exact de votre ville ici :  
👉 **[ESP32 TZ Tool / Database](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)**

### Explication du format :
* **CET-1CEST** : Nom de la zone (Central European Time) et décalage de base (UTC+1).  
* **M3.5.0** : Changement à l’heure d’été (Mars, semaine 5, Dimanche).  
* **M10.5.0/3** : Changement à l’heure d’hiver (Octobre, semaine 5, Dimanche à 03:00).

---

### 5. ☁️ Comment obtenir votre API KEY de Météo

Pour que la barre de notifications affiche la température et l’icône météo, vous avez besoin d’une clé gratuite de **OpenWeatherMap** :

1. Allez sur [OpenWeatherMap.org](https://openweathermap.org/) et créez un compte gratuit.  
2. Une fois connecté, allez dans votre profil et cliquez sur **"My API Keys"**.  
3. Générez une nouvelle clé (vous pouvez l’appeler "RetroPixel").  
4. **IMPORTANT :** La clé peut mettre entre **30 minutes et 2 heures** à s’activer.  
   Si le panneau affiche "0.0C", attendez simplement.  
5. Copiez cette clé dans la section `API_KEY=` de votre fichier `config.ini`.

---

### 🔍 Comment vérifier si le code de votre ville est correct ?

Si vous voulez être sûr que **OpenWeatherMap** reconnaît votre ville avant de copier le fichier sur la Micro SD, vous pouvez faire ce test rapide :

1. Copiez l’adresse suivante dans votre navigateur.  
2. Remplacez `Navalmoral de la Mata` par votre **ville**.  
3. Remplacez `XXXXX` par votre **API Key**.

`http://api.openweathermap.org/data/2.5/weather?q=Navalmoral de la Mata,ES&appid=XXXXX`

* **Si le résultat est un texte avec des données (JSON) :** Le nom est correct et l’ESP32 le lira sans problème.  
* **Si le résultat est une erreur (401 ou 404) :**  
  Vérifiez que votre API Key est active (attendre jusqu’à 2 heures) ou que le nom de la ville est correct.

### 6. ☁️ Mise à jour du Système (OTA)

Il n’est plus nécessaire de connecter le panneau au PC pour le mettre à jour. Si une nouvelle version est disponible dans le dépôt :

1. Vérifie que le WiFi est configuré et activé dans ton `config.ini`.  
2. Accède au menu OSD du panneau.  
3. Navigue jusqu’à **Actualisation > Rechercher OTA**.  
4. Le système téléchargera le nouveau firmware depuis GitHub et redémarrera automatiquement.

> [!WARNING]  
> Ne débranche pas l’alimentation du panneau pendant le processus de mise à jour.

---

### 7. 🌐 Guide du Système Multilingue (Fichiers .json)

La version v2.1.0 utilise un système de **Dictionnaires Dynamiques**. Contrairement à d’autres systèmes, le dictionnaire ne reste PAS en mémoire RAM en permanence ; il n’est chargé que lorsque l’utilisateur entre dans le menu, puis libéré en sortant.  
Cela garantit que le moteur de GIFs dispose de toute la mémoire pour les animations.

#### 📂 Emplacement et Nomenclature

Les fichiers doivent être placés dans le dossier `/idioma/` de la carte SD.  
Le nom du fichier (sans l’extension) est celui qui apparaîtra dans le menu de sélection.

- `/idioma/ES.json` -> Apparaîtra comme « ES »  
- `/idioma/EN.json` -> Apparaîtra comme « EN »

#### 🛠️ Structure du Fichier JSON

Si tu veux créer une nouvelle traduction, tu peux copier le fichier `ES.json` et le renommer.  
Les champs sont organisés par blocs :

1. **`MENU`** : Étiquettes du menu principal.  
2. **`SUBMENU_XXX`** : Étiquettes spécifiques à chaque section.  
3. **`ESTADOS`** : Mots courts d’état (ON, OFF, OUI, NON, RETOUR).  
4. **`CONFIG_INI`** : Commentaires qui seront écrits dans le fichier de configuration physique sur la SD.

#### ⚠️ Règles Critiques pour l’Édition

Pour éviter les blocages du système (*Kernel Panic*) ou les erreurs visuelles, respecte ces règles :

* **🚫 Pas d’accents ni de Ñ :** La police actuelle ne supporte pas les caractères Unicode étendus.  
  Utilise `n` à la place de `ñ` et évite les accents (ex : `Actualisation` → `Actualisation` sans accent).  
* **📏 Limite de caractères :** Les étiquettes des sous-menus ne doivent pas dépasser **21 caractères** pour garantir un centrage parfait dans la zone de 128px.  
* **🔡 Format des étiquettes :** Dans les sous-menus, inclure les deux points et l’espace si tu veux qu’ils apparaissent (ex : `"modo": "Mode : "`).  
* **💾 Format UTF‑8 :** Sauvegarde le fichier en **UTF‑8 (sans BOM)** pour éviter les caractères parasites au début de la lecture.

#### 🔄 Flux de Chargement

Lorsque tu changes la langue dans l’OSD :

1. Le système met à jour la valeur `LANGUAGE` dans le `config.ini`.  
2. Le pointeur du dictionnaire est réinitialisé.  
3. La prochaine fois que tu ouvriras le menu, le système chargera le fichier correspondant à la nouvelle langue.

---
### 8. 📂 Explorateur SD (FTP)
Cette fonction active un serveur de fichiers sans fil sur votre Retro Pixel LED. Son objectif principal est de faciliter la maintenance du système sans avoir à retirer la carte MicroSD.

> [!IMPORTANT]
> **Utilisation recommandée :** Cette fonction a été spécialement conçue pour gérer les **fichiers de configuration (`config.ini`)**, les **fichiers de langue (`ES.json`)**, l’édition des **playlists (`.txt`)** et les fichiers de petite taille. En raison des limitations de bande passante du matériel ESP32, **elle n’est pas recommandée pour le transfert massif de collections de GIFs**, car le processus serait extrêmement lent comparé à un lecteur de carte classique.

#### 🚀 Comment activer le serveur FTP
1. Naviguez dans le menu OSD jusqu’à **Explorateur SD**.  
2. Sélectionnez l’option **Démarrer FTP**.  
3. Le panneau arrêtera la lecture des GIFs et affichera :  
   * **Adresse IP :** (ex. `192.168.1.109`)

#### 💻 Configuration de la connexion
Il est recommandé d’utiliser un client comme **FileZilla** ou **WinSCP** avec les données suivantes :

* **Protocole :** FTP (Protocole de transfert de fichiers).  
* **Serveur/Hôte :** L’adresse IP affichée sur votre panneau LED.  
* **Chiffrement :** Utilisez uniquement FTP en clair.  
* **Mode d’accès :** Normal  
* **Utilisateur :** `admin`  
* **Mot de passe :** `admin`  
* **Port :** `21`  
* **Options de transfert :** Par défaut  

<img width="545" height="227" alt="image" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />

---
Si vous ne souhaitez pas installer de logiciel supplémentaire comme FileZilla, vous pouvez intégrer directement la carte SD du panneau dans votre ordinateur comme s’il s’agissait d’un dossier supplémentaire en utilisant l’**Explorateur de fichiers** :

1. **Ouvrez l’Explorateur :** Allez dans **Ce PC** sur votre ordinateur.  
2. **Ajouter un emplacement :** Faites un clic droit dans un espace vide de la fenêtre et sélectionnez **"Ajouter un emplacement réseau"**.  
3. **Configurez l’adresse :** Lorsque l’assistant demande l’adresse réseau, entrez l’IP affichée sur votre panneau précédée du préfixe FTP.  
   * Exemple : `ftp://192.168.1.109`  
4. **Identifiants :** Décochez la case "Connexion anonyme" et saisissez l’utilisateur : `admin`.  
5. **Terminez :** Donnez un nom descriptif au lecteur (par ex. `Retro Pixel LED`) pour le retrouver facilement par la suite.

#### ⚠️ Notes de sécurité et d’usage
* **Verrouillage de l’écran :** Tant que le FTP est activé, le panneau ne lira pas les GIFs afin de consacrer toute la CPU au transfert des données.  
* **Sortie sécurisée :** Pour fermer le serveur et revenir en mode normal, pressez le bouton physique ou utilisez la touche "Valider" de votre télécommande IR.  
* **Attention à l’arrêt :** Ne coupez pas l’alimentation pendant que vous modifiez un fichier via FTP, car celui-ci pourrait être corrompu.

---

### 9. 🕹️ Intégration avec Batocera (Arcade)

Si tu veux que **Retro Pixel LED Lite** affiche les marquises du jeu que tu lances dans Batocera, tu dois activer dans le menu l’option **Arcade**.

```
🏠 MENU PRINCIPAL
├── 📂 Reproduction
│   └── 🖼️ Mode : [GIFs / Horloge]
│   └── 🔀 Aléatoire : [OUI / NON]
│   └── 🕹️ Arcade : [OUI / NON]   <--
│   └── 🔙 Retour
```

> [!IMPORTANT]
> ### 🕹️ Configuration de Batocera
> Pour apprendre à synchroniser tes ROMs, utiliser le script PC et installer les scripts de communication, consulte notre guide détaillé :
> **[👉 CLIQUE ICI POUR VOIR LES INSTRUCTIONS DE BATOCERA](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_BATOCERA_FR.md)**

---


## 🧠 Caractéristiques Core LITE

* **📡 Contrôle IR & Mappage Dynamique :** Support complet pour télécommandes infrarouges avec mappage des fonctions depuis le menu OSD (Luminosité, Navigation, Activation/Désactivation et Confirmation).
* **📂 Serveur FTP de Maintenance :** Permet la gestion sans fil du fichier `config.ini` et des listes de lecture. Idéal pour des réglages rapides sans avoir à retirer la MicroSD.
* **Gestion Anti-Panique de la RAM :** Système de surveillance du *heap*. Si le DMA ne peut pas allouer de mémoire après utilisation du WiFi, le système effectue un redémarrage préventif pour défragmenter la RAM et garantir une stabilité totale.
* **Moteur de Recherche Binaire (Arcade) :** Capacité à localiser des bandeaux parmi des milliers de fichiers en millisecondes. Le système ne "scanne" pas les dossiers, mais accède directement à la position du fichier sur la SD grâce à des index triés alphabétiquement.
* **Mémoire Adaptative (Single/Double Buffer) :** Gestion intelligente de la RAM. Le système utilise *Double Buffer* pour une fluidité totale des GIFs, mais commute automatiquement en *Single Buffer* en mode Arcade pour garantir une stabilité totale lors du chargement des bitmaps en haute définition.
* **API HTTP en Temps Réel :** Récepteur de commandes intégré permettant la synchronisation avec des systèmes externes comme Batocera ou RetroPie pour changer dynamiquement les bandeaux.
* **Centrage Intelligent du Texte :** Moteur dynamique qui aligne automatiquement les menus et états au centre de la matrice (`offset + 64px`) en calculant la largeur de chaque chaîne de texte.
* **Mode WiFi Discret :** L’ESP32 active le WiFi brièvement pour synchroniser l’heure et la météo. Le reste du temps, le système reste **100% hors ligne**, assurant **0 latence** dans la lecture des GIFs.
* **Barre de Notifications Dynamique :** Si vous activez la météo, l’horloge baisse automatiquement sa position (`startY=9`) pour afficher le message personnalisé (`WEATHER_MSG`), l’icône météo et la température.
* **Icônes en Bitmap :** Inclut des icônes optimisées de 8x8 pixels dessinées à la main pour représenter : Soleil, Nuages, Pluie, Neige, Orage et Brouillard.
* **Iconographie Avancée (Jour/Nuit) :** Inclut des icônes 8x8 pixels dessinées à la main pour représenter : Soleil, Lune (Nuit), Nuages, Pluie, Neige, Orage et Brouillard, s’adaptant dynamiquement selon la phase horaire.
* **Système de Playlists Dynamiques :** Remplace l’ancien moteur de liste unique. Le système peut maintenant gérer plusieurs fichiers `.txt` dans le dossier `/playlists/`, permettant de basculer entre des collections thématiques (Arcade, Consoles, Favoris, etc.) depuis le menu OSD.
* **Arrêt Automatique de l’Horloge :** Le panneau interrompt la galerie tous les "x" GIFs pour afficher l’heure pendant "x" secondes (configurables depuis le menu OSD et dans config.ini), reprenant la lecture exactement là où elle s’est arrêtée.
* **Résilience Hors Ligne :** Si le WiFi n’est pas disponible, le système ignore la synchronisation et commence immédiatement la lecture des GIFs en utilisant l’horloge interne du chipset.
* **Moteur de Rendu Double Buffer :** Tire parti du DMA de l’ESP32 pour dessiner les frames en mode invisible, assurant une fluidité absolue et éliminant tout scintillement dans les animations.

---



## 🛒 Liste du Matériel

Pour garantir la compatibilité, il est recommandé d’utiliser les composants testés lors du développement :

* **Microcontrôleur :** [ESP32 DevKit V1 (30 broches) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)  
* **Panneau LED Matrix (HUB75) :** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)  
* **Lecteur de cartes :** [Module Adaptateur Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)  
* **Carte connexion ESP32-Panneau LED :** [DMDos Board V3 - Mortaca](https://www.mortaca.com/) (Optionnel, pas de soudure, lecteur SD intégré)  
* **Récepteur IR :** [Capteur récepteur infrarouge universel - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)  
* **Bouton poussoir :** [Interrupteur momentané choisir DS-316 - AliExpress](https://es.aliexpress.com/item/4000888761296.html)  
* **Alimentation :** Alimentation 5V (minimum 2A recommandé pour panneaux 64x32).  

---

## ⚙️ Installation

### 1. 🔌 Connexions  
Si vous utilisez la DMDos Board V3, cette partie est déjà prise en charge, passez au point suivant.  

#### 📂 Lecteur de carte Micro SD (Interface SPI)  
| Broche SD | Broche ESP32 | Fonction          |  
| :-------- | :----------- | :---------------- |  
| **CS**    | GPIO 5       | Chip Select       |  
| **CLK**   | GPIO 18      | Horloge           |  
| **MOSI**  | GPIO 23      | Master Out Slave In|  
| **MISO**  | GPIO 19      | Master In Slave Out|  
| **VCC**   | 3.3V         | Alimentation      |  
| **GND**   | GND          | Masse             |  

#### 🖼️ Panneau LED RGB (Interface HUB75)  
| Broche Panneau | Broche ESP32 | Fonction                    |  
| :------------- | :----------- | :--------------------------|  
| **R1**         | GPIO 25      | Données Rouge (Supérieur)  |  
| **G1**         | GPIO 26      | Données Vert (Supérieur)   |  
| **B1**         | GPIO 27      | Données Bleu (Supérieur)   |  
| **R2**         | GPIO 14      | Données Rouge (Inférieur)  |  
| **G2**         | GPIO 12      | Données Vert (Inférieur)   |  
| **B2**         | GPIO 13      | Données Bleu (Inférieur)   |  
| **A**          | GPIO 33      | Sélection Ligne A          |  
| **B**          | GPIO 32      | Sélection Ligne B          |  
| **C**          | GPIO 22      | Sélection Ligne C          |  
| **D**          | GPIO 17      | Sélection Ligne D          |  
| **E**          | GND          | Masse                      |  
| **CLK**        | GPIO 16      | Horloge                   |  
| **LAT**        | GPIO 4       | Verrouillage (Latch)       |  
| **OE**         | GPIO 15      | Activation sortie (Luminosité) |  

#### 🕹️ Contrôle Utilisateur Menu OSD (Physique et Infrarouge)  

Le système permet un contrôle total via un bouton physique (avec logique d’appui long) et un récepteur IR pour une commande à distance.  

| Composant         | Broche ESP32 | Fonction                                            |  
| :---------------- | :----------- | :------------------------------------------------- |  
| **Bouton (PIN)**  | GPIO 21      | **Multifonction :** Clic (Navigation) / Appui long (Confirmer - Power Toggle). |  
| **Bouton (GND)**  | GND          | Masse                                              |  
| **Récepteur IR (Data)** | GPIO 34      | Entrée signal (Protocole NEC, etc.)               |  
| **Récepteur IR (VCC)**  | 3.3V         | Alimentation capteur                               |  
| **Récepteur IR (GND)**  | GND          | Masse                                              |  

<img width="769" height="716" alt="image" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />


---

## 🛠️ Feuille de Route (Roadmap LITE)

### ⚡ Optimisation & Fonctionnalité

### 🎨 Esthétique & Connectivité


---

## ⚖️ Licence et Remerciements

Ce projet est publié sous la **Licence MIT**.

Remerciements spéciaux aux développeurs des bibliothèques de base :
* **Bitbank2** pour l’excellente bibliothèque `AnimatedGIF`.
* **Mrfaptastic** pour le moteur DMA haute performance pour matrices.
* **Communauté Telegram DMDos** : en la découvrant et en voyant ce dont DMDos était capable, j’ai été motivé à développer **Retro Pixel LED**.
* **RpiTe@m** pour l’incroyable compilation de [GIFs.](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065)
* **shan-aya** pour la traduction en français et son logiciel magnifique pour créer des [GIFs.](https://github.com/shan-aya/DMD_GIF_converter)
