#pragma once
static inline float clamp(float val, float minVal, float maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}