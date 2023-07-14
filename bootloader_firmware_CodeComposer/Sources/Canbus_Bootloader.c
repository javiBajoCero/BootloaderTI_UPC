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

#define TRUE            1
#define FALSE           0

#define SENDBUFFER_SIZE                     8
#define RECVBUFFER_SIZE                     8
#define RECVBUFFER_OWERFLOWMASK             0x7
#define SENDBUFFER_OWERFLOWMASK             0x7

// CAN Baud Rate = (SYSCLKOUT/2) / ((BRPREG+1)*(TSEG1REG+TSEG2REG+3))
#define CANA_BPR                            11  // Baud Rate Prescaler: defines the CAN base clock (SYSCLKOUT/2)/(CANA_BPR+1), the clock of the time quanta (TQ)
#define CANA_TSEG_1                         11  // Defines the first segment (CANA_TSEG_1+1)
#define CANA_TSEG_2                         1   // Defines the second segment (CANA_TSEG_2+1)
//sampling point in PU (1 + (CANA_TSEG_1+1)) / (1 + (CANA_TSEG_1+1) + (CANA_TSEG_2+1))
//Examples for 90 MHz SYSCLKOUT:
//BaudRate  BPR TSEG_1  TSEG_2  SP      Recommended. SP
//1000kbs   2   11      1       86.7%   87.5%
//500kbs    5   11      1       86.7%   87.5%
//250kbs    11  11      1       86.7%   87.5%
//125kbs    44  4       1       75.0%   87.5%
//more info and calculator: http://www.bittiming.can-wiki.info

#define ECANA_MODE                          1   // 1--> eCAN_MOde, 0 --> SCC_Mode
#define LSB_CANA_L_first                    0   // 1--> LSB of CAN_L, 0--> MSB of CAN_H
#define MAX_POS_DATA_NEED                   8

// MBox numbers map
#define MBN_RECV_HB                     31
#define MBN_RECV_DATA                   30

// Mapa de objetos - LBP
#define MBN_SEND_HB                     15
#define MBN_SEND_DATA                   14

#define MBN_RECV_FIRST                  30
#define MBN_RECV_LAST                   31

#define MBN_SEND_FIRST                  14
#define MBN_SEND_LAST                   15      // 31 has the highest transmit priority


struct CAN_MBox mbSend;          //canbus transmitting mailboxes
struct CAN_MBox mbRecv;          //canbus receiving mailboxes





/**
* \brief Configures the MailBox indicated from CANA
* translates CITCEA mailbox abstraction to real register from TI
*
*/
void Config_MBox_CANA (struct CAN_MBox *mb) {
    struct ECAN_REGS ECanaShadow;
    Uint32 Mask = (Uint32) (1<<(mb->Number));//removed citcea Get_Mask() long switch case and replaced for a simple bitshift

    // It obtains the directions of the MailBox, Local Acceptance Mask, Message Time Stamps y Message Time-Out
    mb->HardwareMBox = &(ECanaMboxes.MBOX0) + mb->Number;   // Mailbox
    mb->HardwareLAM = &(ECanaLAMRegs.LAM0) + mb->Number;    // Local Acceptance Mask
    mb->HardwareMOTS = &(ECanaMOTSRegs.MOTS0) + mb->Number; // Message Time Stamps
    mb->HardwareMOTO = &(ECanaMOTORegs.MOTO0) + mb->Number; // Message Time-Out

    // ========= Configure each mailbox ========= //
    // Cancela los envios pendientes y espera a que acabe el envio actual
    ECanaShadow.CANTRR.all = ECanaRegs.CANTRR.all;
    ECanaShadow.CANTRR.all |= Mask;                         // Hace un TRR (Transmission Request Reset)
    ECanaRegs.CANTRR.all = ECanaShadow.CANTRR.all;
    while (ECanaRegs.CANTRS.all & Mask);                    // Espera q que haga el TRR
    ECanaRegs.CANAA.all = Mask;                             // Pone el AA a 0

    // It disables the mailbox and it could be configured
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.all &= ~Mask;                         // Required before writing the MSGIDs
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;

    // MSGID configurations: Message Identifier, Auto Answer Mode, Acceptance mask enable & Identifier Extension
    if (mb->ExtendedCAN) {
        mb->HardwareMBox->MSGID.bit.EXTMSGID_L  = ( 0x0000FFFF & mb->Identifier);
        mb->HardwareMBox->MSGID.bit.EXTMSGID_H  = ((0x00030000 & mb->Identifier) >> 16);
        mb->HardwareMBox->MSGID.bit.STDMSGID    = ((0x1FFC0000 & mb->Identifier) >> 18);
        mb->HardwareMBox->MSGID.bit.IDE         = 1;
    } else {
        mb->HardwareMBox->MSGID.bit.STDMSGID = mb->Identifier;
        mb->HardwareMBox->MSGID.bit.IDE         = 0;
    }

    // CANLAM configuration: Local-Acceptance Masks
    if (mb->AcceptanceMaskEnable) {
        if (mb->ExtendedCAN) {
            mb->HardwareLAM->bit.LAM_L = ( 0x0000FFFF & mb->AcceptanceMask);
            mb->HardwareLAM->bit.LAM_H = ((0x1FFF0000 & mb->AcceptanceMask)>>16);
        } else {
            mb->HardwareLAM->all = ((mb->AcceptanceMask & 0x000007FF)<<18);
        }
    }

    // Pone el time-out si está habilitado
    if (mb->TimeOutEnable) {*mb->HardwareMOTO = mb->TimeOut;}

    // It configures the mailbox
    mb->HardwareMBox->MSGID.bit.AME     = mb->AcceptanceMaskEnable;             // Acceptance Mask Enable
    mb->HardwareMBox->MSGID.bit.AAM     = mb->AutoAnswerMode;                   // Auto Answer Mode
    mb->HardwareMBox->MSGCTRL.bit.TPL   = mb->TransmitPriority;                 // Priority
    mb->HardwareMBox->MSGCTRL.bit.RTR   = mb->RemoteTranssmisionRequest;        // Remote Transmision Request
    mb->HardwareMBox->MSGCTRL.bit.DLC   = mb->DataLength;                       // Data Length (it is modified each sending)

    // The direction is configured
    ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
    if (mb->Direction)          {ECanaShadow.CANMD.all |= Mask;}
    else                        {ECanaShadow.CANMD.all &= ~Mask;}
    ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

    // It configures the OverWrite protection
    ECanaShadow.CANOPC.all = ECanaRegs.CANOPC.all;
    if (mb->OverWriteProtect)   {ECanaShadow.CANOPC.all |= Mask;}
    else                        {ECanaShadow.CANOPC.all &= ~Mask;}
    ECanaRegs.CANOPC.all = ECanaShadow.CANOPC.all;

    // If it is needed the time-out is enabled
    ECanaShadow.CANTOC.all = ECanaRegs.CANTOC.all;
    if (mb->TimeOutEnable)      {ECanaShadow.CANTOC.all |= Mask;}
    else                        {ECanaShadow.CANTOC.all &= ~Mask;}
    ECanaRegs.CANTOC.all = ECanaShadow.CANTOC.all;

    // It configure the interrupt for the Mailbox
    // (1) CANMIM enables the Mailbox to generates an interrupt
    EALLOW;

    ECanaShadow.CANMIM.all = ECanaRegs.CANMIM.all;
    if (mb->InterruptMask)      {ECanaShadow.CANMIM.all |= Mask;}
    else                        {ECanaShadow.CANMIM.all &= ~Mask;}
    ECanaRegs.CANMIM.all = ECanaShadow.CANMIM.all;

    if (mb->InterruptLowPriority) {
        // (2) CANMIL direct the interrupt to ECAN0INT (high priority) o ECAN1INT (low priority)
        ECanaShadow.CANMIL.all = ECanaRegs.CANMIL.all;
        ECanaShadow.CANMIL.all |= Mask;
        ECanaRegs.CANMIL.all = ECanaShadow.CANMIL.all;

        // (3) CANGIM enables ECAN1INT interrupt
        ECanaShadow.CANGIM.all = ECanaRegs.CANGIM.all;
        ECanaShadow.CANGIM.bit.I1EN = 1;
        ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;
    } else {
        // (2) CANMIL direct the interrupt to ECAN0INT o ECAN1INT
        ECanaShadow.CANMIL.all = ECanaRegs.CANMIL.all;
        ECanaShadow.CANMIL.all &= ~Mask;
        ECanaRegs.CANMIL.all = ECanaShadow.CANMIL.all;

        // (3) CANGIM enables ECAN0INT interrupt
        ECanaShadow.CANGIM.all = ECanaRegs.CANGIM.all;
        ECanaShadow.CANGIM.bit.I0EN = 1;
        ECanaRegs.CANGIM.all = ECanaShadow.CANGIM.all;
    }

    EDIS;

    // If the mailbox is used it will be enabled
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    if (mb->Enable)             {ECanaShadow.CANME.all |= Mask;}
    else                        {ECanaShadow.CANME.all &= ~Mask;}
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;
}


/**
* \brief configuration of CANBUS TX mailboxes
*
*
*/
void configMailboxTXCanbus(void){//https://www.ti.com/lit/ug/sprueu0/sprueu0.pdf?ts=1689326497046&ref_url=https%253A%252F%252Fwww.google.com%252F
    Uint32 i=0; //used localy for the loop
    // ========= Configure the Transmit MailBoxes ========= //
    // Common parameters for all the transmit MailBoxes
    mbSend.Direction = CAN_TRANSMIT;
    mbSend.ExtendedCAN = TRUE;
    mbSend.Enable = TRUE;
    mbSend.RemoteTranssmisionRequest = FALSE;
    mbSend.AutoAnswerMode = FALSE;
    mbSend.AcceptanceMaskEnable = TRUE;
    mbSend.InterruptMask = FALSE;
    mbSend.OverWriteProtect = FALSE;
    mbSend.TimeOutEnable = TRUE;
    mbSend.TimeOut = 10000;         // 10000 a 1Mbs = 10ms

    // Additional parameters for some transmit MailBoxes
    for (i = MBN_SEND_FIRST; i <= MBN_SEND_LAST; i++) {
        mbSend.Number = i;
        mbSend.TransmitPriority = i;

        switch (i) {
        case MBN_SEND_HB:
            mbSend.Identifier = 0x00000703;
            mbSend.AcceptanceMask = 0xFFFFF000;
            mbSend.DataLength = 4;
            break;
        case MBN_SEND_DATA:
            mbSend.Identifier = 0x00000803;
            mbSend.AcceptanceMask = 0xFFFFF000;
            mbSend.DataLength = 1;
            break;
        default:
            break;
        }
        Config_MBox_CANA(&(mbSend));    // Execute the mailbox configuration
    }
}

/**
* \brief configuration of CANBUS RX mailboxes
*
*
*/
void configMailboxRXCanbus(void){//https://www.ti.com/lit/ug/sprueu0/sprueu0.pdf?ts=1689326497046&ref_url=https%253A%252F%252Fwww.google.com%252F
    Uint32 i=0; //used localy for the loop
    // ========= Configure the Receive MailBoxes ========= //
    // Common parameters for all the receive MailBoxes
    mbRecv.Direction = CAN_RECEIVE;
    mbRecv.ExtendedCAN = TRUE;
    mbRecv.Enable = TRUE;
    mbRecv.RemoteTranssmisionRequest = FALSE;
    mbRecv.AutoAnswerMode = FALSE;
    mbRecv.AcceptanceMask = TRUE;
    mbRecv.AcceptanceMaskEnable = TRUE;
    mbRecv.InterruptMask = TRUE;
    mbRecv.InterruptLowPriority = TRUE;
    mbRecv.OverWriteProtect = FALSE;
    mbRecv.TimeOutEnable = FALSE;

    // Additional parameters for some receive MailBoxes
    for (i = MBN_RECV_FIRST; i <= MBN_RECV_LAST; i++) {
        mbRecv.Number = i;
        switch (i) {
        case MBN_RECV_HB:
            mbRecv.Identifier = 0x00000702;
            mbRecv.AcceptanceMask = 0xFFFFF000;
            break;
        case MBN_RECV_DATA:
            mbRecv.Identifier = 0x00000802;
            mbRecv.AcceptanceMask = 0xFFFFF000;
            break;
        default:
            break;
        }
        Config_MBox_CANA(&(mbRecv));    // Executes the mailbox configuration
    }
}

/**
* \brief Send a determinate MailBox info through CANA
*
*
*/
void Send_MBox_CANA (union CAN_Data *d, unsigned int i) {
    volatile struct MBOX *HardwareMBox;
    unsigned long Mask = (Uint32)(1<<(i));
    struct ECAN_REGS ECanaShadow;

    // It copies the dates from the software variable to hardware variable
    HardwareMBox = &(ECanaMboxes.MBOX0) + i;
    HardwareMBox->MDL.all = d->DWords.DWord0;
    HardwareMBox->MDH.all = d->DWords.DWord1;

    // Reset Time-Stamp counter
    EALLOW;
    ECanaRegs.CANTSC = 0;
    EDIS;

    // Transmission-Request Set Register (it initialize the transmission)
    ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
    ECanaShadow.CANTRS.all |= Mask;
    ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

//  counter_CAN_tx++;       // New transmitted CAN Message
}


/**
* \brief función de inicialización del periférico de CANBUS.
* contains CANBUS clocks and peripheral initializations needed for the bootloader to work.
*
*/
void initCanbus(void){// code inspired from "G:\Shared drives\CITCEA.PRJ_190_EVARM_COHVE\06. documentacio tecnica\7. Disseny software\Noves Implementacions\Bootloader\Projectes Bootloader CAN" CME_firmware_Bootloader/source/CAN_boot.c
    InitECana();// function already provided by F2806x_ECan.c, only changed the speed bit quantas for 250kbps
    configMailboxRXCanbus();
    configMailboxTXCanbus();
}
