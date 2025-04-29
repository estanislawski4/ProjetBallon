<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Analyse des Trames</title>

    <!-- 1. Charger Highmaps, qui inclut Highcharts + Maps -->
    <script src="https://code.highcharts.com/maps/highmaps.js"></script>
    
    <!-- 2. Charger éventuellement les modules export, data, etc. (facultatif) -->
    <!-- <script src="https://code.highcharts.com/modules/exporting.js"></script> -->
    <!-- <script src="https://code.highcharts.com/modules/data.js"></script> -->
    
    <!-- 3. Charger la carte du monde -->
    <script src="https://code.highcharts.com/mapdata/custom/world.js"></script>

    <!-- Exemple : ajout d’une police Google Fonts -->
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Roboto&display=swap" rel="stylesheet">

    <style>
        /* Réinitialisation rapide */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Roboto', Arial, sans-serif;
            background: #f5f6fa; /* Couleur de fond légèrement grisée */
            color: #333;
            padding: 20px;
        }
        h1 {
            text-align: center;
            margin-bottom: 30px;
        }
        /* Barre de menu */
        .menu {
            display: flex;
            justify-content: center;
            margin-bottom: 30px;
        }
        .menu a {
            margin: 0 5px;
            text-decoration: none;
            padding: 10px 15px;
            border-radius: 5px;
            border: 1px solid #ccc;
            background: #fff;
            color: #333;
            transition: background 0.3s, color 0.3s;
        }
        .menu a:hover {
            background: #eee;
        }
        .menu a.active {
            background: #333;
            color: #fff;
            font-weight: bold;
        }
        /* Conteneur principal */
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        /* Tableau d’historique */
        table {
            border-collapse: collapse;
            width: 100%;
            margin-bottom: 30px;
            background: #fff;
        }
        thead {
            background: #333;
            color: #fff;
        }
        th, td {
            padding: 12px;
            border: 1px solid #ddd;
            text-align: left;
        }
        /* Conteneur du graphique */
        #container, #carte {
            background: #fff;
            border: 1px solid #ddd;
            padding: 10px;
            border-radius: 5px;
            width: 100%;
            height: 500px;
        }
    </style>
</head>
<body>

    <h1>Analyse des Trames</h1>

    <div class="menu">
        <!-- Liens pour changer de graphique -->
        <a href="?chart=source" class="<?= ($chartType=='source') ? 'active' : '' ?>">Source</a>
        <a href="?chart=destination" class="<?= ($chartType=='destination') ? 'active' : '' ?>">Destination</a>
        <a href="?chart=day" class="<?= ($chartType=='day') ? 'active' : '' ?>">Par Jour</a>
        <a href="?chart=top_messages" class="<?= ($chartType=='top_messages') ? 'active' : '' ?>">Top Messages</a>
        <a href="?chart=hourly" class="<?= ($chartType=='hourly') ? 'active' : '' ?>">Évolution Horaire</a>
        <a href="?chart=telemetry" class="<?= ($chartType=='telemetry') ? 'active' : '' ?>">Télémétrie</a>
        <a href="?chart=historique" class="<?= ($chartType=='historique') ? 'active' : '' ?>">Historique</a>
        <a href="?chart=carte" class="<?= ($chartType=='carte') ? 'active' : '' ?>">Carte</a>
    </div>

    <div class="container">

        <?php if ($chartType === 'historique'): ?>
            <!-- Affichage du tableau d'historique -->
            <h2>Historique des trames</h2>
            <table>
                <thead>
                    <tr>
                        <th>Date</th>
                        <th>Source</th>
                        <th>Destination</th>
                        <th>Message</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach ($data as $row): ?>
                        <tr>
                            <td><?= htmlspecialchars($row['date_reception']) ?></td>
                            <td><?= htmlspecialchars($row['source']) ?></td>
                            <td><?= htmlspecialchars($row['destination']) ?></td>
                            <td><?= htmlspecialchars($row['message']) ?></td>
                        </tr>
                    <?php endforeach; ?>
                </tbody>
            </table>
        <?php else: ?>
            <?php if ($chartType === 'top_messages' || $chartType === 'historique'): ?>
                <form method="get" style="margin-bottom: 20px;">
                    <input type="hidden" name="chart" value="<?= htmlspecialchars($chartType) ?>">
                    <label for="date_start">Date début:</label>
                    <input type="date" name="date_start" id="date_start" value="<?= isset($_GET['date_start']) ? htmlspecialchars($_GET['date_start']) : '' ?>">
                    <label for="date_end">Date fin:</label>
                    <input type="date" name="date_end" id="date_end" value="<?= isset($_GET['date_end']) ? htmlspecialchars($_GET['date_end']) : '' ?>">

                    <?php if ($chartType === 'historique'): ?>
                        <label for="source">Source:</label>
                        <input type="text" name="source" id="source" value="<?= isset($_GET['source']) ? htmlspecialchars($_GET['source']) : '' ?>">
                        <label for="destination">Destination:</label>
                        <input type="text" name="destination" id="destination" value="<?= isset($_GET['destination']) ? htmlspecialchars($_GET['destination']) : '' ?>">
                    <?php endif; ?>

                    <button type="submit">Filtrer</button>
                </form>
            <?php endif; ?>

            <?php if ($chartType === 'carte'): ?>
                <!-- Conteneur dédié pour la carte (Highmaps) -->
                <div id="carte"></div>
            <?php else: ?>
                <!-- Conteneur pour les autres graphiques (Highcharts) -->
                <div id="container"></div>
            <?php endif; ?>

            <script>
                // Récupération des données PHP en JSON
                var chartData = <?php echo json_encode($data); ?>;
                var chartType = "<?php echo $chartType; ?>";

                var categories = [];
                var seriesData = [];
                var chartOptions = {};

                if (chartType === 'source' || chartType === 'destination') {
                    chartData.forEach(function(item) {
                        var label = (chartType === 'source') ? item.source : item.destination;
                        categories.push(label);
                        seriesData.push(parseInt(item.total));
                    });
                    chartOptions = {
                        chart: { type: 'column' },
                        title: {
                            text: 'Nombre de trames par ' + (chartType === 'source' ? 'machine (source)' : 'machine (destination)')
                        },
                        xAxis: {
                            categories: categories,
                            title: { text: (chartType === 'source') ? 'Source' : 'Destination' }
                        },
                        yAxis: {
                            min: 0,
                            title: { text: 'Nombre de trames' }
                        },
                        series: [{ name: 'Trames', data: seriesData }]
                    };
                } else if (chartType === 'day') {
                    chartData.forEach(function(item) {
                        categories.push(item.day);
                        seriesData.push(parseInt(item.total));
                    });
                    chartOptions = {
                        chart: { type: 'line' },
                        title: { text: 'Nombre de trames par jour' },
                        xAxis: { categories: categories, title: { text: 'Jour' } },
                        yAxis: { min: 0, title: { text: 'Nombre de trames' } },
                        series: [{ name: 'Trames', data: seriesData }]
                    };
                } else if (chartType === 'top_messages') {
                    var pieData = [];
                    chartData.forEach(function(item) {
                        var label = item.message || "Non défini";
                        pieData.push({ name: label, y: parseInt(item.total) });
                    });
                    chartOptions = {
                        chart: { type: 'pie' },
                        title: { text: 'Top 10 messages' },
                        series: [{
                            name: 'Nombre de trames',
                            data: pieData
                        }]
                    };
                } else if (chartType === 'hourly') {
                    chartData.forEach(function(item) {
                        categories.push(item.hour);
                        seriesData.push(parseInt(item.total));
                    });
                    chartOptions = {
                        chart: { type: 'spline' },
                        title: { text: 'Évolution horaire des trames' },
                        xAxis: { categories: categories, title: { text: 'Heure' } },
                        yAxis: { min: 0, title: { text: 'Nombre de trames' } },
                        series: [{ name: 'Trames', data: seriesData }]
                    };
                } else if (chartType === 'telemetry') {
                    var temps = [], hums = [], press = [], ax = [], ay = [], az = [];
                    chartData.forEach(function(item) {
                        categories.push(item.date);
                        temps.push(parseFloat(item.temperature));
                        hums.push(parseFloat(item.humidity));
                        press.push(parseFloat(item.pressure));
                        ax.push(parseFloat(item.accelX));
                        ay.push(parseFloat(item.accelY));
                        az.push(parseFloat(item.accelZ));
                    });
                    chartOptions = {
                        chart: { type: 'line' },
                        title: { text: 'Télémétrie: Température, Humidité & Pression' },
                        xAxis: {
                            categories: categories,
                            title: { text: 'Date' },
                            labels: { rotation: -45 }
                        },
                        yAxis: [{
                            min: 0,
                            title: { text: 'Valeurs T/H/P' }
                        }, {
                            title: { text: 'Accélération (m/s²)' },
                            opposite: true
                        }],
                        series: [
                            { name: 'Température', data: temps, yAxis: 0 },
                            { name: 'Humidité', data: hums, yAxis: 0 },
                            { name: 'Pression', data: press, yAxis: 0 }
                        ]
                    };
                } else if (chartType === 'carte') {
                    // Pour la carte, on s'attend à recevoir des objets { latitude, longitude }
                    var dataPoints = chartData.map(function(item) {
                        return {
                            lat: parseFloat(item.latitude),
                            lon: parseFloat(item.longitude)
                        };
                    });
                    console.log(dataPoints); // Debug : vérifier le contenu

                    chartOptions = {
                        chart: { map: 'custom/world' },
                        title: { text: 'Localisation des Trames' },
                        mapNavigation: {
                            enabled: true,
                            buttonOptions: { verticalAlign: 'bottom' }
                        },
                        series: [{
                            name: 'Basemap',
                            borderColor: '#A0A0A0',
                            nullColor: 'rgba(200, 200, 200, 0.3)',
                            showInLegend: false
                        }, {
                            name: 'Trames',
                            type: 'mappoint',
                            data: dataPoints,
                            marker: { radius: 5 }
                        }]
                    };
                }

                // Création du graphique en fonction du type
                if (chartType === 'carte') {
                    // Crée la carte dans #carte
                    Highcharts.mapChart('carte', chartOptions);
                } else {
                    // Crée les autres graphiques dans #container
                    Highcharts.chart('container', chartOptions);
                }
            </script>
        <?php endif; ?>
    </div>

</body>
</html>

