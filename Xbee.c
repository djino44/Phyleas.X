/*
 * File:   Xbee.c
 * Author: Radjy RAMLALL
 * Project: Phyleas Dev
 * Created on 19 mars 2014, 12:34
 */

#include "Xbee.h"
#include "ConfigProcessor.h"
#include <plib.h>
#include "LCD.h"


volatile static char toto=NULL;


void Init_Xbee(void)
{

    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), DESIRED_BAUDRATE_XBEE);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    // Configure UART RX Interrupt
    INTEnable(INT_SOURCE_UART_RX(UART_MODULE_ID), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_SUB_PRIORITY_LEVEL_1);

    // Enable multi-vector interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    WriteStringXbee("*** UART Xbee Initialized ***\r\n");
    
}



void WriteStringXbee(const char *string)
{
    while(*string != '\0')
    {
        while(!UARTTransmitterIsReady(UART_MODULE_ID));

        UARTSendDataByte(UART_MODULE_ID, *string);

        string++;

        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
    }
}


void PutCharacterXbee(const char character)
{
        while(!UARTTransmitterIsReady(UART_MODULE_ID));

        UARTSendDataByte(UART_MODULE_ID, character);

        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}


void __ISR(_UART_1_VECTOR, ipl2) IntUart1Handler(void)
{
	// Is this an RX interrupt?
	if(INTGetFlag(INT_SOURCE_UART_RX(UART_MODULE_ID)))
	{
            // Clear the RX interrupt Flag
            while (UARTReceivedDataIsAvailable (UART_MODULE_ID))
            {// none zero if UART has received data
                toto = UARTGetDataByte(UART_MODULE_ID);
            }
            PutCharacterXbee(toto);
            if(toto=='c')
            {
                 writeLCD( LCDCMD, 1);
            }
            else
            {
                putLCD(toto);

            }
            
	    INTClearFlag(INT_SOURCE_UART_RX(UART_MODULE_ID));

            // Echo what we just received.
            
            DBPRINTF("%c",UARTGetDataByte(UART_MODULE_ID));

	}

	// We don't care about TX interrupt
	if (INTGetFlag(INT_SOURCE_UART_TX(UART_MODULE_ID)))
	{
            INTClearFlag(INT_SOURCE_UART_TX(UART_MODULE_ID));
	}
}