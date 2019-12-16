/*
 * RN4678.c
 *
 *  Created on: 06.12.2019
 *      Author: Alex
 */
#include "RN4678.h"

// send data packet
void send_pac(UART_Handle *uart, char *data, uint8_t size)
{
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, RN4678_CTS);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(*uart, data, size);
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
}

// send commands to RN4678
void start_com(UART_Handle *uart, char *cmd, uint8_t size, uint8_t retsize, char *input)
{
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, RN4678_CTS);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(*uart, cmd, size); // enter command mode
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    UART_read(*uart, &input, retsize); // CMD sollte zurückkommen
    System_printf("%s\n", cmd);
    System_flush();
    Task_sleep(5);
}

// cyclic task for RN4678
void RN4678Fxn(UArg arg0, UArg arg1)
{
    UART_Handle uart;
    UART_Params uartParams;

    char cmdCmd[3] = "$$$"; // enter command mode
    char connectCmd[15] = "C,0006668CB28E\r"; // connect to copter with MAC-address
    char leaveCmd[5] = "---\r\n"; // leave command mode
    char ret[16];
    char input;
    char buf[100];

    uint8_t i;

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
    start_com(&uart, cmdCmd, sizeof(cmdCmd), 4, ret);

    if(strstr(ret, "CMD") != NULL)
    {
        // connect to copter
        start_com(&uart, connectCmd, sizeof(connectCmd), 16, ret);
        if(strstr(ret, "CONNECT") != NULL)
        {
            // wait for connected status
            while((GPIOPinRead(STATUS2_PORT, STATUS2) != 0x00) || (GPIOPinRead(STATUS1_PORT, STATUS1) == 0x00));
            System_printf("Connected\n");
            System_flush();

            // leave command mode
            start_com(&uart, leaveCmd, sizeof(leaveCmd), 1, ret);

            // flush UART In-Buffer
            while(UARTCharsAvail(UART6_BASE))
            {
                UART_read(uart, &ret, 1);
            }
        }
    }

    Task_sleep(100);

    /* Get Version of the drone via CLI mode
     * TODO: TEST!!!
     */
    send_pac(&uart, "#version", sizeof("#version"));
    i = 0;
    do
    {
        UART_read(uart, &input, 1);
        buf[i] = input;
        i++;
    }while(UARTCharsAvail(UART6_BASE));
    send_pac(&uart, "#exit", sizeof("#exit"));


    System_printf("Begin data\n");
    char toSend[16];
    int speed = 1500;
    int arm = 0;
    int azimuth = 1500;
    int pitch = 1500;
    int roll = 1500;

    toSend[0] = 0x24;
    toSend[1] = 0x4D;
    toSend[2] = 0x3C;
    toSend[3] = 0x0A;
    toSend[4] = 0xC8;

    toSend[5] = pitch;
    toSend[6] = (pitch >> 8);
    toSend[7] = roll;
    toSend[8] = (roll >> 8);
    toSend[9] = speed;
    toSend[10] = (speed >> 8);
    toSend[11] = azimuth;
    toSend[12] = (azimuth >> 8);

    // arm
        //b[13] = 0xe8;
        //b[14] = 0x03;
    // disarm
    toSend[13] = 0xd0;
    toSend[14] = 0x07;

    char checksum = 0;

    for (i = 3; i < 16 - 1; i++)
    {
        checksum ^= toSend[i];
    }

    toSend[15] = checksum;

    while(1)
    {
        send_pac(&uart, toSend, sizeof(toSend));
        Task_sleep(10);
    }
}

void init_bt()
{
    Task_Params taskRN4678Params;
    Task_Handle taskRN4678;
    Error_Block eb;

    /*configure uart6 as interface to bluetooth module*/
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
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);

    // SW_BTN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(SW_BTN_PORT, SW_BTN);
    GPIOPadConfigSet(SW_BTN_PORT, SW_BTN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, 0);

    // RST_N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    GPIOPinTypeGPIOOutput(RST_PORT, RST);
    GPIOPadConfigSet(RST_PORT, RST, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(RST_PORT, RST, RST);

    // WAKE_UP
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(WAKEUP_PORT, WAKEUP);
    GPIOPadConfigSet(WAKEUP_PORT, WAKEUP, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinWrite(WAKEUP_PORT, WAKEUP, WAKEUP);

    // init sequence
    SysCtlDelay((120000000 / 1000) * 500);
    GPIOPinWrite(RST_PORT, RST, RST);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(RST_PORT, RST, RST);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, SW_BTN);

    SysCtlDelay((120000000 / 1000) * 500);
    //SysCtlDelay((120000000 / 1000) * 5000);
    while((GPIOPinRead(STATUS2_PORT, STATUS2) != 0x00) && (GPIOPinRead(STATUS1_PORT, STATUS1) != 0x00));

    Error_init(&eb);
    Task_Params_init(&taskRN4678Params);
    taskRN4678Params.stackSize = 1024; /* stack in bytes */
    taskRN4678Params.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskRN4678 = Task_create((Task_FuncPtr)RN4678Fxn, &taskRN4678Params, &eb);
    if (taskRN4678 == NULL) {
        System_abort("Task RN4678 create failed");
    }
}
