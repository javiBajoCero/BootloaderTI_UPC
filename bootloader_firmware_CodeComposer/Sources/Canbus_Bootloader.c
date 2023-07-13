/*
 *  \brief Canbus_Bootloader.c
 *  \details   Este *.c contiene todo lo referente a la inicialización, comunicaciones y utilidades del periférico de CANBUS .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */
#include "F2806x_Device.h"               //already provided TI functions / register definitiions and utilities, specific to this TMS320F28069M device
#include "F2806x_GlobalPrototypes.h"

#include "Canbus_Bootloader.h"


/**
* \brief función de inicialización del periférico de CANBUS.
* contains CANBUS clocks and peripheral initializations needed for the bootloader to work.
*
*/
void initCanbus(void){// code inspired from "G:\Shared drives\CITCEA.PRJ_190_EVARM_COHVE\06. documentacio tecnica\7. Disseny software\Noves Implementacions\Bootloader\Projectes Bootloader CAN" CME_firmware_Bootloader/source/CAN_boot.c

    struct ECAN_REGS ECanaShadow;

    EALLOW;

    //
    // Asynch Qual
    //
    GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;

    //
    // Configure eCAN RX and TX pins for CAN operation using eCAN regs
    //
     ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
     ECanaShadow.CANTIOC.bit.TXFUNC = 1;
     ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

     ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
     ECanaShadow.CANRIOC.bit.RXFUNC = 1;
     ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

     // Initialize all bits of 'Message Control Register' to zero
     // Some bits of MSGCTRL register come up in an unknown state. For proper operation,
     // all bits (including reserved bits) of MSGCTRL must be initialized to zero
     //
     ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;

     //
     // Clear all RMPn, GIFn bits
     // RMPn, GIFn bits are zero upon reset and are cleared again as a precaution.
     //
     ECanaRegs.CANRMP.all = 0xFFFFFFFF;

     //
     // Clear all interrupt flag bits
     //
     ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
     ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

     //
     // Configure bit timing parameters for eCANA
     //
     ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
     ECanaShadow.CANMC.bit.CCR = 1 ;
     ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

     ECanaShadow.CANES.all = ECanaRegs.CANES.all;

     //
     // Wait for CCE bit to be set..
     //
     do
     {
         ECanaShadow.CANES.all = ECanaRegs.CANES.all;
     } while(ECanaShadow.CANES.bit.CCE != 1 );

     ECanaShadow.CANBTC.all = 0;

// Note: These values are modified from the original boot rom CAN_Init() function
// BRP = 44, TSEG2REG = 3, TSEG1REG = 4
// Values chosen for: SYSCLKOUT = 90 MHz  CAN module clock = 45 MHz Bit rate = 100kbps
//     ECanaShadow.CANBTC.bit.BRPREG = 44;
//     ECanaShadow.CANBTC.bit.TSEG2REG = 3;
//     ECanaShadow.CANBTC.bit.TSEG1REG = 4;

//calculated using this excel http://www.ti.com/lit/zip/sprac35

     // BRP = 11, TSEG2REG = 5, TSEG1REG = 7
     // Values chosen for: SYSCLKOUT = 90 MHz  CAN module clock = 45 MHz Bit rate = 250kbps
     ECanaShadow.CANBTC.bit.BRPREG   = 11;
     ECanaShadow.CANBTC.bit.TSEG2REG = 5;
     ECanaShadow.CANBTC.bit.TSEG1REG = 7;

     ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

     ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
     ECanaShadow.CANMC.bit.CCR = 0 ;
     ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

     ECanaShadow.CANES.all = ECanaRegs.CANES.all;

     //
     // Wait for CCE bit to be  cleared..
     //
     do
     {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
     } while(ECanaShadow.CANES.bit.CCE != 0 );

     //
     // Disable all Mailboxes
     // Required before writing the MSGIDs
     //
     ECanaRegs.CANME.all = 0;

     //
     // Assign MSGID to MBOX1
     // Standard ID of 1, Acceptance mask disabled
     //
     ECanaMboxes.MBOX1.MSGID.all = 0x00040000;

     //
     // Configure MBOX1 to be a receive MBOX
     //
     ECanaRegs.CANMD.all = 0x0002;

     //
     // Enable MBOX1
     //
     ECanaRegs.CANME.all = 0x0002;

     EDIS;

     return;
}
