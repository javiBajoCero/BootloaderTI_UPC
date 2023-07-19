/*
 *  \brief FLASHhandling_Bootloader.c
 *  \details   Este *.c contiene todo lo referente a la inicialización y borrado/grabado de la memoria interna FLASH.
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */
#include "F2806x_Device.h"
#include "F2806x_GlobalPrototypes.h"
#include "FLASHhandling_Bootloader.h"

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
