#include "leds.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

// LED-pinnit
const struct gpio_dt_spec red   = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec blue  = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

// Semafori, jolla vapautetaan seuraava merkki
K_SEM_DEFINE(release_sem, 0, 1);

int init_led(void) {
    int ret;

    ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) return ret;
    ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) return ret;
    ret = gpio_pin_configure_dt(&blue, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) return ret;

    gpio_pin_set_dt(&red, 0);
    gpio_pin_set_dt(&green, 0);
    gpio_pin_set_dt(&blue, 0);

    printk("LEDs initialized\n");
    return 0;
}
