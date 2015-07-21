/**
 * @file time_port.i
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

#include <pthread.h>

static int time_port_get(struct time_t *now)
{
    struct timespec _now;

    clock_gettime(CLOCK_REALTIME, &_now);
    now->seconds = _now.tv_sec;
    now->nanoseconds = _now.tv_nsec;

    return (0);
}

static int time_port_set(struct time_t *new)
{
    return (-1);
}

static void time_port_sleep(int us)
{
}
