/**
 * @file main.c
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

#include "simba.h"

static char t0_stack[256];
static void *t0_entry(void *arg_p)
{
    int b, c[4];
    struct queue_t *queue_p = arg_p;

    thrd_set_name("t0");

    b = 1;
    BTASSERT(chan_write(&queue_p[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 1);
    b = 2;
    BTASSERT(chan_write(&queue_p[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 2);
    b = 3;
    BTASSERT(chan_write(&queue_p[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 3);
    c[0] = 4;
    c[1] = 5;
    c[2] = 6;
    c[3] = 7;
    BTASSERT(chan_write(&queue_p[0], c, sizeof(c)) == sizeof(c));
    BTASSERT(c[0] == 4);
    BTASSERT(c[1] == 5);
    BTASSERT(c[2] == 6);
    BTASSERT(c[3] == 7);
    BTASSERT(chan_write(&queue_p[0], c, sizeof(c)) == sizeof(c));
    BTASSERT(c[0] == 4);
    BTASSERT(c[1] == 5);
    BTASSERT(c[2] == 6);
    BTASSERT(c[3] == 7);

    /* List polling.*/
    c[0] = 8;
    c[1] = 9;
    c[2] = 10;
    c[3] = 11;
    BTASSERT(chan_write(&queue_p[1], c, sizeof(c)) == sizeof(c));

    /* Write to chan that was polled but not read.*/
    b = 12;
    BTASSERT(chan_write(&queue_p[0], &b, sizeof(b)) == sizeof(b));
    
    return (0);
}

static struct queue_t queue[2];

static int test_init(struct harness_t *harness)
{
    struct thrd_t *t0;

    BTASSERT(queue_init(&queue[0], NULL, 0) == 0);
    BTASSERT(queue_init(&queue[1], NULL, 0) == 0);
    
    BTASSERT((t0 = thrd_spawn(t0_entry,
                              queue,
                              1,
                              t0_stack,
                              sizeof(t0_stack))) != NULL);

    return (0);
}

static int test_read_write(struct harness_t *harness)
{
    int b, c[4];

    b = 0;
    BTASSERT(chan_read(&queue[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 1);
    BTASSERT(chan_read(&queue[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 2);
    BTASSERT(chan_read(&queue[0], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 3);
    BTASSERT(chan_read(&queue[0], c, sizeof(c)) == sizeof(c));
    BTASSERT(c[0] == 4);
    BTASSERT(c[1] == 5);
    BTASSERT(c[2] == 6);
    BTASSERT(c[3] == 7);
    BTASSERT(chan_read(&queue[0], c, sizeof(c)) == sizeof(c));
    BTASSERT(c[0] == 4);
    BTASSERT(c[1] == 5);
    BTASSERT(c[2] == 6);
    BTASSERT(c[3] == 7);

    return (0);
}

static int test_poll(struct harness_t *harness_p)
{
    int b;
    struct chan_list_t list;
    char workspace[64];

    /* Use a list with one chan.*/
    BTASSERT(chan_list_init(&list, workspace, sizeof(workspace)) == 0);
    BTASSERT(chan_list_add(&list, &queue[0]) == 0);
    BTASSERT(chan_list_add(&list, &queue[1]) == 0);
    BTASSERT(chan_list_poll(&list) == &queue[1]);
    BTASSERT(chan_read(&queue[1], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 8);
    BTASSERT(chan_list_poll(&list) == &queue[1]);
    BTASSERT(chan_read(&queue[1], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 9);
    BTASSERT(chan_list_poll(&list) == &queue[1]);
    BTASSERT(chan_read(&queue[1], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 10);
    BTASSERT(chan_list_poll(&list) == &queue[1]);
    BTASSERT(chan_read(&queue[1], &b, sizeof(b)) == sizeof(b));
    BTASSERT(b == 11);

    /* Read from inactive polled chan.*/
    /* BTASSERT(chan_read(&queue[0], &b, sizeof(b)) == sizeof(b)); */
    /* BTASSERT(b == 12); */

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_init, "test_init" },
        { test_read_write, "test_read_write" },
        { test_poll, "test_poll" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
