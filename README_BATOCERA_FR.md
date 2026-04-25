# 🕹️ Intégration avec Batocera (Mode Arcade Lite)

Le **Mode Arcade** de la version Lite permet à ta matrice LED de fonctionner comme une marquise dynamique. Le panneau détecte automatiquement le jeu lancé dans **Batocera** et affiche son logo au format **BMP 24 bits**.

---

## 🎯 Exploitation des Ressources (Scraping)

Le système **réutilise les images déjà scrapées dans Batocera** (marquees / wheel art).  
Le script PowerShell se charge de :

- les rechercher  
- les redimensionner  
- les convertir automatiquement  

---

# 1. Philosophie d’Affichage (Hiérarchie)

Pour éviter que le panneau reste vide, le système applique une logique de cascade :

1. **Marquee du Jeu** — ex : `mslug.bmp`  
2. **Logo du Système** — ex : `/arcade/neogeo/logo.bmp`  
3. **Image Par Défaut** — `/arcade/default.bmp`  

---

# 2. Préparation des Assets (Script PowerShell)

Le système utilise une **Recherche Binaire** pour retrouver instantanément les fichiers parmi des milliers de jeux.  
Les fichiers doivent donc être correctement indexés.

## 2.1 🛠️ Utilisation du Script Marquesinas

Le script se trouve dans :  
`Batocera/tools/`

Il contient :

- `Ejecutar Script Marquesinas.bat`  
- `Script.ps1`

### Étapes :

1. Connecte la **carte SD** au PC.  
2. Lance `Ejecutar Script Marquesinas.bat`.  
3. Configure les chemins :  
   - **Origine ROMs :** `\\192.168.1.119\share\roms`  
   - **Destination :** `C:\Export_Arcade` ou `F:\` (SD)  
4. Choisis les systèmes à traiter (un, plusieurs ou **Tous (0)**).  
5. Si tu as utilisé `C:\Export_Arcade`, copie ensuite le dossier `arcade` vers la SD.

### Le script effectue automatiquement :

- **Redimensionnement** → 128×32 px  
- **Conversion** → BMP 24 bits  
- **Indexation** → fichiers `.txt` triés (ex : `neogeo.txt`)

> [!CAUTION]  
> **Accès Samba**  
> Identifiants par défaut Batocera :  
> - Utilisateur : `root`  
> - Mot de passe : `linux`

---

# 3. Structure Obligatoire sur la SD

```
📂 arcade/
├── 📄 default.bmp
├── 📄 neogeo.txt
├── 📄 mame.txt
├── 📂 neogeo/
│   ├── 📄 logo.bmp
│   ├── 📄 mslug.bmp
│   └── 📄 kof98.bmp
└── 📂 mame/
    ├── 📄 logo.bmp
    └── 📄 pacman.bmp
```

### À créer manuellement :

1. **logo.bmp** dans chaque dossier système  
2. **default.bmp** dans `/arcade/`

---

# 4. Configuration dans Batocera

## A. Définir l’IP du Panneau

1. Ouvre `pixel_start.sh`, `pixel_stop.sh`, `pixel_off.sh` avec :  
   - Notepad++  
   - VS Code  
   - Sublime Text  
2. Remplace l’IP par celle de ton ESP32.  
3. Vérifie le format **Unix (LF)**.  
4. Sauvegarde.

> [!CAUTION]  
> Ne jamais utiliser le Bloc‑notes Windows (CRLF).

---

## B. Emplacement des Scripts

Copie vers :

`\\IP_DE_TON_BATOCERA\share\system\configs\emulationstation\scripts`

Organisation :

- `/game-start/pixel_start.sh`  
- `/game-end/pixel_stop.sh`

---

## C. Permissions d’Exécution (SSH)

1. Connecte-toi via **PuTTY**  
2. Identifiants :  
   - root  
   - linux  
3. Donne les permissions :

```bash
chmod +x /userdata/system/configs/emulationstation/scripts/game-start/pixel_start.sh 
chmod +x /userdata/system/configs/emulationstation/scripts/game-end/pixel_stop.sh
```

4. Vérifie :

```bash
ls -l /userdata/system/configs/emulationstation/scripts/game-start/pixel_start.sh
ls -l /userdata/system/configs/emulationstation/scripts/game-end/pixel_stop.sh
```

---

# 5. Fonctionnement en Temps Réel

- **Lancement d’un jeu :** Batocera envoie le système + nom de ROM → le panneau affiche la marquee.  
- **Sortie du jeu :** Batocera envoie `STOP` → retour aux GIFs ou à l’Horloge.  
- **Erreurs :** si un BMP manque → logo du système.

> [!CAUTION]  
> Après ajout de jeux ou nouveau scraping → **relancer le script PowerShell**.

---

# 6. IP Fixe pour l’ESP32 (Critique)

Le mode Arcade dépend d’une IP stable.

> [!TIP]  
> **Configurer une IP fixe dans le routeur :**  
> 1. Ouvre l’interface du routeur  
> 2. Cherche **DHCP statique**  
> 3. Associe la MAC de l’ESP32 à l’IP utilisée dans les scripts  
> 4. Si besoin :  
>    *“Comment assigner IP fixe [modèle du routeur]”*

---
 produire une version **EN.md** ou **ES.md**

Tu veux que je génère aussi la version **FR.json** correspondante ?
