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

//timestamps for Tick timeouts
const Uint32 twoAndAhalfSeconds= 2500; //seconds
Uint32 timestamp=0;


/**
* \brief Escucha CANBUs durante 2,5segundos
* si recibe
*
*/
void initBootloader(void){
    timestamp=getTick();
    while(getTick()-timestamp<twoAndAhalfSeconds){//listen for incomming valid CANBUS frames for 2,5 seconds
//        if(){//if valid CANBUS ID and data, , the state machine should expect to receive some data.
//            state_StateMachine.step=st_wait;
//            break;
//        }

    }

    //if we reached this point 2,5seconds passed with no valid CANBUS, the state machine should jump straight to app.
    state_StateMachine.step=st_go;
}

/**
* \brief Contiene la lógica principal del bootloader, se organiza en una maquina de estados.
*
*
*/
void stateMachineBootloader(void){

    switch (state_StateMachine.step) {
        case st_wait:

            break;
        case st_erase:

            break;
        case st_write:

            break;
        case st_go:

            break;
        case st_error:

            break;
        default:
            break;
    }


}

/**
* \brief Termina el proceso de bootloader, intenta saltar a la aplicación.
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
