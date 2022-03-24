#include <stdio.h>
#include <stdlib.h>
#include "timex.h"
#include "shell.h"
#include "xtimer.h"
#include "periph_conf.h"
#include "board.h"
#include "periph/gpio.h"
#include "msg.h"

#define FLASHING1                1000000
#define FLASHING2                100000

xtimer_ticks32_t button_down;
xtimer_ticks32_t button_up;
int first_press = 1;
int duration_button_us = 0;
int flashing = 0;
int long_press_try = 1;
static char regular_stack[THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t thread_flashing;


void btn_handler(void *arg) {
    (void) arg;
    msg_t msg;
    if (!gpio_read(GPIO_PIN(PORT_C, 13)))
        msg.content.value = 0;
    else 
    	msg.content.value = 1;
    msg_send(&msg, thread_flashing);
    return;
}

void* flashing_thread(void* arg) {
    (void) arg;
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.content.value == 0)
		    button_down = xtimer_now();
	if (msg.content.value == 1) {
		    button_up = xtimer_now();
		    duration_button_us = xtimer_usec_from_ticks(xtimer_diff(button_up, button_down));   
		    if (duration_button_us < 500000) {
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
                    if (duration_button_us > 500000) {
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
		}
    }
    return 0;
}

int main(void) {
	gpio_init(GPIO_PIN(PORT_C, 13), GPIO_IN);
	gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);
	gpio_init_int(GPIO_PIN(PORT_C, 13), GPIO_IN, GPIO_BOTH, btn_handler, NULL);
	thread_flashing = thread_create(regular_stack, sizeof(regular_stack),
                               THREAD_PRIORITY_MAIN-1, THREAD_CREATE_STACKTEST,
                               flashing_thread, NULL, "flashing thread");

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


