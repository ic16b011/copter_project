/*! \file UART_Task.h
    \brief UART task
    \author Matthias Wenzl
    \author Michael Kramer


    UART Task example.

*/

#ifndef UART_TASK_H_
#define UART_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>


/*! \fn UARTFxn
 *  \brief Execute UART Task
 *
 *
 *   \param arg0 void
 *   \param arg1 void
 *
 */
void UARTFxn(UArg arg0, UArg arg1);

/*! \fn setup_UART_Task
 *  \brief Setup UART task
 *
 *  Setup UART task
 *  Task has highest priority and receives 1kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_UART_Task(void);

#endif
