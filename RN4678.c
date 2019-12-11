/*
 * RN4678.c
 *
 *  Created on: 06.12.2019
 *      Author: Alex
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include "RN4678.h"

#define SW_BTN          GPIO_PIN_2
#define SW_BTN_PORT     GPIO_PORTD_BASE
#define RST             GPIO_PIN_4
#define RST_PORT        GPIO_PORTP_BASE
#define RN4678_RTS      GPIO_PIN_5      // is CTS from TivaBoard
#define RN4678_RTS_PORT GPIO_PORTP_BASE
#define RN4678_CTS      GPIO_PIN_4      // is RTS from TivaBoard
#define RN4678_CTS_PORT GPIO_PORTD_BASE
#define U6RX            GPIO_PP0_U6RX
#define U6RX_PIN        GPIO_PIN_0
#define U6RX_PORT       GPIO_PORTP_BASE
#define U6TX            GPIO_PP1_U6TX
#define U6TX_PIN        GPIO_PIN_1
#define U6TX_PORT       GPIO_PORTP_BASE
#define WAKEUP          GPIO_PIN_7
#define WAKEUP_PORT     GPIO_PORTM_BASE
#define STATUS1         GPIO_PIN_3
#define STATUS1_PORT    GPIO_PORTQ_BASE
#define STATUS2         GPIO_PIN_0
#define STATUS2_PORT    GPIO_PORTQ_BASE



void RN4678Fxn(UArg arg0, UArg arg1)
{
    UART_Handle uart;
    UART_Params uartParams;
    char input[8] = { '\0' };
    char buf[100] = { '\0' };
    int i;

    /* Create a UART with data processing off.*/
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uartParams.readMode = UART_MODE_BLOCKING;

    uart = UART_open(Board_UART6, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    // enter command mode with $$$
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0b00010000);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(uart, "$$$", sizeof("$$$")); // enter command mode
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    UART_read(uart, &input, 4); // CMD sollte zurückkommen
    Task_sleep(5);

    // connect to target
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0b00010000);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(uart, "C,0006668CB28E\r", sizeof("C,0006668CB28E\r"));
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    UART_read(uart, &input, 4);
    Task_sleep(5);

    while((GPIOPinRead(STATUS2_PORT, STATUS2) != 0x00)||(GPIOPinRead(STATUS1_PORT, STATUS1) == 0x00));
    System_printf("Connected");

    // leave command mode
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, RN4678_CTS);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(uart, "---\r", sizeof("---\r"));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    UART_read(uart, &input, 1);
    Task_sleep(5);

    Task_sleep(100);


    // MAC Address of XMC: 0006668CB28E

    while(1);
}

void init_bt()
{
    Task_Params taskUARTParams;
    Task_Handle taskUART;
    Error_Block eb;

    /*configure uart6 to interface bluetooth module*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    GPIOPinConfigure(U6RX);
    GPIOPinConfigure(U6TX);
    GPIOPinTypeUART(U6TX_PORT, U6TX_PIN | U6RX_PIN);

    UART_init();

    // Status Pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    GPIOPinTypeGPIOInput(STATUS2_PORT, STATUS2);
    GPIOPinTypeGPIOInput(STATUS1_PORT, STATUS1);

    // CTS/RTS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    GPIOPinTypeGPIOInput(RN4678_RTS_PORT, RN4678_RTS);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(RN4678_CTS_PORT, RN4678_CTS);
    GPIOPadConfigSet(RN4678_CTS_PORT, RN4678_CTS, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0b00000000);

    // SW_BTN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(SW_BTN_PORT, SW_BTN);
    GPIOPadConfigSet(SW_BTN_PORT, SW_BTN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, 0b00000000);

    // RST_N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    GPIOPinTypeGPIOOutput(RST_PORT, RST);
    GPIOPadConfigSet(RST_PORT, RST, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(RST_PORT, RST, 0b00010000);

    // WAKE_UP
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(WAKEUP_PORT, WAKEUP);
    GPIOPadConfigSet(WAKEUP_PORT, WAKEUP, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(WAKEUP_PORT, WAKEUP, 0b10000000);

    // init sequence
    SysCtlDelay((120000000 / 1000) * 500);
    GPIOPinWrite(RST_PORT, RST, 0b00000000);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(RST_PORT, RST, 0b00010000);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, 0b00000100);

    SysCtlDelay((120000000 / 1000) * 500);


    Error_init(&eb);
    Task_Params_init(&taskUARTParams);
    taskUARTParams.stackSize = 1024; /* stack in bytes */
    taskUARTParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskUART = Task_create((Task_FuncPtr)RN4678Fxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("Task RN4678 create failed");
    }
}
