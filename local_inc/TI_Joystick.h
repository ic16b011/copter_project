/**
 * @file TI_Joystick.h
 *
 *
 *@brief Application for getting Joystick data, x y-axis ADC values, accelerometer
 *       and converting them to the right values for controlling the quadcopter
 *
 * @author Alexander Österreicher  es19m008
 * @author Dennis Kwame Addo       es19m006
 * @details More information about the project can be found here
 * @see https://embsys.technikum-wien.at/mio/mes/1/esw/project/project.html#_documentation
 *
 * @date 18.12.2019
 *
 * @version 1.0
 *
 * @todo All implentations must be contained in one method structure unless these functions
 * @     are required by other programs/methods
 *
 */

#ifndef LOCAL_INC_TI_JOYSTICK_H_
#define LOCAL_INC_TI_JOYSTICK_H_

/*\include
 * -------------------------------------------------------------- includes --
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Event.h>

#include <RN4678.h>

/*!
 * @typedef     raw_rc_frame
*/
typedef struct raw_rc_frame {
    uint16_t roll;            /**<  16 bit Range [1000;2000] */
    uint16_t pitch;           /**<  16 bit Range [1000;2000] */
    uint16_t throttle;        /**<  16 bit Range [1000;2000] */
    uint16_t azimuth;         /**<  16 bit Range [1000;2000] */
    uint8_t arm;              /**<  */
}raw_rc_frame;

/*!
 * --------------------------------------------------------------- defines --
 */
#define JOYS_GPIO_BASE GPIO_PORTE_BASE
#define JOYS_ADC_BASE ADC0_BASE
#define JOYS_TIMER_BASE TIMER0_BASE

#define JOYS_SAMPLE_RATE 4

#define JOYS_X GPIO_PIN_4
#define JOYS_Y GPIO_PIN_3
#define JOYS_CH_X ADC_CTL_CH9
#define JOYS_CH_Y ADC_CTL_CH0


#define JOyACC_X GPIO_PIN_0
#define JOyACC_Y GPIO_PIN_1
#define JOyACC_Z GPIO_PIN_2

#define JOyACC_ADC_BASE ADC1_BASE

#define JOyACC_CH_X ADC_CTL_CH3
#define JOyACC_CH_Y ADC_CTL_CH2
#define JOyACC_CH_Z ADC_CTL_CH1


/*
 * Gt All the stuffs for the ADC ready for the TI EDUMIKI Joystick Controller
 * */
/**\fn
 * ------------------------------------------------------------- functions --
 * */
extern void EdM_ADC_Init(void);
extern void setUpJoyStick_Task();

/*
 * Getting the Joystick Display working to display some qcopter data.
 * */
extern void initSPI(void);
extern void send_to_Screen(char data);


#endif /* LOCAL_INC_TI_JOYSTICK_H_ */
