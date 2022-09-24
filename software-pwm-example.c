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
    // stops all threads and deallocates memory
    bcm2835_software_pwm_close();

    // turns all LEDs off
    bcm2835_gpio_write(GPIO_12, LOW);
    bcm2835_gpio_write(GPIO_13, LOW);
    bcm2835_gpio_write(GPIO_18, LOW);
    bcm2835_gpio_write(GPIO_19, LOW);

    // closes the bcm2835 library
    bcm2835_close();
    exit(0);
}

int main() {
    if (!bcm2835_init()) {
        return 1;
    }

    // set's up a ctrl + c handler to shut down the program and clean up resources
    signal(SIGINT, sigIntHandler);

    // create an arbitrary channel 0
    bcm2835_software_pwm_create_channel(CHANNEL_0);
    // sets the range of the channel to RANGE
    bcm2835_software_pwm_set_range(CHANNEL_0, RANGE);
    // assigns the pins GPIO_12 & GPIO_13 as output pins on channel 0
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_0, GPIO_12);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_0, GPIO_13);

    // create an arbitrary channel 1
    bcm2835_software_pwm_create_channel(CHANNEL_1);
    // sets the range of the channel to RANGE
    bcm2835_software_pwm_set_range(CHANNEL_1, RANGE);
    // assigns the pins GPIO_18 & GPIO_19 as output pins on channel 1
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_1, GPIO_18);
    bcm2835_software_pwm_assign_pin_to_channel(CHANNEL_1, GPIO_19);

    // starts two new threads for channels 0 and 1
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

        // sets the data on channel 0 to data
        bcm2835_software_pwm_set_data(CHANNEL_0, data);
        // sets the data on channel 0 to data
        bcm2835_software_pwm_set_data(CHANNEL_1, data);

        // delays for one second before continuing
        bcm2835_delay(1);
    }
}