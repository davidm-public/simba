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

FS_COMMAND_DEFINE("/tmp/foo", tmp_foo);
FS_COMMAND_DEFINE("/tmp/bar", tmp_bar);

FS_COUNTER_DEFINE(foo);
FS_COUNTER_DEFINE(bar);
FS_COUNTER_DEFINE(fie);

FS_PARAMETER_DEFINE("/tmp/fie", tmp_fie, int, 57);

int tmp_foo(int argc,
            const char *argv[],
            void *out_p,
            void *in_p)
{
    UNUSED(in_p);

    BTASSERT(argc == 4);
    std_fprintf(out_p,
                FSTR("argc = %d, argv[0] = %s, argv[1] = %s, argv[2] = %s\n"),
                argc,
                argv[0],
                argv[1],
                argv[2]);

    return (0);
}

int tmp_bar(int argc,
            const char *argv[],
            struct chan_t *out_p,
            struct chan_t *in_p)
{
    int i;

    UNUSED(out_p);

    BTASSERT(argc == 2);
    i = (int)(*argv[1]) - '0';
    std_fprintf(out_p, FSTR("bar %d\n"), 2 * i);

    return (0);
}

static char qinbuf[8];
static QUEUE_INIT_DECL(qin, qinbuf, sizeof(qinbuf));
static char qoutbuf[256];
static QUEUE_INIT_DECL(qout, qoutbuf, sizeof(qoutbuf));

static char shell_stack[356];
static struct shell_args_t shell_args = {
    .chin_p = &qin,
    .chout_p = &qout
};

#ifdef ARCH_LINUX
#    define BUFFER_SIZE 512
#else
#    define BUFFER_SIZE 128
#endif

static int chout_read_until_prompt(char *buf_p)
{
    char c;
    size_t size = 0;

    while (size < BUFFER_SIZE - 3) {
        chan_read(&qout, &c, sizeof(c));
        *buf_p++ = c;
        size++;

        if (c == '$') {
            chan_read(&qout, &c, sizeof(c));
            *buf_p++ = c;
            size++;

            if (c == ' ') {
                *buf_p = '\0';
                return (1);
            }
        }
    }

    return (0);
}

static int test_all(struct harness_t *harness_p)
{
    char buf[BUFFER_SIZE];

    thrd_spawn(shell_entry,
               &shell_args,
               0,
               shell_stack,
               sizeof(shell_stack));

    /* Empty line. */
    chan_write(&qin, "\n", sizeof("\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf, FSTR("\n$ ")) == 0, "%s\n", buf);

    /* Command not found. */
    chan_write(&qin, "/1/2/3\n", sizeof("/1/2/3\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/1/2/3\n"
                             "/1/2/3: command not found\r\n"
                             "$ ")) == 0, "%s\n", buf);

#ifdef ARCH_LINUX
    /* Command too long. */
    chan_write(&qin,
               "This command is too long for the shell to handle. "
               "The maximum command length is probably 64 characters.\n",
               membersof("This command is too long for the shell to handle. "
                         "The maximum command length is probably 64 characters.\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("This command is too long for the shell to handle. "
                             "The maximum co\r\n"
                             "shell: error: command too long\r\n"
                             "$ ")) == 0, "%s\n", buf);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("mmand length is probably 64 characters.\n"
                             "mmand: command not found\r\n"
                             "$ ")) == 0, "%s\n", buf);
#endif

    /* Auto completion. */
    chan_write(&qin, "\t", membersof("\t") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("\r\n"
                             "kernel/\r\n"
                             "tmp/\r\n"
                             "$ ")) == 0, "%s\n", buf);

    /* Test command foo. */
    chan_write(&qin, "/tmp/foo 3 2 1\n", membersof("/tmp/foo 3 2 1\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/tmp/foo 3 2 1\n"
                             "argc = 4, argv[0] = /tmp/foo, argv[1] = 3, argv[2] = 2\n"
                             "$ ")) == 0, "%s\n", buf);

    /* Test command bar. */
    chan_write(&qin, "/tmp/bar 3\n", membersof("/tmp/bar 3\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/tmp/bar 3\n"
                             "bar 6\n"
                             "$ ")) == 0, "%s\n", buf);

    /* Get parameter value. */
    chan_write(&qin, "/tmp/fie\n", membersof("/tmp/fie\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/tmp/fie\n"
                             "57\r\n"
                             "$ ")) == 0, "%s\n", buf);

    /* Set parameter value. */
    chan_write(&qin, "/tmp/fie 58\n", membersof("/tmp/fie 58\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/tmp/fie 58\n$ ")) == 0, "%s\n", buf);

    /* Get modified parameter value. */
    chan_write(&qin, "/tmp/fie\n", membersof("/tmp/fie\n") - 1);
    chout_read_until_prompt(buf);
    BTASSERT(std_strcmp(buf,
                        FSTR("/tmp/fie\n"
                             "58\r\n"
                             "$ ")) == 0, "%s\n", buf);

    /* Set main thread log mask. */
    chan_write(&qin, "/kernel/thrd/set_log_mask shell 0xff\r\n",
               membersof("/kernel/thrd/set_log_mask shell 0xff\r\n") - 1);
    chout_read_until_prompt(buf);

#ifdef ARCH_LINUX
    /* List thrds. */
    chan_write(&qin, "/kernel/thrd/list\r\n", membersof("/kernel/thrd/list\r\n") - 1);
    /* Read until a prompt is found to empty the buffer. */
    BTASSERT(chout_read_until_prompt(buf) == 1);

    /* List counters. */
    FS_COUNTER_INC(foo, 2);
    FS_COUNTER_INC(foo, 2);
    FS_COUNTER_INC(bar, 339283982393);
    FS_COUNTER_INC(fie, 1);
    chan_write(&qin, "/kernel/fs/counter_list\n", membersof("/kernel/fs/counter_list\n") - 1);
    BTASSERT(chout_read_until_prompt(buf) == 1);
    BTASSERT(std_strcmp(buf,
                        FSTR("/kernel/fs/counter_list\n"
                             "NAME                                             VALUE\r\n"
                             "bar                                              "
                             "00000000004efee6b839\r\n"
                             "fie                                              "
                             "00000000000000000001\r\n"
                             "foo                                              "
                             "00000000000000000004\r\n"
                             "$ ")) == 0);
#endif

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t harness_testcases[] = {
        { test_all, "test_all" },
        { NULL, NULL }
    };

    sys_start();
    uart_module_init();

    harness_init(&harness);
    harness_run(&harness, harness_testcases);

    return (0);
}
