/*****************************************************************************
 * File Name        : i3c_controller.h
 *
 * Description      : This file contains Constants and Prototypes.
 *
 * Related Document : See README.md
 *
 *******************************************************************************
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

#ifndef SOURCE_I3C_CONTROLLER_H_
#define SOURCE_I3C_CONTROLLER_H_

#include "cycfg.h"

/***************************************
* MACROS
****************************************/
#define I3C_SUCCESS         (0UL)
#define I3C_FAILURE         (1UL)

#define READ_CMPLT        (TRANSFER_CMPLT)


#define PACKET_SOP_POS      (0UL)  /* SOP - Start of Packet*/
#define PACKET_CMD_POS      (1UL)  /* POS - Position, CMD - Command */
#define PACKET_EOP_POS      (6UL) /* EOP - End of Packet*/
#define PACKET_STS_POS      (1UL)  /* STS - Status */

/* Buffer and packet size */
#define TX_PACKET_SIZE      (7UL)
#define RX_PACKET_SIZE      (4UL)
#define IBI_PACKET_SIZE     (16UL)

/* Start and end of packet markers */
#define PACKET_SOP          (0x01UL)
#define PACKET_EOP          (0x17UL)

#define I3C_CONTROLLER_ERROR_EVENT         (CY_I3C_CRC_ERROR_EVENT |  CY_I3C_PARITY_ERROR_EVENT | \
                                    CY_I3C_FRAME_ERROR_EVENT | CY_I3C_BROADCAST_ADDR_NACK_ERROR_EVENT | \
                                    CY_I3C_ADDR_NACK_ERROR_EVENT | CY_I3C_BUFFER_OVERFLOW_ERROR_EVENT | \
                                    CY_I3C_I2C_TGT_WDATA_NACK_ERROR_EVENT | CY_I3C_CONTROLLER_ERROR_CE0_EVENT)


/***************************************
*        Function prototypes
****************************************/
uint32_t initI3C(void);
cy_en_i3c_status_t I3CWritePacket(uint8_t * buffer, uint32_t bufferSize);
cy_en_i3c_status_t I3CReadPacket(uint8_t * rxbuff, uint32_t rxbufferSize);
uint32_t attachDevices(cy_stc_i2c_device_t *i2cDeviceList, uint8_t i2cDevCount, cy_stc_i3c_device_t *i3cDeviceList, uint8_t i3cDevCount);
#endif /* SOURCE_I3C_CONTROLLER_H_ */

/* [] END OF FILE */