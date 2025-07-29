#pragma once
#include <stdint.h>
static inline float resistorDividerVout(float vin,uint32_t r1,uint32_t r2) {
	return (vin * r2) / (r1 + r2);
}