#include <stdint.h>

#include "main.h"
#include "globals.h"
#include "stm32f1xx.h"

volatile uint32_t g_tick = 0;

int main(void)
{
  clock_init();
  systick_init();
  spi_init();
  led_init();

  enum led_state state = led_off;
  int ix = 0;
  uint8_t digit[10] = {
    0xFC, 0x60, 0xDA, 0xF2, 0x66,
    0xB6, 0xBE, 0xE0, 0xFE, 0xE6
  };
  while (1) {
    state = led_toggle(state);
    spi_write(digit[ix++ % 10]);
    delay_ms(400);
  }
  return 0;
}

void clock_init()
{
  FLASH->ACR |= FLASH_ACR_LATENCY_2;
  RCC->CFGR |= RCC_CFGR_PPRE1_2;
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY));

  RCC->CFGR |= RCC_CFGR_PLLSRC;
  RCC->CFGR |= RCC_CFGR_PLLMULL9;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
  
  SystemCoreClockUpdate();
}

void systick_init()
{
  SysTick_Config(SystemCoreClock/1000);
}

void led_init()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
  GPIOA->CRL |= GPIO_CRL_MODE1;
}

void spi_init()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  GPIOA->CRL &= ~((GPIO_CRL_MODE4 | GPIO_CRL_CNF4) |
		  (GPIO_CRL_MODE5 | GPIO_CRL_CNF5) |
		  (GPIO_CRL_MODE7 | GPIO_CRL_CNF7));

  GPIOA->CRL |= GPIO_CRL_MODE4 ; //| GPIO_CRL_CNF5_1;
  GPIOA->BSRR = GPIO_BSRR_BS4;
  
  GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1;
  GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;

  SPI1->CR1 = SPI_CR1_BR |
    SPI_CR1_LSBFIRST |
    SPI_CR1_SSM |
    SPI_CR1_SSI |
    SPI_CR1_MSTR;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_write(uint8_t byte)
{
  GPIOA->BSRR = GPIO_BSRR_BR4;
  while (!(SPI1->SR & SPI_SR_TXE));
  *((volatile uint8_t *) &(SPI1->DR)) = byte;
  while (!(SPI1->SR & SPI_SR_TXE));
  while (SPI1->SR & SPI_SR_BSY);
  GPIOA->BSRR = GPIO_BSRR_BS4;
  }


enum led_state led_toggle(enum led_state state)
{
  switch(state) {
  case led_off:
    GPIOA->BSRR |= GPIO_BSRR_BS1;
    return led_on;
  case led_on:
    GPIOA->BSRR |= GPIO_BSRR_BR1;
    return led_off;
  }
}

void delay_ms(uint32_t ms) {
  uint32_t time = g_tick;
  while (g_tick - time <= ms);
}
