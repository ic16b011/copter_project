/*
 *  ======== StartBIOS.c ========
 */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Currently unused RTOS headers that are needed
 * for advanced features like IPC. */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Timer.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <Blink_Task.h>
#include <UART_Task.h>
#include <RN4678.h>

/*
 * NOTE: All local variables in main() will be discarded once BIOS_start() is called.
 */

int main(void)
{
    uint32_t ui32SysClock;
    static led_descriptor_t led_desc[2];

    /* Call board init functions. */
    ui32SysClock = Board_initGeneral(120*1000*1000);
    (void)ui32SysClock; // We don't really need this (yet)

    init_bt();

    led_desc[0].port_base = GPIO_PORTN_BASE;
    led_desc[0].led = GPIO_PIN_1;
    /* Initialize+start Blink Task*/
    (void)setup_Blink_Task(&led_desc[0], 500);
    /* System_printf() is VERY slow!*/
    System_printf("Created Blink Task1\n");
    System_flush();

    led_desc[1].port_base = GPIO_PORTF_BASE;
    led_desc[1].led = GPIO_PIN_0;
    /*Initialize+start Blink Task*/
    (void)setup_Blink_Task(&led_desc[1], 250/*ticks to wait*/);
    System_printf("Created Blink Task2\n");
    System_flush();

    /*Initialize+start UART Task*/
    (void)setup_UART_Task();
    System_printf("Created UART Task\n");
    System_flush();

    /* SysMin will only print to the console upon calling flush or exit */
    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}
