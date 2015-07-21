/**
 * @file drivers/can.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#ifndef __DRIVERS_CAN_H__
#define __DRIVERS_CAN_H__

#include "simba.h"

#include "can_port.h"

extern struct can_device_t can_device[CAN_DEVICE_MAX];

/**
 * Initialize driver object.
 * @param[in] drv_p Driver object to initialize.
 * @param[in] dev_p Device to use.
 * @param[in] filter_p RX filter configuration.
 * @param[in] frames_p RX frame buffer in interrupt.
 * @param[in] length Length of frames array.
 * @return zero(0) or negative error code.
 */
int can_init(struct can_driver_t *drv_p,
             struct can_device_t *dev_p,
             struct canif_filter_t *filter_p,
             struct canif_frame_t *frames_p,
             size_t length);

/**
 * Starts the CAN device using given driver object.
 * @param[in] drv_p Initialized driver object.
 * @return zero(0) or negative error code.
 */
int can_start(struct can_driver_t *drv_p);

/**
 * Stops the CAN device referenced by driver object.
 * @param[in] drv_p Initialized driver object.
 * @return zero(0) or negative error code.
 */
int can_stop(struct can_driver_t *drv_p);

/**
 * Read CAN frame from given mailbox.
 * @param[in] drv_p Initialized driver object.
 * @param[in] mailbox Mailbox to read from.
 * @param[out] frame_p Read frame.
 * @return zero(0) or negative error code.
 */
int can_read(struct can_driver_t *drv_p,
             int mailbox,
             struct canif_frame_t *frame_p);

/**
 * Write CAN frame to given mailbox.
 * @param[in] drv_p Initialized driver object.
 * @param[in] mailbox Mailbox to write to.
 * @param[out] frame_p Frame to write.
 * @return zero(0) or negative error code.
 */
int can_write(struct can_driver_t *drv_p,
              int mailbox,
              const struct canif_frame_t *frame_p);

#endif
