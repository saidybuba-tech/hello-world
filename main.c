#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"


#define BUTTON_PIN GPIO_PIN(2, 13)  // PC13 Button
#define LED_PIN GPIO_PIN(0, 5)      // LED  PA5
#define DEBOUNCE_TIME 30000        // Debounce time (30 ms)

int main(void) {
    gpio_init(LED_PIN, GPIO_OUT);       // Initialize LED as output
    gpio_init(BUTTON_PIN, GPIO_IN_PU);  // Initialize button as input with pull-up resistor

    int led_state = 0;  
    int blink_delay = 500;  // Default blink delay (500 ms)

    while (1) {
        if (!gpio_read(BUTTON_PIN)) {  // Button pressed (active low)
            xtimer_usleep(DEBOUNCE_TIME); // Debounce delay

            // Toggle LED state
            led_state = !led_state;
            gpio_write(LED_PIN, led_state);

            // Change blink frequency on long press
            xtimer_msleep(500);  
            if (!gpio_read(BUTTON_PIN)) {  
                blink_delay = (blink_delay == 500) ? 200 : 500;
            }

            // Wait for button release
            while (!gpio_read(BUTTON_PIN));
        }
        
        xtimer_msleep(blink_delay);  
        gpio_toggle(LED_PIN);
    }

    return 0;
}