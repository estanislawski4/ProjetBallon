/**************************************************************************
!
    \file     bme280.h
    \author   Philippe SIMIER (Touchard Wahington le Mans)
    \license  BSD (see license.txt)

    \brief    Classe pour le composant i2c  BME280
    \detail   Le BME280 est un capteur environnemental pour mesurer la température,
              la pression barométrique et l'humidité relative! Ce capteur est idéal
              pour réaliser une petite station météo. Il peut être connecté sur
              un bus I2C ou SPI! La broche CSB doit être connecté à VDDIO pour
              sélectionner l'interface I²C. Son adresse sur le bus est 0x77 ou 0x76
              en fonction du niveau de tension appliquée sur la broche SDO.

    \version    2.0 - Second release
 */



#ifndef BME280_H_INCLUDED
#define BME280_H_INCLUDED

#include <iostream>
#include <iomanip>
#include "i2c.h"
#include <stdint.h>       // pour les types uint8_t etc
#include <math.h>

/**
 * @def ADRESSE_I2C_BME280
 * @brief Adresse I2C par défaut du BME280.
 */
#define ADRESSE_I2C_BME280     0x77
//Register names:

/**
 * @def DIG_T1
 * @brief Registre pour la calibration de la température T1.
 */
#define DIG_T1        0x88

/**
 * @def DIG_T2
 * @brief Registre pour la calibration de la température T2.
 */
#define DIG_T2        0x8A

/**
 * @def DIG_T3
 * @brief Registre pour la calibration de la température T3.
 */
#define DIG_T3        0x8C

/**
 * @def DIG_P1
 * @brief Registre pour la calibration de la pression P1.
 */
#define DIG_P1        0x8E

/**
 * @def DIG_P2
 * @brief Registre pour la calibration de la pression P2.
 */
#define DIG_P2        0x90

/**
 * @def DIG_P3
 * @brief Registre pour la calibration de la pression P3.
 */
#define DIG_P3        0x92

/**
 * @def DIG_P4
 * @brief Registre pour la calibration de la pression P4.
 */
#define DIG_P4        0x94

/**
 * @def DIG_P5
 * @brief Registre pour la calibration de la pression P5.
 */
#define DIG_P5        0x96

/**
 * @def DIG_P6
 * @brief Registre pour la calibration de la pression P6.
 */
#define DIG_P6        0x98

/**
 * @def DIG_P7
 * @brief Registre pour la calibration de la pression P7.
 */
#define DIG_P7        0x9A

/**
 * @def DIG_P8
 * @brief Registre pour la calibration de la pression P8.
 */
#define DIG_P8        0x9C

/**
 * @def DIG_P9
 * @brief Registre pour la calibration de la pression P9.
 */
#define DIG_P9        0x9E

/**
 * @def DIG_H1
 * @brief Registre pour la calibration de l'humidité H1.
 */
#define DIG_H1        0xA1

/**
 * @def DIG_H2
 * @brief Registre pour la calibration de l'humidité H2.
 */
#define DIG_H2        0xE1

/**
 * @def DIG_H3
 * @brief Registre pour la calibration de l'humidité H3.
 */
#define DIG_H3        0xE3

/**
 * @def DIG_H4
 * @brief Registre pour la calibration de l'humidité H4.
 */
#define DIG_H4        0xE4

/**
 * @def DIG_H5
 * @brief Registre pour la calibration de l'humidité H5.
 */
#define DIG_H5        0xE5

/**
 * @def DIG_H6
 * @brief Registre pour la calibration de l'humidité H6.
 */
#define DIG_H6        0xE7


#define CHIPID        0xD0
#define VERSION       0xD1
#define SOFTRESET     0xE0
#define BME280_RESET                  0xB6
#define CAL26         0xE1
#define CONTROLHUMID  0xF2
#define CONTROL       0xF4
#define CONFIG        0xF5
#define PRESSUREDATA  0xF7
#define TEMPDATA      0xFA
#define HUMIDDATA     0xFD

/**
 * @def MEAN_SEA_LEVEL_PRESSURE
 * @brief Pression atmosphérique moyenne au niveau de la mer.
 */
#define MEAN_SEA_LEVEL_PRESSURE         1013


/**
 * @struct bme280_raw_data
 * @brief Structure contenant les données brutes du capteur BME280.
 *
 * Cette structure est utilisée pour stocker les données lues depuis les registres
 * du capteur, notamment la température, la pression et l'humidité.
 */
typedef struct {
    uint8_t pmsb; ///< MSB de la pression
    uint8_t plsb; ///< LSB de la pression
    uint8_t pxsb; ///< XSB de la pression

    uint8_t tmsb; ///< MSB de la température
    uint8_t tlsb; ///< LSB de la température
    uint8_t txsb; ///< XSB de la température

    uint8_t hmsb; ///< MSB de l'humidité
    uint8_t hlsb; ///< LSB de l'humidité

    uint32_t temperature; ///< Température brute
    uint32_t pressure;    ///< Pression brute
    uint32_t humidity;    ///< Humidité brute

} bme280_raw_data;


/**
 * @struct bme280_calib_data
 * @brief Structure contenant les constantes de calibration du capteur BME280.
 *
 * Cette structure est utilisée pour stocker les valeurs de calibration nécessaires
 * pour convertir les données brutes en valeurs exploitables (température, pression, humidité).
 */
typedef struct {
    uint16_t dig_T1; ///< Calibration pour la température T1
    int16_t dig_T2;  ///< Calibration pour la température T2
    int16_t dig_T3;  ///< Calibration pour la température T3

    uint16_t dig_P1; ///< Calibration pour la pression P1
    int16_t dig_P2;  ///< Calibration pour la pression P2
    int16_t dig_P3;  ///< Calibration pour la pression P3
    int16_t dig_P4;  ///< Calibration pour la pression P4
    int16_t dig_P5;  ///< Calibration pour la pression P5
    int16_t dig_P6;  ///< Calibration pour la pression P6
    int16_t dig_P7;  ///< Calibration pour la pression P7
    int16_t dig_P8;  ///< Calibration pour la pression P8
    int16_t dig_P9;  ///< Calibration pour la pression P9

    uint8_t dig_H1;  ///< Calibration pour l'humidité H1
    int16_t dig_H2;  ///< Calibration pour l'humidité H2
    uint8_t dig_H3;  ///< Calibration pour l'humidité H3
    int16_t dig_H4;  ///< Calibration pour l'humidité H4
    int16_t dig_H5;  ///< Calibration pour l'humidité H5
    int8_t dig_H6;   ///< Calibration pour l'humidité H6

    int32_t t_fine; ///< Valeur de calibration fine pour la température
} bme280_calib_data;

/**
 * @class BME280
 * @brief Classe pour interagir avec le capteur environnemental BME280.
 *
 * Cette classe permet de communiquer avec le capteur BME280 en utilisant le bus I2C
 * pour obtenir des mesures de température, de pression et d'humidité. Elle gère également
 * la calibration du capteur et le calcul de la pression au niveau de la mer.
 */
class BME280 {
public:
    /**
     * @brief Constructeur de la classe BME280.
     *
     * Initialise l'objet BME280 avec l'adresse I2C spécifiée (par défaut 0x77).
     * @param i2cAddress Adresse I2C du capteur BME280.
     */
    BME280(int i2cAddress = ADRESSE_I2C_BME280);

    /**
     * @brief Destructeur de la classe BME280.
     *
     * Libère les ressources associées au capteur.
     */
    ~BME280();


    /**
     * @brief Obtient l'ID du chip BME280.
     *
     * @return L'ID du chip (0x60 pour le BME280).
     */
    unsigned int obtenirChipID();

    /**
     * @brief Obtient la température en degrés Celsius.
     *
     * @return La température mesurée en °C.
     */
    double obtenirTemperatureEnC();

    /**
     * @brief Obtient la température en degrés Fahrenheit.
     *
     * @return La température mesurée en °F.
     */
    double obtenirTemperatureEnF();

    /**
     * @brief Obtient la pression atmosphérique en mbar.
     *
     * @return La pression mesurée en mbar.
     */
    double obtenirPression();

    /**
     * @brief Obtient l'humidité relative en pourcentage.
     *
     * @return L'humidité mesurée en %.
     */
    double obtenirHumidite();

    /**
     * @brief Définit l'altitude en mètres pour le calcul de la pression au niveau de la mer.
     *
     * @param h Altitude du capteur en mètres.
     */
    void donnerAltitude(double h);

    /**
     * @brief Obtient la pression au niveau de la mer.
     *
     * @return La pression corrigée pour le niveau de la mer en hPa.
     */
    double obtenirPression0();

    /**
     * @brief Obtient la valeur du point de rosée en degrés Celsius.
     *
     * @return Le point de rosée mesuré en °C.
     */
    double obtenirPointDeRosee();

    /**
     * @brief Affiche la version du firmware du BME280.
     */
    void version();


private:
    i2c *deviceI2C; ///< Objet pour la communication I2C avec le capteur
    bme280_calib_data cal; ///< Structure de calibration
    bme280_raw_data raw; ///< Structure des données brutes du capteur
    double h; ///< Différence d'altitude du capteur par rapport au niveau de la mer (en mètres)

    /**
     * @brief Lit les données de calibration du capteur.
     */
    void readCalibrationData();

    /**
     * @brief Lit les données brutes du capteur (température, pression, humidité).
     */
    void getRawData();
};

#endif // BME280_H_INCLUDED
