
#include <cstdio> // We're running short on memory, use printf instead of cout (program must be loaded to ram to enable flash)
#include <vector>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/flash.h"

constexpr bool resetCounts = true;

constexpr int sleepLength = 50; // ms
constexpr int totalIters = 15 /* seconds */ * (1000 /* ms per s */ / sleepLength);
constexpr int stabIters1 = 5, stabIters2 = 2;
constexpr int minMeasurementTimeDiff = 15;
constexpr int minDelta = 56;

int adcValues[totalIters];

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

struct Counts {
    int successes = 0;
    int fails = 0;
    static constexpr int addr = 256 * 1024; // Multiple of 4096

    void read() {
        successes = * (const int *) (XIP_BASE + addr);
        fails = * (const int *) (XIP_BASE + addr + 4);
    }

    void write() {
        int data[FLASH_PAGE_SIZE/4];
        data[0] = successes;
        data[1] = fails;

        flash_range_erase(addr, FLASH_SECTOR_SIZE);
        flash_range_program(addr, (uint8_t *) data, FLASH_PAGE_SIZE);
    }
};


struct Step {
    int iter = 0;
    int delta = 0;
};

void reset() {
    printf("Switching camera off\n");
    gpio_put(cameraPin, false);
    sleep_ms(10000);
    printf("Switching camera on\n");
    gpio_put(cameraPin, true);
}

Step findMaxStep(int startIter, int stabIters) {
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

bool isCameraRunning() {
    Step s1 = findMaxStep(0, stabIters1);
    printf("1. step: iter: %d,\t delta: %d\n", s1.iter, s1.delta);
    Step s2 = findMaxStep(s1.iter + minMeasurementTimeDiff, stabIters2);
    printf("2. step: iter: %d,\t delta: %d\n", s2.iter, s2.delta);
    if (s2.delta < -minDelta) {
        printf("Camera success\n");
        return true;
    }
    printf("Camera fail\n");
    return false;
}

void init() {
    stdio_init_all();
    sleep_ms(1200);
    printf("\n\nStarting microcontroller\n");

    adc_init();
    adc_select_input(adcPin - adcOffset);
    gpio_init(cameraPin);
    gpio_set_dir(cameraPin, GPIO_OUT);
    gpio_put(cameraPin, true);
    gpio_init(pixelPwrPin);
}

bool experimentCameraRunning() {
    for (int i = 0; i < totalIters; ++i) {
        int adcValue = adc_read();
        adcValues[i] = adcValue;
        sleep_ms(sleepLength);
        printf("ADC value: %d         (iteration %d)\n", adcValue, i);
    }
    return isCameraRunning();
}

void loop() {
    while(true) {
    }
}

int main() {
    init();
    Indicator indicator(pio0);

    if(resetCounts) {
        Counts counts;
        counts.write();
        printf("Resetting success/fail counts\n");
        loop();
    }

    Counts counts;
    counts.read();
    printf("successes: %d  fails: %d\n", counts.successes, counts.fails);

    while(! experimentCameraRunning()) {
        printf("Camera start failed: resetting\n");
        indicator.fail();
        reset();
        indicator.clear();
        ++counts.fails;
        counts.write();
    }

    printf("Camera start successful: looping forever\n");
    ++counts.successes;
    counts.write();

    loop();
    return 0;
}
