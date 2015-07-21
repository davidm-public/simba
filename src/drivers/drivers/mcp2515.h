/**
 * @file drivers/mcp2515.h
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

#ifndef __DRIVERS_MCP2515_H__
#define __DRIVERS_MCP2515_H__

#include "simba.h"

/* CAN BUS speed. */
#define MCP2515_SPEED_1000KBPS 1000
#define MCP2515_SPEED_500KBPS   500

/* Device mode. */
#define MCP2515_MODE_NORMAL       0
#define MCP2515_MODE_LOOPBACK     1

/* Driver data structure. */
struct mcp2515_driver_t {
    struct spi_driver_t spi;
    struct exti_driver_t exti;
    int mode;
    int speed;
    struct queue_t irqchan;
    struct {
        struct fifo_t fifo;
        struct canif_frame_t *frame_p;
        struct spin_lock_t spin;
        struct sem_t sem;
        char stack[256];
    } rx;
    struct {
        struct sem_t sem;
    } tx;
};

/**
 * Initialize driver object.
 * @param[out] drv_p Driver object to initialize.
 * @param[in] spi_p SPI driver to use.
 * @param[in] exti_p External interrupt tp use.
 * @param[in] mode Device mode.
 * @param[in] speed CAN bus speed in kbps.
 * @param[in] frames RX frame buffers.
 * @param[in] length Length of frames array.
 * @return zero(0) or negative error code.
 */
int mcp2515_init(struct mcp2515_driver_t *drv_p,
                 struct spi_device_t *spi_p,
                 struct pin_device_t *cs_p,
                 struct exti_device_t *exti_p,
                 int mode,
                 int speed,
                 struct canif_filter_t filter[],
                 struct canif_frame_t frames[],
                 size_t length);

/**
 * Starts the CAN device using given driver object.
 * @param[in] drv_p Initialized driver object.
 * @return zero(0) or negative error code.
 */
int mcp2515_start(struct mcp2515_driver_t *drv_p);

/**
 * Stops the CAN device referenced by driver object.
 * @param[in] drv_p Initialized driver object.
 * @return zero(0) or negative error code.
 */
int mcp2515_stop(struct mcp2515_driver_t *drv_p);

/**
 * Read CAN frame.
 * @param[in] drv_p Initialized driver object.
 * @param[out] frame_p Read frame.
 * @return zero(0) or negative error code.
 */
int mcp2515_read(struct mcp2515_driver_t *drv_p,
                 struct canif_frame_t *frame_p);

/**
 * Write CAN frame.
 * @param[in] drv_p Initialized driver object.
 * @param[out] frame_p Frame to write.
 * @return zero(0) or negative error code.
 */
int mcp2515_write(struct mcp2515_driver_t *drv_p,
                  const struct canif_frame_t *frame_p);

#endif
