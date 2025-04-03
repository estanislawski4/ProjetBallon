/**
 * @file     i2c.cpp
 * @author   Philippe SIMIER (Touchard Washington Le Mans)
 * @license  BSD (see license.txt)
 * @brief    Implémentation de la classe i2c pour la communication sur le bus I2C.
 *
 * Cette classe permet d'effectuer des opérations de lecture et d'écriture sur un bus I2C,
 * en utilisant les fonctionnalités du protocole SMBus.
 *
 * @version v1.0 - Première version
 */


#include "i2c.h"

/**
 * @brief Constructeur de la classe i2c.
 *
 * Initialise la communication sur le bus I2C en ouvrant le fichier correspondant et en
 * configurant l'adresse du périphérique cible.
 *
 * @param adresseI2C Adresse I2C du périphérique.
 * @param idBusI2C Numéro du bus I2C utilisé.
 */
i2c::i2c(int adresseI2C, int idBusI2C){
    char filename[20];
    union i2c_smbus_data data ;

    snprintf(filename, 19, "/dev/i2c-%d", idBusI2C);
    if ((fd = open (filename, O_RDWR)) < 0){
        cout << "Erreur d'ouverture du bus I2C" << endl;
        exit(1);
    }
    //  Change slave address. The address is passed in the
    //  7 lower bits of the  argument
    if (ioctl (fd, I2C_SLAVE, adresseI2C) < 0){
        cout << "Impossible de sélectionner l'adresse I2C " << adresseI2C << endl ;
        exit(1);
    }
    // test de la présence du composant sur le bus
    if (i2c_smbus_access (I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data)){
        error = true ;
    }
    else{
        error = false ;
    }
}

/**
 * @brief Vérifie si une erreur est survenue sur le bus I2C.
 * @return true si une erreur est détectée, false sinon.
 */
bool i2c::getError(){
    return error;
}

/**
 * @brief Effectue une transaction SMBus sur le bus I2C.
 * @param rw Opération de lecture ou d'écriture.
 * @param command Commande à envoyer.
 * @param size Taille de la transaction.
 * @param data Données à envoyer ou recevoir.
 * @return Code de retour de l'opération ioctl.
 */
int i2c::i2c_smbus_access (char rw, uint8_t command, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args ;

    args.read_write = rw ;
    args.command    = command ;
    args.size       = size ;
    args.data       = data ;
    return ioctl (fd, I2C_SMBUS, &args) ;
}

/**
 * @brief Lit un octet depuis le bus I2C.
 * @return Valeur lue.
 */
unsigned char i2c::Read(){
    union i2c_smbus_data data ;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access (I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
        error = true ;
    else
        valeur = data.byte & 0xFF ;
    return valeur;
}

/**
 * @brief Lit un registre 8 bits.
 * @param reg Adresse du registre.
 * @return Valeur du registre.
 */
unsigned char i2c::ReadReg8 (int reg){
    union i2c_smbus_data data;
    unsigned char valeur = 0x00;
    if (i2c_smbus_access (I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data))
        error = true ;
    else
        valeur = data.byte & 0xFF ;
    return valeur;
}

/**
 * @brief Lit un registre 16 bits.
 * @param reg Adresse du registre.
 * @return Valeur du registre.
 */
unsigned short i2c::ReadReg16 (int reg){
    union i2c_smbus_data data;
    unsigned short valeur = 0x0000;
    if (i2c_smbus_access (I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data))
        error = true ;
    else
        valeur = data.word & 0xFFFF ;
    return valeur;
}

/**
 * @brief Écrit un octet sur le bus I2C.
 * @param data Octet à écrire.
 * @return Code de retour de l'opération.
 */
unsigned char i2c::Write (int data){
    return i2c_smbus_access (I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL) ;

}

/**
 * @brief Écrit un registre 8 bits.
 * @param reg Adresse du registre.
 * @param value Valeur à écrire.
 * @return Code de retour de l'opération.
 */
unsigned char i2c::WriteReg8 (int reg, int value){
    union i2c_smbus_data data ;

    data.byte = value ;
    return i2c_smbus_access (I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) ;
}

/**
 * @brief Écrit un registre 16 bits.
 * @param reg Adresse du registre.
 * @param value Valeur à écrire.
 * @return Code de retour de l'opération.
 */
unsigned short i2c::WriteReg16 (int reg, int value){
    union i2c_smbus_data data ;

    data.word = value ;
    return i2c_smbus_access (I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data) ;
}

/**
 * @brief Lit un bloc de données depuis un registre I2C.
 * @param reg Adresse du registre.
 * @param length Nombre maximal d'octets à lire (max 32).
 * @param values Tableau pour stocker les valeurs lues.
 * @return Nombre d'octets effectivement lus, ou -1 en cas d'erreur.
 */
int i2c::WriteBlockData (int reg, int length, int *values){
    union i2c_smbus_data data ;
    int i;
    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;
    return i2c_smbus_access (I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_BROKEN , &data) ;
}

int i2c::ReadBlockData (int reg, int length, int *values){
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    data.block[0] = length;
    if (i2c_smbus_access(I2C_SMBUS_READ, reg, length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
                                                  I2C_SMBUS_I2C_BLOCK_DATA,&data)){
        error = true;
        return -1;
    }
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}

/**
 * @brief Attend un certain nombre de millisecondes.
 * @param num_ms Durée en millisecondes.
 * @return Code de retour de nanosleep.
 */

int i2c::delay_ms(unsigned long num_ms)
{
    struct timespec ts;

    ts.tv_sec = num_ms / 1000;
    ts.tv_nsec = (num_ms % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}
