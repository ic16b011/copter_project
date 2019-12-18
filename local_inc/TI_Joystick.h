/*
 * TI_Joystick.h
 *
 *  Created on: 18 Dec 2019
 *      Author: addod
 */

#ifndef LOCAL_INC_TI_JOYSTICK_H_
#define LOCAL_INC_TI_JOYSTICK_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"


#define JOYS_GPIO_BASE GPIO_PORTE_BASE
#define JOYS_ADC_BASE ADC0_BASE
#define JOYS_TIMER_BASE TIMER0_BASE

#define JOYS_SAMPLE_RATE 4

#define JOYS_X GPIO_PIN_4
#define JOYS_Y GPIO_PIN_3
#define JOYS_CH_X ADC_CTL_CH9
#define JOYS_CH_Y ADC_CTL_CH0


/*
 * Gt All the stuffs for the ADC ready for the TI EDUMIKI Joystick Controller
 * */

extern void EdM_ADC_Init(void);

extern void setUpJoyStick_Task(void);

/*
 * Getting the Joystick Display working to display some qcopter data.
 * */
extern void initSPI(void);
extern void send_to_Screen(char data);

#endif /* LOCAL_INC_TI_JOYSTICK_H_ */
