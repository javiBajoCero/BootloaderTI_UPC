/*
 *  \brief FLASHhandling_Bootloader.c
 *  \details   Este *.c contiene todo lo referente a la inicialización y borrado/grabado de la memoria interna FLASH.
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */
#include "F2806x_Device.h"
#include "F2806x_GlobalPrototypes.h"
#include "Flash2806x_API_Library.h"

#include "FLASHhandling_Bootloader.h"


const Uint16 programmFlashSectorMask=(SECTORC|SECTORD|SECTORE|SECTORF|SECTORG|SECTORH);
const Uint16 * SECTORCstartAddr= (Uint16 *)0x3E0000;


//some info about flash programming https://e2e.ti.com/cfs-file/__key/communityserver-discussions-components-files/171/Flash-Programming-F2812.pdf
/**
* \brief funcion de inicialización del FLASH memory handler.
* contains función de inicialización de estructuras y variables para la programación de la memoria FLASH.
*
*/
void initFLASHhandling(void){

    //  *IMPORTANT*
    //  IF RUNNING FROM FLASH, PLEASE COPY OVER THE SECTION "ramfuncs"  FROM FLASH
    //  TO RAM PRIOR TO CALLING InitSysCtrl(). THIS PREVENTS THE MCU FROM THROWING
    //  AN EXCEPTION WHEN A CALL TO DELAY_US() IS MADE.

    InitFlash();
}

/**
* \brief calcula CRC de los datos recibidos
* Calcula el CRC(32bits) de 4 posiciones de memoria de programa.
*
*/
void calculateCRCfromFLASHdata(void){

}


/*
CAUTION
During the time that interrupts are enabled, the flash and OTP are not available for
use by the application. Code cannot be executed from the flash/OTP nor can data be
read from the flash/OTP.
Flash and OTP will only again become available after the API function exits. Any ISR
routines that are executed during an API function call must completely reside outside
of the flash/OTP and must not expect to read data from the flash/OTP.
Attempting to execute from the flash/OTP will result in undefined opcode fetch and
likely result in an illegal trap (ITRAP). Data fetched will be an unknown value.
*/

/**
* \brief Erase programm FLASH mempry
* Desactiva interrupciones al entrar en la funcion, las vuelve a activar al salir.
*
*/
Uint16 EraseprogrammFlash(void){
    Uint16 functionstatus;
    FLASH_ST flashstatus;
    DINT;

    functionstatus=Flash_Erase(programmFlashSectorMask,&flashstatus);

    EINT;
    return functionstatus;
}

/**
* \brief Erase programm FLASH mempry
* Desactiva interrupciones al entrar en la funcion, las vuelve a activar al salir.
*
*/
Uint16 copyRAMbufferToFLASHprogrammMemory(void){
    Uint16 functionstatus;
    FLASH_ST flashstatus;
    char buffer[13]="fran perea quien lo lea";
    DINT;
    //extern Uint16  Flash_Program(Uint16 *FlashAddr, Uint16 *BufAddr, Uint32 Length,
    functionstatus=Flash_Program(SECTORCstartAddr,(Uint16*)&buffer,(Uint32)13,&flashstatus);
    EINT;
    return functionstatus;
}

