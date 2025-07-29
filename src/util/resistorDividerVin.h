#pragma once
#include <stdint.h>
static inline float resistorDividerVin(float vout,uint32_t r1,uint32_t r2) {
	return vout * (r1+r2) / r2;
}