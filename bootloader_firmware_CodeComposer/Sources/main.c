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
// Step 1. Initialize System Control registers, PLL, WatchDog, Clocks to default state:
    InitSysCtrl();

// Step 2. Select GPIO for the device or for the specific application:
    InitGpio();  // Not required for this example

// Step 3. Initialize PIE vector table:
    // The PIE vector table is initialized with pointers to shell Interrupt
    // Service Routines (ISR).
    // Insert user specific ISR code in the appropriate shell ISR routine in
    // the F2806x_DefaultIsr.c file.

    // Disable and clear all CPU interrupts:
    DINT;
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize Pie Control Registers To Default State:
    InitPieCtrl();

    // Initialize the PIE Vector Table To a Known State:
    // This function populates the PIE vector table with pointers
    // to the shell ISR functions.
    InitPieVectTable();

// Step 4. Initialize all the Device Peripherals to a known state:
    // InitPeripherals();  // For this example just init the CPU Timers
    InitCpuTimers();

    // ========= Step 4. INITIALIZE ALL THE PERIPHERAL DEVICES ========= //
    initSystick();
    initFLASHhandling();
    initCanbus();
    initBootloader();
    Uint32 time=0;
    while(1){
        //stateMachineBootloader();
        if(getTick()-time>100){
            GpioDataRegs.GPBTOGGLE.bit.GPIO39=0x01; //Toggles blue LED D10 (debugging)
            time=getTick();
        }
    }
}
