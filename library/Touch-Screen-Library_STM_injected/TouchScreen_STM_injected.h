// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License

// Ported to STM32 by Jaret Burkett https://github.com/jaretburkett

#ifndef TOUCHSCREEN_STMM_INJECTED_H_
#define TOUCHSCREEN_STMM_INJECTED_H_
#include <stdint.h>

class TSPoint {
 public:
  TSPoint(void);
  TSPoint(int16_t x, int16_t y, int16_t z);
  
  bool operator==(TSPoint);
  bool operator!=(TSPoint);

  int16_t x, y, z;
};

class TouchScreen {
 public:
  TouchScreen(uint16_t rxplate);

  TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t rx);
  void setADCs();
  bool isTouching(void);
  uint16_t pressure(void);

  TSPoint getPoint();
  int16_t pressureThreshhold;

private:
  uint8_t _yp, _ym, _xm, _xp;
  uint16_t _rxplate;
  int JDR1,JDR2;
  void ADC_WAIT();
};

#endif
