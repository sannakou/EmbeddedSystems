/* 
Tavoitteena korkein pistemäärä, eli koodissa toimii 
1 napissa kaikkien värien vilkutus ja pause, 
2 napissa unainen valo päälle/pois,
3 napissa keltainen valo päälle/pois
4 napissa vihreä valo päälle/pois
5 napissa keltaisen valon vilkutus päälle/pois
*/

#include "leds.h"
#include "buttons.h"
#include <zephyr/kernel.h>

#define STACKSIZE 500
#define PRIORITY 5

//LED threads
K_THREAD_DEFINE(red_thread, STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(yellow_thread, STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(green_thread, STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
    init_led();
    button_init();

    while (1) {
        k_sleep(K_FOREVER);
    }
    return 0;
}
