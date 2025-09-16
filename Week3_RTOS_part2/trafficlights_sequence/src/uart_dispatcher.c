#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/timing/timing.h>
#include <stdlib.h>
#include <string.h>
#include "leds.h"
#include "buttons.h"

#define STACKSIZE 700
#define PRIORITY  5
#define UART_MSG_MAX_LEN 64

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Debug-lippu */
static bool debug_enabled = true;

#define DBG_PRINTK(...) \
    do { if (debug_enabled) printk(__VA_ARGS__); } while (0)

/* Dispatcher FIFO data */
struct data_t {
    void *fifo_reserved;
    char msg[UART_MSG_MAX_LEN];
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
    char uart_buf[UART_MSG_MAX_LEN];
    int idx = 0;
    memset(uart_buf, 0, sizeof(uart_buf));

    while (1) {
        if (uart_poll_in(uart_dev, &rc) == 0) {
            if (rc == '\r' || rc == '\n') {
                if (idx > 0) {
                    struct data_t *buf = k_malloc(sizeof(*buf));
                    if (buf) {
                        strncpy(buf->msg, uart_buf, sizeof(buf->msg) - 1);
                        buf->msg[sizeof(buf->msg) - 1] = '\0';

                        /* Tarkista onko komento debugille */
                        if (buf->msg[0] == 'D') {
                            if (buf->msg[1] == '1') debug_enabled = true;
                            else if (buf->msg[1] == '0') debug_enabled = false;
                            printk("Debug mode %s\n", debug_enabled ? "ON" : "OFF");
                            k_free(buf);
                        } else {
                            k_fifo_put(get_button_fifo(), buf);
                            k_sem_give(&dispatcher_sem);
                            printk("UART seq queued: %s\n", buf->msg);
                        }
                    }
                    idx = 0;
                    memset(uart_buf, 0, sizeof(uart_buf));
                }
            } else if (idx < (UART_MSG_MAX_LEN - 1)) {
                uart_buf[idx++] = rc;
            } else {
                printk("UART message too long, dropped\n");
                idx = 0;
                memset(uart_buf, 0, sizeof(uart_buf));
            }
        } else {
            k_msleep(5);
        }
    }
}

/* Dispatcher task */
static void dispatcher_task(void *a, void *b, void *c) {
    while (1) {
        /* Aloitushetki koko sekvenssin käsittelylle */
        uint64_t total_start_us = k_uptime_get() * 1000ULL;

        k_sem_take(&dispatcher_sem, K_FOREVER);

        struct data_t *rec;
        while ((rec = k_fifo_get(get_button_fifo(), K_NO_WAIT)) != NULL) {
            char *sep = strchr(rec->msg, ',');
            int repeat = 1;

            if (sep != NULL) {
                *sep = '\0';
                repeat = atoi(sep + 1);
                if (repeat <= 0) repeat = 1;
            }

            /* Aloitushetki yksittäiselle sekvenssille */
            uint64_t seq_start_us = k_uptime_get() * 1000ULL;

            for (int r = 0; r < repeat; r++) {
                uint64_t seq_total_us = 0;

                for (size_t i = 0; i < strlen(rec->msg); i++) {
                    char ch = rec->msg[i];

                    /* Aloitushetki yksittäiselle valotehtävälle */
                    uint32_t start = k_cycle_get_32();

                    switch (ch) {
                    case 'R':
                        DBG_PRINTK("Sytytetään punainen\n");
                        gpio_pin_set_dt(&red, 1);
                        k_msleep(500);
                        gpio_pin_set_dt(&red, 0);
                        break;
                    case 'G':
                        DBG_PRINTK("Sytytetään vihreä\n");
                        gpio_pin_set_dt(&green, 1);
                        k_msleep(500);
                        gpio_pin_set_dt(&green, 0);
                        break;
                    case 'Y':
                        DBG_PRINTK("Sytytetään keltainen\n");
                        gpio_pin_set_dt(&red, 1);
                        gpio_pin_set_dt(&green, 1);
                        k_msleep(500);
                        gpio_pin_set_dt(&red, 0);
                        gpio_pin_set_dt(&green, 0);
                        break;
                    default:
                        DBG_PRINTK("Tuntematon merkki: %c\n", ch);
                        break;
                    }

                    /* Loppuhetki yksittäiselle valotehtävälle */
                    uint32_t end = k_cycle_get_32();
                    uint64_t dur_us = k_cyc_to_us_floor64(end - start);
                    seq_total_us += dur_us;

                    printk("Task %c duration: %llu us\n", ch, dur_us);
                }

                printk("Sequence total duration: %llu us\n", seq_total_us);
            }

            /* Sekvenssin käsittelyaika dispatcherissa */
            uint64_t seq_end_us = k_uptime_get() * 1000ULL;
            printk("Processing time for sequence '%s': %llu us\n", rec->msg, seq_end_us - seq_start_us);

            k_free(rec);
        }

        /* Kokonaisaika odotuksesta ja käsittelystä */
        uint64_t total_end_us = k_uptime_get() * 1000ULL;
        printk("Total elapsed time for all sequences: %llu us\n", total_end_us - total_start_us);
    }
}

/* Luodaan säikeet */
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL,
                PRIORITY, 0, 0);
