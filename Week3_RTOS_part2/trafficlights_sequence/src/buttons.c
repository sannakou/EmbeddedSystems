#include "buttons.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* FIFO viesteille */
K_FIFO_DEFINE(button_fifo);

/* Semaphore dispatcherille */
K_SEM_DEFINE(dispatcher_sem, 0, 1);

/* Button pins */
const struct gpio_dt_spec button_red    = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct gpio_dt_spec button_green  = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
const struct gpio_dt_spec button_yellow = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);

/* Data tyyppi FIFO:lle */
struct button_event_t {
    void *fifo_reserved;
    char cmd;
};

/* ISR for buttons */
static void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    struct button_event_t *evt = k_malloc(sizeof(*evt));
    if (!evt) return;

    if (pins & BIT(button_red.pin)) {
        evt->cmd = 'R';
    } else if (pins & BIT(button_green.pin)) {
        evt->cmd = 'G';
    } else if (pins & BIT(button_yellow.pin)) {
        evt->cmd = 'Y';
    } else {
        k_free(evt);
        return;
    }

    k_fifo_put(&button_fifo, evt);

    /* Herätetään dispatcher */
    k_sem_give(&dispatcher_sem);
}

/* GPIO callbacks */
static struct gpio_callback button_cb;

int init_buttons(void) {
    int ret;

    const struct gpio_dt_spec buttons[] = {button_red, button_green, button_yellow};

    for (int i = 0; i < 3; i++) {
        ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT | GPIO_PULL_UP);
        if (ret < 0) return ret;
        ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
        if (ret < 0) return ret;
    }

    gpio_init_callback(&button_cb, button_pressed_isr,
                       BIT(button_red.pin) | BIT(button_green.pin) | BIT(button_yellow.pin));
    gpio_add_callback(button_red.port, &button_cb);

    return 0;
}

struct k_fifo *get_button_fifo(void) {
    return &button_fifo;
}
