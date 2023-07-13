/*
 *  \brief StateMachine_Bootloader.c
 *  \details   Este *.c agrupa todas las funcionalidades del Bootloader, todo el funcionamiento del bootloader se construye alrededor de la maquina de estados .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#include "F2806x_Device.h"
#include "Systick_Bootloader.h"
#include "StateMachine_Bootloader.h"
#include "Canbus_Bootloader.h"
#include "FLASHhandling_Bootloader.h"

//
STATEMACHINE_struct state_StateMachine={st_wait,0x00,0x00};

/**
* \brief Contiene todas las funciones de inicializaci�n.
* contains all clocks and peripheral initializations needed for the bootloader to work.
*
*/
void initBootloader(void){
    initSystick();
    initFLASHhandling();
    initCanbus();
}

/**
* \brief Contiene la l�gica principal del bootloader, se organiza en una maquina de estados.
*
*
*/
void stateMachineBootloader(void){

}

/**
* \brief Termina el proceso de bootloader, intenta saltar a la aplicaci�n.
*
*
*/
void exitBootloader(void){

}

/**
* \brief Gestiona distintos tipos de errores del proceso del bootloader.
*
*
*/
void errorHandlerBootloader(void){

}
