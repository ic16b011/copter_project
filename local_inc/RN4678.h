/**
 * @file RN4678.h
 *
 *
 *@brief The GPIO setup and data transfer functions for the RN4678 module
 *
 *
 * @author Alexander Österreicher  es19m008
 * @author Dennis Kwame Addo       es19m006
 * @details More information about the project can be found here
 * @see https://embsys.technikum-wien.at/mio/mes/1/esw/project/project.html#_documentation
 *
 * @date 06.12.2019
 *
 * @version 1.0
 *
 * @todo All implentations must be contained in one method structure unless these functions
 * @     are required by other programs/methods
 *
 */

#ifndef LOCAL_INC_RN4678_H_
#define LOCAL_INC_RN4678_H_

/*\include
 * -------------------------------------------------------------- includes --
 */
#include <Board.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <inc/hw_memmap.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/std.h>

/*!
 * --------------------------------------------------------------- defines --
 */
#define SW_BTN          GPIO_PIN_2
#define SW_BTN_PORT     GPIO_PORTD_BASE
#define RST             GPIO_PIN_4
#define RST_PORT        GPIO_PORTP_BASE
#define RN4678_RTS      GPIO_PIN_5      /**< is CTS from TivaBoard */
#define RN4678_RTS_PORT GPIO_PORTP_BASE
#define RN4678_CTS      GPIO_PIN_4      /**< is RTS from TivaBoard   */
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

/**\fn
 * ------------------------------------------------------------- functions --
 * */
void init_bt();
extern void send_pac(char *data, size_t size);

#endif /* LOCAL_INC_RN4678_H_ */
