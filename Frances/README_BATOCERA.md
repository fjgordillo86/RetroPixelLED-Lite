# 🕹️ Intégration avec Batocera

Le **Mode Arcade** dans la version Lite permet à votre matrice LED de fonctionner comme une marquise dynamique. Le panneau détectera le système et le jeu que vous parcourez et l'affichera automatiquement — et si le jeu a une marquise animée de prête, il la lira en boucle pendant que vous jouez.

#### Optimisation des Ressources (Scraping)
Le principal avantage de ce système est qu'il **utilise les images que vous avez déjà scrappées dans Batocera** (marquises/wheel art, ainsi que les vidéos de preview si vous en avez). Le script PowerShell se charge de les chercher, de les redimensionner et de les convertir automatiquement.

## 1. Configuration Critique : IP Fixe pour l'ESP32

Pour que le mode **🕹️ Arcade** de Batocera fonctionne toujours correctement, il est fondamental que l'ESP32 conserve toujours la même adresse IP.

> [!TIP]
> **Attribuer une IP fixe à l'ESP32 :** > Les scripts de Batocera envoient les commandes (comme changer le GIF au lancement d'un jeu) à une adresse IP spécifique que vous configurez manuellement. Si le routeur redémarre et attribue une IP différente à l'ESP32, la communication sera coupée et le panneau cessera de se mettre à jour.
>
> **Comment faire ?**
> 1. Accédez à la configuration de votre routeur.
> 2. Cherchez la section **DHCP Statique** ou **Attribution d'IP par MAC**.
> 3. Liez l'adresse MAC de votre ESP32 à l'IP que vous avez écrite dans vos scripts (ex : `192.168.1.117`).
> 4. Étant donné que chaque routeur est différent, si vous avez des doutes, cherchez sur Google : *"Comment attribuer une IP fixe [modèle de votre routeur]"*.

> [!NOTE]
> Depuis la version ajoutant les **marquises animées**, le panneau met à jour votre firmware et détecte automatiquement la reconnexion du WiFi si la connexion est perdue pendant l'utilisation — mais l'IP fixe reste toujours nécessaire, car les scripts de Batocera ne savent pas "chercher" le panneau, ils savent seulement à quelle IP précise ils doivent s'adresser.

## 2. Installation Automatique sur Batocera

À partir de la version **v3.0.0**, il n'est plus nécessaire de modifier des lignes de code à la main, de se soucier des formats de fichiers Windows ou d'utiliser des consoles SSH avancées (comme PuTTY) pour configurer les permissions d'exécution. 

J'ai développé un **Script d'Installation Intelligent en PowerShell** qui réalise tout le déploiement automatiquement depuis votre PC.

---

### 📦 Que fait cet installateur pour vous ?

* **Configuration de l'IP :** Injecte automatiquement l'adresse IP de votre panneau LED dans tous les scripts de communication.
* **Correction de Format :** Force le format de fin de ligne **Unix (LF)**. Cela évite que les scripts échouent s'ils ont été ouverts par erreur avec le Bloc-notes de Windows.
* **Organisation des Fichiers :** Crée la structure de répertoires nécessaire dans Batocera et copie les fichiers à leur emplacement correspondant.
* **Auto-Permissions (Sans PuTTY) :** Génère un script système (`custom.sh`) qui fait que Batocera s'accorde lui-même les permissions d'exécution (`chmod +x`) sur les dossiers à chaque démarrage.
* **Marquises Animées :** Installe également le moteur de lecture de GIF (`pixel_stream.py`) et l'événement `game-start`, chargés de détecter et de lire la marquise animée du jeu que vous venez de lancer.

---

### 🛠️ Prérequis

1. Avoir votre **PC** et votre **Batocera** connectés au même réseau local (ou connecter le stockage physique de Batocera directement au PC).
2. Connaître **l'IP locale de votre panneau LED** Retro Pixel LED (ex. `192.168.1.117`).
3. Télécharger le dossier complet `Instalador Automático` depuis ce dépôt, vous pouvez le trouver [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Instalador%20Automatico).

> [!IMPORTANT]
> Si vous avez téléchargé le dépôt dans un fichier `.zip`, assurez-vous de **le décompresser complètement** avant d'exécuter l'installateur.

---

### 💻 Pas à Pas

1. Ouvrez le dossier `Instalador Automatico` sur votre PC. À l'intérieur, vous trouverez deux fichiers et deux dossiers :
   * `Ejecutar Script Instalador Arcade.bat`
   * `Script_Instalador_Arcade.ps1`
   * `Batocera`
   * `Recalbox`

2. Faites **clic** sur `Ejecutar Script Instalador Arcade.bat`.

3. Suivez les instructions dans la fenêtre de la console :
   * **Étape 1 :** Entrez l'IP de votre panneau LED et appuyez sur `Entrée`.
   * **Étape 2 :** Entrez le chemin de votre Batocera. Cela peut être un chemin réseau (ex : `\\192.168.1.120` ou `\\BATOCERA`) ou la lettre d'un lecteur physique si vous avez connecté le disque/SD au PC (ex : `E:`).

4. Le script nous demandera :
    * Le système que nous utilisons, nous sélectionnerons 1 Batocera.
    * Quel mode de fonctionnement souhaitez-vous activer ?
       * **Option 1 :** Menus et Jeux (Affiche les systèmes lors de la navigation + le jeu lancé)
       * **Option 2 :** Seulement Jeux (Marquise fixe/horloge dans les menus, change uniquement en jouant)

> [!NOTE]
> Les marquises **animées** fonctionnent de la même manière dans les deux modes — la différence entre l'Option 1 et l'Option 2 est uniquement si le panneau réagit également lors de la navigation dans les systèmes, cela n'affecte pas s'il y a un GIF ou non lors du lancement d'un jeu.

5. Le script traitera les fichiers en une seconde. À la fin, vous verrez le message `INSTALACIÓN COMPLETADA!`. Appuyez sur n'importe quelle touche pour quitter.

<img width="941" height="834" alt="image" src="https://github.com/user-attachments/assets/cec91be3-82b4-44e4-8873-6ce67383e2e1" />

6. **Redémarre complètement ton système Batocera.**
> [!CAUTION]
> Le redémarrage complet du système est **obligatoire** pour que le service de permissions apparaisse dans le menu.

7. **Active-le (uniquement la première fois) :** va dans `Menu Principal > Paramètres Système > Services` et active **`retropixelperms`**.
> [!NOTE]
> Ce service remplace l'ancien `custom.sh` et c'est lui qui accorde les permissions d'exécution aux scripts de la marquise à chaque démarrage. Une fois activé ici, il le reste pour toujours — même si tu relances l'installateur plus tard (la réinstallation ne fait que régénérer le fichier du service, elle ne le désactive pas). À partir de là, chaque fois que tu navigues dans le menu, lances ou quittes un jeu, le panneau réagira automatiquement.

### 3. 🛠️ Marquises.
Nous utiliserons le script qui se trouve dans le dossier `Arcade/Marquesinas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Il se compose de deux fichiers `Ejecutar Script Marquesinas Batocera.bat` et `Script Marquesinas Batocera.ps1`.

1.  **Exécutez le fichier** `Ejecutar Script Marquesinas Batocera.bat` (Lanceur pour éviter les blocages de Windows).
2.  **Configuration des chemins :**
    * **Origine :** Entrez le chemin de vos ROMs Batocera (ex : `\\192.168.1.119\share\roms`).
    * **Destination :** Entrez le chemin `C:\marquesinas`.
3.  **Sélection du Système :** Le script détectera automatiquement quels systèmes ont un fichier `gamelist.xml`. Vous pouvez choisir d'en traiter un seul par son numéro, plusieurs ou **Tous (0)**.
4.  **Copier :** Si vous avez sélectionné le chemin `C:\marquesinas`, copiez le dossier `marquesinas` et tout son contenu sur la carte SD ou le SSD où Batocera est installé `roms/`, comme indiqué au point `6. Structure des fichiers sur la carte SD ou le SSD de Batocera`.

<img width="1096" height="572" alt="image" src="https://github.com/user-attachments/assets/388368a7-a57b-4611-89fc-4bfc184c1fa7" />

### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marquises originales en **128x32 pixels**.
* **Format :** Force la couleur en **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès par Réseau (Samba) :**
> Si, lors de l'exécution du script, il n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script ait accès au dossier.
> Pour accéder au chemin `ex -> \\192.168.1.120\share\roms`, Windows vous demande des identifiants, utilisez ceux fournis par Batocera par défaut :
> * **Utilisateur :** `root`
> * **Mot de passe :** `linux`

> [!CAUTION]
> Chaque fois que vous ajoutez de nouveaux jeux ou faites un "Scrape" dans Batocera, **vous devez réexécuter le script PowerShell** sur votre PC pour mettre à jour les index et les images. Sans cette étape, l'ESP32 ne saura pas que les nouveaux fichiers existent.

### 4. 🎬 Marquises Animées (GIF)

En plus de l'image statique, le panneau peut lire un **GIF animé** lors du lancement d'un jeu — la marquise bouge pendant que vous jouez, au lieu de rester fixe.

#### Comment ça marche ?

- Pendant que vous **naviguez** dans les systèmes et les jeux, le panneau se comporte exactement de la même manière qu'avec les marquises statiques : il n'y a aucune différence là-dessus, le GIF n'entre pas encore en jeu.
- Au moment où vous **lancez** un vrai jeu, le panneau cherche s'il existe un `.gif` avec le même nom que la marquise statique de ce jeu, dans le même dossier.
- **S'il le trouve, il le lit en boucle** pendant toute la partie, et revient à la lecture normale des GIFs/horloge dès que vous quittez.
- **S'il ne le trouve pas, il ne se passe rien** — la marquise statique qui s'affichait déjà reste telle quelle, comme si le mode GIF n'existait pas pour ce jeu. Il n'est pas nécessaire de préparer un GIF pour chaque jeu ; vous pouvez les ajouter petit à petit.

#### Nommage des fichiers

Le GIF doit s'appeler **exactement comme la marquise `.bmp`** du même jeu, dans le même dossier :

```
roms/marquesinas/Arcade/neogeo/mslug.bmp   <- vous l'aviez déjà
roms/marquesinas/Arcade/neogeo/mslug.gif   <- vous l'ajoutez, même nom
```

Vous pouvez également préparer une **séquence de plusieurs GIFs** pour un même jeu, en ajoutant le suffixe `_01`, `_02`, `_03`... Le panneau les lit tous dans l'ordre, l'un après l'autre, et lorsqu'il atteint le dernier, il recommence par le premier, en boucle continue :

```
roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif
```

> [!TIP]
> Il n'est pas nécessaire que les trois existent — avec seulement `mslug.gif`, cela fonctionne déjà parfaitement en boucle. Les suffixes `_01`, `_02`... sont facultatifs, pour quand vous voulez alterner entre plusieurs clips différents pour le même jeu.

#### Où puis-je trouver les GIFs ?

Si vous avez déjà (ou avez téléchargé) une collection de GIFs arcade avec des noms "lisibles" au lieu de noms de romset (par exemple `ARCADE_NEOGEO_MetalSlugStory.gif` au lieu de `mslug.gif`), utilisez le script suivant pour renommer les GIFs. Téléchargez-le [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/GIFs) et suivez ces étapes :

1. Exécutez `Ejecutar Script_RetroPixelLED_GIF_Renamer.bat`.

2. **Option 1 — Renommer les GIFs :** indiquez le dossier dans lequel se trouvent vos GIFs. Le script consulte un catalogue public de noms MAME ([`MAME.dat`](https://github.com/libretro/libretro-database)) pour identifier le romset correspondant à chaque titre, ainsi qu'un dictionnaire interne pour les cas les plus courants. Vous pouvez choisir entre une correspondance exacte uniquement, ou exacte + approximative (résout plus de cas, avec un peu plus de risques). Tout ce qu'il ne parvient pas à identifier est déplacé dans un dossier `SinResolver\` afin que vous puissiez le vérifier manuellement — il ne renomme jamais "à l'aveugle".
   <img width="1090" height="830" alt="image" src="https://github.com/user-attachments/assets/58ba389f-367c-4114-b6e1-533018f50e77" />

3. **Option 2 — Copier les GIFs dans les dossiers système :** une fois renommés, cette option compare les GIFs aux romsets réels de chaque système dans votre dossier `ROMS/` et les copie automatiquement dans `Arcade/<système>/`, à côté des fichiers `.bmp` que vous y avez déjà.
   <img width="1106" height="1204" alt="image" src="https://github.com/user-attachments/assets/ee3e51a6-2dd7-4297-8a2a-a4486171f60d" />

Vous pouvez également les générer — le panneau nécessite seulement que le fichier final soit en **128×32 pixels**. À titre de référence, si votre collection Batocera contient déjà des vidéos d'aperçu scrapées (`<video>` dans le `gamelist.xml`), vous pouvez les convertir en GIF avec un outil tel que [dmd_gif_converter](https://github.com/red77290/dmd_gif_converter), qui, en plus du redimensionnement, inclut un mode de cadrage automatique conçu pour ne pas perdre l'action lors de la réduction d'une grande vidéo à une si petite taille. Il s'agit d'un projet tiers, indépendant de ce dépôt — toute autre méthode produisant un fichier `.gif` en 128×32 fonctionnera tout aussi bien.

 ### 5. 🛠️ Logos des Systèmes.
 Nous pouvons utiliser les logos déjà redimensionnés qui se trouvent dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
 1.  **Copier :** Copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Batocera est installé `roms/marquesinas/`, comme indiqué au point `6. Structure des fichiers sur la carte SD ou le SSD de Batocera`.
    
 Si vous préférez utiliser d'autres logos, comme par exemple ceux du thème que vous avez installé. Nous utiliserons le script qui se trouve dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Il se compose de deux fichiers `Ejecutar Script Logos.bat` et `Script Logos.ps1`.

1.  **Exécutez le fichier** `Ejecutar Script Logos.bat` (Lanceur pour éviter les blocages de Windows).
2.  **Configuration des chemins :**
    * **Origine :** Entrez le chemin où vous avez les logos (ex : `\\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos`).
    * **Destination :** Entrez le chemin `C:\Logos`.
4.  **Copier :** Si vous avez sélectionné le chemin `C:\Logos`, copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Batocera est installé `roms/marquesinas/`, comme indiqué au point `6. Structure des fichiers sur la carte SD ou le SSD de Batocera`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />


### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marquises originales en **128x32 pixels**.
* **Format :** Force la couleur en **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès par Réseau (Samba) :**
> Si, lors de l'exécution du script, il n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script ait accès au dossier.
> Pour accéder au chemin `ex -> \\192.168.1.120\userdata\themes\Animatics-DX-master\art\logos`, Windows vous demande des identifiants, utilisez ceux fournis par Batocera par défaut :
> * **Utilisateur :** `root`
> * **Mot de passe :** `linux`

## 6. Structure des fichiers sur la carte SD ou le SSD de Batocera

Pour que l'intégration fonctionne correctement, nous devons coller le dossier marquesinas dans le dossier `roms/`
* **`roms/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquise statique du jeu, ex : `mslug.bmp`)
* **`roms/marquesinas/Arcade/sistema/rom_name.gif`** (Facultatif : marquise animée du même jeu, ex : `mslug.gif`)
* **`roms/marquesinas/Logos/sistema_name.bmp`** (Marquise du système traitée, ex : `mame.bmp`)

#### Exemple visuel des dossiers :
```
📂 roms/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│   │   └── 📂 neogeo/
│   │   │   ├── 📄 mslug.bmp
│   │   │   ├── 📄 mslug.gif       <- facultatif, marquise animée
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

## 7. Profitez des marquises pendant que vous jouez sur votre Arcade !
