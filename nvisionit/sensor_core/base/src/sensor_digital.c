/* Digital Sensors Code */
#include <sensor_digital.h>

void sensor_digital_gpio_callback(struct device *port, struct gpio_callback *cb, uint32_t pins)
{
	int ret;

	ret = ipm_send(ipm, 1, IPM_ID_PIR, &pins, sizeof(pins));
    if (ret) {
        printk("Failed to send IPM message, error (%d)\n", ret);
    }
}

void sensor_digital_start(struct device* ipm){
    struct device *gpio_dev;
	int ret;
	int toggle = 1;

	gpio_dev = device_get_binding(GPIO_DRV_NAME);
	if (!gpio_dev) {
		printk("Cannot find %s!\n", GPIO_DRV_NAME);
		return;
	}

	/* Setup GPIO output */
	ret = gpio_pin_configure(gpio_dev, GPIO_OUT_PIN, (GPIO_DIR_OUT));
	if (ret) {
		printk("Error configuring " GPIO_NAME "%d!\n", GPIO_OUT_PIN);
	}

	/* Setup GPIO input, and triggers on rising edge. */
	ret = gpio_pin_configure(gpio_dev, GPIO_INT_PIN, (GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH | GPIO_INT_DEBOUNCE));
	
    if (ret) {
		printk("Error configuring " GPIO_NAME "%d!\n", GPIO_INT_PIN);
	}

	gpio_init_callback(&sensor_digital_gpio_cb, sensor_digital_gpio_callback, BIT(GPIO_INT_PIN));

	ret = gpio_add_callback(gpio_dev, &sensor_digital_gpio_cb);
	if (ret) {
		printk("Cannot setup callback!\n");
	}

	ret = gpio_pin_enable_callback(gpio_dev, GPIO_INT_PIN);
	if (ret) {
		printk("Error enabling callback!\n");
	}    
    
    printk("Toggling " GPIO_NAME "%d\n", GPIO_OUT_PIN);

    ret = gpio_pin_write(gpio_dev, GPIO_OUT_PIN, toggle);
    if (ret) {
        printk("Error set " GPIO_NAME "%d!\n", GPIO_OUT_PIN);
    }
}