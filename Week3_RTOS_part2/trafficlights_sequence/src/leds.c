#include "leds.h"
#include <zephyr/sys/printk.h>

const struct gpio_dt_spec red   = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec blue  = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

/* Semaphore: dispatcher odottaa aina seuraavaa giveä */
K_SEM_DEFINE(release_sem, 0, 1);

int init_led(void) {
    int ret;
    ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) return ret;
    ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) return ret;
    ret = gpio_pin_configure_dt(&blue, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) return ret;

    printk("LEDs initialized\n");
    return 0;
}
