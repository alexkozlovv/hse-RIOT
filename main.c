#include <stdio.h>
#include <stdlib.h>
#include "timex.h"
#include "shell.h"
#include "xtimer.h"
#include "periph_conf.h"
#include "board.h"
#include "periph/gpio.h"

#define FLASHING1                1000000
#define FLASHING2                100000


int first_press = 1;
xtimer_ticks32_t button_down;
xtimer_ticks32_t duration_button_down;
int duration_button_us = 0;
int flashing = 0;
int long_press_try = 1;

void btn_handler(void *arg) {
	(void) arg;
	button_down = xtimer_now();
	while(!gpio_read(GPIO_PIN(PORT_C, 13))) {
		duration_button_down = xtimer_now();
		}
	duration_button_us = xtimer_usec_from_ticks(duration_button_down);
	if ((duration_button_us - xtimer_usec_from_ticks(button_down)) < 500000) {
		if (first_press){
			flashing = FLASHING1;
			first_press = 0;
			}
                else {
                    flashing = 0;
                    gpio_clear(GPIO_PIN(PORT_A, 5));
                    first_press = 1;
                    }
           }
       if ((duration_button_us - xtimer_usec_from_ticks(button_down)) > 500000) {
       	if (first_press)
       	    ;
       	else if (long_press_try) {
       	    flashing = FLASHING2;
       	    long_press_try = 0;
       	    }
       	else {
       	    flashing = FLASHING1;
       	    long_press_try = 1;
       	    }
       }
       return;
 }
       	        
int main(void) {
	gpio_init(GPIO_PIN(PORT_C, 13), GPIO_IN);
	gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);
	gpio_init_int(GPIO_PIN(PORT_C, 13), GPIO_IN, GPIO_FALLING, btn_handler, NULL);
	while(1){
		if (flashing){
		    gpio_toggle(GPIO_PIN(PORT_A, 5));
	            xtimer_usleep(flashing);
	            printf("\rblink with %d мс\n", flashing/1000);
	            }
	        else {
	            printf("\rno blink\n");
	            xtimer_usleep(1000000);
	            }
	    }
	
	return 0;
}



