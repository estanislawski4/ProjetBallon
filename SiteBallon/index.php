<?php
// index.php (front controller)
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require_once 'config.inc.php';

// Autoload
spl_autoload_register(function ($class) {
    $class = str_replace('\\', '/', $class);
    if (substr($class, 0, 4) === "App/") {
        $class = substr($class, 4);
    }
    $file = __DIR__ . '/app/' . $class . '.php';
    if (!file_exists($file)) {
        error_log("Autoloader error: le fichier $file n'existe pas.");
        return;
    }
    require_once $file;
});

use App\Models\TrameModel;
use App\Controllers\TramesController;

// Instanciation du modèle avec les constantes de config
$model = new TrameModel(host, db, user, mdp);
// Instanciation du contrôleur
$controller = new TramesController($model);

// Appeler la méthode index du contrôleur
$result = $controller->index();

// Variables pour la vue
$data = $result['data'];
$chartType = $result['chartType'];

// Inclure la vue
$viewPath = __DIR__ . '/app/Views/' . $result['view'] . '.php';
require $viewPath;

