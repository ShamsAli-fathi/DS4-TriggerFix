#ifndef GAMEPAD_HELPER_TRIGGER_SCALING_H
#define GAMEPAD_HELPER_TRIGGER_SCALING_H

#include <stdint.h>
#include <stddef.h>
#include "pad.h"

/*
 * Map the measured physical maximum (e.g., 102) onto 255. All arithmetic
 * is integer to keep input handling cheap. The rounded result saturates.
 * Input max 0 is explicitly treated as disabled, never divided by.
 */
static inline uint8_t ds4_scale_trigger_u8(uint8_t input, uint8_t input_max) {
    if (input_max == 0) {
        return input;
    }
    uint32_t mapped = ((uint32_t)input * 255u + (uint32_t)input_max / 2u)
                       / (uint32_t)input_max;
    return mapped >= 255u ? 255u : (uint8_t)mapped;
}

/*
 * Rewrite only the analog R2 byte. Digital threshold is optional:
 *   0: leave the original R2 button bit completely unchanged (default).
 *   1..255: synthesize the digital R2 bit from the scaled analog value.
 * Call BEFORE custom_button() so existing R2 remappings still work.
 */
static inline void ds4_scale_pad_r2(ScePadData* data, uint8_t input_max,
                                     uint8_t digital_threshold) {
    if (data == NULL || input_max == 0) {
        return;
    }
    data->analogButtons.r2 = ds4_scale_trigger_u8(data->analogButtons.r2, input_max);
    if (digital_threshold != 0) {
        if (data->analogButtons.r2 >= digital_threshold) {
            data->buttons |= SCE_PAD_BUTTON_R2;
        } else {
            data->buttons &= ~((uint32_t)SCE_PAD_BUTTON_R2);
        }
    }
}

/*
 * Rewrite only the analog L2 byte. Digital threshold is optional:
 *   0: leave the original L2 button bit completely unchanged (default).
 *   1..255: synthesize the digital L2 bit from the scaled analog value.
 * Call BEFORE custom_button() so existing L2 remappings still work.
 */
static inline void ds4_scale_pad_l2(ScePadData* data, uint8_t input_max,
                                     uint8_t digital_threshold) {
    if (data == NULL || input_max == 0) {
        return;
    }
    data->analogButtons.l2 = ds4_scale_trigger_u8(data->analogButtons.l2, input_max);
    if (digital_threshold != 0) {
        if (data->analogButtons.l2 >= digital_threshold) {
            data->buttons |= SCE_PAD_BUTTON_L2;
        } else {
            data->buttons &= ~((uint32_t)SCE_PAD_BUTTON_L2);
        }
    }
}

#endif
