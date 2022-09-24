# bcm2835_software_pwm

Adds support for software pwm for the [bcm2835 C library](https://www.airspayce.com/mikem/bcm2835/index.html)

## Advantages and Disadvantages of Software PWM

The speed of software pwm is much slower than using hardware PWM. The Raspberry Pi's hardware PWM runs at 19,200,000
Hertz. That means you get one period every 52 nanoseconds. 1s/19.2 MHz = 52.08

Hardware pwm is great if you need reliabiliy and precision. However, the hardware pwm on a raspberry pi is limited to
four output pins and only two channels. If you wanted to light up an RGB LED using pwm, that wouldn't be possible
because you would need three separate channels. That's where using software pwm would be more beneficial.

Software pwm is slower than hardware pwm, but it allows you to use any output pin and is not restricted to two channels.
This software pwm is default configured to run at 60 Hertz. Meaning you get one period every 16 milliseconds. Software
pwm is better if you need more than two channels, and reliability and precision are flexible.

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
```

## Library Documentation

### Set Frequency

```text
  Sets the software frequency in Hertz to the value passed in to the frequency parameter
    Setting the frequency allows us to find how long in time each period is by diving time by freqency
    Tosc = 1 second / Frequency
    
    Setting a higher frequency value will use more processing power, but more periods. The default
    frequency is 60 Hertz
    param[in] frequency Frequency in Hertz
```

```C
  extern void bcm2835_software_pwm_set_frequency(uint32_t frequency);
````

### Create Channel

```text
    Creates a channel that can be assigned a range, data, and pins to output to
      Each channel will start a new thread once \sa bcm2835_software_pwm_start_all_channels is called
      That channel will manage the active on and active off phase of the electric pulses for every pin
      that has been assigned to the channel using \sa bcm2835_software_pwm_assign_pin_to_channel
      param[in] frequency Frequency in Hertz
```

```C
    extern void bcm2835_software_pwm_create_channel(uint8_t channel);
```

### Set Range

```text
  Sets the maximum range of the Software PWM output
    The data value can vary between 0 and this range to control PWM output
    param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
    param[in] range The maximum value permitted for DATA.
```

```C
  extern void bcm2835_software_pwm_set_range(uint8_t channel, uint32_t range);
```

### Set Data

```text
  Sets the Software PWM pulse ratio to emit to DATA/RANGE, where RANGE is set by
    \sa bcm2835_software_pwm_set_range().
    param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
    param[in] data Controls the PWM output ratio as a fraction of the range. 
    Can vary from 0 to RANGE.
```

```C
  extern void bcm2835_software_pwm_set_data(uint8_t channel, uint32_t data);
```

### Assign Pin to Channel

```text
  Adds a RPiGPIOPin pin to the pins the input parameter channel outputs to
    param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
    param[in] pin GPIO number, or one of RPI_GPIO_P1_* from \ref RPiGPIOPin.
```

```C
  extern void bcm2835_software_pwm_assign_pin_to_channel(uint8_t channel, RPiGPIOPin pin);
```

### Start All Channels

```text
  Creates new threads using pthread_create for each of the channels created using
    \sa bcm2835_software_pwm_start_all_channels
    Note that this action must be called after all of your desired channels are created
```

```C
  extern void bcm2835_software_pwm_start_all_channels();
```

### Close

```text
  Closes the software pwm library, stopping all threads and deallocating any allocated memory
    Note that this should be called before any final \sa bcm2835_gpio_write writes so no conflicts in 
    state from multi-threading occur
```

```C
  extern void bcm2835_software_pwm_close();
```
