#pragma once
#include <zephyr/drivers/gpio.h>
#include <stdbool.h>

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;
extern const struct gpio_dt_spec blue;

extern volatile int led_state;
extern volatile int prev_state;
extern volatile int manual_led;
extern volatile bool blink_yellow;
extern volatile bool blink_state;

int init_led(void);
void red_led_task(void *, void *, void *);
void yellow_led_task(void *, void *, void *);
void green_led_task(void *, void *, void *);
