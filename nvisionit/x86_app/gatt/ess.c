/** @file
 *  @brief ESS Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/printk.h>
#include <misc/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "uuid.h"

static struct bt_gatt_ccc_cfg  blvl_ccc_cfg[CONFIG_BLUETOOTH_MAX_PAIRED] = {};
static uint8_t simulate_blvl;

static int16_t ess_temperature = 1;
static uint16_t ess_humidity = 1;
static uint32_t ess_pressure = 1;


static void blvl_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				 uint16_t value)
{
	simulate_blvl = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t read_temperature(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &ess_temperature,
				 sizeof(ess_temperature));
}

static ssize_t read_humidity(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &ess_humidity,
				 sizeof(ess_humidity));
}

static ssize_t read_pressure(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &ess_pressure,
				 sizeof(ess_pressure));
}

/* Automation IO Service Declaration */
// https://nexus.zephyrproject.org/content/sites/site/org.zephyrproject.zephyr/dev/api/html/de/d63/gatt_8h.html
static struct bt_gatt_attr attrs[] = {
	BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),
	BT_GATT_CHARACTERISTIC(BT_UUID_ESS_TEMPERATURE,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(BT_UUID_ESS_TEMPERATURE, BT_GATT_PERM_READ,
			   read_temperature, NULL, NULL),
  BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),

	BT_GATT_CHARACTERISTIC(BT_UUID_ESS_HUMIDITY,
						 BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(BT_UUID_ESS_HUMIDITY, BT_GATT_PERM_READ,
				 read_humidity, NULL, NULL),
	BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),

	BT_GATT_CHARACTERISTIC(BT_UUID_ESS_PRESSURE,
						 BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(BT_UUID_ESS_PRESSURE, BT_GATT_PERM_READ,
				 read_pressure, NULL, NULL),
	BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed)
};

void ess_init(void)
{
	bt_gatt_register(attrs, ARRAY_SIZE(attrs));
}

void ess_notify(uint8_t* value)
{
	ess_temperature = *value;
	ess_humidity = *value;
	ess_pressure = *value;

	bt_gatt_notify(NULL, &attrs[2], &ess_temperature, sizeof(ess_temperature));
  bt_gatt_notify(NULL, &attrs[5], &ess_humidity, sizeof(ess_humidity));
	bt_gatt_notify(NULL, &attrs[5], &ess_pressure, sizeof(ess_pressure));
}
