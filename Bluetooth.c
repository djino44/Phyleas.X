/*
 * Author : Radjy RAMLALL
 * Date : 24/03/14
 * Brief : Drivers for Rn-42 including stack support.
 *
 */

#include <GenericTypeDefs.h>
#include <time.h>
#include "ConfigProcessor.h"
#include "Bluetooth.h"
//#include "Xbee.h"
#include <string.h>
#include <plib.h>
#include <stdbool.h>
#include <peripheral/uart.h>

#define UART_MODULE_ID (UART2)
#define DESIRED_BAUDRATE (115200)
#define Reset_BT (mPORTBClearBits(BIT_9))


void InitBluetooth(void)
{
    mPORTBSetPinsDigitalOut(BIT_9);
    mPORTBSetBits(BIT_9);// BT high level, the reset polarity is active low

    mPORTASetPinsDigitalOut(BIT_3);
    mPORTAClearBits(BIT_3);// Set Baud rate (high = force 9,600, low = 115 K or firmware setting)

    mPORTASetPinsDigitalOut(BIT_2);
    mPORTASetBits(BIT_2);// Set BT master (high = auto-master mode)

    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY);
    //UARTConfigure(UART_MODULE_ID, UART_ENABLE_HIGH_SPEED | UART_ENABLE_PINS_CTS_RTS);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), DESIRED_BAUDRATE);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    // Configure UART RX Interrupt
    INTEnable(INT_SOURCE_UART_RX(UART_MODULE_ID), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_SUB_PRIORITY_LEVEL_2);

    // Enable multi-vector interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    //WriteStringXbee("*** UART Bluetooth demarre ***\r\n");
    WriteStringBluetooth("$$$");
    WriteStringBluetooth("D\n");

}


void WriteStringBluetooth(const char *string)
{
    while(*string != '\0')
    {
        while(!UARTTransmitterIsReady(UART_MODULE_ID));

        UARTSendDataByte(UART_MODULE_ID, *string);

        string++;

        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
    }
}


void PutCharacterBluetooth(const char character)
{
        while(!UARTTransmitterIsReady(UART_MODULE_ID));

        UARTSendDataByte(UART_MODULE_ID, character);

        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}



void __ISR(_UART_2_VECTOR, ipl2) IntUart2Handler(void)
{
	// Is this an RX interrupt?
	if(INTGetFlag(INT_SOURCE_UART_RX(UART_MODULE_ID)))
	{
            // Clear the RX interrupt Flag
            PutCharacterXbee(UARTGetDataByte(UART_MODULE_ID));
	    INTClearFlag(INT_SOURCE_UART_RX(UART_MODULE_ID));

            // Echo what we just received.

            DBPRINTF("%c",UARTGetDataByte(UART_MODULE_ID_BLUETOOTH));

	}

	// We don't care about TX interrupt
	if (INTGetFlag(INT_SOURCE_UART_TX(UART_MODULE_ID)))
	{
            INTClearFlag(INT_SOURCE_UART_TX(UART_MODULE_ID));
	}
}