#pragma once

#include "stm32f4xx_hal.h"
#include "types.h"
struct shiftregister {	
	void(*init)(void);		
	void(*shift_out)(u8 high_byte, u8 low_byte);		
};

extern const struct shiftregister ShiftRegister;