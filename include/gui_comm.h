/**
 * Includes everything that needs to be implemented by users for communication with the GUI:
 *
 * * How the USB Driver finds the measurement-data that is shown in the GUI
 * * How the STM32 gets told that paramters have been changed by the GUI
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
void updateParameters(int32_t parameters[8]);
#ifdef __cplusplus
}
#endif

/**
 * 1 if USB-Driver is accessing the buffers and they therefore are not allowed to be modified
 * 0 if modifying the buffers is ok
 */
extern uint8_t outgoingBufferWriteProtection;

enum DataTypes {
	DataTypeUnknown = 0xFFFF,
	DataTypeUInt32 = 0x2,
	DataTypeInt32 = 0x3,
	DataTypeInt8 = 0x8,
	DataTypeUInt8 = 0x9
};

typedef struct bufferDef {
	void * const start;
	void * const end;
} bufferDef_t;

/**
 * Start and and excl. end of all buffers in the order the UI expects them
 */
extern const bufferDef_t buffers[];

/**
 * The Number of entries of all buffers declared in [buffers]
 */
extern const uint32_t outgoingBufferSize;

/**
 * The current offset (in number of entries) into all buffers declared
 */
extern uint32_t outgoingBufferOffset;


/**
 * The number of buffers declared in [buffers]
 */
extern const uint8_t outgoingBufferCount;

