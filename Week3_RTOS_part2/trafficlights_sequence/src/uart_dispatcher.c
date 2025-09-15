#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include "leds.h"
#include "buttons.h"
#include <string.h>

#define STACKSIZE 700
#define PRIORITY  5

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Dispatcher FIFO data */
struct data_t {
    void *fifo_reserved;
    char msg;
};

/* Dispatcher FIFO */
K_FIFO_DEFINE(dispatcher_fifo);

/* UART init */
int init_uart(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready!\n");
        return 1;
    }
    return 0;
}

/* UART task */
static void uart_task(void *a, void *b, void *c) {
    char rc;
    while (1) {
        if (uart_poll_in(uart_dev, &rc) == 0) {
            if (rc == 'R' || rc == 'G' || rc == 'Y') {
                struct data_t *buf = k_malloc(sizeof(*buf));
                if (buf) {
                    buf->msg = rc;
                    k_fifo_put(get_button_fifo(), buf);
                    k_sem_give(&dispatcher_sem);
                    printk("UART msg queued: %c\n", buf->msg);
                }
            }
        } else {
            k_msleep(5);
        }
    }
}

/* Dispatcher task */
static void dispatcher_task(void *a, void *b, void *c) {
    while (1) {
        k_sem_take(&dispatcher_sem, K_FOREVER);

        struct data_t *rec;
        while ((rec = k_fifo_get(get_button_fifo(), K_NO_WAIT)) != NULL) {
            switch (rec->msg) {
            case 'R':
                gpio_pin_set_dt(&red, 1);
                k_msleep(500);
                gpio_pin_set_dt(&red, 0);
                break;
            case 'G':
                gpio_pin_set_dt(&green, 1);
                k_msleep(500);
                gpio_pin_set_dt(&green, 0);
                break;
            case 'Y':
                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 1);
                k_msleep(500);
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 0);
                break;
            default:
                printk("Unknown character: %c\n", rec->msg);
                break;
            }
            k_free(rec);
        }
    }
}

/* Luodaan säikeet */
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
