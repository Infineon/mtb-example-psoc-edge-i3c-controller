/*******************************************************************************
 * File Name        : main.c
 *
 * Description      : This source file contains the main routine for non-secure
 *                    application in the CM33 CPU
 *
 * Related Document : See README.md
 *
 ********************************************************************************
 * Copyright 2023-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
/*******************************************************************************
 * Header Files
 *******************************************************************************/
#include "i3c_controller.h"
#include "cybsp.h"
#include "retarget_io_init.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* The timeout value in microsecond used to wait for core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC            (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR          (CYMEM_CM33_0_m55_nvm_START + \
                                        CYBSP_MCUBOOT_HEADER_SIZE)

#define OFF                                 (1UL)
#define ON                                  (0UL)
#define CMD_TO_CMD_DELAY                    (1000UL)
#define BYTE_ZERO                           (0U)
#define BYTE_FIVE                           (5U)
#define BYTE_UPDATE                         (1U)
#define STATIC_ADDR                         (0x63)
#define VALUE_ZERO                          (0U)
#define DEVICE_LIST_I3C                     (1U)
#define DEVICE_LIST_I2C                     (0U)
#define ONE_BYTE_FOR_SOP                    (1U)
#define CHECKSUM_VAL                        (40U)
#define RX_BUFFER_SIZE                      (7U)
#define RD_WAIT_DELAY_MS                    (100U)

/*******************************************************************************
* Global variables
*******************************************************************************/
extern cy_stc_i3c_context_t CYBSP_I3C_CONTROLLER_context;

cy_stc_i3c_controller_xfer_config_t controllerTransferCfg =
{
    .targetAddress = 0,
    .buffer       = NULL,
    .bufferSize   = VALUE_ZERO,
    .toc          = true,
};

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 * This is the main function of the CM33 non-secure application.
 * The I3C controller perfoms write operation to the target device.
 *
 * Parameters:
 *  none
 *
 * Return:
 *  int
 *
 *******************************************************************************/

int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    uint32_t status, res;
    uint8_t buffer[TX_PACKET_SIZE];
    status = I3C_FAILURE;

    cy_stc_i2c_device_t i2cDeviceList = { .lvr = VALUE_ZERO, .staticAddress = VALUE_ZERO};
    cy_stc_i3c_device_t i3cDeviceList[DEVICE_LIST_I3C];
   
    cy_stc_i3c_ccc_cmd_t cccCmd;
    cy_stc_i3c_ccc_payload_t payload;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();

    /* Board initialization failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io middleware */
    init_retarget_io();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("****************** ""PSOC Edge MCU: I3C Controller"" ****************** \r\n\n");

    status = initI3C();
    /* I3C init failed. Stop program execution */
    if(I3C_FAILURE == status)
    {
        handle_app_error();
    }
    printf("I3C Initialization Successful\n\r");

    cccCmd.address = CY_I3C_BROADCAST_ADDR;
    cccCmd.cmd = CY_I3C_CCC_RSTDAA(true);
    cccCmd.data = &payload;
    cccCmd.data->data = NULL;
    cccCmd.data->len = 0U;
    status = Cy_I3C_SendCCCCmd(CYBSP_I3C_CONTROLLER_HW, &cccCmd,  &CYBSP_I3C_CONTROLLER_context);
    /*Stop program execution */
    if(I3C_FAILURE == status)
    {
        handle_app_error();
    }

    i3cDeviceList[BYTE_ZERO].dynamicAddress = VALUE_ZERO;
    i3cDeviceList[BYTE_ZERO].staticAddress = STATIC_ADDR;
    status = attachDevices(&i2cDeviceList, DEVICE_LIST_I2C, &(i3cDeviceList[BYTE_ZERO]), DEVICE_LIST_I3C);
    
    controllerTransferCfg.targetAddress = i3cDeviceList->dynamicAddress;
    
      /* Failed to attach device. Stop program execution */
    if(I3C_FAILURE == status)
    {
        handle_app_error();
    }

    /* Enable CM55. */
    /* CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);

    /* create packet to be sent to target.  */
    buffer[PACKET_SOP_POS] = PACKET_SOP;
    buffer[PACKET_EOP_POS] = PACKET_EOP;

    // Fill the array with your 10 data bytes
    for (int i = BYTE_ZERO; i < BYTE_FIVE; i++)
    {
        buffer[ONE_BYTE_FOR_SOP+i]=i+ONE_BYTE_FOR_SOP;
    }

    res = I3CWritePacket(buffer, TX_PACKET_SIZE);
    if( I3C_SUCCESS != res)
    {
        handle_app_error();
        printf("write packet failed\n\n\r");
    }

    Cy_SysLib_Delay(CMD_TO_CMD_DELAY);

    printf("Data Frame=\n");
    printf(" SOP\t");
    printf("\t      Data packets      \t");
    printf("EOP\n");
    printf(" ----\t");
    printf("----------------------------------\t");
    printf("----\t");
    printf("\n");
    printf("  %-2d\t", buffer[PACKET_SOP_POS]);
    for (int i = BYTE_UPDATE; i <= BYTE_FIVE; i++)
    {
        printf("%-2d\t", buffer[i]);
    }
    printf("%-2d\t", buffer[PACKET_EOP_POS]);
    printf("\n");

    uint8_t bufferrx[RX_BUFFER_SIZE];
    I3CReadPacket(bufferrx, RX_BUFFER_SIZE);
    printf("Read packets from target\n\n");
    Cy_SysLib_Delay(RD_WAIT_DELAY_MS);

    int sum = 0;
    for (int i = 0; i < RX_BUFFER_SIZE; i++) {
        sum += bufferrx[i];
    }

    if (CHECKSUM_VAL == sum){

    printf(" SOP\t");
    printf("  Data packets read from Target   \t");
    printf("EOP\n");
    printf(" ----\t");
    printf("----------------------------------\t");
    printf("----\t");
    printf("\n");
    printf (" %-2d\t", bufferrx[PACKET_SOP_POS]);
    for (int i = 1; i <= 5; i++)
    {
        printf("%-2d\t", bufferrx[i]);
    }
    printf ("%-2d\t", bufferrx[PACKET_EOP_POS]);
    }
    else
    {
        printf("Error in data packets");
    }


}

/* [] END OF FILE */
