/*
 * TI_Joystick.c
 *
 *  Created on: 18 Dec 2019
 *      Author: addod
 */


#include <TI_Joystick.h>

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/GPIO.h>
#include <driverlib/sysctl.h>
#include <Board.h>

static Bool qpArmToggle = false;


void joystick_fnx(UArg arg0 );


void gpioSeLFxn0(unsigned int index)
{
    // JoyStick Select Button
    //
        System_printf("JoyStick Select Pressed......\n");
        System_flush();

        qpArmToggle ^= true;

}


void ADC0_HandlerInt(void)
{
    uint32_t adcSamples[4];
    static uint32_t ui32JoysX = 0;
    static uint32_t ui32JoysY = 0;

    ADCIntClear(JOYS_ADC_BASE, 1);
    ADCSequenceDataGet(JOYS_ADC_BASE, 1, adcSamples);
    ui32JoysX = adcSamples[0];
    ui32JoysY = adcSamples[1];
}


void EdM_ADC_Init(void)
{
    /*
     * Center of the Joystick is a normal Button
     * Set it up
     *
     * This will be used later for locking the Qcopter Position or the Joystick
     * */
    GPIO_setCallback(Board_EduMIKI_SEL, gpioSeLFxn0);
    GPIO_enableInt(Board_EduMIKI_SEL);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //Done in the main by the Board_general

    //GPIOPinTypeADC(JOYS_GPIO_BASE, JOYS_X | JOYS_Y);
    GPIOPinTypeADC(JOYS_GPIO_BASE, JOYS_X | JOYS_Y | JOyACC_X |JOyACC_Y | JOyACC_Z );

    //ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_EIGHTH, 1);

    //Use the TI internal sampling algorithm
   // ADCHardwareOversampleConfigure(ADC0_BASE,64); // each sample in the ADC FIFO will be the result of 64 measurements being averaged together


    ADCClockConfigSet(JOYS_ADC_BASE| JOyACC_ADC_BASE , ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_EIGHTH, 1);
    ADCHardwareOversampleConfigure(JOYS_ADC_BASE | JOyACC_ADC_BASE ,64); // each sample in the ADC FIFO will be the result of 64 measurements being averaged together



    ADCSequenceDisable(JOYS_ADC_BASE, 1);
    ADCSequenceDisable(JOyACC_ADC_BASE, 2); //ACC


    ADCSequenceConfigure(JOYS_ADC_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(JOyACC_ADC_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(JOYS_ADC_BASE, 1, 0, JOYS_CH_X);
    ADCSequenceStepConfigure(JOYS_ADC_BASE, 1, 1, JOYS_CH_Y | ADC_CTL_IE | ADC_CTL_END);

    //For ACC
    ADCSequenceStepConfigure(JOyACC_ADC_BASE, 2, 0, JOyACC_CH_X);
    ADCSequenceStepConfigure(JOyACC_ADC_BASE, 2, 1, JOyACC_CH_Y);
    ADCSequenceStepConfigure(JOyACC_ADC_BASE, 2, 2, JOyACC_CH_Z | ADC_CTL_IE | ADC_CTL_END);



    //TimerConfigure(JOYS_TIMER_BASE, TIMER_CFG_PERIODIC);
    //TimerLoadSet(JOYS_TIMER_BASE, TIMER_A, SysCtlClockGet()/JOY_SAMPLE_RATE);
    //TimerControlTrigger(JOY_TIMER_BASE, TIMER_A, true);

    ADCSequenceEnable(JOYS_ADC_BASE, 1);
    ADCSequenceEnable(JOyACC_ADC_BASE, 2);
    //TimerEnable(JOYS_TIMER_BASE, TIMER_A);


#if 0 //fixme: later
    ADCIntRegister(JOY_ADC_BASE, 1, ADC0_HandlerInt);
    ADCIntEnable(JOY_ADC_BASE, 1);
#endif


}

void setUpJoyStick_Task(void)
{

    Task_Params old_taskParams;
    Task_Handle old_taskHandle;
    Error_Block eb;
    Error_init(&eb);

    Task_Params_init(&old_taskParams);
    old_taskParams.stackSize = 2024; //Stacksize in bytes
    old_taskParams.priority = 15; // 0-15, 15 being highest priority
    old_taskParams.arg0 = (UArg) 1;
    old_taskHandle = Task_create((Task_FuncPtr) joystick_fnx, &old_taskParams, &eb);

    if (old_taskHandle == NULL)
    {
        System_abort("Create Joystick_task_setup failed");
    }

    System_printf("Task set up done\n");
    System_flush();

}


void joystick_fnx(UArg arg0 )
{

    System_printf("init done\n");
    System_flush();

    uint32_t adcSamples[6];
    static uint32_t ui32JoyX = 0;
    static uint32_t ui32JoyY = 0;

    static uint32_t ui32AccX = 0;
    static uint32_t ui32AccY = 0;
    static uint32_t ui32AccZ = 0;




    while (1)
    {

        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);
        while (!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }
        ADCSequenceDataGet(ADC0_BASE, 1, adcSamples);

        //Delay a bit and read the ADC1
        SysCtlDelay(200);

        ADCIntClear(JOyACC_ADC_BASE, 2);
        ADCProcessorTrigger(JOyACC_ADC_BASE, 2);
        while (!ADCIntStatus(JOyACC_ADC_BASE, 2, false))
        {
        }
        ADCSequenceDataGet(JOyACC_ADC_BASE, 2, &adcSamples[2]);

        ui32JoyX = adcSamples[0];
        ui32JoyY = adcSamples[1];

        ui32AccX = adcSamples[2];
        ui32AccY = adcSamples[3];
        ui32AccZ = adcSamples[4];
#ifdef _DEBUG
        System_printf("Joystick X-Axis: %u Y-Axis: %u\n",ui32JoyX,ui32JoyY);
        System_flush();
#endif

    }



}


void arm(raw_rc_frame *armF)
{

    System_printf("Arming\n");
    System_flush();

    // the sequence value of yaw to be armed
    armF->yaw = 2000;

}


void disarm(raw_rc_frame *armF)
{

    System_printf("DisArming\n");
    System_flush();
    armF->yaw = 1000; //DEFAULT

}


void set_flight_controls(raw_rc_frame frame){

    // limit to [1000:2000] range
    if (frame.roll > 2000){frame.roll = 2000;}
    if (frame.roll < 1000){frame.roll = 1000;}
    if (frame.pitch > 2000){frame.pitch = 2000;}
    if (frame.pitch < 1000){frame.pitch = 1000;}
    if (frame.yaw > 2000){frame.yaw = 2000;}
    if (frame.yaw < 1000){frame.yaw = 1000;}
    if (frame.throttle > 2000){frame.throttle = 2000;}
    if (frame.throttle < 1000){frame.throttle = 1000;}

    const uint8_t payload_size = 8;

    uint16_t payload[payload_size] = {frame.roll, frame.pitch, frame.yaw, frame.throttle, frame.aux1, 0, 0, 0};


    // Send the RC values to be set on Flight Controller
    //msp_send(msp_set_raw_rc, payload_size, payload);
}
