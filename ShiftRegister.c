#include "ShiftRegister.h"

#define	GPIO_SET(x)		{HAL_GPIO_WritePin(SHIFT_##x##_GPIO_Port, SHIFT_##x##_Pin,GPIO_PIN_SET);}
#define	GPIO_RESET(x)		{HAL_GPIO_WritePin(SHIFT_##x##_GPIO_Port, SHIFT_##x##_Pin,GPIO_PIN_RESET);}


static void init()
{
}
static void shift_bit(u8 byte, u8 idx)
{
	GPIO_RESET(CLOCK);
	if (byte & (1 << idx)){ GPIO_SET(DATA) }
	else{ GPIO_RESET(DATA); }				
	GPIO_SET(CLOCK);
}
static void shift_out(u8 high_byte, u8 low_byte)
{
	GPIO_RESET(STROBE);

	//clear everything
	GPIO_RESET(CLOCK);
	GPIO_RESET(DATA);

	for (int i = 7; i >= 0; i--) shift_bit(high_byte, i);
	for (int i = 7; i >= 0; i--) shift_bit(low_byte, i);
	

	GPIO_SET(STROBE);//When STROBE is HIGH, data propagates through the latch to 3 - STATE output gates.
}
//static void shift_something()
//{
//	shift_out(0x0808);
//	shift_out(0);
//}

const struct shiftregister ShiftRegister= { 
	.init = init,		
	.shift_out = shift_out,		
};