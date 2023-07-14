/*
 *  \brief Canbus_Bootloader.h
 *  \details   Este *.h contiene las declaraciones de variables y funciones de StateMachine_Bootloader.c .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#ifndef INCLUDES_CANBUS_BOOTLOADER_H_
#define INCLUDES_CANBUS_BOOTLOADER_H_


//CITCEA CANBUS structs abstraction to """"""""""""simplify"""""""""""" canbus mailbox configuration
struct CAN_data_DWords {
    Uint32 DWord1;
    Uint32 DWord0;
};

struct CAN_Data_Words {
    Uint16 Word3: 16;
    Uint16 Word2: 16;
    Uint16 Word1: 16;
    Uint16 Word0: 16;
};

struct CAN_Data_Bytes {
    Uint16 Byte7: 8;
    Uint16 Byte6: 8;
    Uint16 Byte5: 8;
    Uint16 Byte4: 8;
    Uint16 Byte3: 8;
    Uint16 Byte2: 8;
    Uint16 Byte1: 8;
    Uint16 Byte0: 8;
};

union CAN_Data {
    struct CAN_Data_Bytes   Bytes;              // 8bit words
    struct CAN_Data_Words   Words;              // 16bit words
    struct CAN_data_DWords  DWords;             // 32bit words
};

enum CAN_Direction {CAN_TRANSMIT = 0, CAN_RECEIVE = 1};

struct CAN_MBox {
    Uint16                      Number;                     // Número de la mailbox (0..31)
    Uint16                      TransmitPriority;           // 0..32 (32=max. priority)
    Uint32                      Identifier;                 // Identificador
    Uint32                      AcceptanceMask;             // Los bits a 0 indican que en el mensaje entrante ese bit debe coincidir con el Identificador para aceptaralo
    Uint32                      TimeOut;                    // Valor en periodos de la frecuencia del CAN
    Uint16                      AcceptanceMaskEnable;       // Si se desabilita solo admitirá ID igual al ID del MBOX
    Uint16                      TimeOutEnable;
    Uint16                      ExtendedCAN;                // 0 = Normal Identifier (11bits) / 1 = Extended Identifier (29bits ID) --> representa el identificador a la hora de enviar (para recibir depende de GANGAM.AIM)
    Uint16                      Enable;
    enum CAN_Direction          Direction;                  // 0 = Transmit / 1 = Receive
    Uint16                      RemoteTranssmisionRequest;
    Uint16                      AutoAnswerMode;
    Uint16                      InterruptMask;
    Uint16                      InterruptLowPriority;       // TRUE=ECAN0INTA, FALSE=ECAN1INTA
    Uint16                      OverWriteProtect;
    Uint16                      DataLength;                 // Longitud de datos (en bytes)
    union CAN_Data              Data;                       // Estructura de datos (es diferente de la del DSP, más intuitiva)
    volatile struct MBOX        *HardwareMBox;              // Una vez inicializado apuntará a la mailbox del DSP que le corresponda
    volatile union CANLAM_REG   *HardwareLAM;               // Idem con la Local Acceptance Mask
    volatile Uint32             *HardwareMOTS;              // Idem con el Message Object Time Stamps
    volatile Uint32             *HardwareMOTO;              // Idem con el Message Object Time-Out
};


void initCanbus(void);
void Send_MBox_CANA (union CAN_Data *d, unsigned int i);

#endif /* INCLUDES_CANBUS_BOOTLOADER_H_ */
