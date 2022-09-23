#include "bcm2835_software_pwm.h"

#define DEFAULT_FREQUENCY 60

typedef struct ChannelData {
    pthread_t threadId;
    uint8_t run;
    uint8_t channel;
    uint32_t range;
    uint32_t data;
    uint8_t pinsLength;
    RPiGPIOPin *pins;
} ChannelData_t;

typedef struct SoftwarePwmData {
    uint8_t initialized;
    uint32_t frequency;
    float microsecondsPerFrequency;
    uint8_t channelDatasLength;
    ChannelData_t *channelDatas;
} SoftwarePwmData_t;

static SoftwarePwmData_t softwarePwcData = {
    .initialized = 0,
    .frequency = DEFAULT_FREQUENCY,
    .microsecondsPerFrequency = (1000000.0 / DEFAULT_FREQUENCY),
    .channelDatasLength = 0,
    .channelDatas = NULL
};

void *bcm2835_software_pwm_thread(void *data) {
    ChannelData_t *channelData = (ChannelData_t *) data;

    for (int timeInterval = 0; channelData->run; timeInterval++) {
        if (channelData->range == 0) {
            continue;
        }

        if (timeInterval == channelData->range - 1) {
            timeInterval = 0;
        }

        float dutyCycle = (float)channelData->data / (float)channelData->range; // "on time" - The ratio between the HIGH time to the full period of a single cycle [%]
        float timeOn = dutyCycle * softwarePwcData.microsecondsPerFrequency; // ratio of time on per pwm frequency
        float timeOff = softwarePwcData.microsecondsPerFrequency - timeOn; // ratio of time off per pwm frequency

        // on period
        for (uint8_t i = 0; i < channelData->pinsLength; i++) {
            bcm2835_gpio_write(channelData->pins[i], HIGH);
        }
        bcm2835_delayMicroseconds(timeOn);

        // off period
        for (uint8_t i = 0; i < channelData->pinsLength; i++) {
            bcm2835_gpio_write(channelData->pins[i], LOW);
        }
        bcm2835_delayMicroseconds(timeOff);
    }
}

ChannelData_t *bcm2835_software_pwm_get_channel(uint8_t channel) {
    for (uint8_t i = 0; i < softwarePwcData.channelDatasLength; i++) {
        if (softwarePwcData.channelDatas[i].channel == channel) {
            return &softwarePwcData.channelDatas[i];
        }
    }

    return NULL;
}

void bcm2835_software_pwm_set_frequency(uint32_t frequency) {
    softwarePwcData.frequency = frequency;
    softwarePwcData.microsecondsPerFrequency = 1000000.0 / frequency;
}

void bcm2835_software_pwm_create_channel(uint8_t channel) {
    size_t channelDataStructSize = sizeof(*softwarePwcData.channelDatas);
    size_t currentSize = channelDataStructSize * softwarePwcData.channelDatasLength;
    size_t newSize = currentSize + channelDataStructSize;

    softwarePwcData.channelDatas = realloc(softwarePwcData.channelDatas, newSize);

    ChannelData_t *newChannel = &softwarePwcData.channelDatas[softwarePwcData.channelDatasLength];
    newChannel->threadId = 0;
    newChannel->run = 0;
    newChannel->channel = channel;
    newChannel->range = 0;
    newChannel->data = 0;
    newChannel->pinsLength = 0;
    newChannel->pins = NULL;

    softwarePwcData.channelDatasLength++;
}

void bcm2835_software_pwm_set_range(uint8_t channel, uint32_t range) {
    ChannelData_t *channelData = bcm2835_software_pwm_get_channel(channel);

    if (channelData) {
        channelData->range = range;
    }
}

void bcm2835_software_pwm_set_data(uint8_t channel, uint32_t data) {
    ChannelData_t *channelData = bcm2835_software_pwm_get_channel(channel);

    if (channelData) {
        channelData->data = data;
    }
}

void bcm2835_software_pwm_assign_pin_to_channel(uint8_t channel, RPiGPIOPin pin) {
    ChannelData_t *channelData = bcm2835_software_pwm_get_channel(channel);

    if (!channelData) {
        return;
    }

    // check for existing entry
    for (uint8_t i = 0; i < channelData->pinsLength; i++) {
        if (channelData->pins[i] == pin) {
            return;
        }
    }

    size_t RPiGPIOPinStructSize = sizeof(*channelData->pins);
    size_t currentSize = RPiGPIOPinStructSize * channelData->pinsLength;
    size_t newSize = currentSize + RPiGPIOPinStructSize;

    channelData->pins = realloc(channelData->pins, newSize);
    channelData->pins[channelData->pinsLength] = pin;
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    channelData->pinsLength++;
}

void bcm2835_software_pwm_start_all_channels() {
    if (softwarePwcData.initialized) {
        return;
    }

    softwarePwcData.initialized = 1;

    for (uint8_t i = 0; i < softwarePwcData.channelDatasLength; i++) {
        ChannelData_t *channelData = &softwarePwcData.channelDatas[i];
        channelData->run = 1;
        pthread_create(&channelData->threadId, NULL, bcm2835_software_pwm_thread, (void *) channelData);
    }
}

void bcm2835_software_pwm_close() {
    for (uint8_t i = 0; i < softwarePwcData.channelDatasLength; i++) {
        softwarePwcData.channelDatas[i].run = 0;
        pthread_join(softwarePwcData.channelDatas[i].threadId, NULL);
    }

    // free resources
    for (uint8_t i = 0; i < softwarePwcData.channelDatasLength; i++) {
        free(softwarePwcData.channelDatas[i].pins);
    }

    free(softwarePwcData.channelDatas);
}