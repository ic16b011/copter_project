/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
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

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include "Blink_Task.h"

/*
 *  Perform blink operation on LED given as arg0, with period defined by arg1.
 */
void BlinkFxn(UArg arg0, UArg arg1)
{
    led_descriptor_t *led_desc = (led_descriptor_t *)arg0;
    uint32_t wait_ticks = (uint32_t)arg1;
    /* GPIO driverlib API uses same bit pattern for GPIO mask and value. */
    uint8_t ui8val = (uint8_t)led_desc->led;

    while(1) {

        ui8val ^= (uint8_t)led_desc->led; // initially off
        GPIOPinWrite (led_desc->port_base, led_desc->led, ui8val);
        Task_sleep(wait_ticks);

    }
}


/*
 *  Setup task function
 */
int setup_Blink_Task(led_descriptor_t *led_desc, uint32_t wait_ticks)
{
    Task_Params taskLedParams;
    Task_Handle taskLed;
    Error_Block eb;

    /* Configure GPIO port_base according to LED descriptor */
    GPIOPinTypeGPIOOutput(led_desc->port_base, led_desc->led);
    
    /* Create blink task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.stackSize = 1024; /* stack in bytes */
    taskLedParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskLedParams.arg0 = (UArg)led_desc; /* pass led descriptor as arg0 */
    taskLedParams.arg1 = (UArg)wait_ticks; /* pass periods in ticks as arg1 */
    taskLed = Task_create((Task_FuncPtr)BlinkFxn, &taskLedParams, &eb);
    if (taskLed == NULL) {
        System_abort("TaskLed create failed");
    }

    return (0);
}
