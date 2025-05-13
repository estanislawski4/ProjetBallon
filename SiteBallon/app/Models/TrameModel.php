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
        // On enlève le filtre sur l'underscore, on prend directement les trames APLT00
        $sql = "SELECT message, date_reception
                FROM trames
                WHERE message LIKE '%t%h%b%'
                ORDER BY date_reception ASC";

        $stmt = $this->bdd->query($sql);
        $rows = $stmt->fetchAll(PDO::FETCH_ASSOC);

        $telemetryData = [];

        // Nouveau regex : on capture 3 chiffres (temp), 2 chiffres (hum), 5 chiffres (pression)
        $regex = '/t(\d{3})h(\d{2})b(\d{5})/';

        foreach ($rows as $row) {
            if (preg_match($regex, $row['message'], $m)) {
                $tempF = (int)$m[1];           // ex: "078" → 78
                $temperatureC = ($tempF - 32) * 5.0 / 9.0;

                $humidity = (int)$m[2];              // ex: "31" → 31 %

                $pressureRaw = (int)$m[3];           // ex: "10148"
                // Selon l’unité : 1014,8 hPa si on divise par 10
                $pressure  = $pressureRaw / 10;

                $telemetryData[] = [
                    'date'        => $row['date_reception'],
                    'temperature' => $temperatureC,
                    'humidity'    => $humidity,
                    'pressure'    => $pressure,
                    // si vous ajoutez un jour les accélérations, traitez-les ici
                    'accelX'      => null,
                    'accelY'      => null,
                    'accelZ'      => null
                ];
            }
        }

        return $telemetryData;
    }

    public function getCoordinates()
    {
        // Récupère uniquement les messages commençant par "!" (format des trames de localisation)
        $sql = "SELECT message FROM trames WHERE message LIKE '!%'";
        $stmt = $this->bdd->query($sql);
        $coordinates = [];

        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
            // Expression régulière pour extraire la latitude et la longitude
            $pattern = '/^!([0-9]{2})([0-9]{2}\.[0-9]+)([NS])\/([0-9]{3})([0-9]{2}\.[0-9]+)([EW])/';
            if (preg_match($pattern, $row['message'], $matches)) {
                // Traitement de la latitude
                $latDeg = (int)$matches[1];                     // Exemple : 47
                $latMin = (float)$matches[2];                   // Exemple : 59.73
                $latHem = $matches[3];                          // 'N' ou 'S'
                $latitude = $latDeg + ($latMin / 60);            // Conversion en degrés décimaux
                if ($latHem === 'S') {
                    $latitude = -$latitude;
                }
                
                // Traitement de la longitude
                $lonDeg = (int)$matches[4];                     // Exemple : 000
                $lonMin = (float)$matches[5];                   // Exemple : 12.26
                $lonHem = $matches[6];                          // 'E' ou 'W'
                $longitude = $lonDeg + ($lonMin / 60);           // Conversion en degrés décimaux
                if ($lonHem === 'W') {
                    $longitude = -$longitude;
                }
                
                $coordinates[] = ['latitude' => $latitude, 'longitude' => $longitude];
            }
        }
        return $coordinates;
    }
}

