/*****************************************************************************
 * File Name        : i3c_controller.c
 *
 * Description      : This file contains the functions and variables required for the
 *                     operation of I3C controller
 *
 * Related Document : See README.md
 *
 *******************************************************************************
 * Copyright 2024-2025, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/
 
#include "i3c_controller.h"
#include <stdio.h>

/******************************************************************************
*MACROS
*******************************************************************************/
#define TRANSFER_ERROR   0xFFUL
#define TRANSFER_CMPLT   0x00UL

#define CHECK_ZERO       (0)
#define VALUE_ZERO       (0)

/*******************************************************************************
* Global variables
*******************************************************************************/
cy_stc_i3c_context_t CYBSP_I3C_CONTROLLER_context;

/* Structure for controller transfer configuration */
extern cy_stc_i3c_controller_xfer_config_t controllerTransferCfg ;

uint8_t rxBuffer[RX_PACKET_SIZE];

/*******************************************************************************
* Function Name: I3C_handle_error
****************************************************************************//**
* Summary:
* This function processes unrecoverable errors such as any component
* initialization errors etc. In case of such error the system will
* stay in the infinite loop of this function..
*
* Parameters:
*  none
*
* Return:
*  void
* \note
* If error occurs interrupts are disabled.
*
*******************************************************************************/
static void I3C_handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    /* Infinite loop. */
    while(1u) ;
}

/*******************************************************************************
* Function Name: CYBSP_I3C_CONTROLLER_Interrupt
****************************************************************************//**
* Summary:
* Invokes the Cy_I3C_Interrupt() PDL driver function.
*
* Parameters:
*  none
*
* Return:
*  void
*
*******************************************************************************/
void CYBSP_I3C_CONTROLLER_Interrupt(void)
{
    Cy_I3C_Interrupt(CYBSP_I3C_CONTROLLER_HW, &CYBSP_I3C_CONTROLLER_context);
}

/*******************************************************************************
* Function Name: HandleEvents
****************************************************************************//**
* Summary:
* Callback function when an event occurs
*
* Parameters:
*  event : Retuns to the callback based on the event
*
* Return:
*  void
*
*******************************************************************************/
static void HandleEvents(uint32_t event)
{
    if( CHECK_ZERO!= (CY_I3C_CONTROLLER_WR_CMPLT_EVENT & event))
    {
        //Write complete event handling
        printf("Write successful\n\r");
    }
    else if( CHECK_ZERO != (CY_I3C_CONTROLLER_RD_CMPLT_EVENT & event))
    {

    }
    else if(CHECK_ZERO != (CY_I3C_XFER_ABORTED_ERROR_EVENT & event))
    {
        //last transfer aborted
        printf("Transfer aborted\n\r");
    }
    else if( CHECK_ZERO != (I3C_CONTROLLER_ERROR_EVENT & event))
    {
        //Write or Read has ended with an ERROR - Error detected in the response packet
        printf("Error detected in the response packet\n\r");
        I3C_handle_error();
    }
}

/*******************************************************************************
* Function Name: I3CWritePacket
****************************************************************************//**
* Summary:
* Writes data to the target.
*
* Parameters:
*  buffer : data to be sent
*  bufferSize : size of the buffer
*
* Return:
*  cy_en_i3c_status_t
*
*******************************************************************************/
cy_en_i3c_status_t I3CWritePacket(uint8_t * buffer, uint32_t bufferSize)
{
    cy_en_i3c_status_t errorStatus;
    controllerTransferCfg.buffer = buffer;
    controllerTransferCfg.bufferSize = bufferSize;
    errorStatus = Cy_I3C_ControllerWrite(CYBSP_I3C_CONTROLLER_HW, &controllerTransferCfg, &CYBSP_I3C_CONTROLLER_context);
    return errorStatus;
}

/*******************************************************************************
* Function Name: I3CReadPacket
****************************************************************************//**
* Summary:
* Reads data from the target.
*
* Parameters:
*  rxbuff : data from the sent
*  rxbufferSize : size of the buffer
*
* Return:
* cy_en_i3c_status_t
*
*******************************************************************************/
cy_en_i3c_status_t I3CReadPacket(uint8_t * rxbuff, uint32_t rxbufferSize)
{
    cy_en_i3c_status_t errorStatus;
    controllerTransferCfg.buffer = rxbuff;
    controllerTransferCfg.bufferSize = rxbufferSize; 
    errorStatus = Cy_I3C_ControllerRead(CYBSP_I3C_CONTROLLER_HW, &controllerTransferCfg, &CYBSP_I3C_CONTROLLER_context);
    return errorStatus;
}

/*******************************************************************************
* Function Name: attachDevices
****************************************************************************//**
* Summary :
* Attaches the specified i2c Devices in the \ref cy_en_i2c_device_t to the bus
* Attaches the specified i3c Devices in the \ref cy_en_i3c_device_t to the bus
* Sends ENTDAA command and assigns dynamic address to the i3c devices if requested
* 
* Parameters:
* i2cDeviceList  : I2C device list
* i2cDeviceCount : Number of I2C devices
* i3cDeviceList  : I3C device list
* i3cDeviceCount : Number of I3C devices
*
* Return:
* uint32_t
*
*******************************************************************************/
uint32_t attachDevices(cy_stc_i2c_device_t *i2cDeviceList, uint8_t i2cDeviceCount, cy_stc_i3c_device_t *i3cDeviceList, uint8_t i3cDeviceCount)
{
    uint8_t idx;
    uint32_t status, devCount;
    cy_stc_i2c_device_t *i2cPtr = i2cDeviceList;
    cy_stc_i3c_device_t *i3cPtr = i3cDeviceList;

    for(idx = VALUE_ZERO; idx < i2cDeviceCount; idx++)
    {
        status = Cy_I3C_ControllerAttachI2CDevice(CYBSP_I3C_CONTROLLER_HW, i2cPtr, &CYBSP_I3C_CONTROLLER_context);
        if(CY_I3C_SUCCESS != status)
        {
            printf("Failed to attach I2C Device - %d\n\r", idx);
            I3C_handle_error();
        }
        printf("I2C Device - %d attached successfully\n\r", idx);
        i2cPtr++;
    }
    for(idx = VALUE_ZERO; idx < i3cDeviceCount; idx++)
    {
        status = Cy_I3C_ControllerAttachI3CDevice(CYBSP_I3C_CONTROLLER_HW, i3cPtr, &CYBSP_I3C_CONTROLLER_context);
        if(CY_I3C_SUCCESS != status)
        {
            printf("Failed to attach I3C Device - %d\n\r", idx);
            I3C_handle_error();
        }
        i3cPtr++;
    }
    devCount = Cy_I3C_GetI3CDeviceCount(CYBSP_I3C_CONTROLLER_HW, &CYBSP_I3C_CONTROLLER_context);
    printf("Number of I3C devices attached using SETDASA = %u\n\r",(unsigned int)devCount);

    status= Cy_I3C_ControllerStartEntDaa(CYBSP_I3C_CONTROLLER_HW, &CYBSP_I3C_CONTROLLER_context);
    if(CY_I3C_SUCCESS != status)
    {
        printf("Dynamic Address Assignment unsuccessful\n\r");
        I3C_handle_error();
    }
    printf("Dynamic Address Assignment successful\n\r");
    return I3C_SUCCESS;
}

/*******************************************************************************
* Function Name: initI3C
****************************************************************************//**
* Summary :
* This function preforms I3C controller initialization
* 
* Parameters:
* None
*
* Return:
* void
*
*******************************************************************************/
uint32_t initI3C(void)
{
    cy_en_i3c_status_t initStatus;
    cy_en_sysint_status_t sysStatus;
    cy_stc_sysint_t CYBSP_I3C_CONTROLLER_IRQ_cfg =
    {
        .intrSrc = CYBSP_I3C_CONTROLLER_IRQ,
        .intrPriority = 2UL
    }; 

    initStatus = Cy_I3C_Init(CYBSP_I3C_CONTROLLER_HW, &CYBSP_I3C_CONTROLLER_config, &CYBSP_I3C_CONTROLLER_context);
    if(initStatus != CY_I3C_SUCCESS)
    {
        return I3C_FAILURE;
    }
    /* Hook interrupt service routine. */
    sysStatus = Cy_SysInt_Init(&CYBSP_I3C_CONTROLLER_IRQ_cfg, &CYBSP_I3C_CONTROLLER_Interrupt);
    if(sysStatus != CY_SYSINT_SUCCESS)
    {
        return I3C_FAILURE;
    }
    /* Enable interrupt in NVIC. */
    NVIC_EnableIRQ((IRQn_Type) CYBSP_I3C_CONTROLLER_IRQ_cfg.intrSrc);
    Cy_I3C_RegisterEventCallback(CYBSP_I3C_CONTROLLER_HW, (cy_cb_i3c_handle_events_t) HandleEvents, &CYBSP_I3C_CONTROLLER_context);
    Cy_I3C_Enable(CYBSP_I3C_CONTROLLER_HW, &CYBSP_I3C_CONTROLLER_context);
    return I3C_SUCCESS; 
}

/* [] END OF FILE */
