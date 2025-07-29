#pragma once
#include <stdint.h>
static inline float adc2voltage(uint32_t adc_value,float vadc)
{
    return ((float)adc_value / 4095.0f) * vadc;
}