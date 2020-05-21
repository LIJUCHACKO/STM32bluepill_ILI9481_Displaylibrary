// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License

// Ported to STM32 by Jaret Burkett https://github.com/jaretburkett
// This code is modified to used injected channels instead of regular channels 
#include <stdint.h>
//#include "pins_arduino.h"
//#include "wiring_private.h"
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#elif defined (__STM32F1__)
  #include <avr/pgmspace.h>
  #include <WProgram.h>
#endif
#include "TouchScreen_STM_injected.h"

// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// 1 is no oversampling, whatever data we get is immediately returned
// 2 is double-sampling and we only return valid data if both points are the same
// 3+ uses insert sort to get the median value.
// We found 2 is precise yet not too slow so we suggest sticking with it!

#define NUMSAMPLES 4
uint32_t adc_read_injected( uint8_t reg)
{
	switch (reg) {
	case 1:
		return ADC1->regs->JDR1;
	case 2:
		return ADC1->regs->JDR2;
	case 3:
		return ADC1->regs->JDR3;
	case 4:
		return ADC1->regs->JDR4;
	}
	return 0;
}

TSPoint::TSPoint(void) {
  x = y = 0;
}

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool TSPoint::operator==(TSPoint p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TSPoint::operator!=(TSPoint p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;
  
  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save; 
  }
}
#endif
void TouchScreen::setADCs()
{
  //power on and calibration already done
  //adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_239DOT5CYC);  //emble
  //adc_set_sample_time(ADC1, ADC_CHANNEL1, ADC_SMPR_SMP_239DOT5CYC);  //enable

  ADC1->regs->JSQR=(ADC1->regs->JSQR&~ADC_JSQR_JSQ3)|(1<<10);
  ADC1->regs->JSQR=(ADC1->regs->JSQR&~ADC_JSQR_JSQ4)|(0<<15);
  ADC1->regs->JSQR=(ADC1->regs->JSQR&~ADC_JSQR_JL )|(ADC_JSQR_JL_2CONV);
  //adc_set_right_aligned(ADC1);
  ADC1->regs->CR2 &=~ ADC_CR2_ALIGN;
  //adc_enable_scan_mode(ADC1);
  ADC1->regs->CR1 |= ADC_CR1_SCAN;

  //adc_set_continuous_conversion_mode(ADC1);
  ADC1->regs->CR2 |= ADC_CR2_CONT;
  //adc_enable_eoc_interrupt_injected(ADC1);
  ADC1->regs->CR1 |= ADC_CR1_JEOCIE;
  //adc_enable_automatic_injected_group_conversion(ADC1);
 // ADC1_CR1|=ADC_CR1_DUALMOD_ISM;//ADC_CR1_DUALMOD_IND;//ADC_CR1_DUALMOD_ISM; //enable

  ADC1->regs->CR2|=ADC_CR2_JEXTTRIG;//enable
  ADC1->regs->CR2|=ADC_CR2_JEXTSEL_JSWSTART;//enable
  //ADC1_CR2 |=ADC_CR2_JSWSTART; //enable
  
}
void TouchScreen::ADC_WAIT()
{
     //adc_start_conversion_injected(ADC1);
    ADC1->regs->CR2 |=ADC_CR2_JSWSTART;
     while(!(ADC1->regs->SR & ADC_SR_JEOC ));
     ADC1->regs->SR &=! ADC_SR_JEOC;

}


TSPoint TouchScreen::getPoint(void) {
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i, valid;
  


  valid = 1;

  //pinMode(_yp, INPUT_ANALOG);//PA0
  GPIOA->regs->CRL = (GPIOA->regs->CRL & 0xFFFFFFF0)|0x00000000 ;
  
  //pinMode(_ym, INPUT);//PB3
  GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFFFF0FFF)|0x00000000 ;

    //digitalWrite(_yp, LOW);//PA0
    GPIOA->regs->BSRR = 0b0000000000000001<<16;
    //digitalWrite(_ym, LOW);//PB3
    GPIOB->regs->BSRR = 0b0000000000001000<<16;
  
    //pinMode(_xp, OUTPUT);//PA15
    GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x10000000 ;
     
    //pinMode(_xm, OUTPUT);//PA1
     GPIOA->regs->CRL = (GPIOA ->regs->CRL & 0xFFFFFF0F)|0x00000010 ;
    //digitalWrite(_xp, HIGH);//PA15
     GPIOA->regs->BSRR = 0b1000000000000000;
    //digitalWrite(_xm, LOW);//PA1
    GPIOA->regs->BSRR = 0b0000000000000010<<16;
   
   for (i=0; i<NUMSAMPLES; i++) {
     //samples[i] = analogRead(_yp);
       ADC_WAIT();
      samples[i] = adc_read_injected(2); 
   }
#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
      if((samples[NUMSAMPLES/2]-samples[NUMSAMPLES/2+1])>2){valid = 0; }
    if((samples[NUMSAMPLES/2]-samples[NUMSAMPLES/2+1])<-2){valid = 0; }
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) { valid = 0; }
#endif


	x = (4095-samples[NUMSAMPLES/2]);


   //pinMode(_xp, INPUT);//PA15
   GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x00000000 ;
   //pinMode(_xm, INPUT_ANALOG);//PA1
   GPIOA->regs->CRL = (GPIOA->regs->CRL & 0xFFFFFF0F)|0x00000000 ;
     
   //digitalWrite(_xp, LOW);//PA15
    GPIOA->regs->BSRR = 0b1000000000000000<<16;
   
   //pinMode(_yp, OUTPUT);//PA0
   GPIOA->regs->CRL = (GPIOA->regs->CRL & 0xFFFFFFF0)|0x00000001 ;

    //digitalWrite(_yp, HIGH);//PA0
   GPIOA->regs->BSRR = 0b0000000000000001;
   
   //pinMode(_ym, OUTPUT);//PB3
   GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFFFF0FFF)|0x00001000 ;
  
   for (i=0; i<NUMSAMPLES; i++) {
     //samples[i] = analogRead(_xm);
       ADC_WAIT();
     samples[i]= adc_read_injected(1);
   }

#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
      if((samples[NUMSAMPLES/2]-samples[NUMSAMPLES/2+1])>2){valid = 0; }
if((samples[NUMSAMPLES/2]-samples[NUMSAMPLES/2+1])<-2){valid = 0; }
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) { valid = 0; }
#endif


	y = (4095-samples[NUMSAMPLES/2]);


  //pinMode(_xp, OUTPUT);//PA15
   GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x10000000 ;
      // Set X+ to ground
      //digitalWrite(_xp, LOW);//PA15
     GPIOA->regs->BSRR = 0b1000000000000000<<16;
      // Set Y- to VCC
      //digitalWrite(_ym, HIGH); //PB3
     GPIOB->regs->BSRR = 0b0000000000001000;
       // Hi-Z X- and Y+
      //digitalWrite(_yp, LOW); //liju//PA0

  
   //digitalWrite(_yp, LOW);//PA0
     GPIOA->regs->BSRR = 0b0000000000000001<<16;
   //pinMode(_yp, INPUT_ANALOG);//PA0
   GPIOA->regs->CRL = (GPIOA->regs->CRL & 0xFFFFFFF0)|0x00000000 ;

        ADC_WAIT();
        int z1 = adc_read_injected(1);
        int z2 = adc_read_injected(2);
        
        if (_rxplate != 0) {
            // now read the x 
            float rtouch;
            rtouch = z2;
            rtouch /= z1;
            rtouch -= 1;
            rtouch *= x;
            rtouch *= _rxplate;
                rtouch /= 4095;

            
            z = rtouch;
        } else {

                z = (4095-(z2-z1));

        }


   
   x=x*1023/4095;
   y=1023-y*1023/4095;
   z=z*1023/4095;
// if (! valid) {
//     z = 1;
//   }
  //A15, as OUTPUT
  GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x10000000 ;
  //A0,A1, as OUTPUT
  GPIOA->regs->CRL = (GPIOA ->regs->CRL & 0xFFFFFF00)|0x00000011 ;
  /*PB3 */
  GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFFFF0FFF)|0x00001000 ;
   return TSPoint(x, y, z);
}

//TouchScreen::TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym) {
TouchScreen::TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym,
			 uint16_t rxplate) {
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
  _rxplate = rxplate;
  pressureThreshhold = 10;
  setADCs();
}


