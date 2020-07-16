/*.
  (c) Andrew Hull - 2015

  STM32-O-Scope - aka "The Pig Scope" or pigScope released under the GNU GENERAL PUBLIC LICENSE Version 2, June 1991

  https://github.com/pingumacpenguin/STM32-O-Scope

  Adafruit Libraries released under their specific licenses Copyright (c) 2013 Adafruit Industries.  All rights reserved.

  The code is modified to use scan two channels and also to use touchscreen (using custom touchscreen library -TouchScreen_STM_injected.h)
  scope uses continuous interleaving mode and touchscreen uses injected mode
*/

#include <SPI.h>
#include <STM32ADC.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <ILI9481_TFT_DISPLAY.h>
#include <TouchScreen_STM_injected.h>//touch library

ILI9481_TFT_DISPLAY TFT;
// Be sure to use the latest version of the SPI libraries see stm32duino.com - http://stm32duino.com/viewtopic.php?f=13&t=127


#define PORTRAIT 0
#define LANDSCAPE 1
void readTouch() ;

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your STM32F103 board
// -----------------------------------------------------------

//
// STM32F103C8XX Pin numbers - chosen for ease of use on the "Red Pill" and "Blue Pill" board

// Touch Panel Pins
// T_CLK T_CS T_DIN T_DOUT T_IRQ
// PB12 PB13 PB14 PB15 PA8
// Example wire colours Brown,Red,Orange,Yellow,Violet
// --------             Brown,Red,Orange,White,Grey


// Also define the orientation of the touch screen. Further
// information can be found in the UTouch library documentation.
//

// This makes no sense.. (BUG) but if you don't actually have a touch screen, you need to declere it anyway then  #undef it below.

#define TOUCH_ORIENTATION  LANDSCAPE

#define BOARD_LED PC13
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 80
#define TS_MAXY 920
#define MINPRESSURE 10
#define MAXPRESSURE 1000
TouchScreen myTouch(XP, YP, XM, YM, 300);
TSPoint p;



// RTC and NVRam initialisation

//#include "RTClock.h"
//RTClock rt (RTCSEL_LSE); // initialise
uint32_t tt;

// Define the Base address of the RTC  registers (battery backed up CMOS Ram), so we can use them for config of touch screen and other calibration.
// See http://stm32duino.com/viewtopic.php?f=15&t=132&hilit=rtc&start=40 for a more details about the RTC NVRam
// 10x 16 bit registers are available on the STM32F103CXXX more on the higher density device.





// #define NVRam register names for the touch calibration values.
#define  TOUCH_CALIB_X 0
#define  TOUCH_CALIB_Y 1
#define  TOUCH_CALIB_Z 2

// Time library - https://github.com/PaulStoffregen/Time
//#include "Time.h" //If you have issues with the default Time library change the name of this library to Time1 for example.
//#define TZ    "UTC+1"

// End RTC and NVRam initialization

// SeralCommand -> https://github.com/kroimon/Arduino-SerialCommand.git
//#include <SerialCommand.h>

// Display colours
#define BEAM1_COLOUR GREEN
#define BEAM2_COLOUR RED
#define GRATICULE_COLOUR 0x07FF
#define BEAM_OFF_COLOUR BLACK
#define CURSOR_COLOUR GREEN

// Analog input
#define ANALOG_MAX_VALUE 4096
const int8_t analogInPin = 8;   // Analog input pin: any of LQFP44 pins (PORT_PIN), 10 (PA0), 11 (PA1), 12 (PA2), 13 (PA3), 14 (PA4), 15 (PA5), 16 (PA6), 17 (PA7), 18 (PB0), 19  (PB1)
float samplingTime = 0;
float displayTime = 0;

uint16_t signalYperiod, signalVperiod;
// Variables for the beam position
uint16_t signalX ;
uint16_t signalY ;
uint16_t signalY1;
uint16_t signalV ;
uint16_t signalV1;
int16_t xZoomFactor = 1;
// yZoomFactor (percentage)
int16_t yZoomFactor = 100; //Adjusted to get 3.3V wave to fit on screen
int16_t yPosition = 0 ;

// Startup with sweep hold off or on
uint8_t triggerHeld = 0 ;
void showCredits();


void showLabels();
void TFTSamples (uint16_t beamColour1, uint16_t beamColour2, uint16_t beamClearColour);
//void TFTSamplesClear (uint16_t beamColour);
void takeSamples ();
void clearTFT();
void blinkLED();
void incEdgeType();
void triggerNegative();
void triggerPositive() ;
void triggerBoth();
void trigger();
void readTouch();
void showGraticule();
void showButtons();
// static inline int32_t map(int32_t value, int32_t fromStart, int32_t fromEnd,
//     int32_t toStart, int32_t toEnd) {
//     return ((int64_t)(value - fromStart) * (toEnd - toStart)) / (fromEnd - fromStart) +
//         toStart;
// }
unsigned long sweepDelayFactor = 1;
unsigned long timeBase = 200;  //Timebase in microseconds

// Screen dimensions
int16_t myWidth ;
int16_t myHeight ;
int buzzer = PA4;
//Trigger stuff
bool notTriggered = false;

// Sensitivity is the necessary change in AD value which will cause the scope to trigger.
// If VAD=3.3 volts, then 1 unit of sensitivity is around 0.8mV but this assumes no external attenuator. Calibration is needed to match this with the magnitude of the input signal.

// Trigger is setup in one of 32 positions
#define TRIGGER_POSITION_STEP ANALOG_MAX_VALUE/32
// Trigger default position (half of full scale)
int32_t triggerValue = 2048;
//int32_t triggerValue = 1024;
int16_t retriggerDelay = 0;
int8_t triggerType = 2; //0-both 1-negative 2-positive

//Array for trigger points
uint16_t triggerPoints[2];


// Serial output of samples - off by default. Toggled from UI/Serial commands.
bool serialOutput = false;

// Create Serial Command Object.
//SerialCommand sCmd;

// Create USB serial port
//USBSerial serial_debug;

// Samples - depends on available RAM 6K is about the limit on an STM32F103C8T6
// Bear in mind that the ILI9341 display is only able to display 240x320 pixels, at any time but we can output far more to the serial port, we effectively only show a window on our samples on the TFT.
# define maxSamples 400*6*2//161*6*2 //1024*6
uint32_t startSample = 0; //10
uint32_t endSample = maxSamples ;

// Array for the ADC data
//uint16_t dataPoints[maxSamples];
uint32_t dataPoints32[maxSamples / 2 + 2];
uint16_t *dataPoints = (uint16_t *)&dataPoints32;

/**
   Prescale 72000000 Hz system clock by 72 = 1000000 Hz.
*/
#define PWM_PRESCALE	(72)

/**
   We need a 50 Hz period (1000 / 20ms = 50), thus devide 100000 by 50 = 20000 (us).
*/
#define PWM_PERIOD		(1000)
#define PULSE_WIDTH             (500)

//array for computed data (speedup)
uint16_t dataPlot[960]; //max(width,height) for this display


// End of DMA indication
volatile static bool dma1_ch1_Active;
#define ADC_CR1_FASTINT 0x70000 // Fast interleave mode DUAL MODE bits 19-16
#define ADC_CR1_FASTINT_PLUS_INJECTED 0X30000

uint8_t time_scaling = 1;
adc_smp_rate sampling_time = ADC_SMPR_1_5;
void setADCs ()
{


  ADC1->regs->CR2 |= ADC_CR2_ADON;
  ADC2->regs->CR2 |= ADC_CR2_ADON;
  delay(1000);



  int pinMapADCin1 = PIN_MAP[PB0].adc_channel;
  int pinMapADCin2 = PIN_MAP[PB1].adc_channel;
  adc_set_sample_rate(ADC1, sampling_time); //=0,58uS/sample.  ADC_SMPR_13_5 = 1.08uS - use this one if Rin>10Kohm,
  adc_set_sample_rate(ADC2, sampling_time);    // if not may get some sporadic noise. see datasheet.

  adc_set_reg_seqlen(ADC1, 1);
  ADC1->regs->SQR3 = pinMapADCin1;
  ADC1->regs->CR2 |= ADC_CR2_CONT; // | ADC_CR2_DMA; // Set continuous mode and DMA
  ADC1->regs->CR1 |= ADC_CR1_FASTINT_PLUS_INJECTED; // Interleaved mode
  ADC1->regs->CR2 |= ADC_CR2_SWSTART;

  adc_set_reg_seqlen(ADC2, 1);
  ADC2->regs->CR2 |= ADC_CR2_CONT; // ADC 2 continuos
  ADC2->regs->SQR3 = pinMapADCin2;

}
void setup(void)
{
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, HIGH);
  pinMode(buzzer, OUTPUT);
  adc_set_prescaler(ADC_PRE_PCLK2_DIV_8);
  adc_calibrate(ADC1);
  adc_calibrate(ADC2);
  setADCs ();//should be done before touch.setADC
  // Set up our sensor pin(s)
  pinMode(PB0, INPUT_ANALOG);
  pinMode(PB1, INPUT_ANALOG);




  TFT.Init_LCD();
  // initialize the display
  clearTFT();
  TFT.Set_Rotation(PORTRAIT);
  myHeight   = TFT.Get_Width() ;
  myWidth  = TFT.Get_Height();
  TFT.Set_Text_colour(CURSOR_COLOUR) ; //, BEAM_OFF_COLOUR
#if defined TOUCH_SCREEN_AVAILABLE
  //  touchCalibrate();
#endif

  TFT.Set_Rotation(LANDSCAPE);
  clearTFT();
  //  showGraticule();
  showCredits(); // Honourable mentions ;¬)
  delay(1000) ; //5000
  clearTFT();
  notTriggered = true;
  showGraticule();
  showLabels();

}


void loop()
{


  //#if defined TOUCH_SCREEN_AVAILABLE
  readTouch(); //hangs at this pt
  //#endif

  //showLabels();

  //sCmd.readSerial();     // Process serial commands
  if ( !triggerHeld  )
  {
    blinkLED();
    // Wait for trigger
    //trigger();
    notTriggered = false;
    if (!notTriggered )
    {

      blinkLED();
      takeSamples();

      showGraticule();

      //Display new samples

      TFTSamples(BEAM1_COLOUR, BEAM2_COLOUR, BEAM_OFF_COLOUR);
      displayTime = (micros() - displayTime);

      // Display the Labels ( uS/Div, Volts/Div etc).
      showLabels();
      displayTime = micros();

    } else {
      showGraticule();
    }

  }

  // Wait before allowing a re-trigger
  delay(retriggerDelay);

}

void showGraticule()
{
  TFT.Set_Rotation(PORTRAIT);
  TFT.Rect(0, 0, myHeight, myWidth, GRATICULE_COLOUR);

  // Dot grid - ten distinct divisions (9 dots) in both X and Y axis.
  for (uint16_t TicksX = 1; TicksX < 10; TicksX++)
  {
    for (uint16_t TicksY = 1; TicksY < 10; TicksY++)
    {
      TFT.Draw_Pixe(  TicksX * (myHeight / 10), TicksY * (myWidth / 10), GRATICULE_COLOUR);
    }
  }
  // Horizontal and Vertical centre lines 5 ticks per grid square with a longer tick in line with our dots
  for (uint16_t TicksX = 0; TicksX < myWidth; TicksX += (myHeight / 50))
  {
    if (TicksX % (myWidth / 10) > 0 )
    {
      TFT.H_line(  (myHeight / 2) - 1 , TicksX, 3, GRATICULE_COLOUR);
    }
    else
    {
      TFT.H_line(  (myHeight / 2) - 3 , TicksX, 7, GRATICULE_COLOUR);
    }

  }
  for (uint16_t TicksY = 0; TicksY < myHeight; TicksY += (myHeight / 50) )
  {
    if (TicksY % (myHeight / 10) > 0 )
    {
      TFT.V_line( TicksY,  (myWidth / 2) - 1 , 3, GRATICULE_COLOUR);
    }
    else
    {
      TFT.V_line( TicksY,  (myWidth / 2) - 3 , 7, GRATICULE_COLOUR);
    }
  }
  showButtons();
}




// Crude triggering on positive or negative or either change from previous to current sample.
void trigger()
{
  notTriggered = true;
  switch (triggerType) {
    case 1:
      triggerNegative() ;
      break;
    case 2:
      triggerPositive() ;
      break;
    default:
      triggerBoth() ;
      break;
  }
}

void triggerBoth()
{
  triggerPoints[0] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
  while (notTriggered) {
    triggerPoints[1] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
    if (((triggerPoints[1] - triggerPoints[0] ) > 100) || ((triggerPoints[0] - triggerPoints[1] ) > 100) ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1]; //analogRead(analogInPin);
  }
}

void triggerPositive() {
  triggerPoints[0] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
  while (notTriggered) {
    triggerPoints[1] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
    if ((triggerPoints[1] - triggerPoints[0] ) > 100 ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1]; //analogRead(analogInPin);
  }
}

void triggerNegative() {
  triggerPoints[0] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
  while (notTriggered) {
    triggerPoints[1] = ADC1->regs->DR | ADC2->regs->DR; //analogRead(analogInPin);
    if ((triggerPoints[0] - triggerPoints[1] ) > 100 ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1]; //analogRead(analogInPin);
  }
}

void incEdgeType() {
  triggerType += 1;
  if (triggerType > 2)
  {
    triggerType = 0;
  }

}

void clearTFT()
{
  TFT.LCD_Clear(BEAM_OFF_COLOUR);                // Blank the display
}

void blinkLED()
{
  digitalWrite(BOARD_LED, LOW);
  delay(10);
  digitalWrite(BOARD_LED, HIGH);
}

// Grab the samples from the ADC
// Theoretically the ADC can not go any faster than this.
//
// According to specs, when using 72Mhz on the MCU main clock,the fastest ADC capture time is 1.17 uS. As we use 2 ADCs we get double the captures, so .58 uS, which is the times we get with ADC_SMPR_1_5.
// I think we have reached the speed limit of the chip, now all we can do is improve accuracy.
// See; http://stm32duino.com/viewtopic.php?f=19&t=107&p=1202#p1194
void takeSamples ()
{
  // This loop uses dual interleaved mode to get the best performance out of the ADCs
  //

  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH1, DMA1_CH1_Event);

  adc_dma_enable(ADC1);
  dma_setup_transfer(DMA1, DMA_CH1, &ADC1->regs->DR, DMA_SIZE_32BITS,
                     dataPoints32, DMA_SIZE_32BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));// Receive buffer DMA
  dma_set_num_transfers(DMA1, DMA_CH1, maxSamples / 2);
  dma1_ch1_Active = 1;
  //  regs->CR2 |= ADC_CR2_SWSTART; //moved to setADC
  dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
  //adc_calibrate(ADC1);
  //adc_calibrate(ADC2);
  samplingTime = micros();
  while (dma1_ch1_Active);
  samplingTime = (micros() - samplingTime);

  dma_disable(DMA1, DMA_CH1); //End of trasfer, disable DMA and Continuous mode.


}

void TFTSamples (uint16_t beamColour1, uint16_t beamColour2, uint16_t beamClearColour)
{
  TFT.Set_Rotation(PORTRAIT);
  signalYperiod = 0; signalVperiod = 0;
  int signalVPREV = 0, previousX_V = 0;
  bool risingX = 0, risingY = 0;
  int signalYPREV = 0, previousX_Y = 0;
  signalX = 0;

  signalY = ((myHeight * dataPoints[(2 * signalX ) * ((endSample - startSample) / (myWidth * timeBase / 100)) +  0]) / ANALOG_MAX_VALUE) * (yZoomFactor / 100) + yPosition ;
  signalV = ((myHeight * dataPoints[(2 * signalX ) * ((endSample - startSample) / (myWidth * timeBase / 100)) +  1]) / ANALOG_MAX_VALUE) * (yZoomFactor / 100) + yPosition ;
  do {
    signalX += 1;

    signalY1 = ((myHeight * dataPoints[(2 * signalX ) * ((endSample - startSample) / (myWidth * timeBase / 100)) +  0]) / ANALOG_MAX_VALUE) * (yZoomFactor / 100) + yPosition ;
    signalV1 = ((myHeight * dataPoints[(2 * signalX ) * ((endSample - startSample) / (myWidth * timeBase / 100)) +  1]) / ANALOG_MAX_VALUE) * (yZoomFactor / 100) + yPosition ;
    //clear
    //BEAM0
    TFT.Draw_Lin (  dataPlot[2 * signalX - 2 + 0], signalX, dataPlot[2 * signalX + 0] , signalX + 1, beamClearColour) ;
    //BEAM1
    TFT.Draw_Lin (  dataPlot[2 * signalX - 2 + 1], signalX, dataPlot[2 * signalX + 1] , signalX + 1, beamClearColour) ;

    TFT.Draw_Lin (  signalY, signalX, signalY1, signalX + 1, beamColour1) ;
    TFT.Draw_Lin (  signalV, signalX, signalV1, signalX + 1, beamColour2) ;
    dataPlot[2 * signalX - 2 + 0] = signalY;
    dataPlot[2 * signalX - 2 + 1] = signalV;
    signalY = signalY1; signalV = signalV1;

    if (risingX && signalVPREV - signalV > signalVPREV / 10) {
      if (previousX_V != 0) {
        if (signalVperiod != 0) {
          signalVperiod = (signalVperiod + (signalX - previousX_V)) / 2;
        } else {
          signalVperiod = (signalX - previousX_V);
        }
      }
      signalVPREV = signalV;
      previousX_V = signalX;
      risingX = 0;
    }
    if (signalVPREV < signalV) {
      risingX = 1; signalVPREV = signalV;
    }

    if (risingY && signalYPREV - signalY > signalYPREV / 10) {
      if (previousX_Y != 0) {
        if (signalYperiod != 0) {
          signalYperiod = (signalYperiod + (signalX - previousX_Y)) / 2;
        } else {
          signalYperiod = (signalX - previousX_Y);
        }
      }
      signalYPREV = signalY;
      previousX_Y = signalX; risingY = 0;
    }

    if (signalYPREV < signalY) {
      risingY = 1; signalYPREV = signalY;
    }

  } while (signalX + 1 + 1 < myWidth - 2);

  dataPlot[2 * signalX + 0] = signalY1;
  dataPlot[2 * signalX + 1] = signalV1;

}
void showButtons()
{

  TFT.Set_Rotation(LANDSCAPE);
  TFT.Set_Text_Back_colour(GREEN);
  TFT.Set_Text_colour(RED);
  TFT.Set_Text_Size(2);
  TFT.Set_Text_Cousur(250, 40);
  TFT.printString(" - ");
  TFT.Set_Text_Cousur(300, 40);
  TFT.printString(" + ");
  TFT.Set_Text_Cousur(400, 40);

  if (!triggerHeld)
  {

    TFT.printString(" HOLD  ");
  }
  else
  {

    TFT.printString("UNHOLD ");
  }
  TFT.Set_Rotation(PORTRAIT);
  TFT.Set_Text_Back_colour(BLACK);
}


void showLabels()
{
  TFT.Set_Rotation(LANDSCAPE);
  TFT.Set_Text_colour(BEAM1_COLOUR);
  TFT.Set_Text_Size(1);
  //TFT.Set_Text_Cousur(10, myHeight-40);
  TFT.Set_Text_Cousur(10, 10);

  float actualsampletime = (float(samplingTime) / float(maxSamples / 2));
  TFT.Set_Text_Cousur(420, 10);
  TFT.printFloat(float (1000000 / float(displayTime)));
  TFT.printString(" fps    ");
  TFT.Set_Text_Size(1);
  TFT.Set_Text_Cousur(10, 10);
  TFT.printString("0.0647");

  TFT.printString("V/Div    ");
  TFT.Set_Text_Cousur(200, 10);
  TFT.printInt(float (float(actualsampletime * endSample / 2) / 51));
  TFT.printString("uS/Div ");
  TFT.printFloat(actualsampletime);
  TFT.printString("uS/Sample ");

  TFT.printInt(time_scaling);
  TFT.printString("x  ");

  TFT.Set_Text_Size(1);
  TFT.Set_Text_colour(BEAM2_COLOUR);
  TFT.Set_Text_Cousur(10, 40);

  TFT.printFloat(float(1 / (float(signalVperiod)*float(endSample / maxSamples)*actualsampletime)) * 25000000);
  //TFT.printFloat(float(signalVperiod));
  TFT.printString(" Hz        ");
  TFT.Set_Text_colour(BEAM1_COLOUR);
  TFT.Set_Text_Cousur(10, 50);

  TFT.printFloat(float(1 / (float(signalYperiod)*float(endSample / maxSamples)*actualsampletime)) * 25000000);
  //TFT.printFloat(float(signalYperiod));
  TFT.printString(" Hz        ");
  TFT.Set_Rotation(PORTRAIT);
}





/**
  @brief Disable DMA requests
  @param dev ADC device on which to disable DMA requests
*/

void adc_dma_disable(const adc_dev * dev) {
  bb_peri_set_bit(&dev->regs->CR2, ADC_CR2_DMA_BIT, 0);
}

static void DMA1_CH1_Event() {
  dma1_ch1_Active = 0;
}


void readTouch() {

recheck:
  p = myTouch.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, TFT.Get_Display_Width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, TFT.Get_Display_Height(), 0);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE )
  {
    if (p.y < 480 && p.y > 403 && p.x > 265 && p.x < 290) {
      tone(buzzer, 3000);
      delay(50);
      noTone(buzzer);
      if (triggerHeld) {
        triggerHeld = 0 ;
        showButtons();
        delay(1000);
      } else {
        triggerHeld = 1 ;
        showButtons();
        delay(1000);
      }
      goto recheck;
    }
    if (!triggerHeld) {
      if (p.y < 342 && p.y > 306 && p.x > 265 && p.x < 290) {
        tone(buzzer, 3000);
        delay(50);
        noTone(buzzer);
        time_scaling--;
        if (time_scaling < 1) {
          time_scaling = 1;

          if (sampling_time == ADC_SMPR_239_5) {

          } else {
            // sampling_time++;
            if (sampling_time == ADC_SMPR_71_5) {
              sampling_time = ADC_SMPR_239_5;
            }
            if (sampling_time == ADC_SMPR_55_5) {
              sampling_time = ADC_SMPR_71_5;
            }
            if (sampling_time == ADC_SMPR_41_5) {
              sampling_time = ADC_SMPR_55_5;
            }
            if (sampling_time == ADC_SMPR_28_5) {
              sampling_time = ADC_SMPR_41_5;
            }
            if (sampling_time == ADC_SMPR_13_5) {
              sampling_time = ADC_SMPR_28_5;
            }
            if (sampling_time == ADC_SMPR_7_5) {
              sampling_time = ADC_SMPR_13_5;
            }
            if (sampling_time == ADC_SMPR_1_5) {
              sampling_time = ADC_SMPR_7_5;
            }
            ADC1->regs->CR2 &= ~ ADC_CR2_ADON;
            ADC2->regs->CR2 &= ~ ADC_CR2_ADON;
            delay(100);
            setADCs();
            
          }
        }

        endSample = maxSamples / time_scaling;
        showLabels();
        delay(500);
        goto recheck;
      }
      if (p.y < 290 && p.y > 245 && p.x > 265 && p.x < 290) {
        tone(buzzer, 3000);
        delay(50);
        noTone(buzzer);

        if (sampling_time == ADC_SMPR_1_5) {
          time_scaling++;
          if (time_scaling > 6) {
            time_scaling = 6;
          }
        } else {
          //sampling_time--;
          if (sampling_time == ADC_SMPR_7_5) {
            sampling_time = ADC_SMPR_1_5;
          }
          if (sampling_time == ADC_SMPR_13_5) {
            sampling_time = ADC_SMPR_7_5;
          }
          if (sampling_time == ADC_SMPR_28_5) {
            sampling_time = ADC_SMPR_13_5;
          }
          if (sampling_time == ADC_SMPR_41_5) {
            sampling_time = ADC_SMPR_28_5;
          }
          if (sampling_time == ADC_SMPR_55_5) {
            sampling_time = ADC_SMPR_41_5;
          }
          if (sampling_time == ADC_SMPR_71_5) {
            sampling_time = ADC_SMPR_55_5;
          }
          if (sampling_time == ADC_SMPR_239_5) {
            sampling_time = ADC_SMPR_71_5;
          }
          ADC1->regs->CR2 &= ~ ADC_CR2_ADON;
          ADC2->regs->CR2 &= ~ ADC_CR2_ADON;

          delay(100);

          setADCs();
          
        }
        endSample = maxSamples / time_scaling;
        showLabels();
        delay(500);
        goto recheck;
        
      }

      
    }

  }

}


void showCredits() {
  TFT.Set_Text_Size(2);                           // Small 26 char / line
  //TFT.setTextColor(CURSOR_COLOUR, BEAM_OFF_COLOUR) ;
  TFT.Set_Text_colour(CURSOR_COLOUR);
  TFT.Set_Text_Back_colour(BEAM_OFF_COLOUR);
  TFT.Set_Text_Cousur(0, 50);
  TFT.printString(" STM-O-Scope by LIJU G CHACKO") ;
  TFT.Set_Text_Cousur(0, 70);
  TFT.printString("      Inspired by");
  TFT.Set_Text_Cousur(0, 90);
  TFT.printString("      Ray Burnette.");
  TFT.Set_Text_Cousur(0, 130);
  TFT.printString("      Victor PV");
  TFT.Set_Text_Cousur(0, 150);
  TFT.printString("      Roger Clark");
  TFT.Set_Text_Cousur(0, 170);
  TFT.printString(" and all at stm32duino.com");
  TFT.Set_Text_Cousur(0, 190);
  TFT.printString(" CH1 Probe STM32F Pin [B0]");
  TFT.Set_Text_Cousur(0, 210);
  TFT.printString(" CH2 Probe STM32F Pin [B1]");


  TFT.Set_Text_Cousur(0, 240);
  TFT.Set_Text_Size(1);
  TFT.printString("     GNU GENERAL PUBLIC LICENSE Version 3 ");
  TFT.Set_Text_Size(2);
  TFT.Set_Rotation(PORTRAIT);
}
