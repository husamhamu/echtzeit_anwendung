
#include <usbtransfer.h>
#include "measurement_logger.h"
#include "gui_comm.h"


/**
 * Number of entries in all buffers
 */
#define BUF_SIZE 2048

/**
 * Example buffer
 */
int32_t ringBuffer[4][BUF_SIZE];

/**
 * DO NOT CHANGE: Number of entries in all buffers
 */
const uint32_t outgoingBufferSize = BUF_SIZE;

/**
 * Offset into all ringBuffers. Points to the next-to-write
 */
uint32_t outgoingBufferOffset = 30;


const bufferDef_t buffers [] = {
		{ringBuffer[0], (ringBuffer[0]+BUF_SIZE)},
		{ringBuffer[1], (ringBuffer[1]+BUF_SIZE)},
		{ringBuffer[2], (ringBuffer[2]+BUF_SIZE)},
		{ringBuffer[3], (ringBuffer[3]+BUF_SIZE)}
};

/**
 * DO NOT CHANGE: Number of buffers declared in [buffers]
 */
const uint8_t outgoingBufferCount = sizeof(buffers) / sizeof(bufferDef_t);


void logMeasurements(int count, const int32_t measurements[]) {
    if (outgoingBufferWriteProtection) {
        // Discard measurement data if data is currently being transferred to the PC
        return;
    }

    if (count < 1 || count > outgoingBufferCount) {
        // Invalid number of measured values, cancel function
        return;
    }

    for (int i = 0; i < count; i++) {
        // Write the measured value to the corresponding ring buffer
        *((int32_t*)buffers[i].start + outgoingBufferOffset) = measurements[i];
    }

    // Update write index in the ring buffers
    outgoingBufferOffset = (outgoingBufferOffset + 1) % BUF_SIZE;
}