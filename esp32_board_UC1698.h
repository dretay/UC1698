#pragma once

#include "UC1698.h"
#include "gfx.h"
#include "stdbool.h"
#include "types.h"

#if defined(ESP32)
#include "Arduino.h"
#endif

#include "string.h"
#include "ShiftRegister.h"

#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))

//macros
//#define	GPIO_SET(x)		{HAL_GPIO_WritePin(LCD_##x##_GPIO_Port, LCD_##x##_Pin,GPIO_PIN_SET);}
//#define	GPIO_RESET(x)		{HAL_GPIO_WritePin(LCD_##x##_GPIO_Port, LCD_##x##_Pin,GPIO_PIN_RESET);}
//#define	LCD_WRITE_DATA(data)	GPIOD->ODR = 0x00FF;GPIOD->ODR = (data) ;		//Write data
//
//#define	LCD_SET_DATA_OUT()		GPIOD->MODER |= 0x00005555; 									//The data line is set to output mode
//#define	LCD_SET_DATA_IN()	    GPIOD->MODER &= 0xffff0000; 									//The data line is set to input mode
//
//#define GPIO_ReadInputData() (uint16_t)GPIOD->IDR
static u8 CE = 4, CD = 3, RD = 2, WR = 1, RST = 0;
static u8 pins;
static void write_command(u8 command)
{		
	bitClear(pins, CE);  //cs0
	bitClear(pins, CD); //cd
	bitSet(pins, RD);  //wr1
	bitClear(pins, WR); //wr0	
		
	ShiftRegister.shift_out(command, pins);
	
	bitSet(pins, WR);
	bitSet(pins, CE);
	ShiftRegister.shift_out(command, pins);	
	
}
static void write_data(u8 data)
{	
	bitClear(pins, CE);
	bitSet(pins, CD);
	bitSet(pins, RD);
	bitClear(pins, WR);	
		
	ShiftRegister.shift_out(data, pins);
	
	bitSet(pins, WR);
	bitSet(pins, CE);
	ShiftRegister.shift_out(data, pins);
}
static void reset(void)
{	
	bitSet(pins, RST);
	ShiftRegister.shift_out(0x00, pins);
	delay(20);         	
	bitClear(pins, RST);
	ShiftRegister.shift_out(0x00, pins);
	delay(20);
	bitSet(pins, RST);
	ShiftRegister.shift_out(0x00, pins);
	delay(200);
	write_command(SYSTEM_RESET); 
	delay(100);   	
}

static GFXINLINE bool init_board(GDisplay* g) {
	(void) g; 
	pinMode(2, OUTPUT);  // s0
    pinMode(4, OUTPUT);  // s1
    pinMode(15, OUTPUT);  // s1

	return true;

}
static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

#if GDISP_NEED_CONTROL
static void board_backlight(GDisplay *g, uint8_t percent) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) percent;
}

static void board_contrast(GDisplay *g, uint8_t percent) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) percent;
}

static void board_power(GDisplay *g, powermode_t pwr) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) pwr;
}
#endif /* GDISP_NEED_CONTROL */


static GFXINLINE void acquire_bus(GDisplay* g)
{
	(void) g;
}

static GFXINLINE void release_bus(GDisplay* g)
{
	(void) g;
}