
# 🚀 Projet Ballon Stratosphérique 2025

**Lycée Touchard-Washington, Le Mans**

----------

## 📌 Contexte du Projet

Chaque année, le Lycée Touchard-Washington réalise un lancement de ballon stratosphérique dans le cadre du BTS Systèmes Numériques (IR). Ce projet, soutenu financièrement par le CNES et l’association Planète Sciences, a pour objectif pédagogique de permettre aux étudiants de développer et mettre en œuvre des systèmes embarqués complexes, assurant notamment la prise de mesures environnementales et la transmission des données vers le sol.

Cette édition 2025 introduit deux principaux objectifs :

1.  **Mesure du diamètre du ballon** par analyse d'images prises en vol pour renouveler l'expérience sur la loi des gaz parfaits.
2.  **Transmission bidirectionnelle** des données télémétriques via liaison radio LoRa, incluant le suivi des signaux (RSSI et SNR).

----------

## 🎯 Objectifs Techniques Principaux

-   Captation et transmission d'images du ballon via SSTV toutes les 5 minutes.
-   Mesure et enregistrement en continu des données : température, pression, humidité et accélérations (axes x, y, z).
-   Transmission télémétrique régulière via LoRa UHF à 433 MHz, toutes les 2 minutes au format APRS.
-   Détection et signalement d'événements critiques (éclatement du ballon, atterrissage).

----------

## 📡 Technologies et Systèmes Utilisés

**Nacelle embarquée :**

-   Raspberry Pi Zero
-   Module caméra embarquée avec stockage SD
-   Module LoRa RA-02 pour télémétrie et commandes à distance
-   Protocole SSTV pour transmission d’images en modulation SSB (HF, 28 MHz)

**Station au sol :**

-   Station de réception SSTV avec décodage des images en temps réel, hébergées sur serveur web Linux Debian accessible au public.
-   Gateway LoRa, communication bidirectionnelle et retransmission vers le serveur [APRS.fi](https://aprs.fi).

----------

## 🛠️ Contraintes et Spécifications Techniques

-   **Budget :** environ 2000 €, financé par le CNES et Planète Sciences.
-   **Durée totale du projet :** 150 heures.
-   **Développement :**
    -   Langage : C++ sous Linux Debian Stable
    -   IDE : NetBeans et PlatformIO
    -   Raspberry Pi Zero avec caméra intégrée et module LoRa RA-02
    -   Logiciels libres (Apache, Debian, NetBeans, Modélio, Doxygen)

----------

## 👨‍💻 Équipe Projet

-   **Étudiants participants :** 4
-   **Professeurs responsables :** Philippe SIMIER, Jilali KHAMLACH
-   **Commanditaires :** CNES & Planète Sciences espace

----------

## ✅ Critères d’Évaluation et Livrables attendus

-   Photos et données télémétriques stockées sur carte SD et accessibles en ligne.
-   Détection fiable des événements critiques (atterrissage, éclatement).
-   Transmission d’images et télémétrie opérationnelles et consultables en temps réel.

----------

## 🌐 Partenaires et Soutiens

-   **Centre National d’Études Spatiales (CNES)**
-   **Planète Sciences Espace**
-   **Radio Club de la Sarthe**

----------

## 🔗 Liens utiles

-   Suivi télémétrique : [APRS.fi](https://aprs.fi)
-   Documentation et détails techniques dans le dossier `Documentation`.

