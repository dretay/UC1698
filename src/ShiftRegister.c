#include "ShiftRegister.h"

#if defined(STM32F407xx) || defined(STM32F103xB)
//#define	GPIO_SET(x)		{HAL_GPIO_WritePin(SHIFT_##x##_GPIO_Port, SHIFT_##x##_Pin,GPIO_PIN_SET);}
//#define	GPIO_RESET(x)		{HAL_GPIO_WritePin(SHIFT_##x##_GPIO_Port, SHIFT_##x##_Pin,GPIO_PIN_RESET);}
#elif defined(ESP32)
#define STROBE 15
#define DATA 2
#define CLOCK 4
#define	GPIO_SET(x)		{digitalWrite(x, HIGH);}
#define	GPIO_RESET(x)		{digitalWrite(x, LOW);}
#else
#error Unsupported board!
#endif

extern SPI_HandleTypeDef hspi1;
static void initialize()
{
}
//static void shift_bit(u8 byte, u8 idx)
//{
//	GPIO_RESET(CLOCK);
//	if (byte & (1 << idx)){ GPIO_SET(DATA) }
//	else{ GPIO_RESET(DATA); }
//	GPIO_SET(CLOCK);
//	GPIO_RESET(DATA);
//}
//static void shift_out(u8 low_byte, u8 high_byte)
//{
//	GPIO_RESET(LATCH);
//
//	//clear everything
//	GPIO_RESET(CLOCK);
//	GPIO_RESET(DATA);
//
//	for (int i = 7; i >= 0; i--) shift_bit(high_byte, i);
//	for (int i = 7; i >= 0; i--) shift_bit(low_byte, i);
//
//
//	GPIO_SET(LATCH);//When STROBE is HIGH, data propagates through the latch to 3 - STATE output gates.
//}
static void shift_out(u8 low_byte, u8 high_byte)
{
	uint8_t TX_Data[2] = {0};
	TX_Data[0]=high_byte;
	TX_Data[1]=low_byte;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,RESET);
	if(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY){
		if(HAL_SPI_Transmit(&hspi1, TX_Data, sizeof(TX_Data), 1000) != HAL_OK){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,SET);
		}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,SET);
}

const struct shiftregister ShiftRegister= {
	.initialize = initialize,
	.shift_out = shift_out,
};
//static void initialize()
//{
//}
//static void shift_bit(u8 byte, u8 idx)
//{
//	//clock in the bit
//	GPIO_RESET(CLOCK);
//	if (byte & (1 << idx)){ GPIO_SET(DATA) }
//	else{ GPIO_RESET(DATA); }
//	GPIO_SET(CLOCK);
//
//	//zero data to prevent bleed
//	GPIO_RESET(DATA);
//}
//
//static void shift_out(u8 high_byte, u8 low_byte)
//{
//	//latch down to start transmission
//	GPIO_RESET(LATCH);
//
//	//clear everything
//	GPIO_RESET(CLOCK);
//	GPIO_RESET(DATA);
//	u8 reversed_high_byte = reverse(high_byte);
//
//	for (int i = 7; i >= 0; i--) shift_bit(high_byte, i);
//	for (int i = 7; i >= 0; i--) shift_bit(low_byte, i);
//
//	//latch up to end transmission
//	GPIO_SET(LATCH);
//}
//
//const struct shiftregister ShiftRegister= {
//	.initialize = initialize,
//	.shift_out = shift_out,
//};
