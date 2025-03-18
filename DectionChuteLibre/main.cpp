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

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "MPU6050.h"

using namespace std;


void rappel_FF(void);

int main(int argc, char** argv) {
    
    int16_t ox , oy , oz;
    
    try{
        
        mpu.begin(0x69);
        
        mpu.calibrateA();  // Calibration du capteur
        mpu.getAccelOffset(ox , oy, oz);
        cout << "offset Acc: " << ox << " , " << oy << " , " << oz << "\r\n" << endl;
        sleep(3);
        
        mpu.onFreeFall(rappel_FF); // Register a user callback function
        mpu.setAccSensibility(MPU6050::FS_4G);
        mpu.enableFreeFall(0x80, 1); // seuil (FF très sensible) 0x80 durée 1 ms
        
        cout << setfill('0') << fixed  << std::showpos;
        while(1){
            cout << setprecision(2) << "Accel:  " << mpu.getAccelX() << "\t";
            cout << mpu.getAccelY() << "\t" << mpu.getAccelZ() << " g\t";
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
    cout << "Chute libre" << endl;
}

