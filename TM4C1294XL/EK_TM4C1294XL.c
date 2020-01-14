/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== EK_TM4C1294XL.c ========
 *  This file is responsible for setting up the board specific items for the
 *  EK_TM4C1294XL board.
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/sysctl.h>
#include <driverlib/i2c.h>
#include <driverlib/ssi.h>
#include <driverlib/uart.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>


#include <EK_TM4C1294XL.h>


#ifndef TI_DRIVERS_UART_DMA
#define TI_DRIVERS_UART_DMA 0
#endif

#ifndef TI_EXAMPLES_PPP
#define TI_EXAMPLES_PPP 0
#else
/* prototype for NIMU init function */
extern int USBSerialPPP_NIMUInit();
#endif

/*
 *  =============================== DMA ===============================
 */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#endif
static tDMAControlTable dmaControlTable[32];
static bool dmaInitialized = false;

/* Hwi_Struct used in the initDMA Hwi_construct call */
static Hwi_Struct dmaHwiStruct;

/* Hwi_Struct used in the usbBusFault Hwi_construct call */
//static Hwi_Struct usbBusFaultHwiStruct;

/*
 *  ======== dmaErrorHwi ========
 */
static Void dmaErrorHwi(UArg arg)
{
#ifdef _DEBUG
    System_printf("DMA error code: %d\n", uDMAErrorStatusGet());
#endif
    uDMAErrorStatusClear();

    System_abort("DMA error!!");
}

/*
 *  ======== EK_TM4C1294XL_usbBusFaultHwi ========
 */
//static Void EK_TM4C1294XL_usbBusFaultHwi(UArg arg)
//{
//    /*
//     *  This function should be modified to appropriately manage handle
//     *  a USB bus fault.
//    */
//    System_printf("USB bus fault detected.");
//    Hwi_clearInterrupt(INT_GPIOQ4);
//    System_abort("USB error!!");
//}

/*
 *  ======== EK_TM4C1294XL_initDMA ========
 */
void EK_TM4C1294XL_initDMA(void)
{
    Error_Block eb;
    Hwi_Params  hwiParams;

    if (!dmaInitialized) {

        Error_init(&eb);

        Hwi_Params_init(&hwiParams);
        Hwi_construct(&(dmaHwiStruct), INT_UDMAERR, dmaErrorHwi,
                      &hwiParams, &eb);
        if (Error_check(&eb)) {
            System_abort("Couldn't construct DMA error hwi");
        }

        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
        uDMAEnable();
        uDMAControlBaseSet(dmaControlTable);

        dmaInitialized = true;
    }
}

/*
 *  =============================== General ===============================
 */
/*
 *  ======== EK_TM4C1294XL_initGeneral ========
 */
uint32_t EK_TM4C1294XL_initGeneral(uint32_t sysclock)
{
   uint32_t ui32SysClock;
   ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                      SYSCTL_OSC_MAIN |
                                      SYSCTL_USE_PLL |
                                      SYSCTL_CFG_VCO_480), sysclock);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);
    return ui32SysClock;
}

/*
 *  =============================== EMAC ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(EMAC_config, ".const:EMAC_config")
#pragma DATA_SECTION(emacHWAttrs, ".const:emacHWAttrs")
#pragma DATA_SECTION(NIMUDeviceTable, ".data:NIMUDeviceTable")
#endif

#include <ti/drivers/EMAC.h>
#include <ti/drivers/emac/EMACSnow.h>

/*
 *  Required by the Networking Stack (NDK). This array must be NULL terminated.
 *  This can be removed if NDK is not used.
 *  Double curly braces are needed to avoid GCC bug #944572
 *  https://bugs.launchpad.net/gcc-linaro/+bug/944572
 */
#if TI_EXAMPLES_PPP
/* Use PPP driver for PPP example only */
NIMU_DEVICE_TABLE_ENTRY  NIMUDeviceTable[2] = {{USBSerialPPP_NIMUInit}, {NULL}};
#else
/* Default: use Ethernet driver */
NIMU_DEVICE_TABLE_ENTRY  NIMUDeviceTable[2] = {{EMACSnow_NIMUInit}, {NULL}};
#endif

/* EMAC objects */
EMACSnow_Object emacObjects[EK_TM4C1294XL_EMACCOUNT];

/*
 *  EMAC configuration structure
 *  Set user/company specific MAC octates. The following sets the address
 *  to ff-ff-ff-ff-ff-ff. Users need to change this to make the label on
 *  their boards.
 */
unsigned char macAddress[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

const EMACSnow_HWAttrs emacHWAttrs[EK_TM4C1294XL_EMACCOUNT] = {
    {EMAC0_BASE, INT_EMAC0, ~0 /* Interrupt priority */, macAddress}
};

const EMAC_Config EMAC_config[] = {
    {
        &EMACSnow_fxnTable,
        &emacObjects[0],
        &emacHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C1294XL_initEMAC ========
 */
void EK_TM4C1294XL_initEMAC(char *mac_addr)
{
    uint32_t ulUser0, ulUser1;

    /* Get the MAC address */
    if(mac_addr == NULL) {
        FlashUserGet(&ulUser0, &ulUser1);
        if ((ulUser0 != 0xffffffff) && (ulUser1 != 0xffffffff)) {
#ifdef _DEBUG
            System_printf("Using MAC address in flash\n");
            System_flush();
#endif
            /*
             *  Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
             *  address needed to program the hardware registers, then program the MAC
             *  address into the Ethernet Controller registers.
             */
            macAddress[0] = ((ulUser0 >>  0) & 0xff);
            macAddress[1] = ((ulUser0 >>  8) & 0xff);
            macAddress[2] = ((ulUser0 >> 16) & 0xff);
            macAddress[3] = ((ulUser1 >>  0) & 0xff);
            macAddress[4] = ((ulUser1 >>  8) & 0xff);
            macAddress[5] = ((ulUser1 >> 16) & 0xff);
        } else {
            System_abort("You need to supply a valid MAC address");
        }
    } else {
        memcpy(macAddress,mac_addr,6);
#ifdef _DEBUG
        System_printf("Using user supplied MAC address\n");
        System_flush();
#endif
    }

    GPIOPinConfigure(GPIO_PF0_EN0LED0);  /* EK_TM4C1294XL_USR_D3 */
    GPIOPinConfigure(GPIO_PF4_EN0LED1);  /* EK_TM4C1294XL_USR_D4 */
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);


    /* Once EMAC_init is called, EMAC_config cannot be changed */
    EMAC_init();
}

/*
 *  =============================== GPIO ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(GPIOTiva_config, ".const:GPIOTiva_config")
#endif

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOTiva.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in EK_TM4C1294XL.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Input pins */
    /* EK_TM4C1294XL_USR_SW1 */
    GPIOTiva_PJ_0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* EK_TM4C1294XL_USR_SW2 */
    GPIOTiva_PJ_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,

    /* EDuMiki_SEL for the Joystick Select Button*/
    GPIOTiva_PC_6 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,

    /* Output pins */
    /* EK_TM4C1294XL_USR_D1 */
    GPIOTiva_PN_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    /* EK_TM4C1294XL_USR_D2 */
    GPIOTiva_PN_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in EK_TM4C1294XL.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,  /* EK_TM4C1294XL_USR_SW1 */
    NULL,   /* EK_TM4C1294XL_USR_SW2 */
    NULL   /* EK_TM4C1294XL_EduMiki_SEL Joystick Select button */
};

/* The device-specific GPIO_config structure */
const GPIOTiva_Config GPIOTiva_config = {
    .pinConfigs = (GPIO_PinConfig *) gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *) gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  ======== EK_TM4C1294XL_initGPIO ========
 */
void EK_TM4C1294XL_initGPIO(void)
{
    /* Initialize peripheral and pins */
    GPIO_init();
}

/*
 *  =============================== I2C ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(I2C_config, ".const:I2C_config")
#pragma DATA_SECTION(i2cTivaHWAttrs, ".const:i2cTivaHWAttrs")
#endif

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/* I2C objects */
I2CTiva_Object i2cTivaObjects[EK_TM4C1294XL_I2CCOUNT];

/* I2C configuration structure, describing which pins are to be used */
const I2CTiva_HWAttrs i2cTivaHWAttrs[EK_TM4C1294XL_I2CCOUNT] = {
#if I2CM_7
    {I2C7_BASE, INT_I2C7, ~0 /* Interrupt priority */},
#endif
#if I2CM_8
    {I2C8_BASE, INT_I2C8, ~0 /* Interrupt priority */}
#endif
};

const I2C_Config I2C_config[] = {
#if I2CM_7
    {&I2CTiva_fxnTable, &i2cTivaObjects[EK_TM4C1294XL_I2C7], &i2cTivaHWAttrs[EK_TM4C1294XL_I2C7]},
#endif
#if I2CM_8
    {&I2CTiva_fxnTable, &i2cTivaObjects[EK_TM4C1294XL_I2C8], &i2cTivaHWAttrs[EK_TM4C1294XL_I2C8]},
#endif
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C1294XL_initI2C ========
 */
void EK_TM4C1294XL_initI2C(void)
{
    /* I2C7 Init */
    /*
     * NOTE: TI-RTOS examples configure pins PD0 & PD1 for SSI2 or I2C7.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
    /* Enable the peripheral */
#if I2CM_7
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C7);

	/* Configure the appropriate pins to be I2C instead of GPIO. */
	GPIOPinConfigure(GPIO_PD0_I2C7SCL);
	GPIOPinConfigure(GPIO_PD1_I2C7SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);
#endif
	/* I2C8 Init */
	/* Enable the peripheral */
#if I2CM_8
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C8);

	/* Configure the appropriate pins to be I2C instead of GPIO. */
	GPIOPinConfigure(GPIO_PA2_I2C8SCL);
	GPIOPinConfigure(GPIO_PA3_I2C8SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_3);
#endif
    I2C_init();
}



/*
 *  =============================== PWM ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(PWM_config, ".const:PWM_config")
#pragma DATA_SECTION(pwmTivaHWAttrs, ".const:pwmTivaHWAttrs")
#endif

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTiva.h>
#include <driverlib/pwm.h>

PWMTiva_Object pwmTivaObjects[EK_TM4C1294XL_PWMCOUNT];

/* PWM configuration structure */
const PWMTiva_HWAttrs pwmTivaHWAttrs[EK_TM4C1294XL_PWMCOUNT] = {
    {
        PWM0_BASE,
        PWM_OUT_0,
        PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN
    }
};

const PWM_Config PWM_config[] = {
    {&PWMTiva_fxnTable, &pwmTivaObjects[0], &pwmTivaHWAttrs[0]},
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C1294XL_initPWM ========
 */
void EK_TM4C1294XL_initPWM(void)
{
    /* Enable PWM peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    /*
     * Enable PWM output on GPIO pins.  PWM output is connected to an Ethernet
     * LED on the development board (D4).  The PWM configuration
     * below will disable Ethernet functionality.
     */
    GPIOPinConfigure(GPIO_PF0_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);

    PWM_init();
}

/*
 *  =============================== SDSPI ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(SDSPI_config, ".const:SDSPI_config")
#pragma DATA_SECTION(sdspiTivaHWattrs, ".const:sdspiTivaHWattrs")
#endif

#include <ti/drivers/SDSPI.h>
#include <ti/drivers/sdspi/SDSPITiva.h>

/* SDSPI objects */
SDSPITiva_Object sdspiTivaObjects[EK_TM4C1294XL_SDSPICOUNT];

/* SDSPI configuration structure, describing which pins are to be used */
const SDSPITiva_HWAttrs sdspiTivaHWattrs[EK_TM4C1294XL_SDSPICOUNT] = {
    {
        SSI2_BASE,          /* SPI base address */

        GPIO_PORTD_BASE,    /* SPI SCK PORT */
        GPIO_PIN_3,         /* SCK PIN */
        GPIO_PORTD_BASE,    /* SPI MISO PORT*/
        GPIO_PIN_0,         /* MISO PIN */
        GPIO_PORTD_BASE,    /* SPI MOSI PORT */
        GPIO_PIN_1,         /* MOSI PIN */
        GPIO_PORTH_BASE,    /* GPIO CS PORT */
        GPIO_PIN_2,         /* CS PIN */
    },
    {
        SSI3_BASE,          /* SPI base address */

        GPIO_PORTQ_BASE,    /* SPI SCK PORT */
        GPIO_PIN_0,         /* SCK PIN */
        GPIO_PORTQ_BASE,    /* SPI MISO PORT*/
        GPIO_PIN_3,         /* MISO PIN */
        GPIO_PORTQ_BASE,    /* SPI MOSI PORT */
        GPIO_PIN_2,         /* MOSI PIN */
        GPIO_PORTP_BASE,    /* GPIO CS PORT */
        GPIO_PIN_4,         /* CS PIN */
    }
};

const SDSPI_Config SDSPI_config[] = {
    {&SDSPITiva_fxnTable, &sdspiTivaObjects[0], &sdspiTivaHWattrs[0]},
    {&SDSPITiva_fxnTable, &sdspiTivaObjects[1], &sdspiTivaHWattrs[1]},
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C1294XL_initSDSPI ========
 */
void EK_TM4C1294XL_initSDSPI(void)
{
    /* SDSPI0 configuration */
    /* Enable the peripherals used by the SD Card */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    /* Configure pad settings */
    GPIOPadConfigSet(GPIO_PORTD_BASE,
                     GPIO_PIN_3 | GPIO_PIN_1,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    GPIOPadConfigSet(GPIO_PORTD_BASE,
                     GPIO_PIN_0,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPadConfigSet(GPIO_PORTH_BASE,
                     GPIO_PIN_2,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);

    /* SDSPI1 configuration */
    /* Enable the peripherals used by the SD Card */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

    /* Configure pad settings */
    GPIOPadConfigSet(GPIO_PORTQ_BASE,
                     GPIO_PIN_0 | GPIO_PIN_2,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    GPIOPadConfigSet(GPIO_PORTQ_BASE,
                     GPIO_PIN_3,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPadConfigSet(GPIO_PORTP_BASE,
                     GPIO_PIN_4,
                     GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    GPIOPinConfigure(GPIO_PQ0_SSI3CLK);
    GPIOPinConfigure(GPIO_PQ3_SSI3XDAT1);
    GPIOPinConfigure(GPIO_PQ2_SSI3XDAT0);

    /*
     *  These GPIOs are connected to PA2 and PA3 and need to be brought into a
     *  GPIO input state so they don't interfere with SPI communications.
     */
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3);

    SDSPI_init();
}

/*
 *  =============================== SPI ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(SPI_config, ".const:SPI_config")
#pragma DATA_SECTION(spiTivaDMAHWAttrs, ".const:spiTivaDMAHWAttrs")
#endif

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPITivaDMA.h>

/* SPI objects */
SPITivaDMA_Object spiTivaDMAObjects[EK_TM4C1294XL_SPICOUNT];
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(spiTivaDMAscratchBuf, 32)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=32
#elif defined(__GNUC__)
__attribute__ ((aligned (32)))
#endif
uint32_t spiTivaDMAscratchBuf[EK_TM4C1294XL_SPICOUNT];

/* SPI configuration structure */
const SPITivaDMA_HWAttrs spiTivaDMAHWAttrs[EK_TM4C1294XL_SPICOUNT] = {
#if SSIM_2
	{
        SSI2_BASE,
        INT_SSI2,
        ~0,         /* Interrupt priority */
        &spiTivaDMAscratchBuf[0],
        0,
        UDMA_SEC_CHANNEL_UART2RX_12,
        UDMA_SEC_CHANNEL_UART2TX_13,
        uDMAChannelAssign,
        UDMA_CH12_SSI2RX,
        UDMA_CH13_SSI2TX
    },
#endif
#if SSIM_3
    {
        SSI3_BASE,
        INT_SSI3,
        ~0,         /* Interrupt priority */
        &spiTivaDMAscratchBuf[1],
        0,
        UDMA_SEC_CHANNEL_TMR2A_14,
        UDMA_SEC_CHANNEL_TMR2B_15,
        uDMAChannelAssign,
        UDMA_CH14_SSI3RX,
        UDMA_CH15_SSI3TX
    }
#endif
};

const SPI_Config SPI_config[] = {
#if SSIM_2
    {&SPITivaDMA_fxnTable, &spiTivaDMAObjects[EK_TM4C1294XL_SPI2], &spiTivaDMAHWAttrs[EK_TM4C1294XL_SPI2]},
#endif
#if SSIM_3
    {&SPITivaDMA_fxnTable, &spiTivaDMAObjects[EK_TM4C1294XL_SPI3], &spiTivaDMAHWAttrs[EK_TM4C1294XL_SPI3]},
#endif
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C1294XL_initSPI ========
 */
void EK_TM4C1294XL_initSPI(void)
{
    /* SSI2 */
    /*
     * NOTE: TI-RTOS examples configure pins PD0 & PD1 for SSI2 or I2C7.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
#if SSIM_2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

	GPIOPinConfigure(GPIO_PD3_SSI2CLK);
	GPIOPinConfigure(GPIO_PD2_SSI2FSS);
	GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
	GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);

	GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                   GPIO_PIN_2 | GPIO_PIN_3);

#endif
#if SSIM_3
	/* SSI3 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

	GPIOPinConfigure(GPIO_PQ0_SSI3CLK);
	GPIOPinConfigure(GPIO_PQ1_SSI3FSS);
	GPIOPinConfigure(GPIO_PQ2_SSI3XDAT0);
	GPIOPinConfigure(GPIO_PQ3_SSI3XDAT1);

	GPIOPinTypeSSI(GPIO_PORTQ_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                   GPIO_PIN_2 | GPIO_PIN_3);

#endif
    EK_TM4C1294XL_initDMA();
    SPI_init();
}

/*
 *  =============================== UART ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(UART_config, ".const:UART_config")
#pragma DATA_SECTION(uartTivaHWAttrs, ".const:uartTivaHWAttrs")
#endif

#include <ti/drivers/UART.h>
#if TI_DRIVERS_UART_DMA
#include <ti/drivers/uart/UARTTivaDMA.h>

/* UART objects using DMA */
UARTTivaDMA_Object uartTivaObjects[EK_TM4C1294XL_UARTCOUNT];

/* UART DMA configuration structure */
const UARTTivaDMA_HWAttrs uartTivaHWAttrs[EK_TM4C1294XL_UARTCOUNT] = {
    {/* EK_TM4C1294XL_UART0 */
        UART0_BASE,
        INT_UART0,
        ~0,        /* Interrupt priority */
        UDMA_CH8_UART0RX,
        UDMA_CH9_UART0TX,
    }
};

const UART_Config UART_config[] = {
    {
        &UARTTivaDMA_fxnTable,
        &uartTivaObjects[0],
        &uartTivaHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

#else
#include <ti/drivers/uart/UARTTiva.h>

/* UART objects */
UARTTiva_Object uartTivaObjects[EK_TM4C1294XL_UARTCOUNT];
#define TIVA_RINGBUF_SIZE 32
unsigned char uartTivaRingBuffer[2][TIVA_RINGBUF_SIZE];

/* UART configuration structure */
const UARTTiva_HWAttrs uartTivaHWAttrs[EK_TM4C1294XL_UARTCOUNT] = {
    {/* EK_TM4C1294XL_UART0 */
        .baseAddr = UART0_BASE,
        .intNum = INT_UART0,
        .intPriority = ~0,
        .flowControl = UART_FLOWCONTROL_NONE,
        .ringBufPtr  = uartTivaRingBuffer[0],
        .ringBufSize = sizeof(TIVA_RINGBUF_SIZE)
    },
    {/* EK_TM4C1294XL_UART6 */
        .baseAddr = UART6_BASE,
        .intNum = INT_UART6,
        .intPriority = ~0,
        .flowControl = UART_FLOWCONTROL_NONE,
        .ringBufPtr  = uartTivaRingBuffer[1],
        .ringBufSize = sizeof(TIVA_RINGBUF_SIZE)
    }
};

const UART_Config UART_config[] = {
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[0],
        &uartTivaHWAttrs[0]
    },
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[1],
        &uartTivaHWAttrs[1]
    },
    {NULL, NULL, NULL}
};
#endif /* TI_DRIVERS_UART_DMA */

/*
 *  ======== EK_TM4C1294XL_initUART ========
 */
void EK_TM4C1294XL_initUART(void)
{
    /* Enable and configure the peripherals used by the uart. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART driver */
#if TI_DRIVERS_UART_DMA
    EK_TM4C1294XL_initDMA();
#endif
    UART_init();
}


/*
 *  =============================== USB ===============================
 */
/*
 *  ======== EK_TM4C1294XL_initUSB ========
 *  This function just turns on the USB
 */
//void EK_TM4C1294XL_initUSB(EK_TM4C1294XL_USBMode usbMode)
//{
//    Error_Block eb;
//    Hwi_Params  hwiParams;
//
//    /* Enable the USB peripheral and PLL */
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
//    SysCtlUSBPLLEnable();
//
//    /* Setup pins for USB operation */
//    GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//    GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
//
//    /* Additional configurations for Host mode */
//    if (usbMode == EK_TM4C1294XL_USBHOST) {
//        /* Configure the pins needed */
//        HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//        HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0xff;
//        GPIOPinConfigure(GPIO_PD6_USB0EPEN);
//        GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
//
//        /*
//         *  USB bus fault is routed to pin PQ4.  We create a Hwi to allow us
//         *  to detect power faults and recover gracefully or terminate the
//         *  program.  PQ4 is active low; set the pin as input with a weak
//         *  pull-up.
//         */
//        GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_4,
//                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
//        GPIOIntTypeSet(GPIO_PORTQ_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
//        GPIOIntClear(GPIO_PORTQ_BASE, GPIO_PIN_4);
//
//        /* Create a Hwi for PQ4 pin. */
//        Error_init(&eb);
//        Hwi_Params_init(&hwiParams);
//        Hwi_construct(&(usbBusFaultHwiStruct), INT_GPIOQ4,
//                      EK_TM4C1294XL_usbBusFaultHwi, &hwiParams, &eb);
//        if (Error_check(&eb)) {
//            System_abort("Couldn't construct USB bus fault hwi");
//        }
//    }
//}
//
///*
// *  =============================== USBMSCHFatFs ===============================
// */
///* Place into subsections to allow the TI linker to remove items properly */
//
//#if defined(__TI_COMPILER_VERSION__)
//#pragma DATA_SECTION(USBMSCHFatFs_config, ".const:USBMSCHFatFs_config")
//#pragma DATA_SECTION(usbmschfatfstivaHWAttrs, ".const:usbmschfatfstivaHWAttrs")
//#endif
//
//#include <ti/drivers/USBMSCHFatFs.h>
//#include <ti/drivers/usbmschfatfs/USBMSCHFatFsTiva.h>
//
///* USBMSCHFatFs objects */
//USBMSCHFatFsTiva_Object usbmschfatfstivaObjects[EK_TM4C1294XL_USBMSCHFatFsCOUNT];
//
///* USBMSCHFatFs configuration structure, describing which pins are to be used */
//const USBMSCHFatFsTiva_HWAttrs usbmschfatfstivaHWAttrs[EK_TM4C1294XL_USBMSCHFatFsCOUNT] = {
//    {INT_USB0, ~0 /* Interrupt priority */}
//};
//
//const USBMSCHFatFs_Config USBMSCHFatFs_config[] = {
//    {
//        &USBMSCHFatFsTiva_fxnTable,
//        &usbmschfatfstivaObjects[0],
//        &usbmschfatfstivaHWAttrs[0]
//    },
//    {NULL, NULL, NULL}
//};
//
///*
// *  ======== EK_TM4C1294XL_initUSBMSCHFatFs ========
// */
//void EK_TM4C1294XL_initUSBMSCHFatFs(void)
//{
//    /* Initialize the DMA control table */
//    EK_TM4C1294XL_initDMA();
//
//    /* Call the USB initialization function for the USB Reference modules */
//    EK_TM4C1294XL_initUSB(EK_TM4C1294XL_USBHOST);
//    USBMSCHFatFs_init();
//}

/*
 *  =============================== Watchdog ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(Watchdog_config, ".const:Watchdog_config")
#pragma DATA_SECTION(watchdogTivaHWAttrs, ".const:watchdogTivaHWAttrs")
#endif

#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogTiva.h>

/* Watchdog objects */
WatchdogTiva_Object watchdogTivaObjects[EK_TM4C1294XL_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogTiva_HWAttrs watchdogTivaHWAttrs[EK_TM4C1294XL_WATCHDOGCOUNT] = {
    /* EK_TM4C1294XL_WATCHDOG0 with 1 sec period at default CPU clock freq */
    {
        WATCHDOG0_BASE,
        INT_WATCHDOG,
        ~0,           /* Interrupt priority */
        80000000
    },
};

const Watchdog_Config Watchdog_config[] = {
    {&WatchdogTiva_fxnTable, &watchdogTivaObjects[0], &watchdogTivaHWAttrs[0]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C1294XL_initWatchdog ========
 *
 * NOTE: To use the other watchdog timer with base address WATCHDOG1_BASE,
 *       an additional function call may need be made to enable PIOSC. Enabling
 *       WDOG1 does not do this. Enabling another peripheral that uses PIOSC
 *       such as ADC0 or SSI0, however, will do so. Example:
 *
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG1);
 *
 *       See the following forum post for more information:
 *       http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/471/p/176487/654390.aspx#654390
 */
void EK_TM4C1294XL_initWatchdog(void)
{
    /* Enable peripherals used by Watchdog */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    /* Initialize the Watchdog driver */
    Watchdog_init();
}

/*
 *  =============================== WiFi ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(WiFi_config, ".const:WiFi_config")
#pragma DATA_SECTION(wiFiCC3100HWAttrs, ".const:wiFiCC3100HWAttrs")
#endif

#include <ti/drivers/WiFi.h>
#include <ti/drivers/wifi/WiFiCC3100.h>

/* WiFi objects */
WiFiCC3100_Object wiFiCC3100Objects[EK_TM4C1294XL_WIFICOUNT];

/* WiFi configuration structure */
const WiFiCC3100_HWAttrs wiFiCC3100HWAttrs[EK_TM4C1294XL_WIFICOUNT] = {
    {
        GPIO_PORTM_BASE, /* IRQ port */
        GPIO_PIN_3,      /* IRQ pin */
        INT_GPIOM,       /* IRQ port interrupt vector */

        GPIO_PORTH_BASE, /* CS port */
        GPIO_PIN_2,      /* CS pin */

        GPIO_PORTC_BASE, /* WLAN EN port */
        GPIO_PIN_6       /* WLAN EN pin */
    }
};

const WiFi_Config WiFi_config[] = {
    {
        &WiFiCC3100_fxnTable,
        &wiFiCC3100Objects[0],
        &wiFiCC3100HWAttrs[0]
    },
    {NULL,NULL, NULL},
};

/*
 *  ======== EK_TM4C1294XL_initWiFi ========
 */
void EK_TM4C1294XL_initWiFi(void)
{
    /* Configure EN & CS pins to disable CC3100 */
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);

    /* Configure SSI2 for CC3100 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);

    /* Configure IRQ pin */
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPD);
    GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_3, GPIO_RISING_EDGE);

    SPI_init();
    EK_TM4C1294XL_initDMA();

    WiFi_init();
}
