/*
 * RN4678.c
 *
 *  Created on: 06.12.2019
 *      Author: Alex
 */
#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>

#include "RN4678.h"

#define SW_BTN          GPIO_PIN_2
#define SW_BTN_PORT     GPIO_PORTD_BASE
#define RST             GPIO_PIN_4
#define RST_PORT        GPIO_PORTP_BASE
#define RN4678_RTS      GPIO_PIN_5      // is CTS from TivaBoard
#define RN4678_RTS_PORT GPIO_PORTP_BASE
#define RN4678_CTS      GPIO_PIN_0      // is RTS from TivaBoard
#define RN4678_CTS_PORT GPIO_PORTA_BASE
#define U6RX            GPIO_PP0_U6RX
#define U6RX_PIN        GPIO_PIN_0
#define U6RX_PORT       GPIO_PORTP_BASE
#define U6TX            GPIO_PP1_U6TX
#define U6TX_PIN        GPIO_PIN_1
#define U6TX_PORT       GPIO_PORTP_BASE
#define WAKEUP          GPIO_PIN_7
#define WAKEUP_PORT     GPIO_PORTM_BASE
#define STATUS2         GPIO_PIN_0
#define STATUS2_PORT    GPIO_PORTQ_BASE
#define STATUS1         GPIO_PIN_3
#define STATUS1_PORT    GPIO_PORTQ_BASE

void init_bt()
{
    GPIOPinTypeGPIOOutput(SW_BTN_PORT, SW_BTN);
    GPIOPinTypeGPIOOutput(RST_PORT, RST);
    GPIOPinTypeGPIOInput(RN4678_RTS_PORT, RN4678_RTS);
    GPIOPadConfigSet(RN4678_RTS_PORT, RN4678_RTS,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);
    GPIOPinTypeGPIOOutput(RN4678_CTS_PORT, RN4678_CTS);
    GPIOPinConfigure(U6RX);
    GPIOPinConfigure(U6TX);
    GPIOPinTypeUART(U6TX_PORT, U6RX_PIN | U6TX_PIN);
    GPIOPinTypeGPIOOutput(WAKEUP_PORT, WAKEUP);
    GPIOPinTypeGPIOInput(STATUS2_PORT, STATUS2);
    GPIOPadConfigSet(STATUS2_PORT, STATUS2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);
    GPIOPinTypeGPIOInput(STATUS1_PORT, STATUS1);
    GPIOPadConfigSet(STATUS1_PORT, STATUS1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);

    GPIOPinWrite(SW_BTN_PORT, SW_BTN, 0);
    GPIOPinWrite(RST_PORT, RST, 0);
    GPIOPinWrite(RN4678_CTS_PORT, RN4678_CTS, 0);
    GPIOPinWrite(WAKEUP_PORT, WAKEUP, 0);
    SysCtlDelay(1);

    // initialization phase
    GPIOPinWrite(SW_BTN_PORT, SW_BTN, SW_BTN);
    while(!GPIOPinRead(STATUS1_PORT, STATUS1)||!GPIOPinRead(STATUS2_PORT, STATUS2));
    SysCtlDelay(360000); // wait 36ms according to datasheet
    GPIOPinWrite(RST_PORT, RST, RST);
    while(GPIOPinRead(STATUS1_PORT, STATUS1)); // wait for STATUS1 Pin getting low to signal that the BT-module is ready
    SysCtlDelay(1);
}

    /*This is the well-known default UUID for bluetooth SPP-Devices(used for encryption of endpoint)
    private static final String BLUETOOTH_UUID_STRING = "00001101-0000-1000-8000-00805F9B34FB";
    private static final String BLUETOOTH_MODUL_NAME = "XMC-Bluetooth";


   UART Baudrate: 115200

   Workflow: AOK if CMD is ok, ERR if not; (C,MAC-Address --> Connects to MAC-Address)
   $$$'\r' --> enter command mode
   - return CMD if ok
   SM,0'\r' --> normal mode
   SA,2'\r' --> Security "Just works" no pin required at connection
   F,3,"XMC-Bluetooth" --> scan for nearby BT devices with name XMC-Bluetooth
   SR,MAC-Address --> Stores MAC-Address, can be used to check if the correct device is found
   SM,6'\r' --> pairing mode to connect to XMC
   R,1'\r' --> reboot the device
   ---'\r' --> return to data mode

   while(status1==high || status2==low);
   waitfor(500ms); --> eventuell weniger?? probieren bzw. status pins messen!
   wenn status1 = low und status2 = high --> connected!!


