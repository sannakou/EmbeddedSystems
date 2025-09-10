#include "buttons.h"
#include "leds.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>

//Buttons
#define BUTTON_0 DT_ALIAS(sw0)
#define BUTTON_1 DT_ALIAS(sw1)
#define BUTTON_2 DT_ALIAS(sw2)
#define BUTTON_3 DT_ALIAS(sw3)
#define BUTTON_4 DT_ALIAS(sw4)

static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});
static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET_OR(BUTTON_1, gpios, {0});
static const struct gpio_dt_spec button_2 = GPIO_DT_SPEC_GET_OR(BUTTON_2, gpios, {0});
static const struct gpio_dt_spec button_3 = GPIO_DT_SPEC_GET_OR(BUTTON_3, gpios, {0});
static const struct gpio_dt_spec button_4 = GPIO_DT_SPEC_GET_OR(BUTTON_4, gpios, {0});

// Callbacks
static struct gpio_callback button_0_cb_data;
static struct gpio_callback button_1_cb_data;
static struct gpio_callback button_2_cb_data;
static struct gpio_callback button_3_cb_data;
static struct gpio_callback button_4_cb_data;

//Button callbacks
void button_0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (led_state != 3) {
        prev_state = led_state;
        led_state = 3;
        manual_led = 0;
        blink_yellow = false;
    } else {
        led_state = prev_state;
    }
}

void button_1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (manual_led == 1) {
        manual_led = 0;
        gpio_pin_set_dt(&red, 0);
    } else {
        manual_led = 1;
        blink_yellow = false;
        gpio_pin_set_dt(&red, 1);
        gpio_pin_set_dt(&green, 0);
        gpio_pin_set_dt(&blue, 0);
    }
}

void button_2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (manual_led == 2) {
        manual_led = 0;
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
    } else {
        manual_led = 2;
        blink_yellow = false;
        gpio_pin_set_dt(&red, 1);
        gpio_pin_set_dt(&green, 1);
        gpio_pin_set_dt(&blue, 0);
    }
}

void button_3_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (manual_led == 3) {
        manual_led = 0;
        gpio_pin_set_dt(&green, 0);
    } else {
        manual_led = 3;
        blink_yellow = false;
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 1);
        gpio_pin_set_dt(&blue, 0);
    }
}

void button_4_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    blink_yellow = !blink_yellow;
    if (blink_yellow) {
        led_state = 3;
        manual_led = 0;
    }
}

//Initialize buttons
void button_init(void) {
    if (gpio_is_ready_dt(&button_0)) {
        gpio_pin_configure_dt(&button_0, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button_0, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button_0_cb_data, button_0_pressed, BIT(button_0.pin));
        gpio_add_callback(button_0.port, &button_0_cb_data);
    }

    if (gpio_is_ready_dt(&button_1)) {
        gpio_pin_configure_dt(&button_1, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button_1, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button_1_cb_data, button_1_pressed, BIT(button_1.pin));
        gpio_add_callback(button_1.port, &button_1_cb_data);
    }

    if (gpio_is_ready_dt(&button_2)) {
        gpio_pin_configure_dt(&button_2, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button_2, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button_2_cb_data, button_2_pressed, BIT(button_2.pin));
        gpio_add_callback(button_2.port, &button_2_cb_data);
    }

    if (gpio_is_ready_dt(&button_3)) {
        gpio_pin_configure_dt(&button_3, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button_3, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button_3_cb_data, button_3_pressed, BIT(button_3.pin));
        gpio_add_callback(button_3.port, &button_3_cb_data);
    }

    if (gpio_is_ready_dt(&button_4)) {
        gpio_pin_configure_dt(&button_4, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button_4, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button_4_cb_data, button_4_pressed, BIT(button_4.pin));
        gpio_add_callback(button_4.port, &button_4_cb_data);
    }
}
