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

Cet outil est bien pour commencer, mais est quand même limité. Comme "vrai" IDE, avec un débuggueur intégré, on 
utilise donc *Keil uVision 5*, qui est le logiciel utilisé dans les formations STM32 au GEI (en MIC et IMACS). 
Ce logiciel est donc installé sur une grande partie des postes du GEI, et sur les PC du Club Robot (TBD). 

Les étapes suivantes devraient vous guider pour mettre en place un environnement de développement sur votre PC. 


**En cas de pépin... demandez conseil aux gens du Club :grin:.**

### 1. Installer Keil uVision 5 
Une version est disponible au téléchargement à cette adresse : https://www.keil.com/download/product/
Il faut sélectionner le produit "MDK-ARM", puis remplir le formulaire de téléchargement. 

##### Windows
L'installation doit se faire sans problème. 

##### Linux

Installer la toolchain arm :

```
sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi openocd
```

Il existe un chouette thème pour gdb par [ici](https://github.com/cyrus-and/gdb-dashboard).

Installer mbed :
 
```
git clone https://github.com/ClubRobotInsat/code-elec-nucleo
sudo apt-get install mercurial python-pip
sudo pip install mbed-cli
cd code-elec-nucleo
mbed deploy
cd .temp/tools
sudo pip install -r requirements.txt
cd ../..
# On désactive la sécurité de gdb.
echo "set auto-load safe-path /" >> ~/.gdbinit
mbed compile --profile .temp/tools/profiles/debug.json
./debug.sh
```

###### Installation
Pour un PC sous Linux, il faut passer par Wine.

Pour installer Wine, voir la page correspondant à votre distro : 
  * [la page pour Ubuntu](https://doc.ubuntu-fr.org/wine), 
  * [la page pour Arch](https://wiki.archlinux.fr/Wine), 
  * *ajouter ici des pages supplémentaires*
  
Ensuite, utiliser `wine` pour installer Keil depuis un terminal : 
``` 
wine /chemin\ vers\ executable\ keil/nom\ executable.EXE
``` 
Par exemple : 
``` 
wine ~/Téléchargements/MDK524a.EXE
``` 

**NB:** Il est possible que l'installation nécessite les droits super-utilisateur. 
Dans ce cas, utilisez `sudo wine` à la place de `wine`. 

###### Simplifiez-vous la vie
Comment-lancer Keil une fois l'installation terminée ??? 

Rendez-vous dans le dossier de Wine :
```
cd /chemin\ vers\ wine/
``` 
(Wine se trouve généralement dans `~/.wine`, ou dans `/root/.wine` si vous avez utilisé `sudo wine`.)

Déplacez-vous dans le dossier `drive_c/Keil_v5/UV4`. 
``` 
cd drive_c/Keil_v5/UV4
``` 

Ce dossier contient les exécutables de Keil. On peut lancer le logiciel avec `wine UV4.EXE`. 

On va se créer un raccourci en ligne de commande (un `alias`). 

Pour cela, ouvrez le fichier `~/.bashrc` avec 
votre éditeur de texte préféré (par exemple `nano`) : 
``` 
nano ~/.bashrc
``` 

Puis ajoutez la ligne suivante tout en bas du fichier :
  * si vous avez utilisé `wine` : 
``` 
alias keil='wine /chemin\ vers\ wine/drive_c/Keil_v5/UV4/UV4.EXE'
``` 
en remplacant le chemin qui va bien, par exemple : 
```
alias keil='wine ~/.wine/drive_c/Keil_v5/UV4/UV4.EXE'
``` 
  * si vous avez utilisé `sudo wine` : 
``` 
alias keil='sudo wine /chemin\ vers\ wine/drive_c/Keil_v5/UV4/UV4.EXE'
``` 
en remplacant le chemin qui va bien, par exemple : 
```
alias keil='sudo wine /root/.wine/drive_c/Keil_v5/UV4/UV4.EXE'
``` 

Sauvegardez le fichier, et rechargez le fichier de configuration : 
``` 
source ~/.bashrc
``` 

**Vous pouvez maintenant lancer Keil en tapant `keil` dans un terminal :blush:** 


### 2. Cloner le dossier partagé sur Github
``` 
git clone --recursive https://github.com/ClubRobotInsat/code-elec-nucleo.git
```
## Petite liste de liens utiles pour le développement

### La base pour trouver des ressources : [MBED Cookbook](https://os.mbed.com/cookbook/Homepage)
### La doc. pour l'API : [ici](https://os.mbed.com/docs/latest/reference/drivers.html)
