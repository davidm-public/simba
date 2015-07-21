/**
 * @file drivers/sh/uart_port.c
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Erik Moqvist
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

#include "drivers/uart.h"
#include <stdint.h>

struct uart_device_t uart_device[UART_DEVICE_MAX];

int uart_module_init()
{
    return (0);
}

int uart_init(struct uart_driver_t *drv,
              const struct uart_device_t *dev,
              const struct uart_config_t *config)
{
    drv_p->dev = dev;
    return (0);
}

int uart_start(struct uart_driver_t *drv)
{
    return (0);
}

int uart_stop(struct uart_driver_t *drv)
{
    return (0);
}

ssize_t uart_write(struct uart_driver_t *drv,
                   const void *txbuf,
                   size_t n)
{
    return (n);
}

ssize_t uart_read(struct uart_driver_t *drv,
                  void *rxbuf,
                  size_t n)
{
    return (n);
}
