<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Analyse des Trames</title>
    <script src="https://code.highcharts.com/highcharts.js"></script>
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
        #container {
            background: #fff;
            border: 1px solid #ddd;
            padding: 10px;
            border-radius: 5px;
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
        <a href="?chart=historique" class="<?= ($chartType=='historique') ? 'active' : '' ?>">Historique</a>
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
            <!-- Si on est sur un autre type de chart, on affiche Highcharts -->
            <div id="container" style="width:100%; height:500px;"></div>
            <script>
                // Récupération des données PHP en JSON
                var chartData = <?php echo json_encode($data); ?>;
                var chartType = "<?php echo $chartType; ?>";

                var categories = [];
                var seriesData = [];
                var chartOptions = {};

                // Préparation des données en fonction du type de graphique
                if(chartType === 'source' || chartType === 'destination') {
                    chartData.forEach(function(item) {
                        var label = (chartType === 'source') ? item.source : item.destination;
                        categories.push(label);
                        seriesData.push(parseInt(item.total));
                    });
                    chartOptions = {
                        chart: { type: 'column' },
                        title: {
                            text: 'Nombre de trames par ' +
                                (chartType === 'source'
                                    ? 'machine (source)'
                                    : 'machine (destination)')
                        },
                        xAxis: {
                            categories: categories,
                            title: {
                                text: (chartType === 'source') ? 'Source' : 'Destination'
                            }
                        },
                        yAxis: {
                            min: 0,
                            title: { text: 'Nombre de trames' }
                        },
                        series: [{ name: 'Trames', data: seriesData }]
                    };
                } else if(chartType === 'day') {
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
                } else if(chartType === 'top_messages') {
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
                } else if(chartType === 'hourly') {
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
                }

                // Création du graphique Highcharts si on n'est pas en "historique"
                Highcharts.chart('container', chartOptions);
            </script>
        <?php endif; ?>
    </div>

</body>
</html>

