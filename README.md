# SACCO_Lab1
## A. Introduction

L'objectif de ce TP était de maîtriser l'utilisation de l'outil Platform Designer et de créer un premier flux de conception en mettant en œuvre un chenillard
sur la carte DE10-Lite, contrôlé à l'aide d'un bouton poussoir et dont la vitesse pouvait être ajustée à l'aide de quatre interrupteurs. Nous commencerons 
tout d'abord par décrire l'architecture du système en présentant les composants utilisés, les choix et les étapes de conception effectués. Ensuite, nous expliquerons 
comment la fonctionnalité a été développée, en utilisant une méthode de balayage par scrutation puis via l'utilisation d'interruptions.
## B. System Architecture
  ### 1. Design QSYS
  
  Pour ce TP, j'ai choisi de structurer mon architecture de la manière suivante:
  
![NIOS2](https://user-images.githubusercontent.com/104905653/212214147-40632346-e7ab-4cf5-bd5a-44d0583bc035.png)

  On retrouve donc les différents blocs IPs suivants:
  -NIOS2
  -JTAG-UART
  -RAM: Étant donnée qu'elle était intialement insuffisante, j'ai augmenté sa taille à 32000 bits pour ne pas avoir de problème
  -1 PIO de largeur 1 bits pour le bouton poussoir
  -1 PIO de largeur 8 bits pour contrôler les 8 leds de mon chenillard
  -1 PIO de largeur 4 bits affectés à 4 interrupteurs afin de contrôler la vitesse de mon chenillard
  
  Pour implémenter cette architecture, j'ouvre Platform Designer puis je sélectionne un par un les différents blocs donc j'ai besoin. J'ajoute également une horloge pour cadencer le système. Je relie l'ensemble, puis j'affecte les adresses de bases. J'obtiens enfin l'architecture suivante:
 
 ![QSYS_Lab1](https://user-images.githubusercontent.com/104905653/212268953-39e0c21f-07ee-42e6-8a7d-42847f9e348a.PNG)

  Une fois cela fait, je sauvegarde et génère le HDL et je retourne sur Quartus pour écrire mon Top-Level, dispnible sous le nom LedChaser.vhd. Je lance alors l'Analyse et la Synthèse. Une fois cela fait, il faut encore assigner chaque broche via le Pin PLanner. Pour savoir sur quelles broche mes signaux doivent être assignés et quels doit être la tension appliquée, je m'appuie sur le Guide Utilisateur de la carte DE10-LITE. J'obtiens ainsi le Pin Assignement suivant:
  
 ![Pin_planner_lab1](https://user-images.githubusercontent.com/104905653/212270197-04bf8b20-7d85-46ce-aceb-228caf2caf9f.png)

Une fois cela fait, je clique sur "Assemble" et "Fitter". Je peux ensuite passer à la partie logicielle, avec la génération de ma BSP et l'écriture du code C.

### 2. Génération BSP

On commence tout d'abord par ouvrir NIOS2 Command Shell. Afin d'organiser plus clairement mon répertoire de projet, je crée un répertoire software et 2 sous répertoire app/ et bsp/. 
Ensuite, on génère notre BSP par la commande suivante:
`nios2-bsp hal ./bsp ../lab1.sopcinfo`

Je créée ensuite mon code C disponible dans le fichier main.c puis je génère le Makefile associé:
 `nios2-app-generate-makefile --app-dir ./app/ --bsp-dir ./bsp/ --elf-name lab1.elf --src-files app/main.c`

Je compile ensuite mon fichier main.c puis je le téléverse dans la carte via les 2 commandes suivantes:
  `make
   make download-elf`

### 3. Écriture du code C
#### 3.1 Méthode par balayage

La méthode par balayage ou polling consiste à évaluer en continu la valeur d'une variable, de sorte que une opération puisse être directement effectuée lorsque celle-ci est modifiée. J'ai d'abord utilisé cette méthode car plus simple à mettre en place. Pour cela, j'utilise les adresses suivantes:
- PIO_0_BASE: contient l'adresse de la PIO des LEDs
- PIO_1_BASE: contient l'adresse de la PIO des interrupteurs
- PIO_2_BASE: contient l'adresse de la PIO du bouton poussoir

J'évalue donc en continue l'état de mon bouton poussoir grâce à la fonction suivante:
  `IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);`
  
Lorsque celle-ci est différente de 1 (actif à l'état bas), je lis alors la valeur de mes interrupteurs afin de déterminer la vitesse du chenillard.
Je rentre ensuite dans ma boucle for, et à chaque itération je décale laquelle des Leds est allumée en écrivant dans le registre des LEDs grâce à la fonction suivante:
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x01<<i);

Une fois le chenillard effectué, j'éteins toutes mes lEDs:
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x00);
  
#### 3.2 Méthode par interruption

Bien que le polling soit une méthode intuitive et facile à mettre en place, elle a le désavantage de recourir en permanence au CPU. J'ai donc par la suite modifier mon code pour que le fonctionnement soit maintenant fait par interruption. Désormais, à chaque fois que l'on appuiera sur le bouton poussoir, une routine d'exécution sera effectuée.

Pour cela, j'ai tout d'abord modifié mon design QSYS afin d'activer les interruptions sur la PIO du bouton poussoirs ainsi que des interrupteurs:

![interrupt](https://user-images.githubusercontent.com/104905653/212270971-e7cbc424-9b9b-4b08-8b94-3257edf065ad.png)

Ensuite, j'ai écrit mes deux routines d'interruptions button_irq et switch_irq. La première contient 2 boucles for permettant de faire un aller retour du chenillard. A l'aller, on décale à chaque itération la led allumée vers la gauche via la ligne:
</p> IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x01<<i); </p>

Et au retour, on décale la led allumée à chaque itération via la ligne:
</p> IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE,0x80>>i); </p>

A la fin du chenillard, on remet toutes les leds à 0 et on réinitialise le flag de l'interruption associé au bouton poussoir via la ligne:
IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_2_BASE, 0x1);


Pour la routine d'interruption des interrupteurs, je définis en variable globale mon timer à une valeur de 25 ms, de sorte que si je ne touche pas à mes interrupteurs, mon chenillard ait quand même une vitesse intiale. Ensuite, à chaque changement d'interrupteurs, je modifie la vitesse grâce à la formule suivante:
	`time = time*10000 + 25000;`
	
Je relâche les flags associés au 4 interrupteurs.
 Enfin, dans le main, je déclare mes interruptions et quelles routines exécutées lrosqu'elles se produisent.
 
## C. Progress and Results


Chacunes des 2 versions produites sont fonctionnelles. Cependant, comme il s'agissait de la 1ère version du chenillard, la méthode par polling ne faisait faire qu'un aller simple à mon chenillard, ce qui fut par la suite corriger dans la version utilisant des interruptions. Voici-ci dessous une démonstration complète du chenillard:

[INSERER VIDEO CHENILLARD]


## D. Conclusion

En conclusion, ce premier TP m'a permis, au travers d'un exemple simple de chenillard, de prendre en main l'outil Platform Designer et de me familiariser avec le flot de conception logicielle et matérielle. Ce TP a également été l'occasion d'utiliser plusieurs périphériques, comme les LEDs ou les boutons poussoirs. J'ai enfin pu apprendre comment se faisait l'implémentation d'interruptions à la fois sous QSYS mais également dans le code C.
