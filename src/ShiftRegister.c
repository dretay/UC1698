#include "ShiftRegister.h"

#if defined(STM32F407xx) || defined(STM32F103xB) || defined(STM32F411xE)
#define SPI_CS_SET(x)                                                        \
  {                                                                          \
    HAL_GPIO_WritePin(SPI##x##_CS_GPIO_Port, SPI##x##_CS_Pin, GPIO_PIN_SET); \
  }
#define SPI_CS_RESET(x)                                                        \
  {                                                                            \
    HAL_GPIO_WritePin(SPI##x##_CS_GPIO_Port, SPI##x##_CS_Pin, GPIO_PIN_RESET); \
  }
#elif defined(ESP32)
#define STROBE 15
#define DATA 2
#define CLOCK 4
#define GPIO_SET(x)        \
  {                        \
    digitalWrite(x, HIGH); \
  }
#define GPIO_RESET(x)     \
  {                       \
    digitalWrite(x, LOW); \
  }
#else
#error Unsupported board!
#endif

extern SPI_HandleTypeDef hspi1;
static void initialize() {}

static void shift_out(u8 low_byte, u8 high_byte) {
  uint8_t TX_Data[2] = {0};
  TX_Data[0] = high_byte;
  TX_Data[1] = low_byte;
  SPI_CS_RESET(1);
  if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY) {
    if (HAL_SPI_Transmit(&hspi1, TX_Data, sizeof(TX_Data), 1000) != HAL_OK) {
    }
  }
  SPI_CS_SET(1);
}

const struct shiftregister ShiftRegister = {
    .initialize = initialize,
    .shift_out = shift_out,
};
