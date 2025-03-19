<?php
namespace App\Controllers;

use App\Models\TrameModel;

class TramesController
{
    private $model;

    public function __construct(TrameModel $model)
    {
        $this->model = $model;
    }

    /**
     * Méthode index : récupère les données selon le type de graphique demandé.
     * Paramètre GET 'chart' peut prendre les valeurs : source, destination, day, top_messages, hourly.
     */
    public function index()
    {
        $chartType = isset($_GET['chart']) ? $_GET['chart'] : 'source';

        switch ($chartType) {
            case 'destination':
                $data = $this->model->getTramesCountByDestination();
                break;
            case 'day':
                $data = $this->model->getTramesCountByDay();
                break;
            case 'top_messages':
                $date_start = isset($_GET['date_start']) ? $_GET['date_start'] : null;
                $date_end   = isset($_GET['date_end'])   ? $_GET['date_end']   : null;
                $data = $this->model->getTopMessages($date_start, $date_end);
                break;
            case 'hourly':
                $data = $this->model->getTramesHourly();
                break;
            case 'historique':
                $date_start  = isset($_GET['date_start'])  ? $_GET['date_start']  : null;
                $date_end    = isset($_GET['date_end'])    ? $_GET['date_end']    : null;
                $source      = isset($_GET['source'])      ? $_GET['source']      : null;
                $destination = isset($_GET['destination']) ? $_GET['destination'] : null;
                $data = $this->model->getAllTrames($date_start, $date_end, $source, $destination);
                break;
            case 'source':
            default:
                $data = $this->model->getTramesCountBySource();
                break;
        }

        return [
            'view' => 'trames/index',
            'data' => $data,
            'chartType' => $chartType
        ];
    }
}

