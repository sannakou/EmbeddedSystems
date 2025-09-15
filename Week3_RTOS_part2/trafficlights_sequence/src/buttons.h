#pragma once
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

int init_buttons(void);

/* Button pins */
extern const struct gpio_dt_spec button_red;
extern const struct gpio_dt_spec button_green;
extern const struct gpio_dt_spec button_yellow;

/* FIFO getter dispatcherille */
struct k_fifo *get_button_fifo(void);

/* Semaphore dispatcherille */
extern struct k_sem dispatcher_sem;
