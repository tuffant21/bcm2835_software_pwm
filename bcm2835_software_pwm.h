/* bcm2835_software_pwm

    MIT License

    Copyright (c) 2022 Anthony House

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

/*! \mainpage bcm2835_software_pwm C library 
    This library adds support for software pulse width modulation (pwm) with the bcm_2835 library created by 
    Mike McCauley (https://www.airspayce.com/mikem/bcm2835/index.html). 


    Advantages and Disadvantages of Software PWM

    The speed of software pwm is much slower than using hardware PWM. The Raspberry Pi's hardware PWM runs at
    19,200,000 Hertz. That means you get one period every 52 nanoseconds. 1s/19.2 MHz = 52.08
    
    Hardware pwm is great if you need reliabiliy and precision. However, the hardware pwm on a raspberry pi is 
    limited to four output pins and only two channels. If you wanted to light up an RGB LED using pwm, that 
    wouldn't be possible because you would need three separate channels. That's where using software pwm would
    be more beneficial.

    Software pwm is slower than hardware pwm, but it allows you to use any output pin and is not restricted to
    two channels. This software pwm is default configured to run at 60 Hertz. Meaning you get one period every 16
    milliseconds. Software pwm is better if you need more than two channels, and reliability and precision are
    flexible.
*/

#ifndef BCM2835_SOFTWARE_PWM_H
#define BCM2835_SOFTWARE_PWM_H

#include <stdlib.h>
#include <bcm2835.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
    /*! Sets the software frequency in Hertz to the value passed in to the frequency parameter
      Setting the frequency allows us to find how long in time each period is by diving time by freqency
      Tosc = 1 second / Frequency

      Setting a higher frequency value will use more processing power, but more periods. The default
      frequency is 60 Hertz
      \param[in] frequency Frequency in Hertz
    */
    extern void bcm2835_software_pwm_set_frequency(uint32_t frequency);

    /*! Creates a channel that can be assigned a range, data, and pins to output to
      Each channel will start a new thread once \sa bcm2835_software_pwm_start_all_channels is called
      That channel will manage the active on and active off phase of the electric pulses for every pin
      that has been assigned to the channel using \sa bcm2835_software_pwm_assign_pin_to_channel
      \param[in] frequency Frequency in Hertz
    */
    extern void bcm2835_software_pwm_create_channel(uint8_t channel);

    /*! Sets the maximum range of the Software PWM output
      The data value can vary between 0 and this range to control PWM output
      \param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
      \param[in] range The maximum value permitted for DATA.
    */
    extern void bcm2835_software_pwm_set_range(uint8_t channel, uint32_t range);

    /*! Sets the Software PWM pulse ratio to emit to DATA/RANGE, where RANGE is set by
      \sa bcm2835_software_pwm_set_range().
      \param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
      \param[in] data Controls the PWM output ratio as a fraction of the range. 
      Can vary from 0 to RANGE.
    */
    extern void bcm2835_software_pwm_set_data(uint8_t channel, uint32_t data);

    /*! Adds a RPiGPIOPin pin to the pins the input parameter channel outputs to
      \param[in] channel An arbitrary PWM channel created earlier by \sa bcm2835_software_pwm_create_channel
      \param[in] pin GPIO number, or one of RPI_GPIO_P1_* from \ref RPiGPIOPin.
    */
    extern void bcm2835_software_pwm_assign_pin_to_channel(uint8_t channel, RPiGPIOPin pin);

    /*! Creates new threads using pthread_create for each of the channels created using
      \sa bcm2835_software_pwm_start_all_channels
      Note that this action must be called after all of your desired channels are created
    */
    extern void bcm2835_software_pwm_start_all_channels();

    /*! Closes the software pwm library, stopping all threads and deallocating any allocated memory
      Note that this should be called before any final \sa bcm2835_gpio_write writes so no conflicts in 
      state from multi-threading occur
    */
    extern void bcm2835_software_pwm_close();
#ifdef __cplusplus
}
#endif

#endif // BCM2835_SOFTWARE_PWM_H