/**
 * API for recording measurements: Implemented by user
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void logMeasurements(int count, const int32_t measurements[]);

#ifdef __cplusplus
}
#endif