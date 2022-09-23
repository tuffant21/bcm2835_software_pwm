#ifndef BCM2835_SOFTWARE_PWM_H
#define BCM2835_SOFTWARE_PWM_H

#include <stdlib.h>
#include <bcm2835.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern void bcm2835_software_pwm_set_frequency(uint32_t frequency);
    extern void bcm2835_software_pwm_create_channel(uint8_t channel);
    extern void bcm2835_software_pwm_set_range(uint8_t channel, uint32_t range);
    extern void bcm2835_software_pwm_set_data(uint8_t channel, uint32_t data);
    extern void bcm2835_software_pwm_assign_pin_to_channel(uint8_t channel, RPiGPIOPin pin);
    extern void bcm2835_software_pwm_start_all_channels();
    extern void bcm2835_software_pwm_close();
#ifdef __cplusplus
}
#endif

#endif // BCM2835_SOFTWARE_PWM_H