Direct capture sur votre écran RGB Matrix leds
----------------------------------------------
Après avoir construit mon écran grace à HZeller, je cherchais un moyen de faire une copie d'écran permanente de la sortie vidéo de la RaspBerry 3 vers ma RGB Matrix. Je suis tombé sur raspi2png de AndrewFromMelbourne.
Mon prog LensCapture est une synthèse des deux, il vous permettra de définir une zone carrée (xa,ya,xb,yb) dont le contenu sera copié en live sur la matrice leds. Ainsi que de faire varier directement le facteur de zoom.
Le processus peut être réglé PRESQUE en live. Pressez CTRL+C pour entrer une commande.
 - q pour quitter
 - + pour augmenter le zoom global
 - - pour diminue le zoom
 - 8 / 2 zone capture vers le haut / bas
 - 4 / 6 zone capture vers gauche / droite
 - w / h zone capture plus large / haute
Enter et le processus de capture en live reprend.

Limitations :
-----------
 - le nombre de leds ne permet pas souvent de voir les détails, sauf très gros écrans. Il faut donc cibler une toute petite zone de capture.
 - attention au ralentissement de la cpu due aus taches en parallèles.

Usage
-----


$ make
$ sudo ./lenscapture
usage: ./lenscapture [optional parameter]

Les paramètres relatifs au circuit des matrices de Leds sont ceux du projet de HZeller d'origine.

  --led-gpio-mapping=<name> : Name of GPIO mapping used. Default "regular"
 
  --led-rows=<rows>         : Panel rows. Typically 8, 16, 32 or 64. (Default: 32).
 
  --led-cols=<cols>         : Panel columns. Typically 32 or 64. (Default: 32).
 
  --led-chain=<chained>     : Number of daisy-chained panels. (Default: 1).
 
  --led-parallel=<parallel> : Parallel chains. range=1..3 (Default: 1).
...
 
Merci à :
* hzeller for is wonderful work and all his canvas RGBMatrix program.
* AndrewFromMelbourne for his code raspi2png wich is the start of the screen capture.
