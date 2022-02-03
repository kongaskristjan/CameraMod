
#include <cstdio> // We're running short on memory, use printf instead of cout (program must be loaded to ram to enable flash)
#include <vector>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/flash.h"

constexpr bool resetCounts = false;

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
        offset = pio_add_program(pio, &ws2812_program);
        ws2812_program_init(pio, 0, offset, pixelPin, 800000, true);
    }

    ~NeoPixel() {
        write(0, 0, 0);
        pio_remove_program(pio, &ws2812_program, offset);
    }

    void write(uint8_t r, uint8_t g, uint8_t b) {
        uint32_t grb_ = ((uint32_t) r << 16) | ((uint32_t) g << 24) | ((uint32_t) b << 8);
        pio_sm_put_blocking(pio0, 0, grb_);
        sleep_ms(1); // 100 us is enough
    }

    void show_ms(uint32_t ms, uint8_t r, uint8_t g, uint8_t b) {
        write(r, g, b);
        sleep_ms(ms);
        write(0, 0, 0);
    }

    void blink_number(uint32_t number, uint8_t brightness) {
        bool showing = false;
        printf("Showing %d\n", number);
        sleep_ms(500);
        for(uint32_t bit = 1u << 31; bit; bit >>= 1) {
            if(number & bit)
                showing = true;

            if(showing) {
                if(number & bit)
                    show_ms(1000, 0, brightness, 0);
                else
                    show_ms(1000, brightness, 0, 0);
                show_ms(200, 0, 0, 0);
            }
        }
        sleep_ms(500);
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

    gpio_init(pixelPwrPin);
    gpio_set_dir(pixelPwrPin, GPIO_OUT);
    gpio_put(pixelPwrPin, true);
    sleep_ms(1200);
    printf("\n\nStarting microcontroller\n");

    adc_init();
    adc_select_input(adcPin - adcOffset);
    gpio_init(cameraPin);
    gpio_set_dir(cameraPin, GPIO_OUT);
    gpio_put(cameraPin, true);
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

void printSuccessesFails() {
    Counts counts;
    counts.read();
    printf("successes: %d  fails: %d\n", counts.successes, counts.fails);
}

void displaySuccessesFails(NeoPixel neo) {
    Counts counts;
    counts.read();

    int brightness = 8;
    neo.show_ms(2000, 0, 0, brightness);
    neo.blink_number(counts.successes, brightness);
    neo.show_ms(2000, 0, 0, brightness);
    neo.blink_number(counts.fails, brightness);
    neo.show_ms(2000, 0, 0, brightness);
}

int main() {
    init();
    NeoPixel neo(pio0);

    if(resetCounts) {
        Counts counts;
        counts.write();
        printf("Resetting success/fail counts\n");
        loop();
    }

    Counts counts;
    counts.read();
    printSuccessesFails();

    while(! experimentCameraRunning()) {
        ++counts.fails;
        counts.write();
        printf("Camera start failed: resetting\n");
        printSuccessesFails();

        neo.write(20, 0, 0);
        reset();
        neo.write(0, 0, 0);
    }

    ++counts.successes;
    counts.write();
    printf("Camera start successful: looping forever\n");
    printSuccessesFails();
    displaySuccessesFails(neo);

    loop();
    return 0;
}
