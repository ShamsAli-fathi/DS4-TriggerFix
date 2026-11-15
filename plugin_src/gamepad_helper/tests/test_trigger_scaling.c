#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "trigger_scaling.h"

static void cases_for_102(void) {
    const struct { uint8_t raw, expected; } cases[] = {
        {0, 0}, {25, 63}, {51, 128}, {75, 188}, {102, 255},
        {103, 255}, {150, 255}, {255, 255}
    };
    for (unsigned i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        uint8_t out = ds4_scale_trigger_u8(cases[i].raw, 102);
        if (out != cases[i].expected) {
            fprintf(stderr, "FAIL raw=%u expected=%u got=%u\n",
                    cases[i].raw, cases[i].expected, out);
            assert(out == cases[i].expected);
        }
    }
    assert(ds4_scale_trigger_u8(72, 0) == 72);
    assert(ds4_scale_trigger_u8(0, 1) == 0);
    assert(ds4_scale_trigger_u8(1, 1) == 255);
    assert(ds4_scale_trigger_u8(255, 255) == 255);
    for (int max = 1; max <= 255; ++max) {
        uint8_t previous = 0;
        for (int raw = 0; raw <= 255; ++raw) {
            uint8_t value = ds4_scale_trigger_u8((uint8_t)raw, (uint8_t)max);
            unsigned reference = (unsigned)((double)raw * 255.0 / max + 0.5);
            if (reference > 255) reference = 255;
            assert(value == reference);
            assert(value >= previous);
            previous = value;
        }
        assert(ds4_scale_trigger_u8((uint8_t)max, (uint8_t)max) == 255);
    }
}

static void fields_and_digital_behavior(void) {
    ScePadData pad;
    memset(&pad, 0xA5, sizeof(pad));
    pad.buttons = SCE_PAD_BUTTON_CROSS | SCE_PAD_BUTTON_R2;
    pad.analogButtons.l2 = 100;
    pad.analogButtons.r2 = 51;
    pad.leftStick.x = 65;
    pad.rightStick.y = 99;

    ds4_scale_pad_r2(&pad, 102, 0);
    assert(pad.analogButtons.r2 == 128);
    assert(pad.analogButtons.l2 == 100);
    assert(pad.buttons == (SCE_PAD_BUTTON_CROSS | SCE_PAD_BUTTON_R2));
    assert(pad.leftStick.x == 65 && pad.rightStick.y == 99);

    pad.analogButtons.r2 = 8;
    pad.buttons = SCE_PAD_BUTTON_CROSS;
    ds4_scale_pad_r2(&pad, 102, 30);
    assert(pad.analogButtons.r2 == 20);
    assert((pad.buttons & SCE_PAD_BUTTON_R2) == 0);
    assert((pad.buttons & SCE_PAD_BUTTON_CROSS) != 0);

    pad.analogButtons.r2 = 20;
    ds4_scale_pad_r2(&pad, 102, 30);
    assert(pad.analogButtons.r2 == 50);
    assert((pad.buttons & SCE_PAD_BUTTON_R2) != 0);

    pad.analogButtons.r2 = 0;
    ds4_scale_pad_r2(&pad, 102, 30);
    assert((pad.buttons & SCE_PAD_BUTTON_R2) == 0);

    pad.analogButtons.r2 = 50;
    pad.buttons = SCE_PAD_BUTTON_CROSS;
    ds4_scale_pad_r2(&pad, 0, 30);
    assert(pad.analogButtons.r2 == 50 && pad.buttons == SCE_PAD_BUTTON_CROSS);
    ds4_scale_pad_r2(NULL, 102, 30);
}

static void both_trigger_fields(void) {
    for (int left = 0; left <= 1; ++left) {
        const int thresholds[] = {0, 30, 255};
        for (unsigned t = 0; t < sizeof(thresholds)/sizeof(thresholds[0]); ++t) {
            int threshold = thresholds[t];
            for (int raw = 0; raw <= 255; ++raw) {
                ScePadData pad, expected;
                memset(&pad, 0xa5, sizeof(pad));
                pad.buttons = SCE_PAD_BUTTON_CROSS | SCE_PAD_BUTTON_L2 | SCE_PAD_BUTTON_R2;
                pad.analogButtons.l2 = left ? raw : 37;
                pad.analogButtons.r2 = left ? 37 : raw;
                expected = pad;
                unsigned scaled = (unsigned)((double)raw * 255.0 / 102 + 0.5);
                if (scaled > 255) scaled = 255;
                uint32_t bit = left ? SCE_PAD_BUTTON_L2 : SCE_PAD_BUTTON_R2;
                if (left) expected.analogButtons.l2 = scaled;
                else expected.analogButtons.r2 = scaled;
                if (threshold) {
                    if (scaled >= (unsigned)threshold) expected.buttons |= bit;
                    else expected.buttons &= ~bit;
                }
                if (left) ds4_scale_pad_l2(&pad, 102, threshold);
                else ds4_scale_pad_r2(&pad, 102, threshold);
                assert(memcmp(&pad, &expected, sizeof(pad)) == 0);
            }
        }
    }
    ScePadData pad, expected;
    memset(&pad, 0, sizeof(pad)); pad.analogButtons.l2 = 51;
    expected = pad;
    ds4_scale_pad_l2(&pad, 0, 30);
    assert(memcmp(&pad, &expected, sizeof(pad)) == 0);
    ds4_scale_pad_l2(NULL, 102, 30);
}

int main(void) {
    cases_for_102();
    fields_and_digital_behavior();
    both_trigger_fields();
    puts("PASS: L2/R2 rounding, clamping, monotonicity, digital bits and unrelated bytes");
    return 0;
}
