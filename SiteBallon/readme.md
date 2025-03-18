
# Analyse des Trames

**Une interface élégante et analytique pour visualiser et comprendre vos données réseau**

----------

## Présentation

Cette application web vous permet d’analyser et de visualiser des trames réseau selon différents critères tels que la source, la destination, l'heure, les messages fréquents, et l'historique complet des trames. L’interface utilise des graphiques dynamiques, générés avec **Highcharts**, afin de simplifier l'interprétation et de rendre les analyses immédiatement exploitables.

----------

## Fonctionnalités principales :

-   **Graphiques dynamiques :**
    
    -   Nombre de trames par machine source
    -   Nombre de trames par machine de destination
    -   Répartition quotidienne des trames
    -   Top 10 des messages les plus fréquents
    -   Évolution horaire des trames
-   Affichage de l'historique détaillé des trames sous forme de tableau interactif.
    

----------

## Structure du projet

`\
├── App
│   ├── Controllers
│   │   └── TramesController.php
│   └── Models
│       └── TrameModel.php
├── Views
│   └── trames
│       └── index.php
├── public │   ├── css (optionnel pour styles)
│   └── index.php
├── assets (éventuellement pour images, css, js externes)
└── README.md`

----------

## Technologies utilisées

-   **PHP (MVC)**
-   **PDO (MySQL)** pour la gestion des données
-   **Highcharts** pour la visualisation dynamique des graphiques
-   **HTML & CSS (responsive & moderne)**

----------

## Installation rapide

### Prérequis

-   PHP >= 7.4
-   Serveur MySQL (MariaDB/MySQL)
-   Serveur Web Apache/Nginx

**Installation rapide :**

`git clone votre_repo.git cd votre-dossier
composer install (si applicable)` 

Configurez ensuite votre connexion à la base de données dans votre fichier d'initialisation (exemple avec `.env` ou directement dans votre contrôleur/modèle).

----------

## Exécution du projet

Placez le projet dans votre serveur local (XAMPP, WAMP, ou serveur Apache/Nginx).

Accédez au projet via :

`http://localhost/votre-projet/?chart=source` 

Changez le paramètre `chart` selon les visualisations désirées :

-   `?chart=source` (par défaut)
-   `?chart=destination`
-   `?chart=day`
-   `?chart=top_messages`
-   `?chart=hourly`
-   `?chart=historique`

----------

## Exemples visuels (captures d'écran éventuelles)

![Alt text](https://i.imgur.com/iAl3153.png)

