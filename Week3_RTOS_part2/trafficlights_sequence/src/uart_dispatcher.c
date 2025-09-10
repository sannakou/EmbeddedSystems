#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include "leds.h"

// UART-laite
#define UART_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE);

// FIFO LED-merkeille
K_FIFO_DEFINE(led_fifo);

// UART-init
int init_uart(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready!\n");
        return 1;
    }
    return 0;
}

// UART receiver-task
void uart_receiver_task(void *arg1, void *arg2, void *arg3) {
    char rc;
    while (1) {
        if (uart_poll_in(uart_dev, &rc) == 0) {
            if (rc != '\r' && rc != '\n') {
                char *c = k_malloc(sizeof(char));
                if (c) {
                    *c = rc;
                    k_fifo_put(&led_fifo, c);
                    printk("Received: %c\n", rc);
                }
            }
        }
        k_yield();
    }
}

// Dispatcher-task
void dispatcher_task(void *arg1, void *arg2, void *arg3) {
    while (1) {
        char *c = k_fifo_get(&led_fifo, K_FOREVER);
        if (!c) continue;

        char sequence[100];
        int len = 0;

        sequence[len++] = *c;
        k_free(c);

        while ((c = k_fifo_get(&led_fifo, K_NO_WAIT)) != NULL) {
            sequence[len++] = *c;
            k_free(c);
        }

        for (int i = 0; i < len; i++) {
            // Odotetaan semaforia ennen seuraavaa merkkiä
            k_sem_take(&release_sem, K_FOREVER);

            switch (sequence[i]) {
                case 'R':
                    gpio_pin_set_dt(&red, 1);
                    printk("punainen sytytetty\n");
                    k_msleep(500);
                    gpio_pin_set_dt(&red, 0);
                    break;
                case 'G':
                    gpio_pin_set_dt(&green, 1);
                    printk("vihreä sytytetty\n");
                    k_msleep(500);
                    gpio_pin_set_dt(&green, 0);
                    break;
                case 'Y': // keltainen = punainen + vihreä
                    gpio_pin_set_dt(&red, 1);
                    gpio_pin_set_dt(&green, 1);
                    printk("keltainen sytytetty\n");
                    k_msleep(500);
                    gpio_pin_set_dt(&red, 0);
                    gpio_pin_set_dt(&green, 0);
                    break;
                default:
                    printk("Unknown character: %c\n", sequence[i]);
                    break;
            }
            k_yield();
        }
    }
}
