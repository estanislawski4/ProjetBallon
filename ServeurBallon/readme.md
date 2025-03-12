
## Présentation

Cette application, développée sous _Qt Creator_, se veut le **tableau de bord d’administration** du système. Ici, nul besoin de vous aventurer dans la stratosphère : tout se pilote depuis un ordinateur, confortablement ancré à la terre ferme. L’objectif est limpide :

1.  **Envoyer des requêtes** vers la nacelle, exactement comme un souverain émettrait ses décrets.
2.  **Recevoir les retours** de données, valider le bon fonctionnement de la nacelle et consigner ces informations.
3.  **Assurer un suivi** en temps réel de la position ou de l’état du ballon.

Ainsi, d’un simple clic, vous commandez à distance l’oiseau volant vers la stratosphère…

----------

## Fonctionnalités principales

1.  **Gestion de ports série** : Dialogue direct avec le module LoRa via un port série. L’application lit et écrit dans ce canal pour transmettre ou recevoir des trames KISS.
2.  **Conversion AX.25 / TNC2** : Pas de place pour l’approximation lorsqu’il s’agit de coder ou décoder les formats de trames. Un module dédié gère cette gymnastique, de sorte qu’aucune parcelle d’information ne vous échappe.
3.  **Connexion APRS-IS** : Le programme se connecte à un serveur APRS, y loge ses identifiants, et relaie ainsi les trames de position ou de message. Votre empire des ondes s’étend là où bon vous semble.
4.  **Base de données MySQL** : Vous consignez chaque trame reçue ou transmise, enregistrant source, destination et contenu dans une BDD distante. Si un espion venait à questionner l’historique, vous aurez toutes les preuves en main.
5.  **Interface graphique** : Développée sous Qt, elle répertorie les ports disponibles, l’état des connexions, et offre un champ de saisie pour vos messages. Ainsi, vous gouvernez vos transmissions au gré de votre humeur et de vos besoins.

----------

## Logique de l’architecture

1.  **Interface (interface.cpp)**
    
    -   Propose une **fenêtre Qt** affichant les logs, la configuration des ports, et divers contrôles.
    -   Récupère la liste des ports série et déclenche l’ouverture du canal souhaité.
    -   Prépare et envoie la trame APRS ou LoRa, puis journalise les opérations.
    -   Stocke systématiquement dans la base MySQL les trames transmises et reçues.
2.  **APRSISClient (aprsisclient.cpp)**
    
    -   Gère la **connexion TCP** à un serveur APRS-IS.
    -   À la connexion, envoie la ligne de _login_ et reste à l’écoute des messages entrants, que l’application répercute dans l’interface.
3.  **AX25Converter (ax25converter.cpp)**
    
    -   Opère la **conversion** entre le format TNC2 et AX.25.
    -   Lecture, découpe et recomposition des adresses, extraction du _payload_.
    -   Ainsi, le flux transite aisément des trames KISS au format APRS et inversement.
4.  **KISSHandler (kisshandler.cpp)**
    
    -   Parse les **trames KISS** reçues du port série.
    -   Reconstitue le flux AX.25, puis le transforme en TNC2 grâce au convertisseur dédié.
    -   En cas de succès, la trame est alors exploitable (transmissible vers APRS, sauvegardée en BDD, etc.).
5.  **MySQLManager (mysqlmanager.cpp)**
    
    -   S’occupe des connexions et requêtes MySQL.
    -   Gère l’insertion de nouvelles machines (indicatifs) et de trames.
    -   Vous maintenez ainsi une **traçabilité** sans faille des messages.
6.  **SerialPortManager (serialportmanager.cpp)**
    
    -   Assure la **communication série** (ouverture, écriture, lecture).
    -   Émet des signaux en cas de réception de données ou d’erreur, signaux ensuite captés par d’autres modules (KISSHandler, interface graphique, etc.).

----------

## Utilisation

1.  **Configuration initiale**
    -   Veillez à renseigner correctement l’adresse du serveur APRS, les identifiants, et les paramètres de la base MySQL.
2.  **Lancement de l’application**
    -   Choisissez le port série adéquat dans l’interface.
    -   Activez l’option d’envoi APRS si vous désirez propager vos trames au grand monde (ou garder le secret si le dessein l’exige).
3.  **Envoi et réception**
    -   Rédigez un message depuis l’interface, sélectionnez la destination, puis laissez la machine orchestrer conversions, envois et journalisations.
    -   Toute trame entrante surgit dans la zone de logs.
4.  **Sauvegarde automatique**
    -   Chaque trame transite par la BDD, vous n’avez rien d’autre à faire que d’observer, à l’abri dans votre forteresse numérique.

> « Contrôler la technique, c’est régner en silence – et nul ne se doute
> alors de la force qui se cache derrière un écran. »
