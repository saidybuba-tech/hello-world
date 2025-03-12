#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define BUTTON_PIN GPIO_PIN(2, 13)  // PC13 Button
#define LED_PIN GPIO_PIN(0, 5)      // LED  PA5
#define DEBOUNCE_TIME 30000         // Debounce time (30 ms)
#define LONG_PRESS_TIME 500000      // Long press threshold (500 ms)

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK
} led_state_t;

int main(void) {
    gpio_init(LED_PIN, GPIO_OUT);       // Initialize LED as output
    gpio_init(BUTTON_PIN, GPIO_IN_PU);  // Initialize button as input with pull-up resistor

    led_state_t led_state = LED_OFF;
    int blink_delay = 500;  // Default blink delay (500 ms)

    while (1) {
        if (!gpio_read(BUTTON_PIN)) {  // Button pressed (active low)
            xtimer_usleep(DEBOUNCE_TIME); // Debounce delay

            uint32_t press_time = xtimer_now_usec();
            while (!gpio_read(BUTTON_PIN)); // Wait for button release
            uint32_t release_time = xtimer_now_usec();
            uint32_t press_duration = release_time - press_time;

            if (press_duration >= LONG_PRESS_TIME) {
                // Long press: Change blinking frequency
                blink_delay = (blink_delay == 500) ? 200 : (blink_delay == 200 ? 100 : 500);
                led_state = LED_BLINK;
            } else {
                // Short press: Toggle LED state
                if (led_state == LED_OFF) {
                    led_state = LED_ON;
                } else if (led_state == LED_ON) {
                    led_state = LED_OFF;
                } else {
                    led_state = LED_ON;
                }
            }
        }

        // Handle LED states
        switch (led_state) {
            case LED_OFF:
                gpio_write(LED_PIN, 0);
                break;
            case LED_ON:
                gpio_write(LED_PIN, 1);
                break;
            case LED_BLINK:
                gpio_toggle(LED_PIN);
                xtimer_msleep(blink_delay);
                break;
        }
    }

    return 0;
}
