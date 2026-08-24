
Gemini
Nueva conversación
Buscar conversaciones
Imágenes
Biblioteca
Nuevo cuaderno
ESP32 LED Panel GIF Control
Guide d'installation Retro Pixel LED
Mapa de Toledo en Google Fotos
Sunny Park Life and Landscape
Neo-Noir Tokyo Street Style
Denegación de generación de imagen
El zorro robó mi cama
Ilustración Animada Infantil
El Refugio De YouTube
El Cuaderno De La Frustración Estudiantil
Petición de Canción Ignorada
Lego Minifigure Vaccination Clinic
Habitación Secreta con Televisión
Toy Story 5 Policy Concern
Cats Running in Park
Jardín Zen Futurista Flotante
Canción Neoperreo Tata Nenita Platanonelon
Statue Frozen In Time
Fotografía Editorial Lavanda Pastel
El Pulpo de Toy Story 3
Marcando el Viernes Próximo
Canción de Tata Nenita Platanonelon
Lego Universidad, Leche, Islas, Neón
Video Creation Requires Subscription Upgrade
Llamada Nocturna a Mamá
Procesión de la Virgen Dolorosa
Escultura de madera tallada y detallada
Retrato de niño en invernadero botánico
Florista en Taller Lleno de Flores

Actividad
España
De tu dirección IP
Actualizar ubicación
Conversación con Gemini
devuelvemelo traducido al frances

Voici la traduction de votre texte en français :

🕹️ Intégration avec Batocera
Le Mode Arcade de la version Lite permet à votre matrice LED de fonctionner comme une marquise dynamique. Le panneau détectera le système et le jeu que vous parcourez et vous l'affichera automatiquement — et si le jeu a une marquise animée préparée, il la jouera en boucle pendant que vous jouez.  
MD
+ 1

Utilisation des ressources (Scraping)
Le principal avantage de ce système est qu'il utilise les images que vous avez déjà scrappées dans Batocera (marquises/wheel art, et aussi les vidéos d'aperçu si vous les avez). Le script PowerShell se charge de les chercher, de les redimensionner et de les convertir automatiquement.  
MD
+ 1

1. Configuration critique : IP fixe pour l'ESP32
Pour que le mode 🕹️ Arcade de Batocera fonctionne toujours correctement, il est essentiel que l'ESP32 conserve toujours la même adresse IP.  
MD

[!TIP]
Attribuer une IP fixe à l'ESP32 : > Les scripts de Batocera envoient les commandes (comme changer le GIF lors du lancement d'un jeu) à une adresse IP spécifique que vous configurez manuellement. Si le routeur redémarre et attribue une adresse IP différente à l'ESP32, la communication sera coupée et le panneau cessera de se mettre à jour.  
MD
+ 1

Comment faire ?

  
MD

Accédez aux paramètres de votre routeur.  
MD

Cherchez la section DHCP Statique ou Attribution d'IP par MAC.  
MD

Liez l'adresse MAC de votre ESP32 à l'IP que vous avez écrite dans vos scripts (ex : 192.168.1.117).  
MD

Étant donné que chaque routeur est différent, si vous avez des doutes, cherchez sur Google : "Comment attribuer une IP fixe [modèle de votre routeur]".  
MD

[!NOTE]
Depuis la version qui ajoute les marquises animées, le panneau met à jour votre firmware et détecte automatiquement la reconnexion WiFi si la connexion est perdue pendant l'utilisation — mais l'IP fixe reste toujours nécessaire, car les scripts de Batocera ne savent pas "chercher" le panneau, ils savent seulement à quelle IP spécifique ils doivent s'adresser.  
MD

2. Installation automatique dans Batocera
À partir de la version v3.0.0, il n'est plus nécessaire de modifier manuellement des lignes de code, de s'inquiéter des formats de fichiers Windows ou d'utiliser des consoles SSH avancées (comme PuTTY) pour configurer les autorisations d'exécution.  
MD

J'ai développé un Script d'Installation Intelligent en PowerShell qui effectue tout le déploiement automatiquement depuis votre PC.  
MD

📦 Que fait cet installateur pour vous ?
Configuration IP : Injecte automatiquement l'adresse IP de votre panneau LED dans tous les scripts de communication.  
MD

Correction de format : Force le format de fin de ligne Unix (LF). Cela évite que les scripts échouent s'ils ont été ouverts par erreur avec le Bloc-notes Windows.  
MD
+ 1

Organisation des fichiers : Crée la structure de répertoires nécessaire dans Batocera et copie les fichiers à leur emplacement correspondant.  
MD

Auto-Autorisations (Sans PuTTY) : Génère un script système (custom.sh) qui fait que Batocera s'accorde à lui-même les droits d'exécution (chmod +x) sur les dossiers à chaque démarrage.  
MD

Marquises Animées : Installe également le moteur de lecture GIF (pixel_stream.py) et l'événement game-start, chargés de détecter et de lire la marquise animée du jeu que vous venez de lancer.  
MD

🛠️ Prérequis
Avoir votre PC et votre Batocera connectés au même réseau local (ou connecter le stockage physique de Batocera directement au PC).  
MD

Connaître l'IP locale de votre panneau LED Retro Pixel LED (ex. 192.168.1.117).  
MD

Télécharger le dossier complet Instalador Automático depuis ce dépôt, vous pouvez le trouver ici.  
MD

[!IMPORTANT]
Si vous avez téléchargé le dépôt sous forme de fichier .zip, assurez-vous de le décompresser complètement avant d'exécuter l'installateur.  
MD

💻 Étape par étape
Ouvrez le dossier Instalador Automatico sur votre PC. À l'intérieur, vous trouverez deux fichiers et deux dossiers :  
MD
+ 1

Ejecutar Script Instalador Arcade.bat

  
MD

Script_Instalador_Arcade.ps1

  
MD

Batocera

  
MD

Recalbox

  
MD

Cliquez sur Ejecutar Script Instalador Arcade.bat.  
MD

Suivez les instructions dans la fenêtre de la console :  
MD

Étape 1 : Entrez l'adresse IP de votre panneau LED et appuyez sur Entrée.  
MD

Étape 2 : Entrez le chemin de votre Batocera. Il peut s'agir d'un chemin réseau (ex : \\192.168.1.120 ou \\BATOCERA) ou de la lettre d'un lecteur physique si vous avez connecté le disque/SD au PC (ex : E:).  
MD
+ 1

Le script nous demandera :  
MD

Le système que nous utilisons, nous sélectionnerons 1 Batocera.  
MD

Quel mode de fonctionnement souhaitez-vous activer ?  
MD

Option 1 : Menus et Jeux (Affiche les systèmes lors de la navigation + jeu lancé)  
MD

Option 2 : Uniquement Jeux (Marquise fixe/horloge dans les menus, ne change que lors du jeu)  
MD

[!NOTE]
Les marquises animées fonctionnent de la même manière dans les deux modes — la différence entre l'Option 1 et l'Option 2 réside uniquement dans le fait que le panneau réagisse également lors de la navigation dans les systèmes, cela n'affecte pas s'il y a un GIF ou non lors du lancement d'un jeu.  
MD

Le script traitera les fichiers en une seconde. À la fin, vous verrez le message INSTALACIÓN COMPLETADA!. Appuyez sur n'importe quelle touche pour quitter.  
MD
+ 1

  
MD

Redémarrez complètement votre système Batocera.

  
MD

[!CAUTION]
Un redémarrage complet du système est obligatoire. Pendant ce démarrage, le script custom.sh configurera les autorisations internes. À partir de ce moment, chaque fois que vous naviguerez dans le menu, lancerez ou fermerez un jeu, le panneau réagira automatiquement.  
MD
+ 2

3. 🛠️ Marquises.
Nous utiliserons le script situé dans le dossier Arcade/Marquesinas/ du projet ici. Il se compose de deux fichiers Ejecutar Script Marquesinas Batocera.bat et Script Marquesinas Batocera.ps1.  
MD
+ 1

Exécutez le fichier Ejecutar Script Marquesinas Batocera.bat (Lanceur pour éviter les blocages de Windows).  
MD

Configuration des chemins :

  
MD

Source : Entrez le chemin de vos ROMs Batocera (ex : \\192.168.1.119\share\roms).  
MD

Destination : Entrez le chemin C:\marquesinas.  
MD

Sélection du système : Le script détectera automatiquement quels systèmes ont un fichier gamelist.xml. Vous pouvez choisir d'en traiter un seul par son numéro, plusieurs ou Tous (0).  
MD
+ 1

Copier : Si vous avez sélectionné le chemin C:\marquesinas, copiez le dossier marquesinas et tout son contenu sur la carte SD ou le SSD où Batocera est installé roms/, comme indiqué au point 6. Structure des fichiers sur la carte SD ou le SSD de Batocera.  
MD

  
MD

Que fait le script automatiquement ?
Redimensionnement : Convertit vos marquises d'origine en 128x32 pixels.  
MD

Format : Force la couleur en BMP 24 bits (format compatible avec le pilote DMA de l'ESP32).  
MD

[!CAUTION]
Accès réseau (Samba) :

  
MD


Si, lors de l'exécution du script, vous n'avez pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script puisse accéder au dossier.
Pour accéder au chemin ex-> \\192.168.1.120\share\roms, Windows vous demandera des identifiants, utilisez ceux fournis par défaut avec Batocera :  
MD
+ 1

Utilisateur : root

  
MD

Mot de passe : linux

  
MD

[!CAUTION]
Chaque fois que vous ajoutez de nouveaux jeux ou que vous faites un "Scrape" dans Batocera, vous devez réexécuter le script PowerShell sur votre PC pour mettre à jour les index et les images. Sans cette étape, l'ESP32 ne saura pas que les nouveaux fichiers existent.  
MD
+ 1

4. 🎬 Marquises animées (GIF)
En plus de l'image statique, le panneau peut lire un GIF animé au lancement d'un jeu — la marquise s'anime pendant que vous jouez, au lieu de rester fixe.  
MD

Comment ça marche ?
Pendant que vous naviguez dans les systèmes et les jeux, le panneau se comporte exactement de la même manière qu'avec les marquises statiques : il n'y a aucune différence, le GIF n'entre pas encore en jeu.  
MD

Au moment où vous lancez réellement un jeu, le panneau cherche s'il existe un .gif avec le même nom que la marquise statique de ce jeu, dans le même dossier.  
MD

S'il le trouve, il le joue en boucle tout au long de la partie, et retourne à la lecture normale des GIFs/horloge dès que vous quittez.  
MD

S'il ne le trouve pas, il ne se passe rien — la marquise statique qui était déjà affichée reste telle quelle, comme si le mode GIF n'existait pas pour ce jeu. Il n'est pas nécessaire de préparer un GIF pour chaque jeu ; vous pouvez les ajouter petit à petit.  
MD
+ 1

Nommage des fichiers
Le GIF doit porter le même nom que la marquise .bmp du même jeu, dans le même dossier :  
MD

Plaintext
roms/marquesinas/Arcade/neogeo/mslug.bmp   <- vous l'aviez déjà
roms/marquesinas/Arcade/neogeo/mslug.gif   <- vous l'ajoutez, même nom
Vous pouvez également préparer une séquence de plusieurs GIFs pour un même jeu, en ajoutant le suffixe _01, _02, _03... Le panneau les joue tous dans l'ordre, l'un après l'autre, et lorsqu'il atteint le dernier, il recommence au premier, en boucle continue :  
MD

Plaintext
roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif
[!TIP]
Il n'est pas nécessaire que les trois existent — avec seulement mslug.gif, cela fonctionne déjà parfaitement en boucle. Les suffixes _01, _02... sont facultatifs, pour les cas où vous souhaitez alterner entre plusieurs clips différents pour le même jeu.  
MD
+ 1

Où puis-je trouver les GIFs ?
C'est à vous de décider comment les générer — le panneau a seulement besoin que le fichier final soit de 128×32 pixels. À titre de référence, si votre collection Batocera contient déjà des vidéos d'aperçu scrappées (<video> dans le gamelist.xml), vous pouvez les convertir en GIF avec un outil comme dmd_gif_converter, qui, en plus de redimensionner, inclut un mode de recadrage automatique conçu pour ne pas perdre l'action lors de la réduction d'une grande vidéo à une taille si petite. C'est un projet tiers, indépendant de ce dépôt — toute autre méthode qui vous donne un .gif de 128×32 fonctionnera tout aussi bien.  
MD
+ 2

5. 🛠️ Logos des systèmes.
Nous pouvons utiliser les logos déjà redimensionnés qui se trouvent dans le dossier Arcade/Logos Sistemas/ du projet ici.  
MD

Copier : Copiez le dossier Logos et tout son contenu sur la carte SD ou le SSD où Batocera est installé roms/marquesinas/, comme indiqué au point 6. Structure des fichiers sur la carte SD ou le SSD de Batocera.  
MD

Si vous préférez utiliser d'autres logos, comme ceux du thème que vous avez installé. Nous utiliserons le script situé dans le dossier Arcade/Logos Sistemas/ du projet ici. Il se compose de deux fichiers Ejecutar Script Logos.bat et Script Logos.ps1.  
MD
+ 2

Exécutez le fichier Ejecutar Script Logos.bat (Lanceur pour éviter les blocages de Windows).  
MD

Configuration des chemins :

  
MD

Source : Entrez le chemin où vous avez les logos (ex : \\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos).  
MD

Destination : Entrez le chemin C:\Logos.  
MD

Copier : Si vous avez sélectionné le chemin C:\Logos, copiez le dossier Logos et tout son contenu sur la carte SD ou le SSD où Batocera est installé roms/marquesinas/, comme indiqué au point 6. Structure des fichiers sur la carte SD ou le SSD de Batocera.  
MD

  
MD

Que fait le script automatiquement ?
Redimensionnement : Convertit vos marquises d'origine en 128x32 pixels.  
MD

Format : Force la couleur en BMP 24 bits (format compatible avec le pilote DMA de l'ESP32).  
MD

[!CAUTION]
Accès réseau (Samba) :

  
MD


Si, lors de l'exécution du script, vous n'avez pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script puisse accéder au dossier.
Pour accéder au chemin ex-> \\192.168.1.120\userdata\themes\Animatics-DX-master\art\logos, Windows vous demandera des identifiants, utilisez ceux fournis par défaut avec Batocera :  
MD
+ 1

Utilisateur : root

  
MD

Mot de passe : linux

  
MD

6. Structure des fichiers sur la carte SD ou le SSD de Batocera
Pour que l'intégration fonctionne correctement, nous devons coller le dossier marquesinas dans le dossier roms/.  
MD

roms/marquesinas/Arcade/sistema/rom_name.bmp (Marquise statique du jeu, ex : mslug.bmp)  
MD

roms/marquesinas/Arcade/sistema/rom_name.gif (Facultatif : marquise animée du même jeu, ex : mslug.gif)  
MD

roms/marquesinas/Logos/sistema_name.bmp (Marquise du système traitée, ex : mame.bmp)  
MD

Exemple visuel des dossiers :
Plaintext
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
7. Profitez des marquises pendant que vous jouez sur votre borne d'Arcade !
  
MD
+ 4
devuelvemelo en .md para descargarlo

Vous pouvez également préparer une séquence de plusieurs GIFs pour un même jeu, en ajoutant le suffixe _01, _02, _03... Le panneau les joue tous dans l'ordre, l'un après l'autre, et lorsqu'il atteint le dernier, il recommence au premier, en boucle continue :

Plaintext
roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif
[!TIP]
Il n'est pas nécessaire que les trois existent — avec seulement mslug.gif, cela fonctionne déjà parfaitement en boucle. Les suffixes _01, _02... sont facultatifs, pour les cas où vous souhaitez alterner entre plusieurs clips différents pour le même jeu.

Où puis-je trouver les GIFs ?
C'est à vous de décider comment les générer — le panneau a seulement besoin que le fichier final soit de 128×32 pixels. À titre de référence, si votre collection Batocera contient déjà des vidéos d'aperçu scrappées (<video> dans le gamelist.xml), vous pouvez les convertir en GIF avec un outil comme dmd_gif_converter, qui, en plus de redimensionner, inclut un mode de recadrage automatique conçu pour ne pas perdre l'action lors de la réduction d'une grande vidéo à une taille si petite. C'est un projet tiers, indépendant de ce dépôt — toute autre méthode qui vous donne un .gif de 128×32 fonctionnera tout aussi bien.

5. 🛠️ Logos des systèmes.
Nous pouvons utiliser les logos déjà redimensionnés qui se trouvent dans le dossier Arcade/Logos Sistemas/ du projet ici.

Copier : Copiez le dossier Logos et tout son contenu sur la carte SD ou le SSD où Batocera est installé roms/marquesinas/, comme indiqué au point 6. Structure des fichiers sur la carte SD ou le SSD de Batocera.

Si vous préférez utiliser d'autres logos, comme ceux du thème que vous avez installé. Nous utiliserons le script situé dans le dossier Arcade/Logos Sistemas/ du projet ici. Il se compose de deux fichiers Ejecutar Script Logos.bat et Script Logos.ps1.

Exécutez le fichier Ejecutar Script Logos.bat (Lanceur pour éviter les blocages de Windows).

Configuration des chemins :

Source : Entrez le chemin où vous avez les logos (ex : \\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos).

Destination : Entrez le chemin C:\Logos.

Copier : Si vous avez sélectionné le chemin C:\Logos, copiez le dossier Logos et tout son contenu sur la carte SD ou le SSD où Batocera est installé roms/marquesinas/, comme indiqué au point 6. Structure des fichiers sur la carte SD ou le SSD de Batocera.

Que fait le script automatiquement ?
Redimensionnement : Convertit vos marquises d'origine en 128x32 pixels.

Format : Force la couleur en BMP 24 bits (format compatible avec le pilote DMA de l'ESP32).

[!CAUTION]
Accès réseau (Samba) :
Si, lors de l'exécution du script, vous n'avez pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script puisse accéder au dossier.
Pour accéder au chemin ex-> \\192.168.1.120\userdata\themes\Animatics-DX-master\art\logos, Windows vous demandera des identifiants, utilisez ceux fournis par défaut avec Batocera :

Utilisateur : root

Mot de passe : linux

6. Structure des fichiers sur la carte SD ou le SSD de Batocera
Pour que l'intégration fonctionne correctement, nous devons coller le dossier marquesinas dans le dossier roms/.

roms/marquesinas/Arcade/sistema/rom_name.bmp (Marquise statique du jeu, ex : mslug.bmp)

roms/marquesinas/Arcade/sistema/rom_name.gif (Facultatif : marquise animée du même jeu, ex : mslug.gif)

roms/marquesinas/Logos/sistema_name.bmp (Marquise du système traitée, ex : mame.bmp)

Exemple visuel des dossiers :
Plaintext
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
7. Profitez des marquises pendant que vous jouez sur votre borne d'Arcade !
"""

with open("README_BATOCERA_FR.md", "w", encoding="utf-8") as f:
f.write(content)


Your Markdown file is ready
[file-tag: code-generated-file-a83f59eb-c6c9-4535-a727-d36409d1030c]

Aquí tienes la traducción lista en formato Markdown para que puedas descargarla y añadirla a tu proyecto[cite: 1].
dame unos iconos acorde a las opciones "Mostrar IP" y "IP" ├── 📶 WiFi: [ON / OFF]

│   ├── 🔄 Activar: [SI / NO]

│   ├── 🔄 Mostar IP: [SI / NO]

│   ├── 🔄 IP: [192.169.1.117]

│   └── 🔙 Volver

Aquí tienes algunas propuestas de iconos (emojis) bien alineadas para las opciones de Mostrar IP y Dirección IP en tu menú OSD, manteniendo la coherencia visual con el resto de líneas:

🗺️ Opción 1 (Geolocalización / Red):

Plaintext
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── 👁️ Mostrar IP: [SI / NO]
│   ├── 📍 IP: [192.168.1.117]
│   └── 🔙 Volver
🖥️ Opción 2 (Estilo Sistema / Redes):

Plaintext
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── 👁️ Mostrar IP: [SI / NO]
│   ├── 🏷️ IP: [192.168.1.117]
│   └── 🔙 Volver
🔍 Opción 3 (Visualización y Dirección):

Plaintext
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── 🔎 Mostrar IP: [SI / NO]
│   ├── 📌 IP: [192.168.1.117]
│   └── 🔙 Volver
Sugerencia: El uso de 👁️ (ojo) o 🔎 (lupa) encaja muy bien con la acción de "mostrar" o consultar un dato en pantalla, mientras que 📍, 🏷️ o 📌 sirven de indicador claro de que se trata de una etiqueta o dirección fija.

i y un icono para idiomas?

Para la opción de idiomas (muy útil ahora que has estado traduciendo la documentación y los assets a varios idiomas), tienes varias opciones visuales ideales para menús OSD:

🌐 Globo terráqueo (El más universal): Representa los idiomas del mundo y la internacionalización.

Plaintext
├── 🌐 Idioma / Language: [ES / EN / FR]
🔤 Letras cruzadas (Estilo tipografía/traducción): Ideal si quieres enfocarlo a la selección de textos o localización.

Plaintext
├── 🔤 Idioma: [Español]
💬 Bocadillo de diálogo: Perfecto si hace referencia al idioma hablado o la interfaz de comunicación.

Plaintext
├── 💬 Idioma: [Français]
🗣️ Silueta hablando: Indica selección de idioma de voz o interfaz activa.

Plaintext
├── 🗣️ Idioma: [English]
Mi recomendación: El icono de 🌐 o 🔤 son los más limpios y legibles en pantallas pequeñas y menús de configuración tipo OSD.

traduce esta parte del readme al frances: ## 🆕 Novedades de la Versión v3.1.1 Lite



#### 🚀 Nuevas Características (Features)



* **🎬 Marquesinas Animadas en Modo Arcade (Batocera y Recalbox):** La gran novedad de esta versión. Además de la imagen estática de siempre, el panel ahora puede reproducir un **GIF animado** (o una secuencia completa de varios clips en bucle) al lanzar un juego — con reconexión automática si se corta la red a mitad de partida. Basta con colocar el `.gif` junto a la marquesina `.bmp` de siempre: si existe, se reproduce; si no, todo sigue funcionando exactamente igual que hasta ahora.

* **📶 IP del Panel en el Menú OSD:** Nuevo apartado en WiFi → que muestra la IP asignada al panel, junto con la opción **"Mostrar IP al iniciar"** para verla también en pantalla nada más arrancar — pensado para configurar la PWA sin tener que buscar la IP por otro lado.

* **🔄 Activar configuración desde la APP en el menú OSD:** Nuevo apartado en Wifi → **"Control APP"** que permite activar la configuración y el control del panel desde la APP.

* **🔄 Descarga de Idiomas en el menú OSD:** Nuevo apartado en Actualización → **"Descargar Idiomas"** que permite descargar los archivos de idioma directamente de GitHub.

* **🔄 Reconexión Automática de WiFi:** El panel detecta si pierde la conexión durante el funcionamiento normal y reintenta reconectar solo, sin necesidad de reiniciarlo a mano.





#### 🛡️ Corrección de Errores (Fixes)

* **Conexión de la PWA:** Corregidos varios fallos que impedían que la app conectara con el panel de forma fiable — el servidor dejaba de atender peticiones en determinados modos, y no se recuperaba solo si la red fallaba durante horas de uso continuado.





#### 🏗️ Cambios Internos

* **Soporte Multi-Idioma Remoto:** Descarga de `.json` de idioma desde GitHub directamente al panel, sin necesidad de tarjeta SD extraíble.



---



## 🕹️ Integración Especial: Modo Arcade (Batocera, Recalbox & RePlayOS)



Esta versión Lite introduce un soporte avanzado para ecosistemas de retrogaming, permitiendo dos vías de sincronización: mediante scripts locales (**Batocera / Recalbox**) o mediante monitorización nativa por red local (**RePlayOS**). 



A través de una jerarquía de archivos inteligente y optimizada para el hardware del ESP32, el panel gestiona el cambio de estado y muestra:



1. **Marquesina del Juego:** Imagen `.bmp` de 24 bits cargada instantáneamente, o un **GIF animado** (Batocera y Recalbox) si existe uno para ese juego — incluyendo secuencias de varios gifs reproducidos uno tras otro en bucle.

2. **Logo del Sistema:** Imagen `.bmp` de 24 bits cargada instantáneamente mientras navegamos por los sistemas.

Voici la traduction de cette partie du README en français :

🆕 Nouveautés de la Version v3.1.1 Lite
🚀 Nouvelles Fonctionnalités (Features)
🎬 Marquises Animées en Mode Arcade (Batocera et Recalbox) : La grande nouveauté de cette version. En plus de l'image statique habituelle, le panneau peut désormais lire un GIF animé (ou une séquence complète de plusieurs clips en boucle) lors du lancement d'un jeu — avec reconnexion automatique si le réseau est coupé en milieu de partie. Il suffit de placer le fichier .gif à côté de la marquise .bmp habituelle : s'il existe, il est lu ; sinon, tout continue de fonctionner exactement comme avant.

📶 IP du Panneau dans le Menu OSD : Nouvelle section dans WiFi → qui affiche l'adresse IP attribuée au panneau, ainsi que l'option "Afficher l'IP au démarrage" pour la voir également à l'écran dès l'allumage — conçue pour configurer la PWA sans avoir à chercher l'IP ailleurs.

🔄 Activer la configuration depuis l'APP dans le menu OSD : Nouvelle section dans Wifi → "Contrôle APP" qui permet d'activer la configuration et le contrôle du panneau depuis l'application.

🔄 Téléchargement des langues dans le menu OSD : Nouvelle section dans Mise à jour → "Télécharger Langues" qui permet de télécharger les fichiers de langue directement depuis GitHub.

🔄 Reconexion Automatique du WiFi : Le panneau détecte s'il perd la connexion pendant le fonctionnement normal et tente de se reconnecter seul, sans qu'il soit nécessaire de le redémarrer manuellement.

🛡️ Corrections de Bugs (Fixes)
Connexion de la PWA : Correction de plusieurs bugs qui empêchaient l'application de se connecter au panneau de manière fiable — le serveur cessait de répondre aux requêtes dans certains modes et ne récupérait pas la connexion de lui-même si le réseau faiblissait après des heures d'utilisation continue.

🏗️ Changements Internes
Support Multi-Langue à Distance : Téléchargement des fichiers .json de langue depuis GitHub directement sur le panneau, sans nécessiter de carte SD amovible.

🕹️ Intégration Spéciale : Mode Arcade (Batocera, Recalbox & RePlayOS)
Cette version Lite introduit un support avancé pour les écosystèmes de retrogaming, permettant deux méthodes de synchronisation : via des scripts locaux (Batocera / Recalbox) ou par surveillance native sur réseau local (RePlayOS).

Grâce à une hiérarchie de fichiers intelligente et optimisée pour le matériel de l'ESP32, le panneau gère le changement d'état et affiche :

Marquise du Jeu : Image .bmp 24 bits chargée instantanément, ou un GIF animé (Batocera et Recalbox) s'il en existe un pour ce jeu — incluant des séquences de plusieurs GIFs lus les uns après les autres en boucle.

Logo du Système : Image .bmp 24 bits chargée instantanément lors de la navigation à travers les systèmes.

en markdown porfa

Markdown
## 🆕 Nouveautés de la Version v3.1.1 Lite

#### 🚀 Nouvelles Fonctionnalités (Features)

* **🎬 Marquises Animées en Mode Arcade (Batocera et Recalbox) :** La grande nouveauté de cette version. En plus de l'image statique habituelle, le panneau peut désormais lire un **GIF animé** (ou une séquence complète de plusieurs clips en boucle) lors du lancement d'un jeu — avec reconnexion automatique si le réseau est coupé en milieu de partie. Il suffit de placer le fichier `.gif` à côté de la marquise `.bmp` habituelle : s'il existe, il est lu ; sinon, tout continue de fonctionner exactement comme avant.
* **📶 IP du Panneau dans le Menu OSD :** Nouvelle section dans WiFi → qui affiche l'adresse IP attribuée au panneau, ainsi que l'option **"Afficher l'IP au démarrage"** pour la voir également à l'écran dès l'allumage — conçue pour configurer la PWA sans avoir à chercher l'IP ailleurs.
* **🔄 Activer la configuration depuis l'APP dans le menu OSD :** Nouvelle section dans Wifi → **"Contrôle APP"** qui permet d'activer la configuration et le contrôle du panneau depuis l'application.
* **🔄 Téléchargement des langues dans le menu OSD :** Nouvelle section dans Mise à jour → **"Télécharger Langues"** qui permet de télécharger les fichiers de langue directement depuis GitHub.
* **🔄 Reconexion Automatique du WiFi :** Le panneau détecte s'il perd la connexion pendant le fonctionnement normal et tente de se reconnecter seul, sans qu'il soit nécessaire de le redémarrer manuellement.


#### 🛡️ Corrections de Bugs (Fixes)
* **Connexion de la PWA :** Correction de plusieurs bugs qui empêchaient l'application de se connecter au panneau de manière fiable — le serveur cessait de répondre aux requêtes dans certains modes et ne récupérait pas la connexion de lui-même si le réseau faiblissait après des heures d'utilisation continue.


#### 🏗️ Changements Internes
* **Support Multi-Langue à Distance :** Téléchargement des fichiers `.json` de langue depuis GitHub directement sur le panneau, sans nécessiter de carte SD amovible.

---

## 🕹️ Intégration Spéciale : Mode Arcade (Batocera, Recalbox & RePlayOS)

Cette version Lite introduit un support avancé pour les écosystèmes de retrogaming, permettant deux méthodes de synchronisation : via des scripts locaux (**Batocera / Recalbox**) ou par surveillance native sur réseau local (**RePlayOS**). 

Grâce à une hiérarchie de fichiers intelligente et optimisée pour le matériel de l'ESP32, le panneau gère le changement d'état et affiche :

1. **Marquise du Jeu :** Image `.bmp` 24 bits chargée instantanément, ou un **GIF animé** (Batocera et Recalbox) s'il en existe un pour ce jeu — incluant des séquences de plusieurs GIFs lus les uns après les autres en boucle.
2. **Logo du Système :** Image `.bmp` 24 bits chargée instantanément lors de la navigation à travers les systèmes.
ahora esto │ ├── 🔎 Mostar IP: [SI / NO]

│ ├── 🏷️ IP: [192.169.1.117]

│ ├── 📱 Control APP: [SI / NO]

Aquí tienes las opciones traducidas al francés con los iconos que elegiste para que encajen perfectamente en tu menú OSD:

Plaintext
│   ├── 🔎 Afficher IP : [OUI / NON]
│   ├── 🏷️ IP : [192.169.1.117]
│   ├── 📱 Contrôle APP : [OUI / NON]
│ ├── 🔤 Descargar Idiomas

Aquí tienes la línea traducida al francés con su icono correspondiente para el menú OSD:

Plaintext
│   ├── 🔤 Télécharger langues
casac de vbhh

Parece que ha habido una pequeña confusión o errata al teclear ("casac de vbhh"). ¿Te referías a alguna parte de la documentación del proyecto, necesitas ayuda con algún script o querías decir otra cosa?

Dime en qué te puedo ayudar y lo vemos enseguida.

devuelvemelo traducido al frances en formato .md

roms/marquesinas/Arcade/neogeo/mslug.bmp   <- vous l'aviez déjà
roms/marquesinas/Arcade/neogeo/mslug.gif   <- vous l'ajoutez, même nom


Vous pouvez également préparer une **séquence de plusieurs GIFs** pour un même jeu, en ajoutant le suffixe `_01`, `_02`, `_03`... Le panneau les lit tous dans l'ordre, l'un après l'autre, et lorsqu'il atteint le dernier, il recommence par le premier, en boucle continue :

roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif


> [!TIP]
> Il n'est pas nécessaire que les trois existent — avec seulement `mslug.gif`, cela fonctionne déjà parfaitement en boucle. Les suffixes `_01`, `_02`... sont facultatifs, pour quand vous voulez alterner entre plusieurs clips différents pour le même jeu.

#### D'où est-ce que je sors les GIFs ?

Vous décidez comment les générer — le panneau a seulement besoin que le fichier final soit de **128×32 pixels**. À titre de référence, si votre collection Batocera contient déjà des vidéos de preview scrappées (`<video>` dans le `gamelist.xml`), vous pouvez les convertir en GIF avec un outil comme [dmd_gif_converter](https://github.com/red77290/dmd_gif_converter), qui en plus de redimensionner inclut un mode de recadrage automatique conçu pour ne pas perdre l'action en réduisant une grande vidéo à une si petite taille. C'est un projet tiers, indépendant de ce dépôt — toute autre méthode qui vous laisse un `.gif` de 128×32 fonctionnera tout aussi bien.

 ### 5. 🛠️ Logos des Systèmes.
 Nous pouvons utiliser les logos déjà redimensionnés qui se trouvent dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
 1.  **Copier :** Copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Batocera est installé `roms/marquesinas/`, comme indiqué au point `6. Structure des fichiers sur la carte SD ou le SSD de Batocera`.
    
 Si vous préférez utiliser d'autres logos, comme par exemple ceux du thème que vous avez installé. Nous utiliserons le script qui se trouve dans le dossier `Arcade/Logos Sistemas/` du projet [ici](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Il se compose de deux fichiers `Ejecutar Script Logos.bat` et `Script Logos.ps1`.

1.  **Exécutez le fichier** `Ejecutar Script Logos.bat` (Lanceur pour éviter les blocages de Windows).
2.  **Configuration des chemins :**
    * **Origine :** Entrez le chemin où vous avez les logos (ex : `\\\\192.168.1.119\\userdata\\themes\\Animatics-DX-master\\art\\logos`).
    * **Destination :** Entrez le chemin `C:\\Logos`.
4.  **Copier :** Si vous avez sélectionné le chemin `C:\\Logos`, copiez le dossier `Logos` et tout son contenu sur la carte SD ou le SSD où Batocera est installé `roms/marquesinas/`, comme indiqué au point `6. Structure des fichiers sur la carte SD ou le SSD de Batocera`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />


### Que fait le script automatiquement ?
* **Redimensionnement :** Convertit vos marquises originales en **128x32 pixels**.
* **Format :** Force la couleur en **BMP 24 bits** (format compatible avec le pilote DMA de l'ESP32).

> [!CAUTION]
> **Accès par Réseau (Samba) :**
> Si, lors de l'exécution du script, il n'a pas accès au chemin indiqué, vous devrez y accéder via l'explorateur de fichiers et vous connecter avec les identifiants de Batocera pour que le script ait accès au dossier.
> Pour accéder au chemin `ex -> \\\\192.168.1.120\\userdata\\themes\\Animatics-DX-master\\art\\logos`, Windows vous demande des identifiants, utilisez ceux fournis par Batocera par défaut :
> * **Utilisateur :** `root`
> * **Mot de passe :** `linux`

## 6. Structure des fichiers sur la carte SD ou le SSD de Batocera

Pour que l'intégration fonctionne correctement, nous devons coller le dossier marquesinas dans le dossier `roms/`
* **`roms/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquise statique du jeu, ex : `mslug.bmp`)
* **`roms/marquesinas/Arcade/sistema/rom_name.gif`** (Facultatif : marquise animée du même jeu, ex : `mslug.gif`)
* **`roms/marquesinas/Logos/sistema_name.bmp`** (Marquise du système traitée, ex : `mame.bmp`)

#### Exemple visuel des dossiers :
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


## 7. Profitez des marquises pendant que vous jouez sur votre Arcade !
"""

with open("readme_batocera_fr.md", "w", encoding="utf-8") as f:
    f.write(content)

print("File generated successfully.")

Your Markdown file is ready

Icono de MD
readme_batocera_fr
 MD 
Abierto
Aquí tienes el archivo Markdown con toda la documentación sobre la integración con Batocera traducida al francés, manteniendo las alertas de Github, estructuras de carpetas y enlaces a imágenes originales.




Gemini es una IA y puede cometer errores, incluso sobre personas. Tu privacidad y GeminiSe abre en una ventana nueva

Clarifying Script Usage
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

<img width="1103" height="686" alt="image" src="https://github.com/user-attachments/assets/d94c2a67-c40a-451e-9c61-981a188a294d" />

6. **Redémarrez complètement votre système Batocera.**
> [!CAUTION]
> Le redémarrage complet du système est **obligatoire**. Pendant ce démarrage, le script `custom.sh` configurera les permissions internes. À partir de ce moment, chaque fois que vous naviguerez dans le menu, lancerez ou fermerez un jeu, le panneau réagira automatiquement.

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

#### D'où est-ce que je sors les GIFs ?

Vous décidez comment les générer — le panneau a seulement besoin que le fichier final soit de **128×32 pixels**. À titre de référence, si votre collection Batocera contient déjà des vidéos de preview scrappées (`<video>` dans le `gamelist.xml`), vous pouvez les convertir en GIF avec un outil comme [dmd_gif_converter](https://github.com/red77290/dmd_gif_converter), qui en plus de redimensionner inclut un mode de recadrage automatique conçu pour ne pas perdre l'action en réduisant une grande vidéo à une si petite taille. C'est un projet tiers, indépendant de ce dépôt — toute autre méthode qui vous laisse un `.gif` de 128×32 fonctionnera tout aussi bien.

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
readme_batocera_fr.md
Mostrando readme_batocera_fr.md.
