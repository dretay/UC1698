#pragma once

#if defined(STM32F407xx) || defined(STM32F103xB)
#include "main.h"
#elif defined(ESP32)
#include "Arduino.h"
#else
#error Unsupported board!
#endif


#include "types.h"
struct shiftregister {
	void(*initialize)(void);
	void(*shift_out)(u8 high_byte, u8 low_byte);
};

extern const struct shiftregister ShiftRegister;
