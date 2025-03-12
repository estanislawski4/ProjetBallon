
## 1. Vue d’Ensemble

Ce projet est mené par les étudiants du Lycée Touchard-Washington dans le cadre du BTS Systèmes Numériques (IR), sous l’égide du CNES et de Planète Sciences. Il s’agit d’un défi annuel visant à lancer un ballon stratosphérique, collecter des mesures environnementales, les transmettre au sol, et innover sans relâche pour sublimer l’expérience d’apprentissage.

Pour l’édition 2025, deux axes primordiaux se distinguent :

1.  **Analyser le diamètre du ballon** grâce à des images prises en vol, afin de revisiter la loi des gaz parfaits.
2.  **Gérer une liaison radio LoRa** en télécommunication bidirectionnelle, réceptionnant et renvoyant des informations de télémétrie.

----------

## 2. Objectifs Stratégiques

1.  **Capturer et transmettre des images** via SSTV toutes les 5 minutes, telles des missives célestes.
2.  **Enregistrer en continu** température, pression, humidité et accélérations (x, y, z).
3.  **Émettre des données télémétriques** (LoRa UHF à 433 MHz, format APRS) toutes les 2 minutes.
4.  **Signaler les événements critiques** (éclatement, atterrissage), assurant qu’aucun rebondissement ne vous échappe.

----------

## 3. Technologies Employées

### Nacelle Embarquée

-   **Raspberry Pi Zero** : cerveau miniature, mais redoutable stratège.
-   **Caméra** : immortalise le diamètre du ballon en plein vol.
-   **Module LoRa RA-02** : télémétrie bidirectionnelle, comme une messagerie secrète.
-   **SSTV** : transmission d’images par modulation SSB (HF, 28 MHz).

### Station au Sol

-   **Réception SSTV** : décodage en direct des images, hébergement web sous Linux Debian pour partage instantané.
-   **Passerelle LoRa** : retransmission vers [APRS.fi](https://aprs.fi) et exécution des ordres royaux depuis votre trône numérique.

----------

## 4. Contraintes et Spécifications

-   **Budget** : ± 2000 €, acquis auprès du CNES et de Planète Sciences.
-   **Temps Imparti** : 150 heures pour concevoir, déployer, valider.
-   **Développement** :
    -   C++ sous Linux Debian Stable
    -   Outils : NetBeans, PlatformIO, Doxygen, Apache
    -   Raspberry Pi Zero, modules LoRa, systèmes libres

----------

## 5. Organisation du Corps Expéditionnaire

-   **Étudiants mobilisés** : 4 braves âmes.
-   **Maîtres d’œuvre** : Philippe SIMIER, Jilali KHAMLACH.
-   **Hauts Commanditaires** : CNES & Planète Sciences Espace.

----------

## 6. Livrables et Critères de Réussite

1.  **Photographies & données télémétriques** stockées sur carte SD, diffusées en ligne.
2.  **Détection fiable** des instants cruciaux (éclatement, atterrissage).
3.  **Transmission en temps réel** des informations et images, consultables à distance.

----------

## 7. Ressources Complémentaires

-   **Suivi Télémétrique** : [APRS.fi](https://aprs.fi)
-   **Documentation** : Détails et notes techniques dans le dossier `Documentation`.

> « Qui s’élève dans les airs ne redescend jamais comme avant… »
