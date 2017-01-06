
/* Developed by nVisionIT */

#include <zephyr.h>

#include <device.h>
#include <gpio.h>
#include <misc/util.h>

#include <ipm.h>
#include <ipm/ipm_quark_se.h>

#include <sensor_ipm_ids.h>

#ifndef SENSOR_DIGITAL
#define SENSOR_DIGITAL

// This Pin maps to A0
#define GPIO_OUT_PIN	2
// This Pin maps to A1
#define GPIO_INT_PIN	3
#define GPIO_NAME	"GPIO_SS_"

#if defined(CONFIG_GPIO_DW_0)
#define GPIO_DRV_NAME CONFIG_GPIO_DW_0_NAME
#elif defined(CONFIG_GPIO_QMSI_0)
#define GPIO_DRV_NAME CONFIG_GPIO_QMSI_0_NAME
#elif defined(CONFIG_GPIO_ATMEL_SAM3)
#define GPIO_DRV_NAME CONFIG_GPIO_ATMEL_SAM3_PORTB_DEV_NAME
#else
#define GPIO_DRV_NAME "GPIO_0"
#endif

static struct gpio_callback sensor_digital_gpio_cb;

struct device* ipm; 

/**
 * @brief The GPIO callback that handles activation events.
 * @param port Pointer to the device structure for the driver instance.
 * @param cb GPIO callback structure.
 * @param pins The GPIO pins that have been activated.
 *
 * Note: enables to add as many callback as needed on the same port.
 */
void sensor_digital_gpio_callback(struct device *port, struct gpio_callback *cb, uint32_t pins);

/**
* @brief Start the digital sensor monitor service 
*/
void sensor_digital_start(struct device* ipm);

#endif