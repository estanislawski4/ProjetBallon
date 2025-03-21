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
    public function getTopMessages($date_start = null, $date_end = null)
    {
        $sql = "SELECT message, COUNT(*) AS total FROM trames";
        $conditions = [];
        $params = [];

        if ($date_start) {
            $conditions[] = "date_reception >= :date_start";
            $params[':date_start'] = $date_start;
        }
        if ($date_end) {
            $conditions[] = "date_reception <= :date_end";
            $params[':date_end'] = $date_end;
        }
        if (!empty($conditions)) {
            $sql .= " WHERE " . implode(" AND ", $conditions);
        }
        
        $sql .= " GROUP BY message ORDER BY total DESC LIMIT 10";

        $stmt = $this->bdd->prepare($sql);
        $stmt->execute($params);
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

    public function getAllTrames($date_start = null, $date_end = null, $source = null, $destination = null)
    {
        $sql = "SELECT * FROM trames";
        $conditions = [];
        $params = [];

        if ($date_start) {
            $conditions[] = "date_reception >= :date_start";
            $params[':date_start'] = $date_start;
        }
        if ($date_end) {
            $conditions[] = "date_reception <= :date_end";
            $params[':date_end'] = $date_end;
        }
        if ($source) {
            $conditions[] = "source = :source";
            $params[':source'] = $source;
        }
        if ($destination) {
            $conditions[] = "destination = :destination";
            $params[':destination'] = $destination;
        }
        if (!empty($conditions)) {
            $sql .= " WHERE " . implode(" AND ", $conditions);
        }
        
        $sql .= " ORDER BY date_reception DESC";

        $stmt = $this->bdd->prepare($sql);
        $stmt->execute($params);
        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    /**
     * Récupère les données de télémétrie (Temp, Humidité, Pression, Accélérations, etc.)
     */
    public function getTelemetryData()
    {
        $sql = "SELECT message, date_reception 
                FROM trames 
                WHERE message LIKE '%_%' 
                ORDER BY date_reception ASC";

        $stmt = $this->bdd->query($sql);
        $rows = $stmt->fetchAll(PDO::FETCH_ASSOC);

        $telemetryData = [];

        foreach ($rows as $row) {
            $message = $row['message'];

            // Exemple : t077h36b9993 ...
            $regex = '/t(\d+)h(\d+)b(\d+)\s+([-+0-9,\.\s]+)/';

            if (preg_match($regex, $message, $matches)) {
                // Conversion en supposant dixièmes de °C
                $temperatureF = (float) $matches[1];  // ex : "077" => 7.7°C
                $temperatureC = ($temperatureF - 32) * 5 / 9; 
                $humidity    = (float) $matches[2];        // ex : "36" => 36% (si c’est bien le cas)
                $pressure    = (float) $matches[3];        // ex : "9993" => 9993 (à adapter si nécessaire)

                // Accélérations
                $accelString = str_replace(',', '.', trim($matches[4]));
                $accelParts  = preg_split('/\s+/', $accelString);

                $accelX = isset($accelParts[0]) ? (float)$accelParts[0] : 0.0;
                $accelY = isset($accelParts[1]) ? (float)$accelParts[1] : 0.0;
                $accelZ = isset($accelParts[2]) ? (float)$accelParts[2] : 0.0;

                $telemetryData[] = [
                    'date'        => $row['date_reception'],
                    'temperature' => $temperatureC,
                    'humidity'    => $humidity,
                    'pressure'    => $pressure,
                    'accelX'      => $accelX,
                    'accelY'      => $accelY,
                    'accelZ'      => $accelZ
                ];
            }
        }

        return $telemetryData;
    }
}

