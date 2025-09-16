/* 

VKO 3. Tavoitteena korkein pistemäärä, eli koodissa toimii 
1. sekvenssien lähetys sarjaportin kautta 
2. sekvessien lähetys painonapeilla
3. sekvenssien toisto lisäämällä sekvenssin perään "," ja toistojen määrä

VKO 4. Tavoitteena korkein pistemäärä, eli koodissa toimii
1. Ajoitukset liikennevaloihin
2. Lisäominaisuuksina ajoituksiin myös sekvenssin käsittelyaika mainissa
ja kaikkien sekvenssien kokonaiskeston
3. Debugin kytkeminen päälle ja pois sarjaportin kautta komeinnoilla D0 ja D1.

*/

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "leds.h"
#include "buttons.h"

extern int init_uart(void);

int main(void) {
    printk("System start\n");

    if (init_led() != 0) {
        printk("LED init failed\n");
    }
    if (init_uart() != 0) {
        printk("UART init failed\n");
    }
    if (init_buttons() != 0) {
        printk("Button init failed\n");
    }

    while (1) {
        k_msleep(1000); // pääloop ei tee muuta
    }

    return 0;
}
