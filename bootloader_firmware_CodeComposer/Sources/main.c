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

// Step 2. Select GPIO for the LED and CANBUS function for canbusRX TX pins, everything else is high impedance GPIO inputs:
    InitGpio();

// Step 3. Initialize PIE vector table:
    // The PIE vector table is initialized with pointers to shell Interrupt
    // Service Routines (ISR).
    // Insert user specific ISR code in the appropriate shell ISR routine .

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

// Step 5. Enable Interrupts:
    // Enable global Interrupts and higher priority real-time debug events:
    EINT;     // Enable Global interrupt INTM (just in case it wasnt enabled already)
    ERTM;     // Enable Global realtime interrupt DBGM

//Superloop
    union CAN_Data a;
    while(1){
        a=receiveDATA_canbus();
        if(a.newmessage_flag){
            stateMachineBootloader();
            a.newmessage_flag=0;
            GpioDataRegs.GPBTOGGLE.bit.GPIO39=0x01; //Toggles blue LED D10 (debugging)
        }

    }
}
