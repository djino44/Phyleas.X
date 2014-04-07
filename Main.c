
/*
 * File:   Main.h
 * Author: Radjy RAMLALL
 * Project: Phyleas Dev
 * Created on 19 mars 2014, 12:34
 */

#include <plib.h>
#include "ConfigProcessor.h"
#include "Bluetooth.h"
#include <GenericTypeDefs.h>
#include "Explore.h"
/*Fuses Config*/

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1



void delay_for_1000_nops_x (int n)
{
    while (n --)
    {
        int i;

        for (i = 0; i < 1000; i++)
            asm volatile ("nop");
    }
}




int main (void)
{
    
    DDPCONbits.JTAGEN = 0;
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    
    initEX16();
    Init_Xbee();
    initLCD();
    putsLCD("RRC");
    DelayMs(1500);
    InitBluetooth();
    mPORTDSetPinsDigitalOut(BIT_0);
    mPORTDSetBits(BIT_0);

    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    
    while(1)
    {
        mPORTDToggleBits(BIT_0);
       
        DelayMs(1000);
        
    }
    return -1;
}