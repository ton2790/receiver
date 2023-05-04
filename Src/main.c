#include <stdint.h>

#include "main.h"
#include "globals.h"
#include "stm32f1xx.h"

volatile uint32_t g_tick = 0;

int main(void)
{
  clock_init();
  systick_init();
  led_init();

  enum led_state state = led_off;
  uint32_t time = g_tick;

  while (1) {
    if (g_tick - time > 1000) {
      time = g_tick;
      state = led_toggle(state);
    }
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
