/** @file
 *  @brief BAS Service sample
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

#include <gpio.h>

#include "uuid.h"

static struct bt_gatt_ccc_cfg  blvl_ccc_cfg[CONFIG_BLUETOOTH_MAX_PAIRED] = {};
static uint8_t simulate_blvl;

static uint8_t aios_digital = 0;
static uint8_t aios_analog = 2;
static char aios_aggregate[] = {0,1,2,3,4};

struct device* aios_gpio_device;

static void blvl_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				 uint16_t value)
{
	simulate_blvl = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t read_analog(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &aios_analog,
				 sizeof(aios_analog));
}

static ssize_t read_digital(struct bt_conn *conn, const struct bt_gatt_attr *attr,
      void *buf, uint16_t len, uint16_t offset)
{
	ssize_t ret = bt_gatt_attr_read(conn, attr, buf, len, offset, &aios_digital,
		sizeof(aios_digital));

	return ret;
}

static ssize_t read_aggregate(struct bt_conn *conn, const struct bt_gatt_attr *attr,
      void *buf, uint16_t len, uint16_t offset)
{

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &aios_aggregate,
	  sizeof(aios_aggregate));
}


/* Automation IO Service Declaration */
// https://nexus.zephyrproject.org/content/sites/site/org.zephyrproject.zephyr/dev/api/html/de/d63/gatt_8h.html
static struct bt_gatt_attr attrs[] = {
	BT_GATT_PRIMARY_SERVICE(BT_UUID_AIOS),
	BT_GATT_CHARACTERISTIC(BT_UUID_AIOS_DIGITAL,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(BT_UUID_AIOS_DIGITAL, BT_GATT_PERM_READ,
			   read_digital, NULL, NULL),
  BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),

	// BT_GATT_CHARACTERISTIC(BT_UUID_AIOS_DIGITAL,
	// 					 BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	// BT_GATT_DESCRIPTOR(BT_UUID_AIOS_DIGITAL, BT_GATT_PERM_READ,
	// 			 read_digital, NULL, NULL),
	// BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),
	//
	// BT_GATT_CHARACTERISTIC(BT_UUID_AIOS_DIGITAL,
	// 					 BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	// BT_GATT_DESCRIPTOR(BT_UUID_AIOS_DIGITAL, BT_GATT_PERM_READ,
	// 			 read_digital, NULL, NULL),
	// BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),
	//
  // BT_GATT_CHARACTERISTIC(BT_UUID_AIOS_ANALOG,
  //           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
  // BT_GATT_DESCRIPTOR(BT_UUID_AIOS_ANALOG, BT_GATT_PERM_READ,
  //       read_analog, NULL, NULL),
  // BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),

	BT_GATT_CHARACTERISTIC(BT_UUID_AIOS_AGGREGATE,
						BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(BT_UUID_AIOS_AGGREGATE, BT_GATT_PERM_READ,
				read_aggregate, NULL, NULL)
};

void aios_init(void)
{
	bt_gatt_register(attrs, ARRAY_SIZE(attrs));
}

void aios_notify(uint8_t* value)
{
  aios_digital = *value;
  aios_analog = *value;

	bt_gatt_notify(NULL, &attrs[2], &aios_digital, sizeof(aios_digital));
  bt_gatt_notify(NULL, &attrs[5], &aios_analog, sizeof(aios_analog));
}
