Direct capture sur votre écran RGB Matrix leds
----------------------------------------------
Après avoir fait mon écran grace à HZeller, je cherchais un moyen de faire une copie d'écran permanente de ma RaspBerry 3 vers mes Leds RGB Matrix. Je suis tombé sur raspi2png , le beau travail de AndrewFromMelbourne.
Mon prog LensCapture est une synthèse des deux prog, il vous permettra de définir une zone carrée (xa,ya,xb,yb) dont le contenu sera copié en live sur la matrice leds.
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
 - capture live ou pas, le nombre de leds ne permet pas souvent de voir les détails, sauf très gros écrans. Il faut donc cibler une toute petite zone de capture.
 - attention au ralentissement de la cpu due aus taches en parallèles.

Usage
-----
Les paramètres relatifs au circuit des matrices de Leds sont ceux du projet de HZeller d'origine.

$ make
$ sudo ./lenscapture
usage: ./lenscapture [optional parameter]
Options:
  --led-gpio-mapping=<name> : Name of GPIO mapping used. Default "regular"
  --led-rows=<rows>         : Panel rows. Typically 8, 16, 32 or 64. (Default: 32).
  --led-cols=<cols>         : Panel columns. Typically 32 or 64. (Default: 32).
  --led-chain=<chained>     : Number of daisy-chained panels. (Default: 1).
  --led-parallel=<parallel> : Parallel chains. range=1..3 (Default: 1).
  --led-multiplexing=<0..10> : Mux type: 0=direct; 1=Stripe; 2=Checkered; 3=Spiral; 4=ZStripe; 5=ZnMirrorZStripe; 6=coreman;
 7=Kaler2Scan; 8=ZStripeUneven; 9=P10-128x4-Z; 10=QiangLiQ8 (Default: 0)
  --led-pixel-mapper        : Semicolon-separated list of pixel-mappers to arrange pixels.
                              Optional params after a colon e.g. "U-mapper;Rotate:90"
                             Available: "Mirror", "Rotate", "U-mapper". Default: ""
  --led-pwm-bits=<1..11>    : PWM bits (Default: 11).
  --led-brightness=<percent>: Brightness in percent (Default: 100).
  --led-scan-mode=<0..1>    : 0 = progressive; 1 = interlaced (Default: 0).
  --led-row-addr-type=<0..3>: 0 = default; 1 = AB-addressed panels; 2 = direct row select; 3 = ABC-addressed panels (experimental) (Default: 0).
  --led-show-refresh        : Show refresh rate.
  --led-inverse             : Switch if your matrix has inverse colors on.
  --led-rgb-sequence        : Switch if your matrix has led colors swapped (Default: "RGB")
  --led-pwm-lsb-nanoseconds : PWM Nanoseconds for LSB (Default: 130)
  --led-pwm-dither-bits=<0..2> : Time dithering of lower bits (Default: 0)
  --led-no-hardware-pulse   : Don't use hardware pin-pulse generation.
  --led-panel-type=<name>   : Needed to initialize special panels. Supported: 'FM6126A'
  --led-slowdown-gpio=<0..4>: Slowdown GPIO. Needed for faster Pis/slower panels (Default: 1).
  --led-daemon              : Make the process run in the background as daemon.
  --led-no-drop-privs       : Don't drop privileges from 'root' after initializing the hardware.

Thanks to :

* hzeller for is wonderful work and all his canvas RGBMatrix program.
* AndrewFromMelbourne for his code raspi2png wich is the start of the screen capture.
