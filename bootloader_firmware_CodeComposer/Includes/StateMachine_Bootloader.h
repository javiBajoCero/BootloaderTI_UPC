/*
 *  \brief StateMachine_Bootloader.h
 *  \details   Este *.h contiene las declaraciones de variables y funciones de StateMachine_Bootloader.c .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#ifndef INCLUDES_STATEMACHINE_BOOTLOADER_H_
#define INCLUDES_STATEMACHINE_BOOTLOADER_H_

#include "F2806x_Device.h"

void initBootloader(void);
void stateMachineBootloader(void);
void exitBootloader(void);
void errorHandlerBootloader(void);


#endif /* INCLUDES_STATEMACHINE_BOOTLOADER_H_ */
