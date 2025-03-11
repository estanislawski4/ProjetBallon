#include "telemetrie.h"

Telemetrie::Telemetrie():
bme(ADDRESS_BME280)
{

    localeAvecVirgule = locale(std::locale(), new VirguleDecimal);
}

string Telemetrie::CreationTrameAPRS()
{
    ObtenirTemperature();
    ObtenirHumidite();
    ObtenirPression();

    VerifierMesures();
    int t = static_cast<int>(( temperature* 9.0 / 5.0) + 32.0);
    int h = static_cast<int>(humidite + 0.5);
    int p = static_cast<int>((pression + 0.5) * 10);

    std::ostringstream out;
    out.imbue(localeAvecVirgule);
    out << setfill('0') << fixed;
    out << "_" << gestionTemps.getDateAprs();
    out << "c...s...g...t" << std::setw(3) << t;
    out << "h" << std::setw(2) << h;
    out << "b" << std::setw(4) << p;
    out << showpos;
    return out.str();
}

void Telemetrie::ObtenirTemperature()
{
    temperature = bme.obtenirTemperatureEnC();
    std::cout << "T = " << temperature << std::endl;
}

void Telemetrie::ObtenirPression()
{
    pression = bme.obtenirPression();
    std::cout << "P = " << pression << std::endl;
}

void Telemetrie::ObtenirHumidite()
{
    humidite = bme.obtenirHumidite();
    std::cout << "H = " << humidite << std::endl;
}

bool Telemetrie::VerifierMesures()
{
    bool valide = true; //Variable qui permet de verifier si les différentes valeurs télémétrique sont correctes


    if(temperature <= VAL_MIN_TEMPERATURE || temperature >= VAL_MAX_TEMPERATURE)
    {
        valide = false;

        temperature = NAN;
    }


    if(pression <= VAL_MIN_PRESSION || pression >= VAL_MAX_PRESSION)
    {
        valide = false;
        pression = NAN;
    }

    if(humidite <= VAL_MIN_HUMIDITE || humidite >= VAL_MAX_HUMIDITE)
    {
        valide = false;
        humidite = NAN;
    }
    return valide;
}
