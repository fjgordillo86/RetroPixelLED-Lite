# ✨ Retro Pixel LED Lite

<p align="center">
  <img alt="Version" src="https://img.shields.io/badge/version-3.1.5-blue">
  <img alt="Plateforme" src="https://img.shields.io/badge/plateforme-ESP32-informational">
  <img alt="Licence" src="https://img.shields.io/badge/licence-MIT-green">
  <img alt="Statut" src="https://img.shields.io/badge/statut-actif-success">
</p>

<p align="center">
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md">🇪🇸 Español</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README.md">🇫🇷 Français</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Ingles/README.md">🇬🇧 English</a> ·
  <a href="https://t.me/RetroPixelLed">✈️ Groupe Telegram</a>
</p>

<p align="center">
  <a href="https://paypal.me/fjgordillo"><img alt="Faire un don avec PayPal" src="https://img.shields.io/badge/☕_Offrez_moi_un_café-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white"></a>
</p>

> 💛 Si Retro Pixel LED illumine votre coin retro à la maison, vous pouvez m'offrir un café via le bouton ci-dessus. Chaque contribution m'aide à continuer à développer de nouvelles fonctionnalités !

---

## 💡 Description du projet

**Retro Pixel LED Lite** est la version haute performance de Retro Pixel LED, conçue pour ceux qui recherchent une stabilité absolue, une vitesse instantanée et un système sans entretien. Contrairement à la version standard, le firmware **LITE** se débarrasse du serveur web et de la connectivité permanente pour consacrer 100 % de la puissance de l'ESP32 à une seule tâche exécutée à la perfection : le rendu de GIFs.

Si la branche 2.x.x a apporté le Menu OSD, la **v3.0.0** a été l'étape définitive vers l'indépendance matérielle : le panneau est devenu un appareil intelligent autonome, sans qu'il soit nécessaire de le connecter à l'ordinateur pour la maintenance ou la configuration. Le fichier `config.ini` et les playlists s'éditent directement depuis l'Explorateur Windows ou un client FTP, transformant la carte SD en une unité réseau sans fil. Il intègre également la prise en charge native des télécommandes infrarouges : naviguez dans le Menu OSD, ajustez la luminosité et allumez/éteignez l'appareil depuis votre canapé.

**Depuis la v3.1.0, vous contrôlez le panneau depuis une application web (PWA)**, et depuis la **v3.1.2**, également depuis **Home Assistant**. 🏠

**Depuis la v3.1.5, il n'est plus nécessaire d'éditer le moindre fichier pour le connecter à votre WiFi :** le panneau ouvre son propre portail de configuration depuis votre smartphone, puis vous le retrouvez toujours à l'adresse `retropixel.local`. 📶

---

## 📑 Sommaire

1. [🆕 Nouveautés de la version actuelle](#-nouveautés-de-la-version-v315-lite)
2. [🚀 Guide rapide](#-guide-rapide-premiers-pas)
3. [🎛️ Fonctionnalités principales](#️-fonctionnalités-principales)
   - [🖥️ Menu OSD](#️-menu-osd-navigation-intelligente)
   - [📱 PWA — Application de contrôle à distance](#-pwa--application-de-contrôle-à-distance)
   - [🏠 Home Assistant](#-home-assistant)
   - [🕹️ Mode Arcade](#️-mode-arcade-batocera-recalbox-retrobat--replayos)
   - [🕒 Horloge et Météo](#-horloge-et-météo)
   - [⏰ Minuteur](#-minuteur)
   - [🌐 Multi-langue](#-multi-langue)
   - [📂 Serveur FTP](#-serveur-ftp)
   - [🔄 Mise à jour OTA](#-mise-à-jour-ota)
   - [📶 Portail WiFi et accès par nom](#-portail-wifi-et-accès-par-nom-mdns)
4. [⚙️ Installation et configuration](#️-installation-et-configuration)
   - [1. Programmer l'ESP32](#1--programmer-lesp32-web-installer)
   - [2. Préparer la carte SD](#2--préparation-de-la-carte-sd)
   - [3. Le fichier `config.ini`](#3--configuration-via-configini)
   - [4. Fuseau horaire (TZ)](#4--configuration-du-fuseau-horaire-tz)
   - [5. Clé API météo](#-horloge-et-m%C3%A9t%C3%A9o)
5. [📖 Générateur de playlists (Windows)](#-générateur-de-playlists-windows)
6. [🕹️ Intégration Arcade](#️-intégration-arcade-batocera-recalbox-retrobat-ou-replayos)
7. [🏠 Intégration Home Assistant](#-intégration-home-assistant-guide-complet)
8. [🧠 Architecture interne / Core Lite](#-architecture-interne--core-lite)
9. [📜 Historique détaillé des modifications](#-historique-détaillé-des-modifications-v300--v315)
10. [🛒 Liste du matériel](#-liste-du-matériel)
11. [🔌 Connexions (Pinout)](#-connexions)
12. [🛠️ Feuille de route (Roadmap)](#️-feuille-de-route-roadmap)
13. [⚖️ Licence et remerciements](#️-licence-et-remerciements)

---

## 🆕 Nouveautés de la version v3.1.5 Lite

- **📶 Portail WiFi intégré :** connectez le panneau à votre réseau depuis votre smartphone, sans éditer `config.ini`. Il s'ouvre depuis `Menu OSD → WiFi → Ajouter réseau`, ou automatiquement au premier démarrage si aucun réseau n'est configuré. Le panneau affiche son réseau et une clé aléatoire, vous choisissez votre WiFi dans une liste et il vérifie lui-même la connexion avant de la sauvegarder. Textes en espagnol, anglais et français. [Voir le fonctionnement](#-portail-wifi-et-accès-par-nom-mdns).
- **🏷️ Accès par nom (mDNS) :** le panneau répond à l'adresse `retropixel.local`, sans dépendre de l'IP. La PWA conserve en mémoire la dernière IP connue et vous propose de l'utiliser si le nom ne se résout pas sur votre appareil.
- **🔔 Notification de mise à jour dans la PWA :** un point ambre sur l'icône 🔄 vous informe lorsqu'une nouvelle version est disponible, avec son numéro et ses nouveautés.
- **ℹ️ Nouvelle page "À propos" dans la PWA :** versions de la PWA et du firmware, créateur, licence et **diagnostic en direct** du panneau (mémoire, dernier redémarrage, WiFi, carte SD...), avec un bouton pour copier le rapport.
- **🩺 Diagnostic `/diag` :** consultation rapide de l'état interne depuis le navigateur (`http://retropixel.local/diag`).
- **🔐 Mots de passe WiFi contenant `#` :** correction d'une erreur de lecture de `config.ini` qui tronquait le SSID ou le mot de passe à partir d'un `#`.
- **🛡️ PWA plus fiable lors des mises à jour :** le service worker possède désormais une version unique, s'installe même si un fichier échoue et n'intercepte plus les requêtes adressées au panneau.

> ⚠️ **Lors d'une mise à jour depuis une version antérieure à la v3.1.4 :** à partir de cette version, le schéma de partitionnement *Minimal SPIFFS* est utilisé (1,9 Mo pour l'application avec OTA) et le firmware ne rentre plus dans l'ancien espace. La mise à jour OTA est impossible : réinstallez-le depuis l'installateur web. Les données sauvegardées dans la mémoire interne seront effacées (playlist active, paramètres du minuteur) ; la configuration et les GIFs de la carte SD seront conservés. Si vous possédez déjà la v3.1.4, vous pouvez passer à la v3.1.5 via OTA.

Pour le détail des versions antérieures (marqueurs GIF en Arcade, reconnexion WiFi, IP dans le menu OSD...), consultez l'[Historique des modifications](#-historique-détaillé-des-modifications-v300--v315).

---

## 🚀 Guide rapide (premiers pas)

S'il s'agit de votre première installation de Retro Pixel LED Lite, voici la méthode la plus rapide :

1. **Flashez le firmware** avec l'[installateur web](#1--programmer-lesp32-web-installer) — vous avez seulement besoin de Chrome ou Edge, rien à installer sur le PC.
2. **Préparez la carte MicroSD** en FAT32 avec le [contenu du dossier `Contenido SD`](#2--préparation-de-la-carte-sd).
3. **Vérifiez `config.ini`** selon vos préférences (fuseau horaire, ville pour la météo...) — [référence complète](#3--configuration-via-configini). Vous pouvez laisser le WiFi vide : le panneau vous le demandera depuis votre smartphone au premier démarrage.
4. **Allumez le panneau.** S'il n'a pas de réseau configuré, ouvrez le [portail WiFi](#-portail-wifi-et-accès-par-nom-mdns) : connectez votre smartphone au réseau affiché à l'écran et sélectionnez votre WiFi. Il synchronisera ensuite l'heure, chargera vos GIFs et sera prêt. ✨
5. *(Optionnel)* Installez la [PWA](#-pwa--application-de-contrôle-à-distance) pour le contrôler depuis votre smartphone, ou [intégrez-le à Home Assistant](#-intégration-home-assistant-guide-complet) si vous utilisez la domotique.
6. *(Optionnel)* Si vous utilisez Batocera, Recalbox, RetroBat ou ReplayOS, suivez le [guide d'intégration Arcade](#️-intégration-arcade-batocera-recalbox-retrobat-ou-replayos) pour afficher des marqueurs dynamiques.

Le reste de ce document constitue la référence détaillée de chaque fonction — il n'est pas nécessaire de tout lire pour commencer. 🙂

---

## 🎛️ Fonctionnalités principales

Cette section résume **ce que fait** chaque partie du système. Pour les étapes d'installation, rendez-vous à la section [⚙️ Installation et configuration](#️-installation-et-configuration).

### 🖥️ Menu OSD (navigation intelligente)

Le système se contrôle à l'aide d'un **bouton unique** (ou de la télécommande IR), avec une logique de pression qui s'adapte au contexte du menu :

- **Pression courte :**
  - **Dans les menus :** déplacer le curseur / naviguer vers le bas.
  - **En mode veille :** réveil immédiat du panneau (wake-up).
- **Pression prolongée :**
  - **Action générale :** entrer dans les sous-menus ou confirmer la sélection.
  - **Dans la configuration du temps (minuteur) :** soustrait **-5 minutes** à la valeur actuelle.
- **Pression très longue (> 4 sec) :**
  - **Override manuel :** force l'extinction (mode veille), bloquant le minuteur jusqu'au prochain cycle.
- **Maintenir la pression continue :**
  - **Dans la configuration du temps (minuteur) :** incrémente automatiquement **+5 minutes** en boucle tant que la pression est maintenue.

```text
🏠 MENU PRINCIPAL
├── 📂 Playlists
│   ├── 📄 Favoris
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Retour
├── 📂 Lecture
│   ├── 🖼️ Mode: [GIFs / Horloge]
│   ├── 🔀 Aléatoire: [OUI / NON]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS / RetroBat]
│   ├── 💬 Texte: [OUI / NON]
│   └── 🔙 Retour
├── ☀️ Luminosité
│   └── Luminosité: [5% - 100%]
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activer: [OUI / NON]
│   ├── ➕ Ajouter réseau Wifi
│   ├── 📱 Contrôle APP: [OUI / NON]
│   ├── 🏷️ IP: [192.168.1.117]
│   ├── 🔎 Afficher IP: [OUI / NON]
│   └── 🔙 Retour
├── 🕒 Horloge: [ON / OFF]
│   ├── 🔄 Activer: [OUI / NON]
│   ├── 🖼️ Tous les: [1...20] GIFs
│   ├── ⏳ Durée: [5...30] sec
│   ├── 🎨 Style Horloge: [Matrix, Solid, Rainbow, Pulse, Gradient]
│   ├── 🎨 Couleur: [Blanc, Rouge, Vert, Bleu, Jaune, Cyan, Magenta, Orange, Rose]
│   ├── 🔄 Transition: [OUI / NON]
│   └── 🔙 Retour
├── 🌡️ Météo: [ON / OFF]
│   ├── 🔄 Activer: [OUI / NON]
│   └── 🔙 Retour
├── 🕒 Minuteur: [ON / OFF]
│   ├── 🔄 Activer: [OUI / NON]
│   ├── ⏳ ON: [00:00 à 24:00]
│   ├── ⏳ OFF: [00:00 à 24:00]
│   └── 🔙 Retour
├── ⚙️ Paramètres avancés
│   ├── ⚡ Vitesse I2S: [8, 10, 16, 20MHz]
│   ├── 🔄 Rafraîchissement: [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer: [OUI / NON]
│   ├── 👻 AntiGhost: [1, 2, 3, 4]
│   ├── 🚀 Lecture SD: [4, 10, 20, 25MHz]
│   ├── 🎮 Mappage télécommande IR: [On, Off, Menu, Valider, Haut, Bas, Brillo+, Brillo-]
│   ├── ⚠️ Réinitialisation
│   └── 🔙 Retour
├── 🚀 Mise à jour
│   ├── 🔄 Chercher OTA
│   ├── 🔤 Télécharger langues
│   └── 🔙 Retour
├── 📂 Explorateur SD
│   ├── 🔄 Démarrer FTP
│   └── 🔙 Retour
├── 🌐 Langue
│   ├── [ES] Español
│   ├── [EN] English
│   ├── [FR] Français
│   ├── ...
│   └── 🔙 Retour
├── 💾 Sauvegarder
└── 🔙 Quitter
```

---

### 📱 PWA — Application de contrôle à distance

**[👉 Installer ou tester Retro Pixel LED Control](https://fjgordillo86.github.io/RetroPixelLED-Lite/control/)**

Application web moderne, installable sur n'importe quel appareil (smartphone, tablette, ordinateur) connecté au même réseau que le panneau. Elle ne nécessite aucun serveur externe, fonctionne sur le réseau local et reste accessible hors ligne une fois installée. 📴

https://github.com/user-attachments/assets/f5231448-7862-4476-901e-ac25ac7f4248

L'interface est divisée en **6 sections** :

**1️⃣ Page principale (Accueil)**
- **☀️ Contrôle de la luminosité :** curseur 0-100% à application instantanée, sans redémarrage.
- **🎛️ Sélecteur de mode :** GIF, Horloge ou Texte en temps réel.
  - **Mode GIF :** playlist active + bascule de lecture aléatoire.
  - **Mode Horloge :** 5 styles (Matrix, Solid, Rainbow, Pulse, Gradient) et 9 couleurs.
  - **Mode Texte :** aperçu en direct sur la matrice pendant la frappe, avec couleur, police et vitesse d'enroulement.
- **🔌 État de la connexion :** indicateur visuel (vert/rouge) du WiFi.

**2️⃣ Minuteur ⏰**
- Activation/désactivation via une bascule.
- Heure d'allumage et d'extinction (format 24h).
- Bouton d'allumage/extinction manuel immédiat (override).
- État actuel : allumé (✓ vert) ou en veille (● gris).

**3️⃣ Mode texte**
- Aperçu en temps réel de la matrice 26×7 au fur et à mesure de la frappe.
- Sélecteur de couleur : palette de 9 couleurs + sélecteur hexadécimal personnalisé.
- **Sélecteur de police :** `Bold`, `SemiBold`, `Regular`, `Light` — *(nouveau en v3.1.2)*.
- Contrôle de vitesse : curseur de 5 à 200 ms/pas avec aperçu en direct.
- Boutons "▶ Envoyer" et "■ Stop".

**4️⃣ Mise à jour 🔄**
- **OTA du firmware :** vérifie GitHub et met à jour automatiquement si une nouvelle version existe.
- **Notification de nouvelle version :** un point ambre sur l'icône 🔄 indique qu'une nouvelle version est disponible ; l'ouverture de la fenêtre affiche le numéro de version et ses nouveautés. *(nouveau en v3.1.5)*
- **Téléchargement des langues :** récupère les fichiers `.json` depuis GitHub vers le dossier `/idiomas` de la carte SD.

**5️⃣ Paramètres 🛠**

Édition à distance du fichier `config.ini`, organisée en 7 sections : WiFi (SSID, mot de passe, afficher IP au démarrage, fuseau horaire), Matériel (nombre de panneaux, ordre des couleurs RGB/RBG/GBR, luminosité, vitesse I2S, rafraîchissement, buffering, anti-ghosting), Arcade (Batocera, Recalbox, ReplayOS ou aucun), Texte défilant, Horloge (active, transition avec particules, intervalle, durée, style, couleur), Météo (activer, ville, clé API, intervalle, texte au-dessus de l'horloge) et Langue (ES, EN, FR...).

Redémarrage automatique après sauvegarde, uniquement si la modification le requiert.

**6️⃣ À propos ℹ️** *(nouveau en v3.1.5)*

S'ouvre depuis le lien "ℹ À propos" en bas de l'écran principal.
- **Versions :** de la PWA et du firmware du panneau, avec alerte si une nouvelle version est disponible.
- **Crédits :** créateur (`fjgordillo86`), licence MIT et lien vers le code source.
- **Diagnostic en direct :** mémoire libre, dernier redémarrage, signal WiFi, carte SD, CPU et utilisation de la partition du firmware, avec bouton pour **copier le rapport** et l'joindre en cas de problème.

#### ⚙️ Installation et configuration de la PWA

1. Ouvrez <https://fjgordillo86.github.io/RetroPixelLED-Lite/control/> depuis votre appareil.
2. Appuyez sur l'icône de connexion (⚙) et saisissez le nom du panneau (`retropixel.local`) ou son IP locale (ex. `192.168.1.117`). Si le nom n'est pas résolu sur votre appareil, la PWA vous proposera d'utiliser la dernière IP connue.
3. *(Optionnel)* Installez-la en tant qu'application : Chrome/Edge le propose généralement automatiquement ; sinon, menu (⋮) → "Installer l'application".
4. C'est prêt — elle apparaît comme une application classique, sans avoir à ressaisir d'URL. 🎉

#### 📝 Prérequis

- Le panneau doit se trouver sur le même réseau WiFi que votre appareil.
- `CONFI_APP_ENABLE=1` dans le panneau, afin de pouvoir le contrôler depuis l'application.
- `TEXT_ENABLE=1` pour permettre l'envoi de messages.
- Connexion Internet sur le panneau pour l'OTA et les langues (accès sortant vers GitHub).
- Les langues sont téléchargées une fois, puis fonctionnent hors ligne.

---

### 🏠 Home Assistant

Contrôle complet depuis votre tableau de bord Home Assistant via l'**API REST locale** — sans cloud, sans dépendre d'Internet. Permet de :
- 🟢 **Allumer / Éteindre** le panneau avec un `switch`.
- 📊 **Consulter l'état actuel** (mode actif, playlist en cours de lecture, etc.).
- 🔄 **Changer de mode** (Horloge / GIF) et de **playlist** instantanément.
- 💬 **Envoyer du texte défilant** en choisissant la couleur, la vitesse et la **police** depuis le tableau de bord.

Consultez le [guide complet d'intégration avec Home Assistant](#-intégration-home-assistant-guide-complet) plus bas, incluant le fichier `.yaml` prêt à être copié.

---

### 🕹️ Mode Arcade (Batocera, Recalbox, RetroBat & ReplayOS)

Transforme le panneau en une enseigne/marqueur dynamique qui réagit à votre jeu en cours, avec deux voies de synchronisation : scripts locaux (**Batocera / Recalbox / RetroBat**) ou surveillance native par réseau (**ReplayOS**).

Affiche automatiquement :
1. **Marqueur du jeu :** image `.bmp` 24 bits, ou un **GIF animé** s'il en existe un pour ce jeu — y compris des séquences de plusieurs GIFs joués les uns après les autres en boucle.
2. **Logo du système :** image `.bmp` lorsque vous naviguez dans les systèmes.

S'active depuis `Menu → Lecture → Arcade`. Le guide d'installation complet se trouve dans la section [Intégration Arcade](#️-intégration-arcade-batocera-recalbox-retrobat-ou-replayos).

---

### 🕒 Horloge et Météo

- **Horloge :** 5 styles (Matrix, Solid, Rainbow, Pulse, Gradient) et 9 couleurs, avec transition de particules à l'apparition/disparition. La galerie de GIFs s'interrompt tous les *x* GIFs pour afficher l'heure pendant *x* secondes (configurable), puis reprend la lecture exactement là où elle s'était arrêtée.
- **Météo :** avec une clé API gratuite d'OpenWeatherMap, affiche la température et l'icône météo au-dessus de l'horloge, accompagnées d'un message personnalisable (`WEATHER_MSG`). Voir [comment obtenir votre clé API](#5--comment-obtenir-votre-clé-api-météo).

---

### ⏰ Minuteur

Allumage/extinction programmés par horaire, avec possibilité de débrayage manuel (override) depuis le bouton physique ou la PWA. Un appui sur le bouton annule l'automatisme jusqu'au prochain cycle programmé.

---

### 🌐 Multi-langue

Système de **dictionnaires dynamiques** : la langue ne réside PAS en permanence dans la RAM ; elle est chargée uniquement lors de l'entrée dans le menu et libérée à la sortie — le moteur de GIFs conserve ainsi toute la mémoire disponible.

- **Emplacement :** `/idioma/` sur la carte SD. Le nom du fichier (sans extension) correspond à ce qui apparaît dans le menu : `/idioma/FR.json` → "FR".
- **Structure du JSON :** blocs `MENU`, `SUBMENU_XXX`, `ESTADOS`, `CONFIG_INI`.
- **Règles critiques :**
  - 🚫 Pas d'accents ni de caractères spéciaux non-ASCII (utilisez des caractères standard).
  - 📏 Maximum 21 caractères pour les étiquettes de sous-menu, pour garantir le centrage sur 128px.
  - 🔡 Incluez les deux-points et l'espace si vous souhaitez qu'ils s'affichent (ex: `"modo": "Mode: "`).
  - 💾 Enregistrez au format UTF-8 sans BOM.
- **Téléchargement à distance :** depuis la PWA ou le menu OSD (`Mise à jour → Télécharger langues`), directement depuis GitHub, sans retirer la carte SD.
- **Portail WiFi :** ses textes (panneau et web) sont intégrés au firmware en ES, EN et FR, selon la variable `LANGUAGE` ; toute autre langue affichera l'anglais. Ils n'utilisent pas ces fichiers JSON.
- **Nouvelles clés en v3.1.5 :** le bloc `SUBMENU_WIFI` inclut `anadir_red`, `portal_abriendo` et `portal_reinicio`. Si vous maintenez votre propre fichier de langue, ajoutez-les ; tant qu'elles manquent, le menu affichera le texte en espagnol pour ces clés.

---

### 📂 Serveur FTP

Serveur de fichiers sans fil pour la maintenance sans extraction de la carte MicroSD.

> [!IMPORTANT]
> Conçu pour **`config.ini`**, les fichiers de langue (`.json`) et les **playlists** (`.txt`) — non recommandé pour transférer des collections complètes de GIFs, la vitesse étant limitée par rapport à un lecteur de carte physique.

**Activation :** `Menu OSD → Explorateur SD → Démarrer FTP`. Le panneau stoppe la lecture des GIFs et affiche son adresse IP.

**Client recommandé — FileZilla :**

| Paramètre | Valeur |
| :--- | :--- |
| Protocole | FTP simple (non sécurisé) |
| Serveur/Hôte | IP affichée sur le panneau |
| Utilisateur / Mot de passe | `admin` / `admin` |
| Port | `21` |
| Connexions simultanées | 1 (limitation activée) |
| Limite de téléchargement/envoi | 20 KiB/s |

Vous pouvez également le monter comme un emplacement réseau dans l'Explorateur Windows (`ftp://<IP>`, utilisateur `admin`) — bien que lors des tests, FileZilla se soit révélé plus fiable.

**Consignes de sécurité :** le panneau ne lit pas de GIFs tant que le FTP est actif (tout le CPU est alloué au transfert). Pour quitter, appuyez sur le bouton physique ou sur la touche "Valider" de la télécommande. Ne coupez pas l'alimentation pendant l'édition d'un fichier via FTP.

Voir le [guide étape par étape avec captures](#-explorateur-sd-ftp--détail) plus bas.

---

### 🔄 Mise à jour OTA

Sans avoir besoin de connecter le panneau au PC :
1. WiFi configuré et actif (dans `config.ini` ou via le [portail WiFi](#-portail-wifi-et-accès-par-nom-mdns)).
2. La PWA vous avertit lorsqu'une nouvelle version est disponible : point ambre sur 🔄, avec la version disponible et ses nouveautés.
3. `Menu OSD → Mise à jour → Chercher OTA` (ou le bouton dans la PWA).
4. Le système télécharge le firmware depuis GitHub et redémarre automatiquement. 🔃

> [!WARNING]
> Ne débranchez pas l'alimentation pendant la mise à jour.

---

### 📶 Portail WiFi et accès par nom (mDNS)

Connectez le panneau à votre réseau **sans éditer `config.ini`** ni retirer la carte MicroSD, et retrouvez-le à tout moment à l'adresse `retropixel.local`.

**Quand le portail s'ouvre-t-il :**
- `Menu OSD → WiFi → Ajouter réseau` — pour changer de routeur ou de mot de passe quand vous le souhaitez.
- **Automatiquement** au démarrage, si `WIFI_ENABLE=1` et que `SSID` est vide dans `config.ini` (première installation).

**Étape par étape :**
1. Le panneau affiche son propre réseau : `WiFi: RetroPixel-LED`, `Pass:` (8 chiffres aléatoires, différents à chaque fois) et `IP: 192.168.4.1`.
2. Connectez votre smartphone à ce réseau. La page de configuration s'ouvre automatiquement ; sinon, naviguez vers `192.168.4.1`.
3. Appuyez sur votre réseau dans la liste (ou saisissez le nom s'il est masqué), entrez le mot de passe et appuyez sur **Connecter**.
4. Le panneau teste la connexion (jusqu'à 15 s). S'il se connecte, il enregistre les données dans `config.ini`, affiche son IP et `retropixel.local`, puis redémarre. En cas d'échec, il affiche `NO CONECTA` et vous permet de réessayer.

**Pour quitter sans sauvegarder :** bouton **Annuler** de la page web, touche **OK** de la télécommande IR ou le bouton physique. En l'absence d'interaction, le portail se ferme automatiquement au bout de 5 minutes.

> [!NOTE]
> - Nécessite la carte SD présente, car la configuration réseau est enregistrée dans `config.ini`.
> - L'ESP32 fonctionne uniquement avec les réseaux **2,4 GHz**.
> - Les textes s'affichent en espagnol ou en français si `LANGUAGE` est réglé sur `ES` ou `FR` ; pour toute autre langue, l'anglais est utilisé.
> - Pour économiser de la mémoire, le panneau démarre sans *Double Buffer* tant que le portail est ouvert. Votre paramètre initial est restauré après sauvegarde.

**Accès par nom (mDNS) :** lorsque le WiFi est actif, le panneau répond à l'adresse **`http://retropixel.local`**. Dans la PWA, saisissez `retropixel.local` au lieu de l'IP ; si votre appareil ne résout pas le nom, la PWA vous proposera de revenir à la dernière IP connue.

> [!TIP]
> `.local` fonctionne sous Windows 10/11, macOS, iOS et Linux. Certains appareils Android et certains réseaux (invités, VLAN, routeurs filtrant le multicast) ne le résolvent pas : dans ce cas, utilisez l'adresse IP.

**Diagnostic :** `http://retropixel.local/diag` renvoie un objet JSON contenant la mémoire libre et minimale, le motif du dernier redémarrage, le signal WiFi, la carte SD et l'utilisation de la partition du firmware. Il s'agit des mêmes données que celles affichées sur la page **À propos** de la PWA.

---

## ⚙️ Installation et configuration

### 1. 🚀 Programmer l'ESP32 (Web Installer)

Installez cette version sans rien installer sur votre PC :

**[👉 Ouvrir l'installateur web de Retro Pixel LED Lite](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

1. Utilisez un navigateur compatible (**Google Chrome** ou **Microsoft Edge**).
2. Connectez votre ESP32 en USB.
3. Appuyez sur **"Install"** et sélectionnez le port COM correspondant.
4. **Important :** cochez **"Erase device"** pour un nettoyage complet de la mémoire et éviter les erreurs de fragmentation.

> 💡 **Votre ESP32 n'est pas reconnu ?** Si aucun port COM n'apparaît, installez les pilotes du composant USB de votre carte :
> - **Puce CP2102 :** [Pilotes Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> - **Puce CH340/CH341 :** [Pilotes SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 Préparation de la carte SD

Formatez votre carte MicroSD en **FAT32** et ajoutez tout le contenu du dossier [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) à la racine :

```text
/ (Racine de la carte SD)
├── gifs/                        <-- Vos dossiers contenant les GIFs (Arcade, Consoles, etc.)
├── idioma/                      <-- Fichiers .json avec les textes traduits.
│   ├── ES.json
│   ├── EN.json
│   └── FR.json
├── playlists/                   <-- Listes générées par le script "Generador de Playlists".
│   ├── Arcade.txt
│   ├── Computers.txt
│   ├── Consolas.txt
│   └── Todos.txt
├── config.ini                   <-- Configuration du WiFi et du panneau.
└── Generador de Playlists.bat   <-- Script Windows pour générer les playlists.
```

> [!IMPORTANT]
> Si vous ajoutez, supprimez ou déplacez des GIFs dans `/gifs/`, exécutez à nouveau le fichier **Generador de Playlists.bat** pour mettre à jour l'index.

### 3. 📝 Configuration via `config.ini`

Le fichier `config.ini` se trouve dans le dossier [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) — copiez-le à la racine de la carte SD et ajustez-le selon vos besoins :

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.1.5 - FICHIER DE CONFIGURATION
# ============================================================
# Note: Ne laissez pas d'espaces autour du symbole '='.
# Exemple correct: BRIGHTNESS=40

[WIFI_NTP]
# Configurez votre réseau WiFi. Laissez vide (SSID= et PASS=) pour le configurer depuis votre smartphone via le portail WiFi du panneau
# Si votre mot de passe contient '#', écrivez-le tel quel (seul un '#' précédé d'un espace est considéré comme un commentaire)
WIFI_ENABLE=1
SSID=
PASS=
# Afficher l'IP au démarrage
MOSTRAR_IP=0
# Configurez votre fuseau horaire
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Nombre de panneaux chaînés
PANEL_CHAIN=2
# Ordre des couleurs du panneau: RGB, RBG ou GBR
COLOR_ORDER=RGB
# Luminosité (0 à 255)
BRIGHTNESS=38
# Vitesse I2S: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Rafraîchissement minimum (Hz): 30 à 120
REFRESH_MIN=120
# Double Buffer: 0=OFF, 1=ON (Élimine les scintillements)
DOUBLE_BUFF=0
# Anti-Ghosting: 1 à 4 (Augmentez si des rémanences apparaissent)
LATCH_BLANK=1
# Vitesse SD (MHz): 4, 10, 20 ou 25. Réduisez en cas d'erreurs de lecture
SD_SPEED=10

[LOGIC]
# Mode d'affichage: 0=GIFs, 1=Horloge seule
PLAY_MODE=0
# Active ou désactive la configuration via l'APP: 0=OFF, 1=ON (Nécessite le WiFi)
CONFI_APP_ENABLE=1
# Sélectionnez votre système Arcade: 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS, 4=RetroBat
ARCADE_ENABLE=0
# Active ou désactive le texte défilant: 0=OFF, 1=ON (Nécessite le WiFi)
TEXT_ENABLE=1
# Active ou désactive l'horloge: 0=OFF, 1=ON (Nécessite le WiFi)
CLOCK_ENABLE=1
# Mode de lecture: 0=Séquentiel, 1=Aléatoire
RANDOM_MODE=1
# Intervalle: Nombre de GIFs affichés entre chaque affichage de l'horloge
AUTO_CLOCK_INT=6
# Durée: Nombre de secondes d'affichage de l'horloge
CLOCK_DURATION=10
# Styles d'horloge: 0=Solid, 1=Aurora, 2=Rainbow, 3=Gradient, 4=Fuego, 5=Glitch
CLOCK_STYLE=2
# Active la transition de l'horloge vers les GIFs avec une explosion de particules : 0=OFF, 1=ON
TRANSITION_ENABLE=1
# Couleur de l'horloge (0=Blanc, 1=Rouge, 2=Vert, 3=Bleu, 4=Jaune, 5=Cyan, 6=Magenta, 7=Orange, 8=Rose)
CLOCK_COLOR=4

[WEATHER]
# Active la météo: 0=OFF, 1=ON (Nécessite le WiFi)
WEATHER_ENABLE=1
# Votre ville (Sans espaces, utilisez '+' si nécessaire: Paris,FR ou Lyon,FR)
CITY=Navalmoral+de+la+Mata,ES
# Votre clé API gratuite OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalle de mise à jour météo en MINUTES
WEATHER_INT=60
# Texte affiché au-dessus de l'horloge
WEATHER_MSG=Game Room

[LANGUAGE]
# Indique la langue (Nom du fichier sans .json: ES, EN, FR...)
LANGUAGE=FR

[IR_REMOTE]
# Codes HEX de la télécommande IR (Retro Pixel LED les enregistrera automatiquement via l'OSD)
BTN_ON=F20DFF00
BTN_OFF=E01FFF00
BTN_BRILLO_UP=F609FF00
BTN_BRILLO_DOWN=E21DFF00
BTN_MENU=EA15FF00
BTN_OK=ED12FF00
BTN_SUBIR=E41BFF00
BTN_BAJAR=B34CFF00

[REPLAY_OS]
# Adresse IP assignée à ReplayOS
IP=192.168.1.101
# Token ReplayOS: SYSTEM > INFORMATION > NET CONTROL CODE
TOKEN=xxxxxx

[END]
```

### 4. 🌍 Configuration du fuseau horaire (TZ)

Pour que l'**horloge** et le **minuteur** fonctionnent correctement, la variable `TZ` doit respecter la syntaxe POSIX.

- **France / Espagne (Péninsule) / Italie :** `TZ=CET-1CEST,M3.5.0,M10.5.0/3`
- **Royaume-Uni / Portugal / Îles Canaries :** `TZ=WET0WEST,M3.5.0/1,M10.5.0`

👉 Consultez l'[**outil / base de données ESP32 TZ**](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv) pour obtenir la chaîne exacte correspondant à votre région.

**Explication du format :** `CET-1CEST` (nom standard, décalage UTC+1, nom d'été) · `M3.5.0` (passage à l'heure d'été : mars, dernière semaine, dimanche) · `M10.5.0/3` (passage à l'heure d'hiver : octobre, dernière semaine, dimanche à 03:00).

### 5. ☁️ Comment obtenir votre clé API météo

1. Rendez-vous sur [OpenWeatherMap.org](https://openweathermap.org/) et créez un compte gratuit.
2. Dans votre profil, accédez à **"My API Keys"**.
3. Générez une nouvelle clé (ex. "RetroPixel").
4. **Important :** l'activation peut prendre entre 30 minutes et 2 heures. Si le panneau affiche "0.0C", patientez un moment. ⏳
5. Copiez-la dans la ligne `API_KEY=` de votre fichier `config.ini`.

**🔍 Vérifier le nom de votre ville :** collez dans votre navigateur l'URL `http://api.openweathermap.org/data/2.5/weather?q=VOTRE_VILLE&appid=VOTRE_CLE_API` (en remplaçant les deux valeurs). Un retour JSON avec des données indique que la configuration est correcte ; une erreur 401/404 indique une clé non activée ou un nom de ville incorrect.

---

## 📖 Générateur de playlists (Windows)

Le script `Generador de Playlist v1.0.1.bat` permet de créer des collections personnalisées sans modifier de code. Il est situé dans le dossier [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD).

1. **Préparation :** placez le fichier `.bat` à la racine de votre carte SD, aux côtés du dossier `gifs`.
2. **Exécution :** double-cliquez sur le fichier — une fenêtre d'invite de commandes s'ouvre.
3. **Sélection :** le script liste les sous-dossiers situés dans `/gifs`. Saisissez les numéros séparés par des virgules (ex: `3,4,10`) ou tapez `TODO` pour tout inclure.
4. **Nom :** choisissez le nom de votre liste (ex: `MesFavoris`).
5. **Résultat :** le fichier `playlists/MesFavoris.txt` est créé avec les chemins formatés pour l'ESP32.
6. **Utilisation :** insérez la carte SD ; le système lira la première playlist trouvée. Pour changer de liste, accédez à `Menu OSD → Playlists`. 🎞️

<img width="514" height="565" alt="Script PlayList" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />

---

## 🕹️ Intégration Arcade (Batocera, Recalbox, RetroBat ou ReplayOS)

Activez l'option `Menu → Lecture → Arcade` pour que le panneau affiche les marqueurs du jeu ou du système en cours d'utilisation :

```text
🏠 MENU PRINCIPAL
├── 📂 Lecture
│   ├── 🖼️ Mode: [GIFs / Horloge]
│   ├── 🔀 Aléatoire: [OUI / NON]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS / RetroBat]   <-- ICI
│   └── 🔙 Retour
```

> [!IMPORTANT]
> Pour synchroniser vos ROMs, utiliser le script PC et installer les scripts de communication, consultez le guide détaillé propre à chaque système :
>
> **[👉 Instructions pour Batocera](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_BATOCERA.md)**
>
> **[👉 Instructions pour Recalbox](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RECALBOX.md)**
>
> **[👉 Instructions pour RetroBat](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RETROBAT.md)**
>
> **[👉 Instructions pour ReplayOS](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_REPLAYOS.md)**

---

## 🏠 Intégration Home Assistant (guide complet)

Vous pouvez intégrer et contrôler entièrement **Retro Pixel LED Lite** depuis **Home Assistant** via l'API REST locale, sans dépendre du cloud.

Cette intégration vous permet de :
- 🟢 **Allumer / Éteindre** le panneau à l'aide d'un interrupteur (*switch*).
- 📊 **Consulter l'état actuel** (mode actif, playlist en cours, etc.).
- 🔄 **Changer de mode** (Horloge / GIF) et de **playlist** de manière instantanée.
- 💬 **Envoyer des messages texte défilants** en sélectionnant la couleur, la vitesse et la police depuis le tableau de bord.

### 📦 Ajouter la configuration à Home Assistant

Si vous utilisez une structure par paquets (`packages`), enregistrez le fichier `retropixel.yaml` disponible **[ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Home%20Asisstant)** dans le dossier `/config/packages/`. Si vous n'utilisez pas les paquets, collez son contenu dans votre fichier `configuration.yaml`.

> ⚠️ **Important :** remplacez l'adresse IP `192.168.31.210` par celle de votre ESP32, et adaptez les noms des playlists selon votre configuration.

Une carte `entities.yaml` prête à être intégrée à votre tableau de bord est également disponible.

<img width="822" height="1005" alt="Captura HA" src="https://github.com/user-attachments/assets/9294b479-f428-4c68-9fcc-c871ad2e88e4" />

---

## 🧠 Architecture interne / Core Lite

Détails techniques sur le fonctionnement interne du système :

- **📡 Contrôle IR et mappage dynamique :** prise en charge des télécommandes infrarouges avec mappage des fonctions depuis le menu OSD (luminosité, navigation, alimentation, confirmation).
- **📂 Serveur FTP de maintenance :** gestion sans fil du fichier `config.ini` et des playlists sans retirer la carte MicroSD.
- **Anti-Panic RAM Management :** surveillance du *heap* — si le DMA ne peut pas allouer de mémoire après l'utilisation du WiFi, le système bascule en Single Buffer pour garantir la stabilité.
- **Moteur de recherche binaire (Arcade) :** localise les marqueurs parmi des milliers de fichiers en quelques millisecondes en sautant directement à la position sur la SD grâce à des index triés par ordre alphabétique — sans scanner les dossiers.
- **Mémoire adaptative (Single/Double Buffer) :** Double Buffer pour une fluidité optimale dans la lecture des GIFs ; bascule automatique en Single Buffer en mode Arcade lors du chargement de bitmaps haute définition.
- **API HTTP en temps réel :** gestionnaire de commandes pour la synchronisation avec Batocera, Recalbox, ReplayOS et Home Assistant.
- **📶 Portail WiFi intégré :** point d'accès temporaire avec DNS captive portal et serveur web dédié détruit à la fin de l'opération, sans laisser de routes ouvertes sur le réseau. Recherche les réseaux *avant* d'activer le point d'accès, génère une clé aléatoire affichée uniquement sur le panneau et démarre en Single Buffer pour économiser la RAM (la valeur de l'utilisateur est restaurée à la sauvegarde).
- **🏷️ mDNS :** le panneau s'annonce sous le nom `retropixel.local` tant que le WiFi est actif ; la PWA conserve la dernière IP connue comme solution de secours.
- **🩺 Diagnostic `/diag` :** JSON en lecture seule, sans accès à la SD, indiquant la mémoire libre et minimale, la raison du dernier redémarrage, le signal WiFi et l'utilisation de la partition OTA.
- **🧩 Code modulaire :** les routes HTTP sont gérées dans `WebRoutes.ino` et le portail dans `PortalWiFi.ino` (utilisant `PortalPaginas.h` et `PortalTextos.h`).
- **Smart Text Centering :** aligne automatiquement les menus et les états au centre de la matrice (`offset + 64px`), en calculant la largeur de chaque chaîne de caractères.
- **WiFi Stealth Mode :** le WiFi ne s'active que brièvement pour synchroniser l'heure et la météo ; le reste du temps, le système fonctionne **100% hors ligne**, garantissant **0 lag** lors de la lecture.
- **Barre de notifications dynamique :** lorsque la météo est active, l'horloge abaisse sa position (`startY=9`) pour afficher le paramètre `WEATHER_MSG`, l'icône météo et la température.
- **Iconographie avancée (jour/nuit) :** icônes de 8×8 px dessinées sur mesure (Soleil, Lune, Nuages, Pluie, Neige, Orage, Brouillard), adaptées en fonction de l'heure.
- **Système de playlists dynamiques :** prise en charge de plusieurs fichiers `.txt` dans `/playlists/`, permettant de basculer entre des collections thématiques depuis le menu OSD.
- **Interruption automatique par l'horloge :** interrompt la galerie de GIFs tous les *x* GIFs pour afficher l'heure pendant *x* secondes, puis reprend la lecture là où elle s'était arrêtée.
- **Résilience hors ligne :** en l'absence de WiFi, le système ignore la synchronisation et lit immédiatement les GIFs en s'appuyant sur l'horloge interne de la puce.
- **Moteur de rendu Double Buffer :** exploite le DMA de l'ESP32 pour dessiner les images de manière invisible, sans aucun scintillement.

---

## 📜 Historique détaillé des modifications (v3.0.0 → v3.1.5)

| Fonctionnalité | Détail technique | Bénéfice |
| :--- | :--- | :--- |
| **📶 Portail WiFi intégré** | Point d'accès temporaire (`RetroPixel-LED`) avec clé aléatoire affichée à l'écran, DNS captive portal et page web intégrée listant les réseaux triés par puissance de signal. Teste la connexion avant de l'enregistrer dans `config.ini`. S'ouvre depuis `Menu OSD → WiFi → Ajouter réseau` ou automatiquement si `WIFI_ENABLE=1` et `SSID` est vide. Textes en ES/EN/FR intégrés au firmware. | **Connectez le panneau sans éditer de fichiers.** Changer de routeur ne nécessite plus de retirer la carte SD. |
| **🏷️ Accès par nom (mDNS)** | Le panneau s'annonce sous le nom `retropixel.local`. La PWA accepte le nom ou l'IP (`GET /status` inclut désormais `ip`), conserve la dernière IP connue et propose son utilisation si le nom ne se résout pas. | **Indépendance vis-à-vis de l'adresse IP**, qui peut changer selon le routeur. |
| **🔔 Notification de mise à jour** | La PWA compare la version du firmware (`fw` et `fwNum` dans `GET /status`) avec le fichier `version.json` sur GitHub Pages. Point ambre sur 🔄, affichant la version disponible et ses nouveautés (`notes`). | **Soyez informé des nouvelles versions** sans recherche manuelle. |
| **ℹ️ Page "À propos" (PWA)** | Affiche la version de la PWA (lue depuis le cache du service worker) et du firmware, le créateur, la licence et un diagnostic en direct avec bouton de copie du rapport. | **Rapports d'erreurs simplifiés** et visibilité immédiate sur l'état du panneau. |
| **🩺 Endpoint `/diag`** | `GET /diag` : temps de fonctionnement (uptime), heap libre et minimum, plus grand bloc libre, raison du dernier redémarrage, RSSI, taille et vitesse de la SD, CPU et utilisation de la partition OTA. | **Diagnostic de mémoire et de stabilité** sans câble USB. |
| **🔐 Mots de passe WiFi avec `#`** | Dans `config.ini`, pour `SSID` et `PASS`, seul un `#` précédé d'un espace est considéré comme un commentaire. | **Tous les mots de passe fonctionnent**, y compris ceux contenant un `#`. |
| **🛡️ Service worker PWA** | Version unique dans `sw.js` (nom du cache), installation tolérante aux pannes, correspondance exacte des routes (n'intercepte plus les requêtes du panneau) et suppression ciblée de ses propres caches. | **Mises à jour de la PWA plus fiables.** |
| **🕹️ Support RetroBat** | Intégration avec le frontend RetroBat pour la détection et la lecture automatique des GIFs/marqueurs associés aux jeux. | **Expérience arcade dynamique.** Affiche le marqueur du jeu actif lors du changement de titre dans RetroBat. |
| **💾 Vitesse de lecture SD configurable** | Paramètre `SD_SPEED` (4, 10, 20 ou 25 MHz ; 10 par défaut) éditable depuis la PWA, le menu OSD (Paramètres avancés) et `config.ini`. Démarrage en deux phases avec bascule automatique vers une vitesse inférieure si la carte ne répond pas. | **Chargement des GIFs plus rapide** et ajustable selon la qualité de chaque carte SD, sans recompilation. |
| **🎞️ Correctif de vitesse des GIFs** | Suppression du double temps d'attente entre les images (`playFrame` synchronisé + délai propre). Le temps de décodage est désormais déduit du temps d'attente. | **Lecture plus fluide** et fidèle à la vitesse réelle de chaque image du GIF. |
| **🧩 Partitionnement Minimal SPIFFS** | Schéma *Minimal SPIFFS* (1,9 Mo pour l'application avec OTA) au lieu du schéma par défaut de 1,25 Mo. | **Plus d'espace firmware** pour de futures fonctionnalités. Nécessite une réinstallation web depuis les versions antérieures. |
| **🕹️ Support RePlayOS** | Intégration avec le frontend RePlayOS pour la détection et la lecture automatique des GIFs/marqueurs associés aux jeux. | **Expérience arcade dynamique.** Affiche le marqueur du jeu actif lors du changement de titre dans RePlayOS. |
| **⏰ Nouveaux styles d'horloge** | Ajout de nouveaux modes de rendu et visuels sélectionnables pour l'affichage de l'heure. | **Personnalisation esthétique accrue** adaptée à différents environnements. |
| **📡 Mappage IR dans la PWA** | Interface dans la PWA pour capturer, attribuer et enregistrer les codes des touches de la télécommande dans `config.ini`. | **Configuration intuitive de la télécommande IR** désormais accessible depuis la PWA. |
| **🏠 Intégration Home Assistant** | Exposition d'endpoints REST API (`GET /status`, `POST /control`, `/playlist`, `/texto`, `/timer/toggle`) et paquet YAML complet. | **Domotique et automatisation.** Allumez, éteignez, changez de mode, de playlist et envoyez du texte depuis HA. |
| **🔤 Sélection de police pour le texte** | 4 typographies sélectionnables par paramètre (`Bold`, `SemiBold`, `Regular`, `Light`) dans les endpoints HTTP, la PWA et l'intégration REST. | **Personnalisation visuelle** du texte défilant. |
| **🎛️ Panneau de contrôle PWA** | Application web progressive comprenant 5 modules de contrôle et d'édition à distance de `config.ini`. | **Contrôle total depuis n'importe quel appareil**, sans serveur externe. |
| **☀️ Contrôle de la luminosité** | Curseur en temps réel (0-100%), application immédiate. | **Ajustement fluide** selon la luminosité ambiante. |
| **🔤 Texte défilant UTF-8** | Décodeur UTF-8→Latin-1 en temps réel, prise en charge des caractères accentués. | Messages avec caractères accentués sans altération. |
| **🎨 Mode GIF / Horloge / Texte** | Sélecteur de mode sur la page d'accueil de la PWA, sans redémarrage. | Modifications visibles instantanément. |
| **🎞️ Playlists dynamiques** | Changement de playlist en temps réel depuis la PWA. | Basculez entre vos collections sans interrompre la lecture. |
| **⏰ Minuteur intelligent** | Plage horaire programmée, débrayage manuel par bouton ou via la PWA. | Automatisation complète de l'allumage et de l'extinction. |
| **🔄 Mise à jour à distance (OTA + langues)** | Téléchargement du firmware et des langues depuis GitHub sans extraire la carte SD. | Maintenance entièrement sans fil. |
| **⚙️ Paramètres à distance** | Édition à distance du fichier `config.ini` : WiFi, matériel, lecture, météo, langue. | Configuration sans fil intégrale. |
| **💬 Contrôle du texte défilant** | Endpoints HTTP POST + PWA pour la gestion des messages, de la couleur et de la vitesse. | Envoi d'alertes à la volée sans reprogrammation. |
| **💥 Transition de particules** | Moteur de particules animé pour l'apparition et la disparition de l'horloge. | Fluidité visuelle, sans coupures brutes. |
| **🎨 Sélection de couleur OSD** | Menu interactif mappé avec télécommande IR et sauvegarde en mémoire EEPROM/SD. | Changez la couleur de l'horloge sans éditer `config.ini`. |
| **⚡ Horloge sans scintillement** | Rendu optimisé en mode *Single Buffer*. | Zéro scintillement lors du rafraîchissement des données. |
| **🧠 Optimisation de la RAM** | Remplacement de `String` par `char[]`, utilisation massive de `PSTR()`/`F()`. | Zéro fragmentation ; plus de heap disponible pour le Double Buffer. |
| **🛡️ Système Anti-Panique** | Vérification de `display->begin()` avec bascule en Single Buffer en cas d'échec. | Évite les plantages (`StoreProhibited`) liés à la fragmentation mémoire après l'usage du WiFi. |
| **🖱️ Confirmation sécurisée** | Détection par durée de pression (*Long Press*). | Évite les validations accidentelles dans les menus. |
| **📂 Serveur FTP intégré** | Transfert de fichiers sans fil directement sur la carte SD. | Gère les playlists ainsi que les fichiers `.ini`/`.json` sans retirer la carte MicroSD. |
| **📡 Contrôle distant IR** | Mappage dynamique des fonctions et de la navigation. | Luminosité, allumage et menu accessibles depuis la télécommande. |
| **🎨 Configuration des couleurs** | Paramètre `colorOrder` (RGB/RBG/GBR) dynamique depuis `config.ini`. | Compatible avec n'importe quel panneau HUB75 sans reprogrammation. |

---

## 🛒 Liste du matériel

Composants testés durant le développement :

- **Microcontrôleur :** [ESP32 DevKit V1 (30 broches) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
- **Panneau LED Matrix (HUB75) :** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)
- **Lecteur de carte :** [Module adaptateur Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
- **Carte d'interconnexion ESP32-Panneau LED :** [DMDos Board V3 - Mortaca](https://www.mortaca.com/) *(optionnelle, sans soudure, avec lecteur SD intégré)*
- **Récepteur IR :** [Module récepteur infrarouge universel - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)
- **Bouton-poussoir :** [Interrupteur momentané DS-316 - AliExpress](https://es.aliexpress.com/item/4000888761296.html)
- **Alimentation :** alimentation 5V (minimum 2A recommandé pour les panneaux 64×32).

---

## 🔌 Connexions

Si vous utilisez la carte **DMDos Board V3**, cette étape est déjà réalisée — passez au point suivant.

#### 📂 Lecteur de carte Micro SD (Interface SPI)
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
| **R1** | GPIO 25 | Données Rouge (Partie supérieure) |
| **G1** | GPIO 26 | Données Vert (Partie supérieure) |
| **B1** | GPIO 27 | Données Bleu (Partie supérieure) |
| **R2** | GPIO 14 | Données Rouge (Partie inférieure) |
| **G2** | GPIO 12 | Données Vert (Partie inférieure) |
| **B2** | GPIO 13 | Données Bleu (Partie inférieure) |
| **A** | GPIO 33 | Sélection de ligne A |
| **B** | GPIO 32 | Sélection de ligne B |
| **C** | GPIO 22 | Sélection de ligne C |
| **D** | GPIO 17 | Sélection de ligne D |
| **E** | GND | Masse |
| **CLK** | GPIO 16 | Clock |
| **LAT** | GPIO 4 | Latch |
| **OE** | GPIO 15 | Output Enable (Luminosité) |

#### 🕹️ Contrôles utilisateur (Physique et Infrarouge)
| Composant | Broche ESP32 | Fonction |
| :--- | :--- | :--- |
| **Bouton (PIN)** | GPIO 21 | **Multifonction :** Clic (Naviguer) / Pression longue (Confirmer - Allumage/Extinction). |
| **Bouton (GND)** | GND | Référence de masse. |
| **Récepteur IR (Data)** | GPIO 34 | Entrée du signal (Protocole NEC/etc). |
| **Récepteur IR (VCC)** | 3.3V | Alimentation du capteur. |
| **Récepteur IR (GND)** | GND | Référence de masse. |

<img width="769" height="716" alt="image" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />

---

### 📂 Explorateur SD (FTP) — Détail

Cette fonction active un serveur de fichiers sans fil afin de faciliter la maintenance sans retirer la carte MicroSD.

> [!IMPORTANT]
> **Utilisation recommandée :** édition de `config.ini`, des fichiers de langue (`.json`), des playlists (`.txt`) et de petits fichiers. En raison des limitations de bande passante de l'ESP32, **il n'est pas recommandé de l'utiliser pour transférer des collections complètes de GIFs** — le processus serait très lent par rapport à un lecteur de carte mémoire.

**Activer le serveur FTP :**
1. `Menu OSD → Explorateur SD`.
2. Sélectionnez **Démarrer FTP**.
3. Le panneau stoppe la lecture des GIFs et affiche son **adresse IP** (ex. `192.168.1.109`).

**Configuration de la connexion (FileZilla) :**
- **Protocole :** FTP simple (non sécurisé).
- **Serveur/Hôte :** l'adresse IP affichée sur le panneau.
- **Type d'authentification :** Normal.
- **Utilisateur / Mot de passe :** `admin` / `admin`.
- **Port :** `21`.
- **Connexions simultanées :** limiter strictement à 1.

<img width="545" height="227" alt="image" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />
<img width="544" height="193" alt="image" src="https://github.com/user-attachments/assets/ba4c85bc-920a-48c9-83d8-99b96ecbc57f" />

**Dans Édition → Options → Transferts :**
- Nombre maximal de transferts simultanés : 1.
- Limites de vitesse activées : 20 KiB/s en téléchargement et en envoi.

<img width="841" height="522" alt="image" src="https://github.com/user-attachments/assets/e90d3e84-9c93-45c0-b942-8b601db40041" />

**Alternative sans FileZilla (Explorateur Windows)** *(non recommandé — lors des tests, certains fichiers n'ont pas été transférés intégralement)* :
1. **Ce PC** → clic droit → **"Ajouter un emplacement réseau"**.
2. Adresse : `ftp://<IP_du_panneau>` (ex. `ftp://192.168.1.109`).
3. Décochez "Ouvrir une session anonyme", nom d'utilisateur `admin`.
4. Attribuez un nom explicite à l'emplacement réseau.

**Remarques de sécurité :**
- Le panneau ne lit aucun GIF tant que le serveur FTP est actif.
- Pour quitter : appuyez sur le bouton physique ou sur la touche "Valider" de la télécommande IR.
- Ne coupez pas l'alimentation pendant la modification d'un fichier via FTP sous peine de le corrompre.

---

## 🛠️ Feuille de route (Roadmap)

### ⚡ Optimisation et fonctionnalités

- 📂 Gestionnaire de fichiers dans la PWA : téléversement de GIFs et création de playlists via WiFi (sous réserve de stabilité et de vitesse satisfaisantes).

### 🎨 Esthétique et connectivité

- 🎮 Mini-jeux (Snake, Pong) contrôlés à l'aide de la télécommande IR.

---

## ⚖️ Licence et remerciements

Ce projet est publié sous **Licence MIT**.

Remerciements particuliers aux développeurs des bibliothèques fondamentales :
- **Bitbank2** pour l'excellente bibliothèque `AnimatedGIF`.
- **Mrfaptastic** pour le moteur DMA haute performance dédié aux matrices LED.
- **La communauté Telegram DMDos**, où la découverte des capacités de DMDos m'a encouragé à développer **Retro Pixel LED**.
- **RpiTe@m** pour le partage du pack gratuit de 600 GIFs et leur compilation de 11 000 GIFs disponible [ici](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065).
- **shan-aya** pour la traduction en français et son formidable logiciel de création de [GIFs](https://github.com/shan-aya/DMD_GIF_converter).
- **joseAveleira** pour l'effet de particules sur l'horloge. [GitHub](https://github.com/joseAveleira/RelojPixel/tree/main)

---

<p align="center">Fait avec ❤️ et de nombreux GIFs rétro. Si ce projet vous est utile, une ⭐ sur le dépôt aide également beaucoup !</p>
