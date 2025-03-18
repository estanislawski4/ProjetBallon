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
                $data = $this->model->getTopMessages();
                break;
            case 'hourly':
                $data = $this->model->getTramesHourly();
                break;
            case 'historique':
                $data = $this->model->getAllTrames();
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

