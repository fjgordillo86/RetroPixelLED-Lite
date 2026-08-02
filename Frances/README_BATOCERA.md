# 🕹️ Intégration avec Batocera

Le **Mode Arcade** de la version Lite permet à votre matrice LED de fonctionner comme un marquee dynamique. Le panneau détectera automatiquement le système et le jeu en cours de navigation ou de lecture et l'affichera.

#### Utilisation des ressources (Scraping)
Le principal avantage de ce système est qu'**il utilise les images que vous avez déjà scrapées dans Batocera** (marquees / wheel art). Le script PowerShell se charge de les rechercher, les redimensionner et les convertir automatiquement.

## 1. Configuration critique : IP fixe pour l'ESP32

Pour que le mode **🕹️ Arcade** de Batocera fonctionne toujours correctement, il est essentiel que l'ESP32 conserve toujours la même adresse IP.

> [!TIP]
> **Attribuer une IP fixe à l'ESP32 :**
> Les scripts de Batocera envoient les commandes (comme changer le GIF lors du lancement d'un jeu) à une adresse IP spécifique que vous configurez manuellement. Si le routeur redémarre et attribue une IP différente à l'ESP32, la communication sera interrompue et le panneau cessera de se mettre à jour.
>
> **Comment faire ?**
> 1. Accédez à la configuration de votre routeur.
> 2. Recherchez la section **DHCP statique** ou **Bail DHCP permanent (IP par adresse MAC)**.
> 3. Associez l'adresse MAC de votre ESP32 à l'adresse IP que vous avez saisie dans vos scripts (ex : `192.168.1.117`).
> 4. Chaque routeur étant différent, si vous avez des doutes, recherchez sur Google : *"Comment attribuer une IP fixe [modèle de votre routeur]"*.

## 2. Installation automatique sur Batocera

À partir de la version **v3.0.0**, il n'est plus nécessaire d'éditer manuellement des lignes de code, de se soucier des formats de fin de ligne Windows ou d'utiliser des consoles SSH avancées (comme PuTTY) pour configurer les autorisations d'exécution.

J'ai développé un **Script d'installation intelligent en PowerShell** qui réalise l'ensemble du déploiement automatiquement depuis votre PC.

---

### 📦 Que fait cet installateur pour vous ?

* **Configuration de l'IP :** Injecte automatiquement l'adresse IP de votre panneau LED dans tous les scripts de communication.
* **Correction du format :** Force le format de fin de ligne **Unix (LF)**. Cela évite que les scripts échouent s'ils ont été ouverts par erreur avec le Bloc-notes de Windows.
* **Organisation des fichiers :** Crée la structure de répertoires nécessaire dans Batocera et copie les fichiers à l'emplacement correspondant.
* **Gestion automatique des permissions (sans PuTTY) :** Génère un script système (`custom.sh`) qui permet à Batocera de s'accorder à lui-même les permissions d'exécution (`chmod +x`) sur les dossiers à chaque démarrage.

---

### 🛠️ Prérequis

1. Votre **PC** et votre **Batocera** doivent être connectés au même réseau local (ou connectez le support de stockage physique de Batocera directement au PC).
2. Connaître l'**IP locale de votre panneau LED** Retro Pixel LED (ex : `192.168.1.117`).
3. Télécharger l'intégralité du dossier `Instalador Automático` depuis ce dépôt, disponible [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Instalador%20Automatico).

> [!IMPORTANT]
> Si vous avez téléchargé le dépôt sous forme de fichier `.zip`, veillez à le **décompresser entièrement** avant d'exécuter l'installateur.

---

### 💻 Pas à pas

1. Ouvrez le dossier `Instalador Automático` sur votre PC. Vous y trouverez deux fichiers et deux dossiers :
   * `Ejecutar Script Instalador Arcade.bat`
   * `Script_Instalador_Arcade.ps1`
   * `Batocera`
   * `Recalbox`

2. Faites un **double-clic** sur `Ejecutar Script Instalador Arcade.bat`.

3. Suivez les instructions dans la fenêtre de la console :
   * **Étape 1 :** Entrez l'adresse IP de votre panneau LED et appuyez sur `Entrée`.
   * **Étape 2 :** Entrez le chemin d'accès à votre Batocera. Il peut s'agir d'un chemin réseau (ex : `\\192.168.1.120` ou `\\BATOCERA`) ou de la lettre d'un lecteur physique si vous avez connecté le disque/carte SD au PC (ex : `E:`).

4. Le script vous demandera :
   * Le système utilisé : sélectionnez `1` pour Batocera.
   * Quel mode de fonctionnement souhaitez-vous activer ?
     * **Option 1 :** Menus et Jeux (Affiche les systèmes lors de la navigation + le jeu lancé)
     * **Option 2 :** Jeux uniquement (Marquee fixe/horloge dans les menus, change uniquement en jeu)

5. Le script traitera les fichiers en quelques secondes. Une fois terminé, vous verrez le message `INSTALACIÓN COMPLETADA!`. Appuyez sur n'importe quelle touche pour quitter.

<img width="1103" height="686" alt="image" src="https://github.com/user-attachments/assets/d94c2a67-c40a-451e-9c61-981a188a294d" />

6. **Redémarrez complètement votre système Batocera.**
> [!CAUTION]
> Le redémarrage complet du système est **obligatoire**. Lors de ce démarrage, le script `custom.sh` configurera les permissions internes. À partir de ce moment, chaque fois que vous naviguerez dans le menu, lancerez ou quitterez un jeu, le panneau réagira automatiquement.

---

### 3. 🛠️ Marquees (Marquesinas)
Nous utiliserons le script situé dans le dossier `Arcade/Marquesinas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Il se compose de deux fichiers : `Ejecutar Script Marquesinas Batocera.bat` et `Script Marquesinas Batocera.ps1`.

1. **Exécutez le fichier** `Ejecutar Script Marquesinas Batocera.bat` (Lanceur pour éviter les blocages de Windows).
2. **Configuration des chemins d'accès :**
   * **Source :** Entrez le chemin de vos ROMs Batocera (ex : `\\192.168.1.119\share\roms`).
   * **Destination :** Entrez le chemin `C:\marquesinas`.
3. **Sélection du système :** Le script détectera automatiquement les systèmes contenant un fichier `gamelist.xml`. Vous pouvez choisir d'en traiter un seul par son numéro, plusieurs, ou **Tous (0)**.
4. **Copie :** Si vous avez sélectionné le chemin `C:\marquesinas`, copiez le dossier `marquesinas` et tout son contenu sur la carte SD ou SSD où Batocera est installé dans `roms/`, comme indiqué à la section `5. Structure des fichiers sur la carte SD ou SSD de Batocera`.

<img width="1096" height="572" alt="image" src="https://github.com/user-attachments/assets/388368a7-a57b-4611-89fc-4bfc184c1fa7" />

### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marquees d'origine au format **128x32 pixels**.
* **Formatage :** Force la couleur au format **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).
* **Fichiers d'indexation .txt :** Génère des fichiers texte (ex : `neogeo.txt`) triés par ordre alphabétique. Ce sont ces fichiers que l'ESP32 lit pour connaître les fichiers existants sans explorer toute la carte SD.

> [!CAUTION]
> **Accès réseau (Samba) :**
> Si lors de l'exécution du script, celui-ci n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera afin que le script puisse accéder au dossier.
> Pour accéder au chemin (ex : `\\192.168.1.120\share\roms`), Windows vous demandera des identifiants. Utilisez ceux par défaut de Batocera :
> * **Utilisateur :** `root`
> * **Mot de passe :** `linux`

> [!CAUTION]
> Chaque fois que vous ajoutez de nouveaux jeux ou effectuez un "Scrape" dans Batocera, **vous devez réexécuter le script PowerShell** sur votre PC pour mettre à jour les index et les images. Sans cette étape, l'ESP32 ne saura pas que de nouveaux fichiers existent.

---

### 4. 🛠️ Logos des systèmes
Vous pouvez utiliser les logos déjà redimensionnés qui se trouvent dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).

1. **Copie :** Copiez le dossier `Logos` et tout son contenu sur la carte SD ou SSD où Batocera est installé dans `roms/marquesinas/`, comme indiqué au point **5. Structure des fichiers sur la carte SD ou SSD de Batocera.**

Si vous préférez utiliser d'autres logos (par exemple ceux du thème que vous avez installé), utilisez le script situé dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Il se compose de deux fichiers : `Ejecutar Script Logos.bat` et `Script Logos.ps1`.

1. **Exécutez le fichier** `Ejecutar Script Logos.bat` (Lanceur pour éviter les blocages de Windows).
2. **Configuration des chemins d'accès :**
   * **Source :** Entrez le chemin où se trouvent vos logos (ex : `\\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos`).
   * **Destination :** Entrez le chemin `C:\Logos`.
3. **Copie :** Si vous avez sélectionné le chemin `C:\Logos`, copiez le dossier `Logos` et tout son contenu sur la carte SD ou SSD où Batocera est installé dans `roms/marquesinas/`, comme indiqué au point `5. Structure des fichiers sur la carte SD ou SSD de Batocera`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />

### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos logos d'origine au format **128x32 pixels**.
* **Formatage :** Force la couleur au format **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès réseau (Samba) :**
> Si lors de l'exécution du script, celui-ci n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera.
> Pour accéder au chemin (ex : `\\192.168.1.120\userdata\themes\Animatics-DX-master\art\logos`), Windows vous demandera des identifiants. Utilisez ceux par défaut de Batocera :
> * **Utilisateur :** `root`
> * **Mot de passe :** `linux`

---

## 5. Structure des fichiers sur la carte SD ou SSD de Batocera

Pour que l'intégration fonctionne correctement, nous devons coller le dossier `marquesinas` dans le dossier `roms/` :
* **`roms/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquee du jeu traitée, ex : `mslug.bmp`)
* **`roms/marquesinas/Logos/sistema_name.bmp`** (Marquee du système traitée, ex : `mame.bmp`)

#### Exemple visuel de la structure des dossiers :
```
📂 roms/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│   │   └── 📂 neogeo/
│   │   │   ├── 📄 mslug.bmp
│   │   │   ├── 📄 kof98.bmp
│   │   │   └── ...
│   │   └── 📂 mame/
│   │       ├── 📄 pacman.bmp
│   │       ├── 📄 tetris.bmp
│   │       └── ...
│   └── 📂 Logos/
│       ├── 📄 atari2600.bmp
│       ├── 📄 mame.bmp
│       └── ...
```

---

## 6. Profitez de vos marquees tout en jouant sur votre borne Arcade !
