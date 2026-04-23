/*****************************************************************************
 * File Name        : i3c_controller.h
 *
 * Description      : This file contains Constants and Prototypes.
 *
 * Related Document : See README.md
 *
 *******************************************************************************
 * (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
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