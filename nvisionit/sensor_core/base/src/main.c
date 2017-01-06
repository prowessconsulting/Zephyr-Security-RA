/* main_arc.c - main source file for ARC app */

/*
 * Copyright (c) 2016 Intel Corporation.
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

#include <zephyr.h>

#include <ipm.h>
#include <ipm/ipm_quark_se.h>
#include <init.h>
#include <device.h>
#include <misc/byteorder.h>
#include <stdio.h>
#include <string.h>
#include <misc/printk.h>

#include <sensor_ipm_ids.h>

/* measure every 2ms */
#define INTERVAL 	200
#define SLEEPTICKS MSEC(INTERVAL)


/* ID of IPM channel */
#define HRS_ID		99

QUARK_SE_IPM_DEFINE(sensor_ipm, 0, QUARK_SE_IPM_OUTBOUND);


void main(void)
{
	struct device *ipm;
	struct nano_timer timer;
	uint32_t data[2] = {0, 0};
	uint8_t count = 90;
	int ret;

	nano_timer_init(&timer, data);

	/* Initialize the IPM */
  ipm = device_get_binding("sensor_ipm");
	if (!ipm) {
    printk("IPM: Device not found.\n");
  }

  	sensor_digital_start(ipm);


	while (1) {

		/* send data over ipm to x86 side */
		if (ipm) {
			ret = ipm_send(ipm, 1, HRS_ID, &count, sizeof(count));
			if (ret) {
				printk("Failed to send IPM message, error (%d)\n", ret);
			}
		}

		count++;
		if (count >=160)
		{
			count = 90;
		}

		nano_timer_start(&timer, SLEEPTICKS);
		nano_timer_test(&timer, TICKS_UNLIMITED);
	}
}
