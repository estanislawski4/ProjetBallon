<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Analyse des Trames</title>

    <!-- 1. Highcharts Maps (moteur) -->
    <script src="https://code.highcharts.com/maps/highmaps.js"></script>

    <!-- (Modules facultatifs : exporting, data…) -->
    <!-- <script src="https://code.highcharts.com/modules/exporting.js"></script> -->

    <!-- Police Google -->
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Roboto&display=swap" rel="stylesheet">

    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{font-family:'Roboto',Arial,sans-serif;background:#f5f6fa;color:#333;padding:20px}
        h1{text-align:center;margin-bottom:30px}
        .menu{display:flex;justify-content:center;margin-bottom:30px}
        .menu a{margin:0 5px;text-decoration:none;padding:10px 15px;border-radius:5px;
                border:1px solid #ccc;background:#fff;color:#333;transition:.3s}
        .menu a:hover{background:#eee}
        .menu a.active{background:#333;color:#fff;font-weight:bold}
        .container{max-width:1200px;margin:0 auto}
        table{border-collapse:collapse;width:100%;margin-bottom:30px;background:#fff}
        thead{background:#333;color:#fff}
        th,td{padding:12px;border:1px solid #ddd;text-align:left}
        #container,#carte{background:#fff;border:1px solid #ddd;padding:10px;border-radius:5px;width:100%;height:500px}
    </style>
</head>
<body>

<h1>Analyse des Trames</h1>

<div class="menu">
    <a href="?chart=source"       class="<?=($chartType=='source')?'active':''?>">Source</a>
    <a href="?chart=destination"  class="<?=($chartType=='destination')?'active':''?>">Destination</a>
    <a href="?chart=day"          class="<?=($chartType=='day')?'active':''?>">Par jour</a>
    <a href="?chart=top_messages" class="<?=($chartType=='top_messages')?'active':''?>">Top messages</a>
    <a href="?chart=hourly"       class="<?=($chartType=='hourly')?'active':''?>">Évolution horaire</a>
    <a href="?chart=telemetry"    class="<?=($chartType=='telemetry')?'active':''?>">Télémétrie</a>
    <a href="?chart=historique"   class="<?=($chartType=='historique')?'active':''?>">Historique</a>
    <a href="?chart=carte"        class="<?=($chartType=='carte')?'active':''?>">Carte</a>
</div>

<div class="container">

<?php if ($chartType==='historique'): ?>
    <!-- Tableau d'historique -->
    <h2>Historique des trames</h2>
    <table>
        <thead><tr><th>Date</th><th>Source</th><th>Destination</th><th>Message</th></tr></thead>
        <tbody>
        <?php foreach($data as $row): ?>
            <tr>
                <td><?=htmlspecialchars($row['date_reception'])?></td>
                <td><?=htmlspecialchars($row['source'])?></td>
                <td><?=htmlspecialchars($row['destination'])?></td>
                <td><?=htmlspecialchars($row['message'])?></td>
            </tr>
        <?php endforeach; ?>
        </tbody>
    </table>

<?php else: /* ------------------- AUTRES GRAPHIQUES ------------------- */ ?>

    <?php if (in_array($chartType,['top_messages','historique'])): ?>
        <form method="get" style="margin-bottom:20px">
            <input type="hidden" name="chart" value="<?=htmlspecialchars($chartType)?>">
            <label>Date début:<input type="date" name="date_start" value="<?=isset($_GET['date_start'])?htmlspecialchars($_GET['date_start']):''?>"></label>
            <label>Date fin:<input type="date" name="date_end" value="<?=isset($_GET['date_end'])?htmlspecialchars($_GET['date_end']):''?>"></label>
            <?php if ($chartType==='historique'): ?>
                <label>Source:<input type="text" name="source" value="<?=isset($_GET['source'])?htmlspecialchars($_GET['source']):''?>"></label>
                <label>Destination:<input type="text" name="destination" value="<?=isset($_GET['destination'])?htmlspecialchars($_GET['destination']):''?>"></label>
            <?php endif; ?>
            <button type="submit">Filtrer</button>
        </form>
    <?php endif; ?>

    <?php if ($chartType==='carte'): ?>
        <div id="carte"></div>
    <?php else: ?>
        <div id="container"></div>
    <?php endif; ?>


    <script>
    /* ---------------- Données PHP → JS ---------------- */
    const chartData = <?=json_encode($data)?>;
    const chartType = "<?= $chartType ?>";

    /* -------------- GRAPHIQUES CLASSIQUES ------------- */
    if (chartType !== 'carte') {

        let categories = [], seriesData = [], chartOptions = {};

        if (chartType === 'source' || chartType === 'destination') {
            chartData.forEach(it=>{
                categories.push(chartType==='source'?it.source:it.destination);
                seriesData.push(+it.total);
            });
            chartOptions = {
                chart:{type:'column'},
                title:{text:'Nombre de trames par '+(chartType==='source'?'machine (source)':'machine (destination)')},
                xAxis:{categories,title:{text:chartType==='source'?'Source':'Destination'}},
                yAxis:{min:0,title:{text:'Nombre de trames'}},
                series:[{name:'Trames',data:seriesData}]
            };

        } else if (chartType === 'day') {
            chartData.forEach(it=>{categories.push(it.day);seriesData.push(+it.total);});
            chartOptions = {
                chart:{type:'line'},
                title:{text:'Nombre de trames par jour'},
                xAxis:{categories,title:{text:'Jour'}},
                yAxis:{min:0,title:{text:'Nombre de trames'}},
                series:[{name:'Trames',data:seriesData}]
            };

        } else if (chartType === 'top_messages') {
            const pieData = chartData.map(it=>({name:it.message||'Non défini',y:+it.total}));
            chartOptions = {
                chart:{type:'pie'},
                title:{text:'Top 10 messages'},
                series:[{name:'Nombre de trames',data:pieData}]
            };

        } else if (chartType === 'hourly') {
            chartData.forEach(it=>{categories.push(it.hour);seriesData.push(+it.total);});
            chartOptions = {
                chart:{type:'spline'},
                title:{text:'Évolution horaire des trames'},
                xAxis:{categories,title:{text:'Heure'}},
                yAxis:{min:0,title:{text:'Nombre de trames'}},
                series:[{name:'Trames',data:seriesData}]
            };

        } else if (chartType === 'telemetry') {
            const temps=[],hums=[],press=[];
            chartData.forEach(it=>{
                categories.push(it.date);
                temps.push(+it.temperature);
                hums.push(+it.humidity);
                press.push(+it.pressure);
            });
            chartOptions = {
                chart:{type:'line'},
                title:{text:'Télémétrie : Température, Humidité & Pression'},
                xAxis:{categories,title:{text:'Date'},labels:{rotation:-45}},
                yAxis:[{min:0,title:{text:'Valeurs T/H/P'}}],
                series:[
                    {name:'Température',data:temps},
                    {name:'Humidité',   data:hums},
                    {name:'Pression',   data:press}
                ]
            };
        }

        Highcharts.chart('container', chartOptions);
    }


    /* --------------------- CARTE FRANCE --------------------- */
    if (chartType === 'carte') (async () => {

        // 1) Topologie France
        const topology = await fetch(
            'https://code.highcharts.com/mapdata/countries/fr/fr-all.topo.json'
        ).then(r => r.json());

        // 2) Conversion de vos lignes en points mappoint
        const points = chartData.map(it => ({
            name : it.message || 'Trame',
            lat  : +it.latitude,
            lon  : +it.longitude
        }));

        // 3) Carte
        Highcharts.mapChart('carte', {
            chart: { map: topology },
            title:  { text: 'Localisation des trames' },
            subtitle:{ text: 'Chaque point correspond à une trame GPS' },

            mapNavigation:{
                enabled:true,
                buttonOptions:{verticalAlign:'bottom'}
            },

            tooltip:{
                headerFormat:'',
                pointFormat:'<b>{point.name}</b><br>Lat&nbsp;: {point.lat:.4f}, Lon&nbsp;: {point.lon:.4f}'
            },

            series:[{
                name:'France',
                borderColor:'#A0A0A0',
                nullColor:'rgba(200,200,200,0.3)',
                showInLegend:false
            },{
                name:'Trames',
                type:'mappoint',
                color:Highcharts.getOptions().colors[3],
                marker:{radius:6},
                data:points
            }]
        });

    })();
    </script>

<?php endif; ?>
</div>

</body>
</html>

