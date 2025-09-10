#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include "leds.h"

#define UART_MSG_MAX_LEN  20
#define STACKSIZE         700
#define PRIORITY          5

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* FIFO viestien välitykseen */
K_FIFO_DEFINE(dispatcher_fifo);

struct data_t {
    void *fifo_reserved;
    char msg[UART_MSG_MAX_LEN];
};

/* UART init */
int init_uart(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready!\n");
        return 1;
    }
    return 0;
}

/* UART vastaanottosäie */
static void uart_task(void *a, void *b, void *c) {
    char rc;
    char uart_msg[UART_MSG_MAX_LEN];
    int idx = 0;

    memset(uart_msg, 0, sizeof(uart_msg));

    while (1) {
        if (uart_poll_in(uart_dev, &rc) == 0) {
            if (rc == '\r' || rc == '\n') {
                if (idx > 0) {
                    struct data_t *buf = k_malloc(sizeof(*buf));
                    if (buf) {
                        strncpy(buf->msg, uart_msg, sizeof(buf->msg) - 1);
                        buf->msg[sizeof(buf->msg) - 1] = '\0';
                        k_fifo_put(&dispatcher_fifo, buf);
                        printk("UART msg queued: %s\n", buf->msg);
                    }
                    idx = 0;
                    memset(uart_msg, 0, sizeof(uart_msg));
                }
            } else if (idx < (UART_MSG_MAX_LEN - 1)) {
                uart_msg[idx++] = rc;
            } else {
                printk("uart_task: message too long, dropped\n");
                idx = 0;
                memset(uart_msg, 0, sizeof(uart_msg));
            }
        } else {
            k_msleep(5);
        }
    }
}

/* Dispatcher-säie */
static void dispatcher_task(void *a, void *b, void *c) {
    while (1) {
        struct data_t *rec = k_fifo_get(&dispatcher_fifo, K_FOREVER);
        if (!rec) continue;

        printk("Dispatcher got: %s\n", rec->msg);

        for (size_t i = 0; i < strlen(rec->msg); i++) {
            char ch = rec->msg[i];
            k_sem_take(&release_sem, K_FOREVER);

            switch (ch) {
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
            case 'Y':
                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 1);
                printk("keltainen sytytetty\n");
                k_msleep(500);
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 0);
                break;
            default:
                printk("Unknown character: %c\n", ch);
                break;
            }
        }

        k_free(rec);
    }
}

/* Luodaan säikeet */
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
