# bcm2835_software_pwm
Adds support for software pwm for the bcm2835 C library

## Example Code
```C
#include <signal.h>
#include <stdio.h>
#include <bcm2835.h>
#include "bcm2835_software_pwm.h"

#define GPIO_12 RPI_V2_GPIO_P1_32
#define GPIO_13 RPI_V2_GPIO_P1_33
#define GPIO_18 RPI_V2_GPIO_P1_12
#define GPIO_19 RPI_V2_GPIO_P1_35

#define RANGE 1024
#define CHANNEL_0 0
#define CHANNEL_1 1

void sigIntHandler() {
    bcm2835_software_pwm_close();

    bcm2835_gpio_write(GPIO_12, LOW);
    bcm2835_gpio_write(GPIO_13, LOW);
    bcm2835_gpio_write(GPIO_18, LOW);
    bcm2835_gpio_write(GPIO_19, LOW);
    bcm2835_close();
    exit(0);
}

int main() {
    if (!bcm2835_init()) {
        return 1;
    }

    signal(SIGINT, sigIntHandler);

    bcm2835_software_pwm_create_channel(CHANNEL_0);
    bcm2835_software_pwm_set_range(CHANNEL_0, RANGE);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_0, GPIO_12);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_0, GPIO_13);

    bcm2835_software_pwm_create_channel(CHANNEL_1);
    bcm2835_software_pwm_set_range(CHANNEL_1, RANGE);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_1, GPIO_18);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_1, GPIO_19);

    bcm2835_software_pwm_start_all_channels();

    int direction = 1; // 1 is increase, -1 is decrease
    int data = 0;
    while (1) {
        if (data == 0) {
            direction = 1;   // Switch to increasing
        }
        else if (data == RANGE - 1) {
            direction = -1;  // Switch to decreasing
        }
        data += direction;
        bcm2835_software_pwm_set_data(CHANNEL_0, data);
        bcm2835_software_pwm_set_data(CHANNEL_1, data);
        bcm2835_delay(1);
    }
}
```
