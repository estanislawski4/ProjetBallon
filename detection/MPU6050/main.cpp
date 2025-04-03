/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: dlancien
 *
 * Created on 6 mars 2025, 14:20
 */

#include <cstdlib>       // Pour la gestion des erreurs et exit()
#include <iostream>      // Pour l'affichage avec cout
#include <stdexcept>     // Pour la gestion des exceptions
#include <thread>        // Pour la gestion des threads et des délais
#include <iomanip>       // Pour la mise en forme des nombres affichés
#include <chrono>        // Pour la gestion des délais (sleep)
#include "MPU6050.h"     // Bibliothèque pour le capteur MPU6050
#include "GestionFile.h" // Classe pour la gestion des files de messages IPC

using namespace std;

// Déclaration des fonctions de rappel (callbacks) pour les interruptions du capteur
void rappel_FF(void);
void rappel_ZM(void);
// Objet global pour la gestion des files de messages IPC
GestionFile gestionFile;


int main(int argc, char** argv) {
    
    int16_t ox , oy , oz; // Variables pour stocker les offsets du capteur MPU6050
    
    try{
        // Initialisation du capteur MPU6050 à l'adresse I2C 0x69
        mpu.begin(0x69);
        // Calibration automatique du capteur pour améliorer la précision des mesures
        mpu.calibrateA();
        // Récupération des offsets après calibration
        mpu.getAccelOffset(ox , oy, oz);
        cout << "offset Acc: " << ox << " , " << oy << " , " << oz << "\r\n" << endl;
        sleep(5);
        
        mpu.onFreeFall(rappel_FF); // Register a user callback function
        mpu.onZeroMotion(rappel_ZM); // Register a user callback function
        mpu.setAccSensibility(MPU6050::FS_4G);
        mpu.enableFreeFall(0x80, 1); // seuil (FF très sensible) 0x80 durée 1 ms
        mpu.enableZeroMotion(0x05, 1); // seuil (FF très sensible) 0x80 durée 1 m

        gestionFile.obtenirFileIPC(5679);  // Obtenir la file pour l'émission key 5679
        string payload = "!4800.48N/00011.98E_/A=000268";
        cout << " : >APLT : ";
        cout << payload  << endl;


        cout << setfill('0') << fixed  << std::showpos;
        while(1){
            cout << setprecision(2) << "Accel:  ";
            cout << mpu.getAccelX() << "\t";
            cout << mpu.getAccelY() << "\t";
            cout << mpu.getAccelZ() << " g\t";
            cout << endl;
            sleep(1);
        }
        
    }
    catch (const runtime_error &e) {
        cout << "Exception caugth: " << e.what() << endl;
    }
    return 0;
    
}

void rappel_FF(void){
    cout << "Free fall detected" << endl;
    gestionFile.ecrireDansLaFileIPC(":f4kmn    :Free fall detected");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Attendre 1000 ms
}

void rappel_ZM(void){
    cout << "Zero motion detected" << endl;
    gestionFile.ecrireDansLaFileIPC(":f4kmn    :Zero Motion detected ");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Attendre 1000 ms
}
