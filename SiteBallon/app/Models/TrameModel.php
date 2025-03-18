<?php
namespace App\Models;

use PDO;
use PDOException;

class TrameModel
{
    private $bdd;

    public function __construct($host, $dbname, $user, $pass)
    {
        try {
            $this->bdd = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $user, $pass);
            $this->bdd->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        } catch (PDOException $e) {
            throw new \Exception("Erreur de connexion PDO : " . $e->getMessage());
        }
    }

    // Graphique 1 : Nombre de trames par machine (source)
    public function getTramesCountBySource()
    {
        $sql = "SELECT source, COUNT(*) AS total FROM trames GROUP BY source";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    // Graphique 2 : Nombre de trames par machine (destination)
    public function getTramesCountByDestination()
    {
        $sql = "SELECT destination, COUNT(*) AS total FROM trames GROUP BY destination";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    // Graphique 3 : Nombre de trames par jour
    public function getTramesCountByDay()
    {
        $sql = "SELECT DATE(date_reception) AS day, COUNT(*) AS total FROM trames GROUP BY day ORDER BY day";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    // Graphique 4 : Top 5 messages (les 5 messages les plus fréquents)
    public function getTopMessages()
    {
        $sql = "SELECT message, COUNT(*) AS total FROM trames GROUP BY message ORDER BY total DESC LIMIT 10";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    // Graphique 5 : Évolution horaire des trames (groupées par heure)
    public function getTramesHourly()
    {
        $sql = "SELECT DATE_FORMAT(date_reception, '%Y-%m-%d %H:00:00') AS hour, COUNT(*) AS total 
                FROM trames GROUP BY hour ORDER BY hour";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    public function getAllTrames()
    {
        $sql = "SELECT * FROM trames ORDER BY date_reception DESC";
        $stmt = $this->bdd->query($sql);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }
}

