# 📝 Changelog (Journal des Modifications)


---

### [v2.1.4] - 2026-04-24  
**Retro Pixel LED Lite : "Arcade Mastery & Binary Speed"**

#### ✨ Ajouté
- **Mode Arcade (Intégration Batocera) :** Implémentation d’un récepteur d’événements HTTP pour la synchronisation automatique des marquises avec des systèmes externes.  
- **Moteur de Recherche Binaire :** Algorithme haute performance permettant de localiser les jeux sur la SD en millisecondes, éliminant tout lag dans les grandes collections.  
- **Logique de Cascade (Fallback) :** Système d’affichage intelligent : Jeu > Logo du Système > Image Par Défaut.  
- **Outil d’Indexation (PowerShell) :** Script interactif pour PC automatisant le traitement des images (BMP 24 bits) et la création des index `.txt`.

#### ⚙️ Améliorations
- **Gestion Dynamique du Buffer :** Le système bascule désormais automatiquement en **Single Buffer** lorsque le mode Arcade est actif afin de maximiser la RAM disponible.  
- **Optimisation de la Mémoire SD :** Mise en place d’une fermeture forcée des fichiers après lecture des index pour éviter l’épuisement des descripteurs de fichiers (File Handles).  
- **Feedback via Console :** Journal série détaillé pour diagnostiquer la réception des commandes IP et la présence des fichiers sur la SD.

#### 🛡️ Corrections
- **Erreur de Terminator dans les Scripts :** Correction d’un problème de syntaxe dans le lanceur `.ps1` empêchant son exécution sur certains systèmes Windows avec politiques restrictives.  
- **Nettoyage des Noms (Trim) :** Les noms de jeux et de systèmes ignorent désormais les espaces accidentels envoyés par Batocera, évitant les erreurs “Fichier introuvable”.  
- **Stabilité du Changement d’État :** Correction d’un bug empêchant le retour correct à la galerie de GIFs après réception d’une commande d’arrêt (STOP/OFF).

---

### [v2.1.0] - 2026-04-18  
**Retro Pixel LED Lite : “Global Voice & Wireless Evolution”**

#### ✨ Ajouté
- **Système Multilingue Dynamique :** Support pour des dictionnaires externes `.json` (ES, EN, FR...). Chargement intelligent depuis la SD pour économiser la RAM.  
- **Mise à jour Sans Fil (OTA) :** Téléchargement et installation du firmware directement depuis le menu OSD via GitHub.  
- **Smart Menu Centering :** Algorithme de centrage automatique basé sur la largeur réelle des caractères de chaque langue.  
- **Feedback Visuel “Sleep” :** Icônes Lune et Emoji 😴 dessinés pixel par pixel pour le mode économie d’énergie.

#### ⚙️ Améliorations
- **Gestion de RAM (Anti-Panic) :** Libération forcée de mémoire après fermeture du menu OSD pour éviter les redémarrages accidentels.  
- **Génération de Config.ini :** Les commentaires du fichier de configuration sont désormais générés automatiquement dans la langue sélectionnée.  
- **UX des Langues :** Changement de langue en temps réel sans redémarrage manuel du panneau.

#### 🛡️ Corrections
- **Stabilité du Parser JSON :** Correction d’un bug critique provoquant un *Kernel Panic* lors de la lecture de fichiers de langue avec des étiquettes trop longues.  
- **OTA Secure Client :** Ajustements dans la gestion des certificats pour garantir une connexion sécurisée aux serveurs de mise à jour.  
- **Texte OSD :** Suppression des doubles “:” dans les chaînes du menu pour une meilleure lisibilité.

---

### [v2.0.5] - 2026-04-11  
**Retro Pixel LED Lite : “Smart Energy, Dual Vision & Safety Core”**

#### ✨ Ajouté
- **Mode Visuel Dual :** Permet d’alterner entre “Horloge seule” (minimaliste) et “Playlist de GIFs” (animé).  
- **Minuterie Intelligente (Smart Timer) :** Programmation d’allumage/extinction automatique avec support du passage après minuit.  
- **Manual Override :** Appui long (4s) pour forcer l’état d’alimentation et bloquer la minuterie jusqu’au prochain cycle.  
- **I2S Safety Shield :** Limitation dynamique à 16MHz lorsque le Double Buffer est activé pour garantir une stabilité totale.

#### ⚙️ Améliorations
- **Navigation UI Intelligente :**  
  - Appui rapide → Réveiller / Naviguer  
  - Appui long → Retour rapide (-5 min)  
  - Appui continu → Accélération (+5 min)  
- **Ultra-Responsive Loop :** Suppression du code bloquant ; le bouton interrompt instantanément toute animation ou tâche réseau.  
- **Cycle d’Horloge Optimisé :** Plage [2...10] GIFs avec pas de +2 pour une configuration plus logique.  
- **Sanitisation API Météo :** Meilleure gestion des villes avec espaces ou tirets.  
- **Menus Paginés :** Réorganisation du menu OSD en plusieurs pages pour une meilleure lisibilité.

---

### [v2.0.0] - 2026-03-26  
**Retro Pixel LED Lite : “OSD Menu, Night Mode & Smart RAM”**

#### ✨ Ajouté
- **Menu OSD :** Interface native pour configurer Playlists, Luminosité, WiFi et Horloge via un seul bouton.  
- **Mode Nuit Dynamique :** Icônes de Lune et palettes froides automatiques basées sur l’heure locale et la météo.  
- **Plug & Play Automatique :** Lecture automatique de la première playlist trouvée dans `/playlists`.  
- **Persistance sur SD :** Sauvegarde automatique des réglages dans `config.ini`.

#### ⚙️ Améliorations
- **Smart RAM Refresh :** Réinitialisation intelligente lors des mises à jour météo/heure pour éviter la fragmentation mémoire.  
- **WiFi Stealth :** Désactivation totale du WiFi après synchronisation pour réduire la latence et la température.  
- **Synchronisation NTP Silencieuse :** Ajustement de l’horloge interne à chaque mise à jour météo.  
- **Optimisation des Playlists :** Changement instantané de liste sans redémarrage.

---

### [v1.1.2] - 2026-03-19  
**Retro Pixel LED Lite : “Double Buffering & Splash Screen y Branding”**

#### ✨ Ajouté
- **Moteur de Rendu :** Double Buffering pour une fluidité maximale.  
- **Logo RGB Dynamique :** Logo “RETRO PIXEL LED lite” avec couleurs indépendantes et contour stylisé.  
- **Identification du Firmware :** Affichage direct de la version (`v1.1.2`) sur l’écran de chargement.

#### ⚙️ Améliorations
- **Séquentialité Critique :** WiFi → NTP → Météo avant l’initialisation du panneau LED.  
- **Libération de Ressources :** WiFi désactivé après récupération des données pour libérer la RAM et éviter l’erreur `0x3001`.

---

### [v1.1.0] - 2026-03-03  
**Retro Pixel LED Lite : “The Weather & Notification Update”**

#### ✨ Ajouté
- **Barre de Notifications :** Bande supérieure (Y=0 à Y=8) pour les informations système.  
- **Message Personnalisé :** Nouvelle étiquette `WEATHER_MSG` dans `config.ini`.  
- **Support OpenWeatherMap :** Intégration de la météo en temps réel.  
- **Iconographie Bitmap :** 6 icônes exclusives 8×8 (Soleil, Nuages, Pluie, Neige, Orage, Brouillard).  
- **Positionnement Dynamique :** Ajustement automatique du Reloj (`startY=9`) lorsque la météo est active.

#### ⚙️ Améliorations
- **Gestion du WiFi :** Mode Stealth avec réveil périodique selon `WEATHER_INT`.  
- **Lecture INI :** Support pour `CITY`, `API_KEY`, `WEATHER_MSG`.  
- **Esthétique du Reloj :** Symbole °C redessiné en 2×2 pixels pour plus de netteté.

#### 🛡️ Corrections
- Correction du scintillement de la barre supérieure.  
- Conversion des températures ajustée pour n’afficher que des valeurs entières.

---
