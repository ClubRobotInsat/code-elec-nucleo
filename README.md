# Code Elec Nucleo

## *Getting started*

### Introduction 

Cette section contient les étapes à suivre pour préparer son PC pour pouvoir développer du code embarqué sur le robot, coté élec. 

La carte mère du robot embarque une [Nucleo-F446RE](http://www.st.com/en/evaluation-tools/nucleo-f446re.html), que l'on programme en utilisant le langage C++. Le processeur utilisé est un STM32 Cortex M4 (qui diffère un peu des Cortex M3 des Nucleo-F103RB utilisées en TP de 3A et 4A au GEI). 

Pour pouvoir intéragir avec le "monde extérieur", on utilise la librairie [MBED OS](https://os.mbed.com/) fournie par ARM. 
Cette librairie nous permet de garder un code simple et accessible, très proche de ce qu'on peut trouver sur les cartes Arduino, 
tout en ayant la possibilité de mettre les mains dans le cambouis quand on en a vraiment besoin. 

De cette librairie, nous n'utilisons que les *drivers*, et nous laissons de côté la partie *OS*. 

Pour développer du code, on peut utiliser l'outil de développement en ligne de mbed, disponible [ici](https://os.mbed.com/compiler/) (il faut créer un compte pour y accéder). 

On trouve un tutoriel pour *mbed online compiler* [sur leur site web](https://os.mbed.com/docs/latest/tutorials/blinky-on-the-arm-mbed-online-compiler.html). 


Cet outil est bien pour commencer, mais est quand même limité. Faute de "vrai" IDE, pour le moment on développe comme des barbus : ligne de commande et éditeur de texte.

Les étapes suivantes devraient vous guider pour mettre en place un environnement de développement sur votre PC. 

**En cas de pépin... demandez conseil aux gens du Club :grin:.**

### Installation

On commence par installer tous les paquets dont on va avoir besoin : la toolchain arm, gdb (débugueur), openocd pour communiquer avec la carte, git pour versionner le code, et mercurial et python-pip pour mbed.

```
sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi openocd git mercurial python-pip
```

Ensuite on clone le repo avec git.

```
git clone https://github.com/ClubRobotInsat/code-elec-nucleo
```

Installons `mbed-cli` via le gestionnaire de paquets de python.

```
sudo pip install mbed-cli
```

Il est temps de configurer le dossier de travail

```
cd code-elec-nucleo
mbed deploy
cd .temp/tools
sudo pip install -r requirements.txt
```

On configure `gdb` (2 parties). 
*Optionnel* : on peut installer une jolie interface pour gdb (recommandé, mais optionnel si vous avez déjà votre setup de barbu).

```
wget -P ~ git.io/.gdbinit
```

On dois surtout autoriser le chargement du script pour flasher la carte (*pas optionnel*).

```
echo "set auto-load safe-path /" >> ~/.gdbinit
```

### Compiler et executer du code sur la carte

#### Compilation

Pour compiler, se mettre dans le dossier cloné plus tôt (depuis https://github.com/ClubRobotInsat/code-elec-nucleo) et utiliser la commande :

```
mbed compile --profile debug_robot.json
```

On spécifie un profil de compilation qui fait plusieurs choses :

* il active les symboles de debug pour que gdb ait des choses à nous montrer
* il active le standard C++11

#### Openocd

Il faut maintenant brancher la carte au pc, et toujours dans le dossier cloné plus tôt lancer `openocd` dans un terminal séparé.
Il va servir de "serveur" à gdb et va communiquer les infos de debug de la carte à gdb via un socket tcp.

```
sudo openocd -f stm32.cfg
```

#### Debug

Dans un deuxième terminal, on peut maintenant lancer le script de debug.

```
./debug.sh
```

Ensuite, une fois le chargement terminé, entrer *c* ou *continue*

Il est aussi possible d'afficher les printf sur notre pc (tout du moins ceux réalisé avec l'appel à `debug`) :

```
sudo screen /dev/ttyACM0 9600
```

Pour quitter `screen` il faut taper : `ctrl-a` , puis k, puis y.

## Petite liste de liens utiles pour le développement

### La base pour trouver des ressources : [MBED Cookbook](https://os.mbed.com/cookbook/Homepage)
### La doc. pour l'API : [ici](https://os.mbed.com/docs/latest/reference/drivers.html)
