/*
 *  \brief Systick_Bootloader.c
 *  \details   Este *.c contiene todo lo referente a la inicialización y funcionamiento del timer utilizado como tick del micro (delays y medidas de tiempos en miliseconds).
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#include "F2806x_Device.h"               //already provided TI functions / register definitiions and utilities, specific to this TMS320F28069M device
#include "F2806x_GlobalPrototypes.h"

#include "Systick_Bootloader.h"

#define TIMER0clockfreq 90      //Mhz
#define Systick_period  1000    //us=1ms

Uint32 systemtick=0;

/**
* \brief interruption handler del tim0 / systick.
* It just advances the systemtick +1.
*
*/
interrupt void interrupt_systick(void){
    GpioDataRegs.GPBTOGGLE.bit.GPIO39=0x01; //Toggles blue LED D10 (debugging)
    systemtick++;                           //adds +1 to system tick
    PieCtrlRegs.PIEACK.all= PIEACK_GROUP1;
}


/**
* \brief funcion de inicialización del timer usado como systick.
* contiene la inicializacion de clocks y periférico del timer.
*
*/
void initSystick(void){
    //Enable the peripheral interrupcion del tim0 es INT1.7
    ConfigCpuTimer(&CpuTimer0, TIMER0clockfreq, Systick_period);

    //Address the interrupt vector to the desired function
    EALLOW;
    PieVectTable.TINT0 = &interrupt_systick;
    EDIS;
    //Enable interrupt Y of group X. //javi page 177: https://www.ti.com/lit/ug/spruh18i/spruh18i.pdf?ts=1689232997328&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTMS320F28062
    PieCtrlRegs.PIEIER1.bit.INTx7=1;//1=enable 0=disable
    //Enable group X of interrupts:
    IER |= M_INT1;
    // Enable global interrupts:
    EINT;
    //Allow next interrupt service (write before leaving the interruption routine)
    PieCtrlRegs.PIEACK.bit.ACK1=1;
    //arranca el timer
    StartCpuTimer0();
}

/**
* \brief Basic getter
* exposes local systemtick to outside code, (much like ST's HAL_GetTick())
*
*/
Uint32 getTick(void){
    return systemtick;
}
