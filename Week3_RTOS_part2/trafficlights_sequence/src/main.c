#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "leds.h"

// Task prototypes
void uart_receiver_task(void *, void *, void *);
void dispatcher_task(void *, void *, void *);

#define STACKSIZE 500
#define PRIORITY 5

K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_receiver_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(dispatcher_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void) {
    if (init_led() != 0) {
        printk("LED init failed\n");
        return -1;
    }

    if (init_uart() != 0) {
        printk("UART init failed\n");
        return -1;
    }

    printk("Traffic light sequence started\n");

    // Test: vapautetaan semafori manuaalisesti (voit tehdä myös napilla)
    while (1) {
        k_msleep(1000);
        k_sem_give(&release_sem); // vapauttaa yhden merkin käsittelyn
    }

    return 0;
}
