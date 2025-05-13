/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>
#include <modem/modem_slm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

SLM_MONITOR(network, "\r\n+CEREG:", cereg_monitor);

#define SLM_AT_CMD_TIMEOUT 10

static void cereg_monitor(const char *notif)
{
	int status = atoi(notif + strlen("\r\n+CEREG: "));

	if (status == 1 || status == 5) {
		LOG_INF("LTE connected");
	}
}

void metering_data_indication(const uint8_t *data, size_t datalen)
{
	LOG_DBG("Data received (len=%d): %.*s", datalen, datalen, (const char *)data);
}

void metering_indication_handler(void)
{
	int err;

	LOG_INF("SLM indicate pin triggered");
	err = modem_slm_power_pin_toggle();
	if (err) {
		LOG_ERR("Failed to toggle power pin");
	}
}

static int metering_init(void)
{
	int err;

	err = modem_slm_init(metering_data_indication);
	if (err) {
		LOG_ERR("Failed to initialize SLM: %d", err);
	}

	err = modem_slm_register_ind(metering_indication_handler, true);
	if (err) {
		LOG_ERR("Failed to register indication: %d", err);
	}

	k_sleep(K_SECONDS(1));

	LOG_INF("Register to network");

	err = modem_slm_send_cmd("AT+CEREG=5", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CEREG: %d", err);
	}

	err = modem_slm_send_cmd("AT%REDMOB=1", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set REDMOB: %d", err);
	}
	err = modem_slm_send_cmd("AT%XMODEMSLEEP=1,5000,10240", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CEREG: %d", err);
	}
	err = modem_slm_send_cmd("AT%XTIME=1", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set XTIME: %d", err);
	}
	err = modem_slm_send_cmd("AT+CFUN=1", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CFUN: %d", err);
	}

	LOG_INF("Check time");
	err = modem_slm_send_cmd("AT+CCLK?", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CCLK: %d", err);
	}

	return 0;
}

static int edrx(void)
{
	int err;

	LOG_INF("Set eDRX");
	err = modem_slm_send_cmd("AT+CPSMS=0", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CPSMS: %d", err);
	}
	err = modem_slm_send_cmd("AT+CEDRXS=1,4,\"0100\"", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CEDRXS: %d", err);
	}
	err = modem_slm_send_cmd("AT%XPTW=4,\"0111\"", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set XPTW: %d", err);
	}

	return 0;
}

static int psm(void)
{
	int err;

	LOG_INF("Set PSM");
	err = modem_slm_send_cmd("AT+CEDRXS=0", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CEDRXS: %d", err);
	}
	err = modem_slm_send_cmd("AT+CPSMS=1,\"\",\"\",\"00000010\",\"00000010\"", SLM_AT_CMD_TIMEOUT);
	if (err != 0) {
		LOG_ERR("Failed to set CPSMS: %d", err);
	}

	return 0;
}

static int ping(void)
{
	int err;

	err = modem_slm_send_cmd("slm AT#XPING=\"8.8.8.8\",10,1000,5", SLM_AT_CMD_TIMEOUT);
	if (err < 0) {
		LOG_ERR("Failed to perform XPING: %d", err);
	}

	return 0;
}

int main(void)
{
	LOG_INF("SLM Shell starts on %s", CONFIG_BOARD);

	metering_init();
	k_sleep(K_SECONDS(1));

	edrx();
	k_sleep(K_SECONDS(1));

	ping();

	return 0;
}
