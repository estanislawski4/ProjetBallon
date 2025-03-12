
## Aperçu général

Ce programme C++ orchestre la réception et le traitement de requêtes LoRa émises vers la nacelle d’un ballon stratosphérique. Point d’oisiveté : nous ne daignons agir que sur trois codes Q savamment sélectionnés. Chacun d’eux ouvre la voie à une réponse spécifique, à condition que la requête ait été formulée dans les règles de l’art, c’est-à-dire :

1.  Qu’un point d’interrogation (`?`) suive le code Q.
2.  Que la destination soit `F4KMN-8` (l’indicatif réservé à notre nacelle de test).
3.  Que l’indicatif source soit soit `F4KMN` soit `F4LTZ`, _agrémenté ou non_ d’un suffixe du type `-1`.

----------

## Codes Q gérés

-   **QSA** : Obtenir la force du signal (`RSSI` et `SNR`).
-   **QSL** : Recevoir un accusé de réception prouvant la validité de la transmission.
-   **QTR** : Obtenir l’heure locale en temps réel.

----------

## Règles de traitement

1.  **Détection du code Q** : Seul un code convenablement formé (par ex. `QSA?`) est considéré digne d’une réponse.
2.  **Contrôle de la source** : L’indicatif source doit être `F4KMN` ou `F4LTZ`, peu importe s’il arbore un joli `-X` (comme `-1`).
3.  **Contrôle de la destination** : La requête doit explicitement s’adresser à `F4KMN-8`, sous peine d’être royalement ignorée.
4.  **Réponse adaptée** : En fonction du code Q validé, le programme assemble la réponse et l’expédie dans la file de messages dédiée.

----------

## Structure du projet

### GestionFile.cpp

Ce fichier gère la **file de messages** (IPC) pour prioriser le flux de requêtes. Il s’assure qu’aucune supplique, même la plus urgente, ne vienne interrompre un traitement en cours. Dans les entrailles de cette implémentation, le verrouillage par mutex protège l’écriture et la lecture afin d’éviter tout conflit.

### reception.cpp

Ici se trouve l’intelligence qui traite la requête reçue et fabrique la réponse appropriée. Au menu :

1.  Extraction de l’indicatif source et vérification qu’il s’agit bien de `F4KMN` ou `F4LTZ`.
2.  Contrôle de la destination (`F4KMN-8`).
3.  Identification du code Q (QSA, QSL, QTR).
4.  Construction du message retour, contenant soit la force du signal, soit l’heure locale, soit un accusé de réception.

> « La précision est la politesse des rois… mais surtout celle des
> reines, stratégiquement, pour qui veut commander le ciel. »
