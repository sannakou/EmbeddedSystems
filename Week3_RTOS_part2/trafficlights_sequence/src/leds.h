#ifndef LEDS_H
#define LEDS_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;
extern const struct gpio_dt_spec blue;

int init_led(void);

// Semafori headerissa
extern struct k_sem release_sem;

#endif
