/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <device.h>
#include <gpio.h>
#include <misc/util.h>

#define DEVICE_NAME		"nV Zephyr Security Sensor"
#define DEVICE_NAME_LEN		(sizeof(DEVICE_NAME) - 1)

// START: GATT stuff
// GATT includes
#include <gatt/gap.h>
#include <gatt/hrs.h>
#include <gatt/dis.h>
#include <gatt/bas.h>
#include <gatt/aios.h>
#include <gatt/ess.h>

//GPIO stuff
#if defined(CONFIG_SOC_QUARK_SE_C1000_SS)
#define GPIO_OUT_PIN	10
#define GPIO_INT_PIN	11
#define GPIO_NAME	"GPIO_SS_"
#elif defined(CONFIG_SOC_QUARK_SE_C1000)
#define GPIO_OUT_PIN	16
#define GPIO_INT_PIN	19

/* 49   B08,    gpio_15, */    /* IO5 */
/* 50   A08,    gpio_16, */    /* IO8 */
/* 51   B09,    gpio_17, */    /* IO3 */
/* 52   A09,    gpio_18, */    /* IO2 */
/* 53   C09,    gpio_19, */    /* IO4 */
/* 54   D09,    gpio_20, */    /* IO7 */

#define GPIO_IO2 18
#define GPIO_IO3 17
#define GPIO_IO4 19
#define GPIO_IO5 15
#define GPIO_IO7 20
#define GPIO_IO8 16

#define GPIO_IO2_VAL 0x00040000
#define GPIO_IO3_VAL 0x00020000
#define GPIO_IO4_VAL 0x00080000
#define GPIO_IO5_VAL 0x00008000
#define GPIO_IO7_VAL 0x00100000
#define GPIO_IO8_VAL 0x00010000

#define GPIO_IO2_LOGICAL 0x1
#define GPIO_IO3_LOGICAL 0x2
#define GPIO_IO4_LOGICAL 0x4
#define GPIO_IO5_LOGICAL 0x8
#define GPIO_IO7_LOGICAL 0x10
#define GPIO_IO8_LOGICAL 0x20

#define GPIO_NAME	"GPIO_"
#elif defined(CONFIG_SOC_ATMEL_SAM3)
#define GPIO_OUT_PIN	25
#define GPIO_INT_PIN	27
#define GPIO_NAME	"GPIO_"
#elif defined(CONFIG_SOC_QUARK_D2000)
#define GPIO_OUT_PIN	8
#define GPIO_INT_PIN	24
#define GPIO_NAME	"GPIO_"
#endif

#if defined(CONFIG_GPIO_DW_0)
#define GPIO_DRV_NAME CONFIG_GPIO_DW_0_NAME
#elif defined(CONFIG_GPIO_QMSI_0)
#define GPIO_DRV_NAME CONFIG_GPIO_QMSI_0_NAME
#elif defined(CONFIG_GPIO_ATMEL_SAM3)
#define GPIO_DRV_NAME CONFIG_GPIO_ATMEL_SAM3_PORTB_DEV_NAME
#else
#define GPIO_DRV_NAME "gpio_dw"
#endif

static struct gpio_callback gpio_0_cb;
static struct gpio_callback gpio_1_cb;
static struct gpio_callback gpio_2_cb;
static struct gpio_callback gpio_3_cb;
static struct gpio_callback gpio_4_cb;
static struct gpio_callback gpio_5_cb;

static uint8_t gpio_active = 0;

void gpio_callback(struct device *port,
		   struct gpio_callback *cb, uint32_t pins)
{
  uint32_t val = 0;

  int pin_num = 0;
  int gpio_val = 0;

  switch(pins){
		case GPIO_IO2_VAL:
			pin_num = GPIO_IO2;
			gpio_val = GPIO_IO2_LOGICAL;
			break;
		case GPIO_IO3_VAL:
			pin_num = GPIO_IO3;
			gpio_val = GPIO_IO3_LOGICAL;
			break;
		case GPIO_IO4_VAL:
			pin_num = GPIO_IO4;
			gpio_val = GPIO_IO4_LOGICAL;
			break;
		case GPIO_IO5_VAL:
			pin_num = GPIO_IO5;
			gpio_val = GPIO_IO5_LOGICAL;
			break;
		case GPIO_IO7_VAL:
			pin_num = GPIO_IO7;
			gpio_val = GPIO_IO7_LOGICAL;
			break;
		case GPIO_IO8_VAL:
			pin_num = GPIO_IO8;
			gpio_val = GPIO_IO8_LOGICAL;
			break;
	}

  if(pin_num){
	  int ret = gpio_pin_read(port, pin_num, &val);
		if(ret){
			printk("gpio_pin_read failed (err %d)\n", ret);
		}

		if(val){
		  gpio_active = gpio_active | gpio_val;
		} else {
			gpio_active = gpio_active & ((gpio_val) ^ 0xFF);
		}

		printk(GPIO_NAME "Input 0x%x triggered = 0x%x\n", pins, val);
		printk(GPIO_NAME "GPIO status = 0x%x\n", gpio_active);

		aios_notify(&gpio_active);
  }
}
// END GPIOs

// Define what the device 'look' like
#define GAP_APPEARANCE	0x0341

// Register / initialize GATT sensors here
static void start_gatt()
{
	gap_init(DEVICE_NAME, GAP_APPEARANCE);
	//hrs_init(0x01);
	//bas_init();
	aios_init();
	//ess_init();
	dis_init(CONFIG_SOC, "Manufacturer");
}
// END: GATT stuff

// STOP HERE
// Don't touch the rest of the x86_app main.c
// it should work by only adding the gatt stuff and ipm stuff above

struct bt_conn *default_conn;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),

	// Declare the services that will be advertised here
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0d, 0x18, 0x0f, 0x18, 0x05, 0x18),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err %u)\n", err);
	} else {
		default_conn = bt_conn_ref(conn);
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason %u)\n", reason);

	if (default_conn) {
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	start_gatt();

	printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

int configure_gpio_in(struct device *gpio_dev, int pin_num, struct gpio_callback* callback_pst){

		/* Setup GPIO input, and triggers on rising edge. */
	  int	ret = gpio_pin_configure(gpio_dev, pin_num,
				(GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE | GPIO_PUD_PULL_DOWN
				 | GPIO_INT_DOUBLE_EDGE | GPIO_INT_DEBOUNCE));

		if (ret) {
			printk("Error configuring " GPIO_NAME "%d!\n", pin_num);
		}

		gpio_init_callback(callback_pst, gpio_callback, BIT(pin_num));

		ret = gpio_add_callback(gpio_dev, callback_pst);
		if (ret) {
			printk("Cannot setup callback!\n");
		}

		ret = gpio_pin_enable_callback(gpio_dev, pin_num);
		if (ret) {
			printk("Error enabling callback!\n");
		}

		return ret;
}

void main(void)
{
	int ret;
	struct device *gpio_dev;
	int toggle = 1;

	ret = bt_enable(bt_ready);
	if (ret) {
		printk("Bluetooth init failed (err %d)\n", ret);
		return;
	}

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	////// GPIO stuff

	gpio_dev = device_get_binding(GPIO_DRV_NAME);
	if (!gpio_dev) {
		printk("Cannot find %s!\n", GPIO_DRV_NAME);
	}

	aios_gpio_device = gpio_dev;

	/* Setup GPIO output */
	ret = gpio_pin_configure(gpio_dev, GPIO_OUT_PIN, (GPIO_DIR_OUT));
	if (ret) {
		printk("Error configuring " GPIO_NAME "%d!\n", GPIO_OUT_PIN);
	}

  configure_gpio_in(gpio_dev, GPIO_IO2, &gpio_0_cb);
  configure_gpio_in(gpio_dev, GPIO_IO3, &gpio_1_cb);
  configure_gpio_in(gpio_dev, GPIO_IO4, &gpio_2_cb);
  configure_gpio_in(gpio_dev, GPIO_IO5, &gpio_3_cb);
  configure_gpio_in(gpio_dev, GPIO_IO7, &gpio_4_cb);
  configure_gpio_in(gpio_dev, GPIO_IO8, &gpio_5_cb);

	while (1) {
		printk("Toggling " GPIO_NAME "%d\n", GPIO_OUT_PIN);


		if (toggle) {
			toggle = 0;
		} else {
			toggle = 1;
		}

		k_sleep(MSEC_PER_SEC);
	}
}
