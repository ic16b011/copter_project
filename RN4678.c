/**
 * @file RN4678.c
 *
 *
 *@brief The GPIO setup and data transfer functions for the RN4678 module
 *
 * @author Alexander Österreicher  es19m008
 * @author Dennis Kwame Addo       es19m006
 * @date 06.12.2019
 *
 * @details More information about the project can be found here
 * @see https://embsys.technikum-wien.at/mio/mes/1/esw/project/project.html#_documentation
 *
 * @version 1.0
 *
 *
 */
#include "RN4678.h"

/*! uart   Global UART handler for UART write command */
UART_Handle uart;
/*! ready_for_data   Global 8 bit variable for controlling the data transfer */
uint8_t ready_for_data = 0;

/*!
 * @brief      This is used to send Multiwii command to the quadcopter via bluetooth.
 *             The header and the command are packed in one packet of bytes.
 *             Activate the GPIO port for sending data check if port is ready and
 *             send data using UART write and wait till bytes is completely written and turnoff
 *             GPIO pin.
 *
 *@param       data    Multiwii package in bytes to send.
 *@param       size    Total size of data to send.
 *
 *@result      Nothing
 * */
void send_pac(char *data, size_t size)
{
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, RN4678_CTS);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(uart, data, size);
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
}

/*!
 * @brief      This is used to send Multiwii command to the quadcopter via bluetooth.
 *             The header and the command are packed in one packet of bytes.
 *             Activate the GPIO port for sending data check if port is ready and
 *             send data using UART write and wait till bytes is completely written and turnoff
 *             GPIO pin.
 *
 *@param       cmd     Command to send to the quadcopter
 *@param       size    Total size of data to send.
 *@param       retsize Total size of the return bytes (input data).
 *@param       input   Input data from the commandline.
 *
 * PNB: This is used in the CLI application
 *
 *@result      Nothing
 * */
void start_com(char *cmd, uint8_t size, uint8_t retsize, char *input)
{
    char inp[16] = { '\0' };
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, RN4678_CTS);
    while(GPIOPinRead(RN4678_RTS_PORT, RN4678_RTS) != 0x00);
    UART_write(uart, cmd, size); // enter command mode
    while(UARTBusy(UART6_BASE));
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    UART_read(uart, &inp, retsize); // CMD sollte zur�ckkommen
    strcpy(input, inp);
#ifdef _DEBUG
    System_printf("%s\n", cmd);
    System_flush();
#endif
    Task_sleep(5);
}

/*!
 * @brief      This is RN4678 task. This task is managed by the RTOS
 *             task manager using the multi-task priority scheduling.
 *             A connection is made with the bluetooth device (RN4678 module)
 *             after setting up the UART. This glabal uart is then used
 *             to later send command to the (RN4678 module).
 *
 *@param       arg0    xdc data for passing commands to RTOS task
 *@param       arg1    xdc data for passing commands to RTOS task
 *
 * PNB: This is used in the CLI application
 *
 *@result      Nothing
 * */
void RN4678Fxn(UArg arg0, UArg arg1)
{
    UART_Params uartParams;

    char cmdCmd[3] = "$$$"; // enter command mode
    char connectCmd[15] = "C,0006668CB218\r"; // connect to copter with MAC-address
    char leaveCmd[5] = "---\r\n"; // leave command mode
    char ret[17];

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

#ifdef _DEBUG
    System_printf("UART initialized\n");
    System_flush();
#endif
    Task_sleep(10);

    // enter command mode with $$$
    start_com(cmdCmd, sizeof(cmdCmd), 4, ret);

    if(strstr(ret, "CMD") != NULL)
    {
        // connect to copter
        start_com(connectCmd, sizeof(connectCmd), 16, ret);
        if(strstr(ret, "CONNECT") != NULL)
        {
            // wait for connected status
            while((GPIOPinRead(STATUS2_PORT, STATUS2) != 0x00) || (GPIOPinRead(STATUS1_PORT, STATUS1) == 0x00));
#ifdef _DEBUG
            System_printf("Connected\n");
            System_flush();
#endif

            // leave command mode
            start_com(leaveCmd, sizeof(leaveCmd), 1, ret);

            // flush UART In-Buffer
            while(UARTCharsAvail(UART6_BASE))
            {
                UART_read(uart, &ret, 1);
            }
        }
    }

    Task_sleep(100);

#ifdef _DEBUG
    System_printf("Begin data\n");
    System_flush();
#endif
    ready_for_data = 1;
}

/*!
 * @brief      Setup of the GPIO pins and Peripherable ports for the RN4678 module
 *
 *@param       void    nothing
 *@result      nothing but log and abort of failure to setup task for the BLUE Module
 * */
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
    GPIOPinWrite(RST_PORT, RST, 0);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(RST_PORT, RST, RST);
    SysCtlDelay((120000000 / 1000) * 100);
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, SW_BTN);
    SysCtlDelay((120000000 / 1000) * 500);

    // wait for the correct status of the RN4678
    while((GPIOPinRead(STATUS2_PORT, STATUS2) != 0x00) && (GPIOPinRead(STATUS1_PORT, STATUS1) != 0x00));
    SysCtlDelay((120000000 / 1000) * 500);

    Error_init(&eb);
    Task_Params_init(&taskRN4678Params);
    taskRN4678Params.stackSize = 1024; /* stack in bytes */
    taskRN4678Params.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskRN4678 = Task_create((Task_FuncPtr)RN4678Fxn, &taskRN4678Params, &eb);
    if (taskRN4678 == NULL) {
        System_abort("Task RN4678 create failed");
    }
}
