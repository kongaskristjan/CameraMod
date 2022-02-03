
#include <iostream>
#include <vector>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

constexpr int sleepLength = 50; // ms
constexpr int totalIters = 15 /* seconds */ * (1000 /* ms per s */ / sleepLength);
constexpr int stabIters1 = 5, stabIters2 = 2;
constexpr int minMeasurementTimeDiff = 15;
constexpr int minDelta = 56;

const int adcPin = 28, cameraPin = 0; // Voltage pin = A2=D2, cameraPin = D6
const int pixelPwrPin = 11, pixelPin = 12;
const int adcOffset = 26;

class NeoPixel {
private:
    pio_hw_t *pio;
    uint offset;

public:
    explicit NeoPixel(pio_hw_t *pio): pio(pio) {
        gpio_set_dir(pixelPwrPin, GPIO_OUT);
        gpio_put(pixelPwrPin, true);
        offset = pio_add_program(pio, &ws2812_program);
        ws2812_program_init(pio, 0, offset, pixelPin, 800000, true);
    }

    ~NeoPixel() {
        gpio_put(pixelPwrPin, false);
        pio_remove_program(pio, &ws2812_program, offset);
    }

    void write(uint8_t r, uint8_t g, uint8_t b) {
        uint32_t grb_ = ((uint32_t) r << 16) | ((uint32_t) g << 24) | ((uint32_t) b << 8);
        std::cout << r << " " << g << " " << b << std::endl;
        std::cout << grb_ << std::endl;
        pio_sm_put_blocking(pio0, 0, grb_);
    }
};

class Indicator {
private:
    NeoPixel neo;
public:
    Indicator(pio_hw_t *pio): neo(pio) {
    }

    void clear() {
        neo.write(0, 0, 0);
    }

    void fail() {
        neo.write(20, 0, 0);
    }
};


struct Step {
    int iter = 0;
    int delta = 0;
};

void reset() {
    std::cout << "Switching camera off" << std::endl;
    gpio_put(cameraPin, false);
    sleep_ms(10000);
    std::cout << "Switching camera on" << std::endl;
    gpio_put(cameraPin, true);
}

Step findMaxStep(const std::vector<int> &adcValues, int startIter, int stabIters) {
    Step best;
    for (int i = startIter; i < totalIters - stabIters; ++i) {
        int delta = adcValues[i + stabIters] - adcValues[i];
        if (delta < best.delta) {
            best.iter = i;
            best.delta = delta;
        }
    }
    return best;
}

bool isCameraRunning(const std::vector<int> &adcValues) {
    Step s1 = findMaxStep(adcValues, 0, stabIters1);
    std::cout << "1. step: iter: " << s1.iter << "\t delta: " << s1.delta << std::endl;
    Step s2 = findMaxStep(adcValues, s1.iter + minMeasurementTimeDiff, stabIters2);
    std::cout << "2. step: iter: " << s2.iter << "\t delta: " << s2.delta << std::endl;
    if (s2.delta < -minDelta) {
        std::cout << "Camera success" << std::endl;
        return true;
    }
    std::cout << "Camera fail" << std::endl;
    return false;
}

void init() {
    stdio_init_all();
    std::cout << "Starting microcontroller" << std::endl;

    adc_init();
    adc_select_input(adcPin - adcOffset);
    gpio_init(cameraPin);
    gpio_set_dir(cameraPin, GPIO_OUT);
    gpio_put(cameraPin, true);
    gpio_init(pixelPwrPin);
}

bool experimentCameraRunning() {
    std::vector<int> adcValues; // Sensor value range: 0-4095 (0-3.3V)
    for (int i = 0; i < totalIters; ++i) {
        int adcValue = adc_read();
        adcValues.push_back(adcValue);
        sleep_ms(sleepLength);
        std::cout << "ADC value: " << adcValue << std::endl;
    }
    return isCameraRunning(adcValues);
}

int main() {
    init();
    Indicator indicator(pio0);

    while(! experimentCameraRunning()) {
        std::cout << "Camera start failed: resetting" << std::endl;
        indicator.fail();
        reset();
        indicator.clear();
    }

    std::cout << "Camera start successful: looping forever" << std::endl;
    while (true)
        sleep_ms(10000);

    return 0;
}
