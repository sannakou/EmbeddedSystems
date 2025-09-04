#include "leds.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>

const struct gpio_dt_spec red   = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec blue  = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

volatile int led_state = 0;
volatile int prev_state = 0;
volatile int manual_led = 0;
volatile bool blink_yellow = false;
volatile bool blink_state = false;

#define STACKSIZE 500
#define PRIORITY 5

// LED Tasks
void red_led_task(void *arg1, void *arg2, void *arg3) {
    while (1) {
        if (led_state == 0 && manual_led == 0 && !blink_yellow) {
            gpio_pin_set_dt(&red, 1);
            gpio_pin_set_dt(&green, 0);
            gpio_pin_set_dt(&blue, 0);
            k_msleep(1000);
            if (led_state == 0) led_state = 1;
        } else if (manual_led == 1) {
            gpio_pin_set_dt(&red, 1);
            gpio_pin_set_dt(&green, 0);
            gpio_pin_set_dt(&blue, 0);
        }
        k_msleep(10);
    }
}

void yellow_led_task(void *arg1, void *arg2, void *arg3) {
    while (1) {
        if (blink_yellow) {
            blink_state = !blink_state;
            gpio_pin_set_dt(&red, blink_state ? 1 : 0);
            gpio_pin_set_dt(&green, blink_state ? 1 : 0);
            gpio_pin_set_dt(&blue, 0);
            k_msleep(500);
        } else if (led_state == 1 && manual_led == 0) {
            gpio_pin_set_dt(&red, 1);
            gpio_pin_set_dt(&green, 1);
            gpio_pin_set_dt(&blue, 0);
            k_msleep(1000);
            if (led_state == 1) led_state = 2;
        } else if (manual_led == 2) {
            gpio_pin_set_dt(&red, 1);
            gpio_pin_set_dt(&green, 1);
            gpio_pin_set_dt(&blue, 0);
        }
        k_msleep(10);
    }
}

void green_led_task(void *arg1, void *arg2, void *arg3) {
    while (1) {
        if (led_state == 2 && manual_led == 0 && !blink_yellow) {
            gpio_pin_set_dt(&red, 0);
            gpio_pin_set_dt(&green, 1);
            gpio_pin_set_dt(&blue, 0);
            k_msleep(1000);
            if (led_state == 2) led_state = 0;
        } else if (manual_led == 3) {
            gpio_pin_set_dt(&red, 0);
            gpio_pin_set_dt(&green, 1);
            gpio_pin_set_dt(&blue, 0);
        }
        k_msleep(10);
    }
}

// Initialize LEDs
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
