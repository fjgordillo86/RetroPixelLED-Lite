## 📝 Changelog (Registre des modifications)

### [v3.1.0] - 2026-08-02
**Retro Pixel LED Lite: "PWA Total Control"**

#### ✨ Ajouté
* **Progressive Web App (PWA) de contrôle à distance :** Interface web installable permettant un contrôle total du panneau depuis n'importe quel appareil du réseau local :
  - **Page principale :** Réglage de la luminosité en temps réel et sélecteur de mode GIF / Horloge / Texte, chacun affichant uniquement ses commandes pertinentes (playlist + mode aléatoire en GIF ; style + couleur pour l'Horloge ; aperçu en direct de la matrice + couleur + vitesse pour le Texte).
  - **Minuteur (Temporisateur) :** Programmation des horaires d'allumage/extinction avec dérogation manuelle immédiate depuis l'application elle-même.
  - **Mise à jour :** Mises à jour OTA du firmware et téléchargement des fichiers de langue `.json` depuis GitHub directement dans le dossier `/idiomas` de la carte SD, sans avoir à l'extraire.
  - **Paramètres :** Édition à distance du fichier `config.ini` (WiFi, Matériel, Arcade, Texte défilant, Horloge, Météo, Langue), avec redémarrage automatique lorsque la modification le requiert.
* **Mode Texte défilant avec prise en charge de polices personnalisées :** Moteur de texte défilant avec police `GFXfont` dédiée (incluant les caractères polonais) et décodeur UTF-8 en temps réel, activable depuis la PWA avec couleur et vitesse configurables.
* **Second mode d'installation pour Batocera :** Identique à Recalbox, possibilité de choisir entre "Menus et Jeux" (réagit également lors de la navigation dans les systèmes) ou "Jeux uniquement" (marquee fixe/horloge dans les menus, change uniquement au lancement d'une partie).
* **Hooks d'extinction/redémarrage pour Batocera :** Nouveaux scripts pour les événements `quit`, `shutdown` et `reboot` d'EmulationStation, avertissant le panneau de quitter le marquee lors de la fermeture du système.

#### ⚙️ Améliorations / Modifications internes
* **Refactorisation des routes Web :** Les plus de 20 points de terminaison (endpoints) HTTP du serveur ont été extraits de `setup()` vers un fichier `WebRoutes.ino` indépendant, organisés par fonction (état, contrôle en direct, minuteur, configuration, playlists, OTA, langues, arcade, texte).
* **Contrôle en direct sans redémarrage :** La luminosité, le mode de lecture, le mode aléatoire, ainsi que le style et la couleur de l'horloge s'appliquent désormais instantanément depuis la PWA et sont enregistrés dans `config.ini` sans nécessiter de redémarrer le panneau.
* **Animation d'extinction renouvelée :** Remplacement du visage endormi par une animation de style extinction de tube cathodique (CRT), tout en réduisant de moitié le temps de transition bloquant.
* **Qualité d'image dans les outils de marquees :** Intégration du dithering Floyd-Steinberg vers RGB565 et rendu en haute qualité sur les convertisseurs de Logos et Batocera, afin d'éliminer le banding de couleur sur les dégradés affichés sur le panneau.
* **Installateur de scripts Arcade réorganisé :** Les scripts Batocera et Recalbox sont désormais distribués dans des sous-dossiers distincts `Batocera/` et `Recalbox/` au lieu d'être mélangés dans un seul dossier.

#### 🛡️ Correctifs
* **Cycle de réglage de la luminosité :** Correction d'une erreur logique dans la boucle de luminosité qui empêchait la réinitialisation à 5 % une fois la valeur de 100 % atteinte.
* **Minuteur et serveur web :** Correction d'un problème en raison duquel le panneau cessait de répondre aux requêtes HTTP en mode veille, empêchant son allumage à distance depuis la PWA.

---

### [v3.0.5] - 2026-06-18

#### 🛡️ Correctifs
* **Stabilité de l'effet Rainbow :** Correction du rendu de l'effet dynamique *Rainbow* (Arc-en-ciel), qui était affecté négativement par l'optimisation anti-scintillement de l'horloge lorsque le *Double Buffer* était désactivé.
* **Rafraîchissement de la luminosité dans l'OSD :** Correction d'un bug visuel dans le menu OSD ; le pourcentage de luminosité s'actualise désormais dynamiquement à l'écran en temps réel lors de son ajustement à la télécommande ou au bouton.

---  

### [v3.0.4] - 2026-05-31
**Retro Pixel LED Lite: "Particles & System Stability"**

#### ✨ Ajouté
- **Transition "Explosion de particules" :** Nouvel effet dynamique pour l'apparition et la disparition de l'horloge, améliorant la fluidité visuelle.
- **Sélection de couleur via OSD :** Nouvelle option dans le menu pour changer la couleur de l'horloge en temps réel sans éditer le fichier `.ini`.
- **Serveur FTP intégré :** Protocole de transfert de fichiers sans fil permettant de gérer les playlists et configurations directement sur la carte SD.
- **Télécommande IR :** Mappage dynamique des fonctions pour naviguer dans les menus, ajuster la luminosité et contrôler le panneau à distance.

#### ⚙️ Améliorations
- **Mise en œuvre du Buffer Unique (Single Buffer) :** Refactorisation de la logique de rendu afin d'éliminer les scintillements sur l'horloge et les menus.
- **Optimisation de la RAM :** Migration complète des objets `String` vers des tableaux `char[]` et utilisation intensive de `PSTR()` / `F()` pour libérer le Heap et éviter la fragmentation.
- **Système Anti-Panic :** Vérification de sécurité lors de `display->begin()` avec basculement automatique sur le Buffer Unique en cas de fragmentation mémoire après l'utilisation du WiFi.
- **Confirmation sécurisée (Pression longue) :** Détection de la pression longue sur le bouton physique pour éviter les actions accidentelles dans les menus.
- **Configuration de couleur universelle :** Traitement dynamique du paramètre `colorOrder` (RGB/RBG/GBR) depuis `config.ini` pour assurer la compatibilité avec tous les panneaux HUB75.

#### 🛡️ Correctifs
- **Stabilité du rendu :** Élimination des erreurs d'allocation mémoire (*StoreProhibited*) lors de fortes charges réseau.
- **Nettoyage des journaux (Logs) :** Amélioration des diagnostics d'initialisation du système pour détecter rapidement les échecs d'allocation de buffer.
  
---  

### [v2.1.4] - 2026-04-24
**Retro Pixel LED Lite: "Arcade Mastery & Binary Speed"**

#### ✨ Ajouté
- **Mode Arcade (Intégration Batocera) :** Implémentation d'un récepteur d'événements HTTP pour la synchronisation automatique des marquees avec des systèmes externes.
- **Moteur de recherche binaire :** Algorithme haute performance pour localiser les jeux sur la carte SD en quelques millisecondes, éliminant le ralentissement sur les grandes collections.
- **Logique de repli (Fallback) :** Système d'affichage intelligent : Jeu > Logo de système > Image par défaut.
- **Outil d'indexation (PowerShell) :** Script interactif pour PC automatisant le traitement des images (BMP 24 bits) et la création de fichiers d'indexation `.txt`.

#### ⚙️ Améliorations
- **Gestion dynamique du buffer :** Le système bascule automatiquement en **Single Buffer** lorsque le mode Arcade est actif afin de maximiser la mémoire RAM disponible.
- **Optimisation de la mémoire SD :** Fermeture forcée des fichiers après la lecture des index pour éviter la saturation des descripteurs de fichiers (File Handles).
- **Retour d'information via console :** Journaux série détaillés pour diagnostiquer la réception des commandes IP et vérifier l'existence des fichiers sur la carte SD.

#### 🛡️ Correctifs
- **Erreur de fin de ligne dans les scripts :** Correction d'une erreur de syntaxe dans le lanceur `.ps1` qui empêchait son exécution sur les systèmes Windows soumis à des politiques restrictives.
- **Nettoyage des noms (Trim) :** Les noms de jeux et de systèmes ignorent désormais les espaces superflus envoyés par Batocera, évitant les erreurs de type "Fichier non trouvé".
- **Stabilité de changement d'état :** Correction d'un bug qui empêchait le retour correct à la galerie de GIFs après la réception d'une commande d'arrêt (STOP/OFF).

---

### [v2.1.0] - 2026-04-18
**Retro Pixel LED Lite: "Global Voice & Wireless Evolution"**

#### ✨ Ajouté
- **Système multilingue dynamique :** Prise en charge de dictionnaires externes au format `.json` (ES, EN, FR...). Chargement intelligent depuis la carte SD pour économiser la RAM.
- **Mise à jour sans fil (OTA) :** Moteur de téléchargement et d'installation du firmware directement depuis le menu OSD via GitHub.
- **Centrage intelligent du menu (Smart Menu Centering) :** Algorithme de centrage automatique du texte ajustant l'affichage des menus selon la largeur des caractères de chaque langue.
- **Indicateur visuel "Sleep" :** Iconographie personnalisée (Lune et Emoji 😴) conçue pixel par pixel pour le mode d'économie d'énergie.

#### ⚙️ Améliorations
- **Gestion de la RAM (Anti-Panic) :** Libération forcée de la mémoire après la fermeture du menu OSD pour éviter les redémarrages intempestifs.
- **Génération du fichier Config.ini :** Le système génère désormais automatiquement les commentaires du fichier de configuration dans la langue sélectionnée par l'utilisateur.
- **Expérience utilisateur (UX) multilingue :** Sélecteur de langue en temps réel appliquant les modifications sans nécessiter de redémarrage manuel du panneau.

#### 🛡️ Correctifs
- **Stabilité de l'analyseur JSON (JSON Parser) :** Correction d'une erreur critique provoquant un *Kernel Panic* lors de la lecture de fichiers de langue contenant des clés trop longues.
- **Sécurisation du client OTA :** Ajustement de la gestion des certificats pour garantir une connexion sécurisée avec les serveurs de mise à jour.
- **Texte OSD :** Élimination des deux-points ":" en double dans les chaînes de caractères du menu pour un affichage plus propre.

---

### [v2.0.5] - 2026-04-11
**Retro Pixel LED Lite: "Smart Energy, Dual Vision & Safety Core"**

#### ✨ Ajouté
- **Mode visuel double :** Option dans le menu permettant d'alterner entre "Horloge seule" (minimaliste) et "Playlist de GIFs" (animé).
- **Minuteur intelligent (Smart Timer) :** Programmation de l'allumage/extinction automatique avec gestion du passage de minuit.
- **Dérogation manuelle (Manual Override) :** Pression très longue (4s) pour forcer l'état d'alimentation, bloquant le minuteur jusqu'au cycle suivant.
- **Bouclier de sécurité I2S (I2S Safety Shield) :** Système de protection limitant dynamiquement la fréquence à 16 MHz lors de l'activation du Double Buffer pour garantir une stabilité totale.

#### ⚙️ Améliorations
- **Navigation UI intelligente :**
    - Pression courte : Sortie de veille / Navigation.
    - Pression longue : Diminution rapide des valeurs (-5 min sur le minuteur).
    - Pression continue : Accélération des valeurs (+5 min sur le minuteur).
- **Boucle ultra-réactive :** Élimination du code bloquant ; le bouton interrompt désormais instantanément toute animation ou processus réseau.
- **Cycle d'horloge optimisé :** Fréquence d'apparition de l'horloge ajustée de [2...10] GIFs avec des incréments de +2 pour une configuration plus logique et rapide.
- **Assainissement de l'API Météo :** Amélioration du traitement des URL pour les villes contenant des espaces ou des tirets, évitant les échecs de récupération des données météo.
- **Menus paginés :** Réorganisation de l'OSD sur plusieurs pages pour améliorer la lisibilité des nouveaux paramètres avancés.

---

### [v2.0.0] - 2026-03-26
**Retro Pixel LED Lite: "OSD Menu, Night Mode & Smart RAM"**

#### ✨ Ajouté
- **Menu OSD (On-Screen Display) :** Interface native sur le panneau LED pour configurer les playlists, la luminosité, le WiFi et l'horloge à l'aide d'un seul bouton physique.
- **Mode nuit dynamique :** Intégration d'icônes de lune et de palettes de couleurs froides automatiques basées sur l'heure locale et les données OpenWeatherMap.
- **Plug & Play automatique :** Le système recherche et lit désormais la première playlist trouvée dans `/playlists` si aucune n'est sélectionnée.
- **Sauvegarde sur carte SD :** Enregistrement automatique de tous les réglages effectués depuis le menu OSD directement dans le fichier `config.ini`.

#### ⚙️ Améliorations
- **Rafraîchissement intelligent de la RAM (Smart RAM Refresh) :** Logique de réinitialisation intelligente lors de l'actualisation de la météo ou de l'heure afin d'éviter la fragmentation de la mémoire causée par le Double Buffer.
- **Gestion du WiFi en mode furtif (Stealth) :** Désactivation complète de la pile réseau après la synchronisation pour éliminer les ralentissements et réduire la température de l'ESP32.
- **Synchronisation NTP silencieuse :** Ajustement de l'horloge interne à chaque plage d'actualisation météo pour éviter les décalages horaires.
- **Optimisation des playlists :** Transitions instantanées entre les listes thématiques depuis le menu sans redémarrage de l'appareil.

---

### [v1.1.2] - 2026-03-19
**Retro Pixel LED Lite: "Double Buffering & Splash Screen y Branding"**

#### ✨ Ajouté
- **Moteur de rendu :** Implémentation de la technique de Double Buffering (double tampon mémoire) pour une lecture de contenu fluide.
- **Logo RGB dynamique :** Écran de démarrage avec le logo "RETRO PIXEL LED lite" utilisant des couleurs distinctes pour le sigle LED et des cadres de contour stylisés.
- **Identification du firmware :** Affichage direct de la version du système (`v1.1.2`) sur l'écran de chargement pour faciliter le suivi des versions et le support.

#### ⚙️ Améliorations
* **Séquencement critique :** Le système gère désormais la connexion WiFi, la synchronisation NTP et le téléchargement des données météo *avant* d'initialiser le panneau LED.
* **Libération des ressources :** Une fois les données récupérées, le pilote WiFi est totalement désactivé afin d'allouer toute la mémoire RAM au moteur graphique, évitant ainsi l'erreur d'initialisation `0x3001`.

---

### [v1.1.0] - 2026-03-03
**Retro Pixel LED Lite: "The Weather & Notification Update"**

#### ✨ Ajouté
- **Barre de notifications :** Intégration d'une bande supérieure (Y=0 à Y=8) dédiée aux informations système.
- **Message personnalisé :** Nouveau paramètre `WEATHER_MSG` dans `config.ini` pour afficher un texte fixe (ex : "Game Room") sur le marquee.
- **Prise en charge d'OpenWeatherMap :** Intégration de l'API officielle pour télécharger les données météorologiques en temps réel.
- **Iconographie Bitmap :** Ajout de 6 icônes exclusives en 8x8 pixels (Soleil, Nuages, Pluie, Neige, Orage, Brouillard) optimisées pour les panneaux LED.
- **Positionnement dynamique :** Réglage automatique de la position de l'Horloge (`startY=9`) lorsque la météo est active afin d'éviter les chevauchements visuels.

#### ⚙️ Améliorations
- **Gestion du WiFi :** Optimisation du "Stealth Mode". Le WiFi s'active désormais périodiquement selon l'intervalle `WEATHER_INT` pour rafraîchir les données avant de se désactiver à nouveau.
- **Lecture du fichier INI :** Ajout de la logique d'analyse pour `CITY`, `API_KEY` et `WEATHER_MSG`.
- **Esthétique de l'horloge :** Le symbole du degré (°C) utilise désormais un tracé vectoriel en 2x2 pixels pour une plus grande netteté.

#### 🛡️ Correctifs
- Correction du scintillement de la barre supérieure en intégrant son rendu directement dans le buffer DMA de l'horloge.
- Ajustement de la conversion des températures pour n'afficher que des valeurs entières, évitant ainsi que le texte ne dépasse du panneau.
