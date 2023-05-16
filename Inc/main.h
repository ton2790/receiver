#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

  enum led_state {
    led_off, led_on
  };

  void clock_init();
  void led_init();
  void spi_init();
  enum led_state led_toggle(enum led_state);
  void systick_init();
  void delay_ms(uint32_t ms);
  void spi_write(uint8_t);
  


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
