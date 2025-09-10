#pragma once
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

/* LED-aliasit (käytetään boardin DTS:ssä määriteltyjä) */
extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;
extern const struct gpio_dt_spec blue;

/* LED-init funktio */
int init_led(void);

/* Dispatcher käyttää tätä semaforia */
extern struct k_sem release_sem;
