/*
 *  \brief Canbus_Bootloader.c
 *  \details   Este *.c contiene todo lo referente a la inicialización, comunicaciones y utilidades del periférico de CANBUS .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */
#include "F2806x_Device.h"                           //already provided TI functions / register definitiions and utilities, specific to this TMS320F28069M device
#include "F2806x_GlobalPrototypes.h"

#include "Canbus_Bootloader.h"

#define TRUE                        (Uint16) 1
#define FALSE                       (Uint16) 0

//mailboxes defines, las configuran asi y no se modifican ni los ID ni DLC en todo el programa
#define MBN_RECV_HB                     (Uint16) 31         //numero de CANBUS mailbox que se usa para recibir heartbeat
#define canbusADDR_MBN_RECV_HB          (Uint32) 0x00000702 //extended addr de canbus para recibir heartbeat

#define MBN_RECV_DATA                   (Uint16) 30         //numero de CANBUS mailbox que se usa para recibir data
#define canbusADDR_MBN_RECV_DATA        (Uint32) 0x00000802 //extended addr de canbus para recibir data

#define MBN_SEND_HB                     (Uint16) 15         //numero de CANBUS mailbox que se usa para enviar HEARTBEAT
#define canbusADDR_MBN_SEND_HB          (Uint32) 0x00000703 //extended addr de canbus para enviar heartbeat
#define datalengthinbytes_MBN_SEND_HB   (Uint16) 4          //numero de bytes de datos a enviar con cada mensaje de heartbeat

#define MBN_SEND_DATA                   (Uint16) 14         //numero de CANBUS mailbox que se usa para enviar data
#define canbusADDR_MBN_SEND_DATA        (Uint32) 0x00000803 //extended addr de canbus para enviar data
#define datalengthinbytes_MBN_SEND_DATA (Uint16) 1          //numero de bytes de datos a enviar con cada mensaje de data

#define mailboxesAcceptanceMasks        (Uint32) 0xFFFFF000



struct CAN_MBox mbSend;          //canbus transmitting mailboxes
struct CAN_MBox mbRecv;          //canbus receiving mailboxes


//todas las definiciones de las funciones de este .c , asi son accesibles entre ellas localmente
    void initCanbus(void);
    void configMailboxTXCanbus(void);
    void configMailboxRXCanbus(void);
    void Config_MBox_CANA (struct CAN_MBox *mb);
    void Send_MBox_CANA (union CAN_Data *d, Uint32 i);
    void sendHeartbeat_canbus(union CAN_Data *d);
    void sendDATA_canbus(union CAN_Data *d);


/**
* \brief función de inicialización del periférico de CANBUS.
* contains CANBUS clocks and peripheral initializations needed for the bootloader to work.
*
*/
void initCanbus(void){              // code inspired from "G:\Shared drives\CITCEA.PRJ_190_EVARM_COHVE\06. documentacio tecnica\7. Disseny software\Noves Implementacions\Bootloader\Projectes Bootloader CAN" CME_firmware_Bootloader/source/CAN_boot.c
    InitECana();                    // function already provided by F2806x_ECan.c, only changed the speed bit quantas for 250kbps
    configMailboxRXCanbus();
    configMailboxTXCanbus();
}

/**
* \brief configuration of CANBUS TX mailboxes
*
*
*/
void configMailboxTXCanbus(void){//https://www.ti.com/lit/ug/sprueu0/sprueu0.pdf?ts=1689326497046&ref_url=https%253A%252F%252Fwww.google.com%252F
    // ========= Configure the Transmit MailBoxes ========= //
    // Common parameters for all the transmit MailBoxes
    mbSend.Direction                    = CAN_TRANSMIT;
    mbSend.ExtendedCAN                  = TRUE;
    mbSend.Enable                       = TRUE;
    mbSend.RemoteTranssmisionRequest    = FALSE;
    mbSend.AutoAnswerMode               = FALSE;
    mbSend.AcceptanceMaskEnable         = TRUE;
    mbSend.InterruptMask                = FALSE;
    mbSend.OverWriteProtect             = FALSE;
    mbSend.TimeOutEnable                = TRUE;
    mbSend.TimeOut                      = 10000;                 // 10000 a 250Kbs = 40ms

    // Additional parameters for some transmit MailBoxes

    mbSend.Number           = MBN_SEND_HB;
    mbSend.TransmitPriority = MBN_SEND_HB;
    mbSend.Identifier       = canbusADDR_MBN_SEND_HB;
    mbSend.AcceptanceMask   = mailboxesAcceptanceMasks;
    mbSend.DataLength       = datalengthinbytes_MBN_SEND_HB;
    Config_MBox_CANA(&(mbSend));                                // Execute the mailbox configuration

    mbSend.Number           = MBN_SEND_DATA;
    mbSend.TransmitPriority = MBN_SEND_DATA;
    mbSend.Identifier       = canbusADDR_MBN_SEND_DATA;
    mbSend.AcceptanceMask   = mailboxesAcceptanceMasks;
    mbSend.DataLength       = datalengthinbytes_MBN_SEND_DATA;
    Config_MBox_CANA(&(mbSend));                                // Execute the mailbox configuration
}

/**
* \brief configuration of CANBUS RX mailboxes
*
*
*/
void configMailboxRXCanbus(void){                               //https://www.ti.com/lit/ug/sprueu0/sprueu0.pdf?ts=1689326497046&ref_url=https%253A%252F%252Fwww.google.com%252F

    // ========= Configure the Receive MailBoxes ========= //
    // Common parameters for all the receive MailBoxes

    mbRecv.Direction                = CAN_RECEIVE;
    mbRecv.ExtendedCAN              = TRUE;
    mbRecv.Enable                   = TRUE;
    mbRecv.RemoteTranssmisionRequest= FALSE;
    mbRecv.AutoAnswerMode           = FALSE;
    mbRecv.AcceptanceMask           = TRUE;
    mbRecv.AcceptanceMaskEnable     = TRUE;
    mbRecv.InterruptMask            = FALSE;
    mbRecv.InterruptLowPriority     = FALSE;
    mbRecv.OverWriteProtect         = FALSE;
    mbRecv.TimeOutEnable            = FALSE;

    // Additional parameters for some receive MailBoxes
    mbRecv.Number           = MBN_RECV_HB;
    mbRecv.Identifier       = canbusADDR_MBN_RECV_HB;
    mbRecv.AcceptanceMask   = mailboxesAcceptanceMasks;
    Config_MBox_CANA(&(mbRecv));                                // Executes the mailbox configuration

    mbRecv.Number           = MBN_RECV_DATA;
    mbRecv.Identifier       = canbusADDR_MBN_RECV_DATA;
    mbRecv.AcceptanceMask   = mailboxesAcceptanceMasks;
    Config_MBox_CANA(&(mbRecv));                                // Executes the mailbox configuration


}

/**
* \brief Configures the MailBox indicated from CANA
* translates CITCEA mailbox abstraction to real register from TI
*
*/
void Config_MBox_CANA (struct CAN_MBox *mb) {
    struct ECAN_REGS ECanaShadow;
    Uint32 Mask =0;
    Mask = (Uint32) (((Uint32)1)<<((Uint32)(mb->Number)));//removed citcea Get_Mask() long switch case and replaced for a simple bitshift

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
* \brief Send a determinate MailBox info through CANA
* por algun motivo las mailboxes se usan con una unica e ininmutable dirección de canbus, por cada dirección de canbus diferente que se quiera usar se ocupa un mailbox diferente
*
*/
void Send_MBox_CANA (union CAN_Data *d, Uint32 i) {
    volatile struct MBOX *HardwareMBox;
    Uint32 Mask = (Uint32)((Uint32)1<<(i));
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
}

/**
* \brief sends a canbus message with data d, using the setup in the heartbeat mailbox
*
*/
void sendHeartbeat_canbus(union CAN_Data *d){
    Send_MBox_CANA(d, MBN_SEND_HB);
}

/**
* \brief sends a canbus message with data d, using the setup in the data mailbox
*
*/
void sendDATA_canbus(union CAN_Data *d){
    Send_MBox_CANA(d, MBN_SEND_DATA);
}


