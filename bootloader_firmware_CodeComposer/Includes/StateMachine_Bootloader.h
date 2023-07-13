/*
 *  \brief StateMachine_Bootloader.h
 *  \details   Este *.h contiene las declaraciones de variables y funciones de StateMachine_Bootloader.c .
 *  \author    javier.munoz.saez@upc.edu
 *  \version   0.0
 *  \date      Jul 10, 2023
 */

#ifndef INCLUDES_STATEMACHINE_BOOTLOADER_H_
#define INCLUDES_STATEMACHINE_BOOTLOADER_H_


//function declarations
void initBootloader(void);
void stateMachineBootloader(void);
void exitBootloader(void);
void errorHandlerBootloader(void);

//this struct contains the state of the statemachine
typedef struct {
    enum {  st_wait =(Uint32)0x0000,
            st_erase=(Uint32)0x0001,
            st_write=(Uint32)0x0002,
            st_go   =(Uint32)0x0003,
            st_error=(Uint32)0x0004
    } step;
    Uint32  graf_init;
    Uint32  step_chng;
} STATEMACHINE_struct;


typedef union {
    Uint32 all;
    struct{
        Uint8 Program :1;
        Uint8 Erase   :1;
        Uint8 CAN     :1;
        Uint8 Go      :1;
    } bit;
} Error_struct;

#endif /* INCLUDES_STATEMACHINE_BOOTLOADER_H_ */
