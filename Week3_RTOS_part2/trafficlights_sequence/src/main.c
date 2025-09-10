#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "leds.h"

extern int init_uart(void);

void main(void) {
    printk("System start\n");

    if (init_led() != 0) {
        printk("LED init failed\n");
    }
    if (init_uart() != 0) {
        printk("UART init failed\n");
    }

    /* Testiksi annetaan release 1s välein → dispatcher etenee sekvenssissä */
    while (1) {
        k_msleep(1000);
        k_sem_give(&release_sem);
    }
}
