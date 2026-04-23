# ✨ Retro Pixel LED Lite v2.1.0
**[🇪🇸 Español](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md) | [🇫🇷 Français](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_FR.md)**

### **[✈️ Rejoindre le Groupe Telegram : Retro Pixel LED pour rester informé des mises à jour](https://t.me/RetroPixelLed)**


## 💡 Description du Projet

**Retro Pixel LED Lite** est la version haute performance conçue pour ceux qui recherchent une stabilité absolue, une vitesse instantanée et un système sans maintenance. Contrairement à la version standard, le firmware LITE élimine la charge du serveur web et la connectivité permanente pour consacrer 100 % de la puissance de l’ESP32 au rendu des GIFs.

La version **2.x.x** représente une révolution avec l’intégration d’un Menu OSD (On-Screen Display) natif, qui permet à l’utilisateur de naviguer dans les listes de lecture, d’ajuster la luminosité, de configurer l’horloge et de gérer la connectivité WiFi… directement depuis le panneau LED, sans avoir besoin d’appareils externes.

C’est la solution parfaite pour les enseignes fixes, les salles d’arcade ou la décoration rétro où vous voulez simplement **allumer et profiter**.

> [!TIP]
> **🚀 Philosophie Lite :** Moins, c’est plus. En éteignant le WiFi après avoir synchronisé l’heure et la météo, le système élimine le lag, réduit la chaleur du chip et évite les blocages dus à la saturation du réseau, permettant une lecture fluide de collections massives.

Si vous voulez essayer la version standard, voici le lien vers le **[GitHub.](https://github.com/fjgordillo86/RetroPixelLED)**


---
Bien sûr — voici **la traduction en français**, en **respectant strictement la mise en page et le sens**, sans rien modifier d’autre que la langue.

---

## 🆕 Nouveautés de la Version v2.1.0 Lite

| Fonctionnalité | Détail Technique | Bénéfice |
| :--- | :--- | :--- |
| **☁️ Mise à jour OTA** | Moteur de mise à jour sans fil via `WiFiClientSecure` et GitHub. | **Maintenance Simple.** Met à jour le firmware vers la dernière version depuis le menu sans utiliser de câbles. |
| **🌐 Multilingue Dynamique** | Dictionnaires `.json` externes avec système de chargement paresseux (**Lazy Loading**). | **Internationalisation.** Support pour n’importe quelle langue sans sacrifier la mémoire RAM pour les GIFs. |
| **📐 Smart Menu Centering** | Calcul dynamique des coordonnées basé sur la largeur du texte pour des zones de 128px. | **Esthétique Supérieure.** Menus parfaitement équilibrés et alignés, quel que soit le langage. |
| **😴 Feedback Universel** | Iconographie rétro (Emoji 😴) dessinée pixel par pixel. | **Clarté Visuelle.** Indicateurs d’état compréhensibles universellement sans dépendre de textes. |

---

## 📜 Historique de Changements Détaillé (v2.0.5 -> v2.1.0)

| Type | Composant | Description du Changement |
| :--- | :--- | :--- |
| **✨ Nouveau** | **Système** | **Support Multilingue :** Menus traduisibles via des fichiers dans le dossier `/idioma/`. |
| **✨ Nouveau** | **Système** | **Mise à jour OTA :** Téléchargement sécurisé de binaires directement depuis le menu OSD. |
| **✨ Nouveau** | **UX / OSD** | **Auto-centrage :** Les textes se positionnent automatiquement au centre de la zone totale (`offset + 64`). |
| **✨ Nouveau** | **Visuel** | **Feedback Visuel :** Ajout d’une animation d’emoji endormi pour le mode sommeil. |
| **⚡ Amélioration** | **Performance** | **Gestion de RAM :** Le dictionnaire JSON est libéré en quittant le menu pour éviter les blocages du système (*Panic*). |
| **⚡ Amélioration** | **Configuration** | Le fichier `config.ini` se génère automatiquement avec des commentaires dans la langue sélectionnée par l’utilisateur. |


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

```text
Voici la **traduction en français**, en **respectant strictement la mise en page et le sens**, sans rien modifier d’autre que la langue.

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
├── config.ini                   <-- Configuration du WiFi et du Panneau.  
└── Générateur de Playlists.bat  <-- Script pour générer les Playlists.


```
[!IMPORTANT]
Si vous ajoutez, supprimez ou déplacez des GIFs dans le dossier /gifs/, assurez-vous d’exécuter à nouveau le script Generador de Playlists.bat pour mettre à jour l’index.

### 3. 📝 Configuration via config.ini
Modifiez le fichier texte nommé config.ini à la racine de la SD pour configurer Retro Pixel LED Lite selon vos préférences :

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v2.1.0 - FICHIER DE CONFIGURATION
# ============================================================
# Note : Ne laissez pas d’espaces autour du symbole '='.
# Exemple correct : BRIGHTNESS=40

[WIFI_NTP]
# Configurez votre réseau seulement si vous allez utiliser l’horloge (CLOCK_ENABLE=1)
WIFI_ENABLE=1
SSID=Nom_De_Votre_Réseau
PASS=Mot_De_Passe_De_Votre_Réseau
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Nombre de panneaux en cascade
PANEL_CHAIN=2
# Luminosité générale (0 à 255)
BRIGHTNESS=40
# Vitesse I2S : 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Rafraîchissement Minimum (Hz) : 30 à 120
REFRESH_MIN=120
# Double Buffer : 0=OFF, 1=ON (Élimine les scintillements)
DOUBLE_BUFF=1
# Anti-Ghosting (Latch Blanking) : 1 à 4 (Augmentez si vous voyez un halo lumineux)
LATCH_BLANK=1

[LOGIC]
# Mode d’affichage : 0=GIFs, 1=Seulement Horloge
PLAY_MODE=0
# Active ou désactive l’horloge : 0=OFF, 1=ON
CLOCK_ENABLE=1
# Mode de lecture : 0=Séquentiel, 1=Aléatoire
RANDOM_MODE=1
# Intervalle : Tous les combien de GIFs apparaît l’horloge
AUTO_CLOCK_INT=6
# Durée : Combien de secondes l’horloge est affichée
CLOCK_DURATION=10
# Styles d’Horloge : 0=Matrix, 1=Solid, 2=Rainbow, 3=Pulse, 4=Gradient
CLOCK_STYLE=2
# Couleur de l’Horloge (Format HEX)
CLOCK_COLOR=#FF0055

[WEATHER]
# Active la météo : 0=OFF, 1=ON (Nécessite CLOCK_ENABLE=1)
WEATHER_ENABLE=1
# Votre ville (Sans espaces, utilisez '+' si nécessaire : Madrid,ES ou Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Votre API Key gratuite de OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalle de mise à jour en MINUTES
WEATHER_INT=60
# Texte affiché au-dessus de l’horloge
WEATHER_MSG=Game Room

[LANGUAGE]  
# Indique la langue (Nom du fichier sans .json : ES, EN, FR...)  
LANGUAGE=ES

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


## 🧠 Fonctionnalités Core LITE


* **Smart Text Centering :** Moteur dynamique qui aligne automatiquement les menus et les états au centre de la matrice (`offset + 64px`) en calculant la largeur de chaque chaîne de texte.

* **WiFi Stealth Mode :** L’ESP32 n’active le WiFi que brièvement pour synchroniser l’heure et la météo. Le reste du temps, le système reste **100 % hors-ligne**, garantissant **0 lag** dans la lecture des GIFs.

* **Barre de Notifications Dynamique :**  
  Si vous activez la météo, l’horloge descend automatiquement (`startY=9`) pour afficher :  
  - le message personnalisé (`WEATHER_MSG`),  
  - l’icône météo,  
  - la température.

* **Icônes en Bitmap :**  
  Inclut des icônes optimisés de 8x8 pixels dessinés à la main pour représenter :  
  Soleil, Nuages, Pluie, Neige, Orage, Brouillard.

* **Iconographie Avancée (Jour/Nuit) :**  
  Icônes 8x8 pour : Soleil, Lune (Nuit), Nuages, Pluie, Neige, Orage, Brouillard — s’adaptant automatiquement selon l’heure.

* **Système de Playlists Dynamiques :**  
  Remplace l’ancien moteur à liste unique.  
  Le système peut maintenant gérer plusieurs fichiers `.txt` dans `/playlists/`, permettant de changer de collection (Arcade, Consoles, Favoris, etc.) depuis l’OSD.

* **Horloge Auto-Interruption :**  
  Le panneau interrompt la galerie tous les “x” GIFs pour afficher l’heure pendant “x” secondes, puis reprend exactement où il s’était arrêté.

* **Résilience Hors-Ligne :**  
  S’il n’y a pas de WiFi, le système ignore la synchronisation et commence immédiatement la lecture des GIFs en utilisant l’horloge interne.

* **Moteur de Rendu Double Buffer :**  
  Utilise le DMA de l’ESP32 pour dessiner les frames de manière invisible, offrant une fluidité totale et éliminant tout scintillement.

---



## 🛒 Liste de Matériel

Pour garantir la compatibilité, il est recommandé d’utiliser les composants testés durant le développement :

* **Microcontrôleur :** [ESP32 DevKit V1 (38 pins) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
* **Panneau LED Matrix (HUB75) :** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005007439017560.html)
* **Lecteur de Cartes :** [Module Adaptateur Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
* **Carte de connexion ESP32-Panneau LED :** [DMDos Board V3 - Mortaca](https://www.mortaca.com/) (Optionnel, pas besoin de souder et possède un lecteur SD intégré)
* **Alimentation :** Alimentation 5V (Minimum 2A recommandé pour panneaux 64x32).

---
## ⚙️ Installation

### 1. 🔌 Connexions
Si vous utilisez la DMDos Board V3, cette partie est déjà faite, passez au point suivant.

#### 📂 Lecteur de Carte Micro SD (Interface SPI)
| Pin SD | Pin ESP32 | Fonction |
| :--- | :--- | :--- |
| **CS** | GPIO 5 | Chip Select |
| **CLK** | GPIO 18 | Clock |
| **MOSI** | GPIO 23 | Master Out Slave In |
| **MISO** | GPIO 19 | Master In Slave Out |
| **VCC** | 3.3V | Alimentation |
| **GND** | GND | Masse |

#### 🖼️ Panneau LED RGB (Interface HUB75)
| Pin Panneau | Pin ESP32 | Fonction |
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

#### 🕹️ Bouton momentané (poussoir) de Contrôle (Menu OSD)
| Composant | Pin ESP32 | Fonction |
| :--- | :--- | :--- |
| **Bouton (PIN)** | GPIO 21 | Entrée de signal (Pull-Up interne) |
| **Bouton (GND)** | GND | Masse |

<img width="652" height="609" alt="Pulsador" src="https://github.com/user-attachments/assets/7b2ad821-e369-498a-a9cf-b1fac93472de" />


---

## 🛠️ Feuille de Route (Roadmap LITE)

### ⚡ Optimisation & Fonctionnalité
* **[Prochainement] Intégration avec Batocera / RetroPie :** Support pour les scripts `game-start` qui enverront le nom du jeu au panneau afin d’afficher automatiquement le GIF correspondant lors de la partie.
* **[Recherche] Recherche Binaire :** Optimisation de la fonction `buscarEnCache` pour gérer des collections de milliers de GIFs sans latence.

### 🎨 Esthétique & Connectivité
* **[Prochainement] Support MQTT (Home Assistant) :** Intégration totale pour contrôler la luminosité, l’allumage/extinction et le changement de Playlists depuis ton panneau domotique.


---

## ⚖️ Licence et Remerciements

Ce projet est publié sous la **Licence MIT**.

Remerciements spéciaux aux développeurs des bibliothèques de base :
* **Bitbank2** pour l’excellente bibliothèque `AnimatedGIF`.
* **Mrfaptastic** pour le moteur DMA haute performance pour matrices.
* **Communauté Telegram DMDos** : en la découvrant et en voyant ce dont DMDos était capable, j’ai été motivé à développer **Retro Pixel LED**.
* **RpiTe@m** pour l’incroyable compilation de [GIFs.](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065)

