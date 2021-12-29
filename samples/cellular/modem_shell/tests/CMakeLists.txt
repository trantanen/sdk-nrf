#
# Copyright (c) 2021 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(modem_shell_test)

FILE(GLOB app_sources
	../../../../samples/cellular/modem_shell/src/shell.c
	../../../../samples/cellular/modem_shell/src/at/at_shell.c
	../../../../samples/cellular/modem_shell/src/main.c
	../../../../samples/cellular/modem_shell/src/utils/mosh_print.c
	../../../../samples/cellular/modem_shell/src/uart/uart.c
	../../../../samples/cellular/modem_shell/src/uart/uart_shell.c
	)
target_sources(app PRIVATE ${app_sources})

# generate runner for the test
test_runner_generate(src/modem_shell_test.c)

target_include_directories(app PRIVATE
	src
	../../../../samples/cellular/modem_shell/src
	../../../../samples/cellular/modem_shell/src/utils
	../../../../samples/cellular/modem_shell/src/at
	)

cmock_handle(${ZEPHYR_NRF_MODULE_DIR}/include/modem/lte_lc.h)
cmock_handle(${ZEPHYR_NRFXLIB_MODULE_DIR}/nrf_modem/include/nrf_modem_at.h
	     FUNC_EXCLUDE ".*nrf_modem_at_printf"
             FUNC_EXCLUDE ".*nrf_modem_at_scanf")

# When mocking nrf_modem_at then nrf_modem/include must manually be added
# because CONFIG_NRF_MODEM_LINK_BINARY=n
zephyr_include_directories(${ZEPHYR_NRFXLIB_MODULE_DIR}/nrf_modem/include/)
#zephyr_include_directories(${ZEPHYR_NRF_MODULE_DIR}/../modules/hal/nordic/nrfx)

# add test file
target_sources(app PRIVATE src/modem_shell_test.c)
