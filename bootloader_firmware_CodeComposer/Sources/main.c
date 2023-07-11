/**
 * main.c
 */

#include "StateMachine_Bootloader.h"

int main(void)
{
    initBootloader();

    while(1){
        stateMachineBootloader();
    }
}
