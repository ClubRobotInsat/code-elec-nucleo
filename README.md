# Code Elec Nucleo

## *Getting started*
#### 1. Installer Keil uVision 5 
Une version est disponible au téléchargement à cette adresse : https://www.keil.com/download/product/
Il faut sélectionner le produit "MDK-ARM", puis remplir le formulaire de téléchargement. 

##### Windows
L'installation doit se faire sans problème. 

##### Linux
###### Installation
Pour un PC sous Linux, il faut passer par Wine.

Pour installer Wine, voir la page correspondant à votre distro : 
  * [la page pour Ubuntu](https://doc.ubuntu-fr.org/wine), 
  * [la page pour Arch](https://wiki.archlinux.fr/Wine)), 
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

** Vous pouvez maintenant lancer Keil en tapant `keil` dans un terminal :) ** 


#### 2. Mettre en place son environnement de travail
TBD


#### 3. Cloner le dossier partagé sur Github
``` 
git clone --recursive https://github.com/ClubRobotInsat/code-elec-nucleo.git
```
## Petite liste de liens utiles pour le développement

### La base pour trouver des ressources : [MBED Cookbook](https://os.mbed.com/cookbook/Homepage)
### La doc. pour l'API : [ici](https://os.mbed.com/docs/latest/reference/drivers.html)


