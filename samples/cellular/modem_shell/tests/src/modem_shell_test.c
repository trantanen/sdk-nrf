/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <unity.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/shell/shell.h>
#include <mock_nrf_modem_at.h>
#include <cmock_lte_lc.h>

void setUp(void)
{
	mock_nrf_modem_at_Init();
}

void tearDown(void)
{
	mock_nrf_modem_at_Verify();
}

static void mosh_execute_cmd(const char *cmd, int result)
{
	int ret;

	ret = shell_execute_cmd(NULL, cmd);

	printf("shell_execute_cmd(%s): %d\n", cmd, ret);

	TEST_ASSERT_EQUAL(result, ret);
}

void test_cmd_shell(void)
{
	mosh_execute_cmd("shell colors off", 0);
	mosh_execute_cmd("shell colors on", 0);
}

void test_cmd_at(void)
{
	static const char at_resp[] = "OK";

	mosh_execute_cmd("shell colors off", 0);
	mosh_execute_cmd("shell colors on", 0);

	__cmock_nrf_modem_at_cmd_ExpectAndReturn(NULL, 0, "%s", 0);
	__cmock_nrf_modem_at_cmd_IgnoreArg_buf();
	__cmock_nrf_modem_at_cmd_IgnoreArg_len();
	__cmock_nrf_modem_at_cmd_ReturnArrayThruPtr_buf((char *)at_resp, sizeof(at_resp));
	mosh_execute_cmd("at at", 0);

	mosh_execute_cmd("at", 0);
}

/* It is required to be added to each test. That is because unity is using
 * different main signature (returns int) and zephyr expects main which does
 * not return value.
 */
extern int unity_main(void);

/* This is needed because AT Monitor library is initialized in SYS_INIT. */
static int mosh_test_sys_init(const struct device *unused)
{
	__cmock_nrf_modem_at_notif_handler_set_ExpectAnyArgsAndReturn(0);

	return 0;
}

void main(void)
{
	printk("BEFORE MOSH START\n");

	__cmock_lte_lc_init_ExpectAndReturn(0);
	start_main();

	printk("TESTS START\n");
	(void)unity_main();
	printk("TESTS END\n");
}

SYS_INIT(mosh_test_sys_init, POST_KERNEL, 0);
