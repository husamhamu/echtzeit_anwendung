#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void use4BSineTable();
void buildSineTable(uint8_t bits);
uint32_t interpolateSine(uint16_t angle, uint32_t scale);
uint32_t floatSine(uint16_t angle, uint32_t scale);
uint32_t doubleSine(uint16_t angle, uint32_t scale);

#ifdef __cplusplus
}
#endif
