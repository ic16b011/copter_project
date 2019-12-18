/*
 *  ======== StartBIOS.c ========
 *
 */

/* XDCtools Header files */
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Board Header files */
#include <Board.h>

/* Application headers */
#include <RN4678.h>

/*
 * NOTE: All local variables in main() will be discarded once BIOS_start() is called.
 */

int main(void)
{
    uint32_t ui32SysClock;

    /* Call board init functions. */
    ui32SysClock = Board_initGeneral(120*1000*1000);
    (void)ui32SysClock; // We don't really need this (yet)

    // initialize the RN4678 and start the communication task
    init_bt();
    System_printf("Created RN4678 Task\n");
    System_flush();

    /* SysMin will only print to the console upon calling flush or exit */
    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}
