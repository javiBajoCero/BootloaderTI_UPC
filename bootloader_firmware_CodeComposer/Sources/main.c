/**
 * main.c
 */

#include "F2806x_Device.h"               //already provided TI functions / register definitiions and utilities, specific to this TMS320F28069M device
#include "F2806x_GlobalPrototypes.h"

#include "StateMachine_Bootloader.h"     //These next includes are all created for the bootloader
#include "Systick_Bootloader.h"
#include "FLASHhandling_Bootloader.h"
#include "Canbus_Bootloader.h"

int main(void)
{
    // ========= STEP 1. INITIALIZE SYSTEM CONTROL ========= //
    // Configuration system clock, Disable Watchdog & Enable Peripheral Clocks
    InitSysCtrl();
    // ========= Step 2. INITALITZE GPIO ========= //
    InitGpio();             // All pins defined as GPIO input (high impedance) no pullup, except CAN and LED pins

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

//    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
//    // This will populate the entire table.
    InitPieVectTable();

    // ========= Step 4. INITIALIZE ALL THE PERIPHERAL DEVICES ========= //
    initSystick();
    initFLASHhandling();
    initCanbus();
    initBootloader();

    while(1){
        stateMachineBootloader();
    }
}
