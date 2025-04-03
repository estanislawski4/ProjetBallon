
#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <iostream>



// I2C definitions

/**
 * @def I2C_SLAVE
 * @brief Définit l'adresse de l'esclave à utiliser pour la communication I2C.
 */
#define I2C_SLAVE       0x0703  /* Utilise cette adresse esclave */

/**
 * @def I2C_ACK_TEST
 * @brief Teste si un esclave est présent à une adresse spécifique.
 */
#define I2C_ACK_TEST    0x0710  /* Voir si un esclave est à une adresse spécifique */

/**
 * @def I2C_SMBUS
 * @brief Définit la commande pour effectuer un transfert SMBus.
 */
#define I2C_SMBUS       0x0720  /* Transfert SMBus */

/**
 * @def I2C_SMBUS_READ
 * @brief Indique une opération de lecture dans SMBus.
 */
#define I2C_SMBUS_READ  1

/**
 * @def I2C_SMBUS_WRITE
 * @brief Indique une opération d'écriture dans SMBus.
 */
#define I2C_SMBUS_WRITE 0

// Types de transactions SMBus

/**
 * @def I2C_SMBUS_QUICK
 * @brief Envoie un bit à l'appareil, à la place du bit Rd/Wr.
 */
#define I2C_SMBUS_QUICK             0  // Envoie un seul bit à l'appareil


/**
 * @def I2C_SMBUS_BYTE
 * @brief Permet de lire et écrire un octet dans le SMBus.
 */
#define I2C_SMBUS_BYTE              1  // Lecture et écriture d'un octet


/**
 * @def I2C_SMBUS_BYTE_DATA
 * @brief Permet de lire et écrire un octet de données.
 */
#define I2C_SMBUS_BYTE_DATA         2  // Lecture et écriture de données (octet)


/**
 * @def I2C_SMBUS_WORD_DATA
 * @brief Permet de lire et écrire un mot de données.
 */
#define I2C_SMBUS_WORD_DATA         3  // Lecture et écriture d'un mot de données


/**
 * @def I2C_SMBUS_PROC_CALL
 * @brief Permet de sélectionner un registre de périphérique, d'envoyer 16 bits de données et de lire 16 bits de données en retour.
 */
#define I2C_SMBUS_PROC_CALL         4  // Sélectionne un registre et échange des données

/**
 * @def I2C_SMBUS_BLOCK_DATA
 * @brief Permet de lire et écrire un bloc de données.
 */
#define I2C_SMBUS_BLOCK_DATA        5  // Lecture et écriture d'un bloc de données


/**
 * @def I2C_SMBUS_I2C_BLOCK_BROKEN
 * @brief Transaction SMBus I2C bloquée (non spécifiée dans le standard SMBus).
 */
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6

/**
 * @def I2C_SMBUS_BLOCK_PROC_CALL
 * @brief Sélectionne un registre, envoie entre 1 et 31 octets de données et lit entre 1 et 31 octets de données en retour.
 */
#define I2C_SMBUS_BLOCK_PROC_CALL   7

/**
 * @def I2C_SMBUS_I2C_BLOCK_DATA
 * @brief Permet de lire et écrire des données dans un bloc I2C.
 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// Messages SMBus

/**
 * @def I2C_SMBUS_BLOCK_MAX
 * @brief Taille maximale d'un bloc de données dans SMBus.
 */
#define I2C_SMBUS_BLOCK_MAX     32      /* taille maxi d'un bloc de données */

/**
 * @def I2C_SMBUS_I2C_BLOCK_MAX
 * @brief Taille maximale d'un bloc de données dans SMBus pour I2C.
 */
#define I2C_SMBUS_I2C_BLOCK_MAX 32      /* Non spécifié, mais utilise la même structure */

// Structures utilisées par les appels ioctl()

using namespace std;

/**
 * @union i2c_smbus_data
 * @brief Union permettant de stocker les données pour une transaction SMBus.
 *
 * Cette union peut contenir un octet, un mot ou un tableau d'octets, en fonction du type de transaction.
 */
union i2c_smbus_data
{
    uint8_t  byte ; ///< Données sous forme d'octet
    uint16_t word ; ///< Données sous forme de mot
    uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;    ///< Données sous forme de bloc (taille ajustée pour CRC)
};

/**
 * @struct i2c_smbus_ioctl_data
 * @brief Structure utilisée pour les opérations SMBus via ioctl().
 *
 * Cette structure est passée lors de l'appel à `ioctl()` pour effectuer des transactions SMBus.
 */
struct i2c_smbus_ioctl_data
{
    char read_write ; ///< Indique si l'opération est de lecture ou d'écriture
    uint8_t command ; ///< Commande SMBus à envoyer
    int size ;        ///< Taille des données à lire/écrire
    union i2c_smbus_data *data ; ///< Données à envoyer ou à recevoir
};

/**
 * @class i2c
 * @brief Classe permettant la gestion des communications I2C avec un périphérique.
 *
 * Cette classe permet de lire et d'écrire des données avec un périphérique via le bus I2C.
 * Elle prend en charge les transactions SMBus et fournit des méthodes pour interagir
 * avec un esclave I2C.
 */
class i2c
{

public:

    /**
     * @brief Constructeur de la classe i2c.
     *
     * Initialisation de l'objet i2c avec l'adresse de l'esclave et le bus I2C spécifié.
     * @param adresseI2C Adresse de l'esclave I2C.
     * @param idBusI2C Identifiant du bus I2C (0 pour Raspberry Pi version 1, 1 pour les versions 2 et 3).
     */
    i2c(int adresseI2C, int idBusI2C=1);
    //idBusI2C = 0 pour les raspberry version 1
    //idBusI2C = 1 pour les raspberry version 2 et 3

    /**
     * @brief Vérifie s'il y a une erreur dans la communication.
     *
     * @return True si une erreur est présente, sinon False.
     */
    bool getError();

    /**
     * @brief Lit un octet depuis l'esclave I2C.
     *
     * @return L'octet lu.
     */
    unsigned char Read ();

    /**
     * @brief Lit un octet à partir d'un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre à lire.
     * @return L'octet lu depuis le registre.
     */
    unsigned char ReadReg8 (int reg);

    /**
     * @brief Lit un mot à partir d'un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre à lire.
     * @return Le mot lu depuis le registre.
     */
    unsigned short ReadReg16 (int reg);

    /**
     * @brief Lit un bloc de données depuis un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre à partir duquel lire.
     * @param length La longueur des données à lire.
     * @param values Tableau dans lequel les valeurs lues seront stockées.
     * @return Le nombre d'octets lus.
     */
    int ReadBlockData (int reg, int length, int *values);

    /**
     * @brief Écrit un octet dans l'esclave I2C.
     *
     * @param data L'octet à écrire.
     * @return Le résultat de l'écriture.
     */
    unsigned char Write (int data);

    /**
     * @brief Écrit un octet dans un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre dans lequel écrire.
     * @param value La valeur à écrire dans le registre.
     * @return Le résultat de l'écriture.
     */
    unsigned char WriteReg8 (int reg, int value);

    /**
     * @brief Écrit un mot dans un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre dans lequel écrire.
     * @param value La valeur à écrire dans le registre.
     * @return Le résultat de l'écriture.
     */
    unsigned short WriteReg16 (int reg, int value);

    /**
     * @brief Écrit un bloc de données dans un registre spécifique de l'esclave I2C.
     *
     * @param reg Le registre dans lequel écrire.
     * @param length La longueur des données à écrire.
     * @param data Les données à écrire.
     * @return Le nombre d'octets écrits.
     */
    int WriteBlockData (int reg, int length, int *data);

    /**
     * @brief Effectue une pause de la durée spécifiée en millisecondes.
     *
     * @param num_ms Le nombre de millisecondes à attendre.
     * @return 0 si l'attente a été effectuée avec succès.
     */
    int delay_ms(unsigned long num_ms);

private:

    int fd; ///< Descripteur de fichier pour la communication I2C
    bool error; ///< Indicateur d'erreur de communication

    /**
     * @brief Effectue une transaction SMBus.
     *
     * Cette méthode gère l'accès à l'esclave I2C en fonction de l'opération de lecture/écriture et du type de données.
     * @param rw Indique l'opération (lecture ou écriture).
     * @param command La commande SMBus à exécuter.
     * @param size La taille des données à lire/écrire.
     * @param data Les données à envoyer ou à recevoir.
     * @return Le code de résultat de l'opération.
     */
    inline int i2c_smbus_access (char rw, uint8_t command, int size, union i2c_smbus_data *data);

};

#endif // I2C_H_INCLUDED
