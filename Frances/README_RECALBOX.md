# 🕹️ Intégration avec Recalbox

Le **Mode Arcade** dans la version Lite permet à votre matrice LED de fonctionner comme une marqueuse (marquee) dynamique. Le panneau détectera le système et le jeu sur lequel vous naviguez et l'affichera automatiquement.

#### Exploitation des Ressources (Scraping)
Le principal avantage de ce système est qu'**il utilise les images que vous avez déjà scrapées dans Recalbox** (marqueuses / wheel art). Le script PowerShell se charge de les chercher, de les redimensionner et de les convertir automatiquement.

## 1. Configuration Critique : IP Fixe pour l'ESP32

Pour que le mode **🕹️ Arcade** de Batocera/Recalbox fonctionne toujours correctement, il est fondamental que l'ESP32 conserve toujours la même adresse IP.

> [!TIP]
> **Attribuer une IP fixe à l'ESP32 :**
> Les scripts envoient les commandes (comme changer le GIF lors du lancement d'un jeu) à une adresse IP spécifique que vous configurez manuellement. Si le routeur redémarre et attribue une IP différente à l'ESP32, la communication sera coupée et le panneau cessera de se mettre à jour.
>
> **Comment faire ?**
> 1. Accédez à la configuration de votre routeur.
> 2. Cherchez la section **DHCP Statique** ou **Bail DHCP permanent par MAC**.
> 3. Associez l'adresse MAC de votre ESP32 à l'IP que vous avez inscrite dans vos scripts (ex : `192.168.1.117`).
> 4. Chaque routeur étant différent, si vous avez des doutes, recherchez sur Google : *"Comment attribuer une IP fixe [modèle de votre routeur]"*.

## 2. Installation Automatique sur Recalbox

À partir de la version **v3.0.0**, il n'est plus nécessaire d'éditer des lignes de code à la main, de se soucier des formats de fichier Windows ou d'utiliser des consoles SSH avancées (comme PuTTY) pour configurer les autorisations d'exécution.

J'ai développé un **Script d'Installation Intelligent en PowerShell** qui effectue tout le déploiement de manière automatique depuis votre PC.

---

### 📦 Que fait cet installateur pour vous ?

* **Configuration de l'IP :** Injecte automatiquement l'adresse IP de votre panneau LED dans tous les scripts de communication.
* **Correction de Format :** Force le format de fin de ligne **Unix (LF)**. Cela évite que les scripts échouent s'ils ont été ouverts par erreur avec le Bloc-notes de Windows.
* **Organisation des Fichiers :** Crée la structure de répertoires nécessaire sur Recalbox et copie les fichiers à leur emplacement correspondant.
* **Auto-Autorisations (Sans PuTTY) :** Génère un script système (`custom.sh`) qui fait en sorte que le système s'accorde lui-même les autorisations d'exécution (`chmod +x`) sur les dossiers à chaque démarrage.

---

### 🛠️ Prérequis

1. Avoir votre **PC** et votre **Recalbox** connectés au même réseau local (ou connecter le support de stockage physique de Recalbox directement au PC).
2. Connaître l'**IP locale de votre panneau LED** Retro Pixel LED (ex. `192.168.1.117`).
3. Télécharger le dossier complet `Instalador Automático` depuis ce dépôt, vous pouvez le trouver [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Instalador%20Automatico).

> [!IMPORTANT]
> Si vous avez téléchargé le dépôt sous forme de fichier `.zip`, veillez à le **décompresser entièrement** avant d'exécuter l'installateur.

---

### 💻 Pas à Pas

1. Ouvrez le dossier `Instalador Automático` sur votre PC. Vous y trouverez deux fichiers et deux dossiers :
   * `Ejecutar Script Instalador Arcade.bat`
   * `Script_Instalador_Arcade.ps1`
   * `Batocera`
   * `Recalbox`

2. Faites un **clic** sur `Ejecutar Script Instalador Arcade.bat`.

3. Suivez les instructions dans la fenêtre de la console :
   * **Étape 1 :** Entrez l'IP de votre panneau LED et appuyez sur `Entrée`.
   * **Étape 2 :** Entrez le chemin d'accès à votre Recalbox. Il peut s'agir d'un chemin réseau (ex : `\192.168.1.118`) ou de la lettre d'un lecteur physique si vous avez connecté le disque/SD au PC (ex : `E:`).

4. Le script vous demandera :
   * Le système que vous utilisez : sélectionnez **2 Recalbox**.
   * Quel mode de fonctionnement souhaitez-vous activer ?
     * Option 1 : Menus et Jeux (Affiche les systèmes lors de la navigation + le jeu lancé)
     * Option 2 : Jeux uniquement (Marqueuse fixe/horloge dans les menus, ne change qu'en jouant)
5. Le script traitera les fichiers en un instant. À la fin, vous verrez le message `INSTALACIÓN COMPLETADA!`. Appuyez sur n'importe quelle touche pour quitter.

<img width="1102" height="532" alt="image" src="https://github.com/user-attachments/assets/3e367c0a-d305-475e-95c9-ed9d3ae352e9" />

6. **Redémarrez complètement votre système Recalbox.**
> [!CAUTION]
> Le redémarrage complet du système est **obligatoire**. À partir de ce moment, chaque fois que vous naviguerez dans le menu, lancerez ou fermerez un jeu, le panneau réagira automatiquement.

### 3. 🛠️ Marqueuses (Marquees)
Nous utiliserons le script qui se trouve dans le dossier `Arcade/Marquesinas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Il se compose de deux fichiers `Ejecutar Script Marquesinas Recalbox.bat` et `Script Marquesinas Recalbox.ps1`.

1. **Exécutez le fichier** `Ejecutar Script Marquesinas Recalbox.bat` (Lanceur pour éviter les blocages de Windows).
2. **Configuration des chemins :**
   * **Source :** Entrez le chemin de vos ROMs Recalbox (ex : `\192.168.1.118\share
oms`).
   * **Destination :** Entrez le chemin `C:\marquesinas`.
3. **Sélection de l'image :** Sélectionnez le type d'image à utiliser pour les marqueuses.
4. **Sélection du système :** Le script détectera automatiquement les systèmes qui possèdent un fichier `gamelist.xml`. Vous pouvez choisir d'en traiter un seul par son numéro, plusieurs ou **Tous (0)**.
5. **Copier :** Si vous avez sélectionné le chemin `C:\marquesinas`, copiez le dossier `marquesinas` et tout son contenu sur la carte SD ou le SSD où Recalbox est installé dans `share\`, comme indiqué au point `5. Structure des fichiers sur la carte SD ou SSD de Recalbox`.

<img width="1098" height="630" alt="image" src="https://github.com/user-attachments/assets/f2a99ce2-0b83-40cc-84bc-d24962f2c83e" />

### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marqueuses originales au format **128x32 pixels**.
* **Format :** Force la couleur en **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès Réseau (Samba) :**
> Si lors de l'exécution du script, celui-ci n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Recalbox afin que le script ait accès au dossier.
> En accédant au chemin `ex-> \192.168.1.120\share
oms`, Windows vous demandera des identifiants, utilisez ceux fournis par défaut :
> * **Utilisateur :** `root`
> * **Mot de passe :** `recalboxroot`

> [!CAUTION]
> Chaque fois que vous ajoutez de nouveaux jeux ou que vous faites un "Scrape" dans Recalbox, **vous devez réexécuter le script PowerShell** sur votre PC pour mettre à jour les index et les images. Sans cette étape, l'ESP32 ne saura pas que les nouveaux fichiers existent.

### 4. 🛠️ Logos des Systèmes.
Vous pouvez utiliser les logos déjà redimensionnés qui se trouvent dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
1. **Copier :** Copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Recalbox est installé dans `share\marquesinas`, comme indiqué au point `5. Structure des fichiers sur la carte SD ou SSD de Recalbox`.

Si vous préférez utiliser d'autres logos, comme par exemple ceux du thème que vous avez installé : nous utiliserons le script situé dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Il se compose de deux fichiers `Ejecutar Script Logos.bat` et `Script Logos.ps1`.

1. **Exécutez le fichier** `Ejecutar Script Logos.bat` (Lanceur pour éviter les blocages de Windows).
2. **Configuration des chemins :**
   * **Source :** Entrez le chemin où vous avez les logos (ex : `\192.168.1.119\share	hemes\Animatics-DX-masterrt\logos`).
   * **Destination :** Entrez le chemin `C:\Logos`.
3. **Copier :** Si vous avez sélectionné le chemin `C:\Logos`, copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Recalbox est installé dans `share/marquesinas/`, comme indiqué au point `5. Structure des fichiers sur la carte SD ou SSD de Recalbox`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />

### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marqueuses originales au format **128x32 pixels**.
* **Format :** Force la couleur en **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès Réseau (Samba) :**
> Si lors de l'exécution du script, celui-ci n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Recalbox afin que le script ait accès au dossier.
> En accédant au chemin `ex-> \192.168.1.120\share	hemes\Animatics-DX-masterrt\logos`, Windows vous demandera des identifiants, utilisez ceux fournis par défaut :
> * **Utilisateur :** `root`
> * **Mot de passe :** `recalboxroot`

## 5. Structure des fichiers sur la carte SD ou SSD de Recalbox

Pour que l'intégration fonctionne correctement, nous devons coller le dossier `marquesinas` dans le dossier `share/`
* **`share/marquesinas/Arcade/sistema/rom_name.bmp`** (Marqueuse du jeu traitée, ex : `mslug.bmp`)
* **`share/marquesinas/Logos/sistema_name.bmp`** (Marqueuse du système traitée, ex : `mame.bmp`)

#### Exemple visuel des dossiers :
```
📂 share/
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

## 6. Profitez des marqueuses pendant que vous jouez sur votre Arcade !
