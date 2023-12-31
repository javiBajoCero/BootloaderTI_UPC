/*
 *  \brief FLASHhandling_Bootloader.h
 *  \details   Este *.h contiene las declaraciones de variables y funciones de StateMachine_Bootloader.c .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#ifndef INCLUDES_FLASHHANDLING_BOOTLOADER_H_
#define INCLUDES_FLASHHANDLING_BOOTLOADER_H_


void initFLASHhandling(void);
void calculateCRCfromFLASHdata(void);
Uint16 EraseprogrammFlash(void);
Uint16 copyRAMbufferToFLASHprogrammMemory(void);

#endif /* INCLUDES_FLASHHANDLING_BOOTLOADER_H_ */
