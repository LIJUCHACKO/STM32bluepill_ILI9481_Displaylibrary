/***********************************************************************************
*This program is a demo of drawing
*This demo was made for 3.5" LCD module  8bit with ILI9486 controller. 
*ILI9481_TFT_DISPLAY.h library is created by refering to LCDWIKI_KBV.h, LCDWIKI_KBV.cpp. 
*TouchScreen_STM library can be used along with this libraray.
*
*This program requires the the LCDKIWI  library and TouchScreen_STM.
*
* File                : touch_pen.ino
* Hardware Environment: STM32F103C (STM32 bluepill)
* Build Environment   : Arduino
*
*pin usage as follow:
*                  LCD_CS  LCD_RS  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK 
*     STM32F103C    PA0     PA1    PA9     PA8     PA10     PA4   PA7   PA6   PA5                            
*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*     STM32F103C    PA15    PB3    PB4     PB5     PB6     PB7      PB8     PB9                  
*Remember to connect pins as described above
*
**********************************************************************************/
#include "ILI9481_TFT_DISPLAY.h"


///TFT_DATA  A15 ,B3 TO B9,


//PA8
#define LCD_RD_LOW     GPIOA->regs->BSRR = 0b0000000100000000<<16; 
#define LCD_RD_HIGH    GPIOA->regs->BSRR = 0b0000000100000000;


//PA9
#define LCD_WR_LOW     GPIOA->regs->BSRR = 0b0000001000000000<<16;
#define LCD_WR_HIGH    GPIOA->regs->BSRR = 0b0000001000000000;



//PA1
#define LCD_RS_LOW     GPIOA->regs->BSRR = 0b0000000000000010<<16;
#define LCD_RS_HIGH    GPIOA->regs->BSRR = 0b0000000000000010;


//PA0
#define LCD_CS_LOW     GPIOA->regs->BSRR = 0b0000000000000001<<16;
#define LCD_CS_HIGH    GPIOA->regs->BSRR = 0b0000000000000001;


//PA10
#define LCD_REST_LOW   GPIOA->regs->BSRR = 0b0000010000000000<<16;
#define LCD_REST_HIGH  GPIOA->regs->BSRR = 0b0000010000000000;

#define setWriteDir() { /*PB3-PB7 */GPIOB->regs->CRL = (GPIOB->regs->CRL & 0x00000FFF)|0x11111000 ;\
                        /*PB8-PB9 */GPIOB->regs->CRH = (GPIOB->regs->CRH & 0xFFFFFF00)|0x00000011 ; \
                        /*A15*/GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x10000000 ;  LCD_WR_HIGH;}
#define setReadDir() { /*PB3-PB7 */GPIOB->regs->CRL = (GPIOB->regs->CRL & 0x00000FFF)|0x00000000 ;\
                        /*PB8-PB9 */GPIOB->regs->CRH = (GPIOB->regs->CRH & 0xFFFFFF00)|0x00000000 ;\
                        /*A15*/GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FFFFFFF)|0x80000000 ;  LCD_WR_LOW;}

ILI9481_TFT_DISPLAY::ILI9481_TFT_DISPLAY(void)
{
  HEIGHT=height=480;
  WIDTH=width=320;
  rotation=0;
XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=ILI9341_MEMORYWRITE,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=ILI9341_MADCTL,VL=0,R24BIT=0;
  
  setWriteDir();
  
  //A10,A8,A9, as OUTPUT
  GPIOA->regs->CRH = (GPIOA->regs->CRH & 0xFFFFF000)|0x00000111 ;
  
  //A0,A1, as OUTPUT
  GPIOA->regs->CRL = (GPIOA ->regs->CRL & 0xFFFFFF00)|0x00000011 ;
  
  
  
  LCD_RD_HIGH;
  LCD_WR_HIGH;
  LCD_RS_HIGH;
  LCD_CS_HIGH;
  LCD_REST_HIGH;
}



//Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ILI9481_TFT_DISPLAY::Color_To_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
        
}       
uint8_t ILI9481_TFT_DISPLAY::Lcd_Read_Bus() //8bit
{
  uint8_t dst;
  LCD_WR_HIGH;

  //D0->A15 D1-D7->B3-B10
  //DELAY7; 
  delay(1); 
  //D0->A15 
  dst = ((GPIOA->regs->IDR & 0x8000)>>15);
  //D1-D7->B3-B10
  dst=dst|((GPIOB->regs->IDR & 0x03F8)>>2);

  
  LCD_WR_LOW;
  return dst;
}



void ILI9481_TFT_DISPLAY::Lcd_Writ_Bus(uint8_t d)
{

  //D1-D7->B3-B10
  GPIOB->regs->BSRR = ((~(d<<2))<<16|(d<<2))  & 0x03F803F8;
  //D0->A15  
  GPIOA->regs->BSRR = ((~(d<<15))<<16|(d<<15)) & 0x80008000; 
 
  LCD_WR_LOW;
  LCD_WR_HIGH;
}


void ILI9481_TFT_DISPLAY::Lcd_Write_Com(uint8_t VH)  
{   
  LCD_RS_LOW;
  Lcd_Writ_Bus(VH);
}
void ILI9481_TFT_DISPLAY::Lcd_Write_Com16(uint16_t VH)  
{   
  LCD_RS_LOW;
  Lcd_Writ_Bus(VH>>8);
  Lcd_Writ_Bus(VH); 
}

uint8_t ILI9481_TFT_DISPLAY::Lcd_Read_Data()
{
  LCD_RS_HIGH;
  return Lcd_Read_Bus();
}
void ILI9481_TFT_DISPLAY::Lcd_Write_Data(uint8_t VH)
{
  LCD_RS_HIGH;
  Lcd_Writ_Bus(VH);
}

void ILI9481_TFT_DISPLAY::Lcd_Write_Data16(uint16_t j)
{
        Lcd_Write_Data(j>>8);
        Lcd_Write_Data(j); 
}

void ILI9481_TFT_DISPLAY::Lcd_Write_Com_Data(uint8_t com,uint8_t dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}
//read value from lcd register 
uint16_t ILI9481_TFT_DISPLAY::Read_Reg(uint16_t reg, int8_t index)
{
	uint16_t ret,high;
    uint8_t low;
//  if (!have_reset)
 // {
 //     reset();
//  }
   LCD_CS_LOW;
    Lcd_Write_Com16(reg);
    setReadDir();
     delay(1); 
	do 
	{ 
	//ead8(high);
        high=Lcd_Read_Data();
	//ead8(low);
        low=Lcd_Read_Data();
	ret = (high << 8) | low;
		//read16(ret);  //read 16bits
	}while (--index >= 0);   
   LCD_RD_HIGH;
    LCD_CS_HIGH;
    setWriteDir();
    return ret;
}
//read LCD controller chip ID 
uint16_t ILI9481_TFT_DISPLAY::Read_ID(void)
{
	uint16_t ret;
	if ((Read_Reg(0x04,0) == 0x00)&&(Read_Reg(0x04,1) == 0x8000))
	{
		uint8_t buf[] = {0xFF, 0x83, 0x57};
		Push_Command(HX8357D_SETC, buf, sizeof(buf));
		ret = (Read_Reg(0xD0,0) << 16) | Read_Reg(0xD0,1);
		if((ret == 0x990000) || (ret == 0x900000))
		{
			return 0x9090;
		}
	}
	ret = Read_Reg(0xD3,1); //0x9341 0x9486
	if(ret == 0x9341)
	{
		return 0x9341;
	}
	else if(ret == 0x9486)
	{
		return 0x9486;
	}
	else if(ret == 0x9488)
	{
		return 0x9488;
	}
	else if(ret == 0x9481)
	{
		return 0x9481;
	}
	else
	{
		return Read_Reg(0, 0); //others
	}
}

void ILI9481_TFT_DISPLAY::Set_Addr_Window(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
        Lcd_Write_Com(0x2a);
	Lcd_Write_Data(x1>>8);
	Lcd_Write_Data(x1);
	Lcd_Write_Data(x2>>8);
	Lcd_Write_Data(x2);
        Lcd_Write_Com(0x2b);
	Lcd_Write_Data(y1>>8);
	Lcd_Write_Data(y1);
	Lcd_Write_Data(y2>>8);
	Lcd_Write_Data(y2);
	Lcd_Write_Com(0x2c); 							 
}
void ILI9481_TFT_DISPLAY::Init_LCD(void)
{
  
  LCD_REST_HIGH;
  delay(5); 
  LCD_REST_LOW;
  delay(15);
  LCD_REST_HIGH;
  delay(15);

  LCD_CS_HIGH;
  LCD_WR_HIGH;

  LCD_CS_LOW;  //CS

    Lcd_Write_Com(0xF9);  
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x08);
    Lcd_Write_Com(0xC0);  
    Lcd_Write_Data(0x19); 
    Lcd_Write_Data(0x1A);  
    Lcd_Write_Com(0xC1);  
    Lcd_Write_Data(0x45); 
    Lcd_Write_Data(0X00); 
    Lcd_Write_Com(0xC2);  
    Lcd_Write_Data(0x33); 
    Lcd_Write_Com(0xC5);  
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x28); 
    Lcd_Write_Com(0xB1);  
    Lcd_Write_Data(0x90); 
    Lcd_Write_Data(0x11); 
    Lcd_Write_Com(0xB4);  
    Lcd_Write_Data(0x02); 
    Lcd_Write_Com(0xB6);  
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x42);
    Lcd_Write_Data(0x3B);
    Lcd_Write_Com(0xB7);  
    Lcd_Write_Data(0x07); 
    Lcd_Write_Com(0xE0); 
    Lcd_Write_Data(0x1F); 
    Lcd_Write_Data(0x25); 
    Lcd_Write_Data(0x22); 
    Lcd_Write_Data(0x0B); 
    Lcd_Write_Data(0x06); 
    Lcd_Write_Data(0x0A); 
    Lcd_Write_Data(0x4E); 
    Lcd_Write_Data(0xC6); 
    Lcd_Write_Data(0x39); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Com(0xE1); 
    Lcd_Write_Data(0x1F); 
    Lcd_Write_Data(0x3F); 
    Lcd_Write_Data(0x3F); 
    Lcd_Write_Data(0x0F); 
    Lcd_Write_Data(0x1F); 
    Lcd_Write_Data(0x0F); 
    Lcd_Write_Data(0x46); 
    Lcd_Write_Data(0x49); 
    Lcd_Write_Data(0x31); 
    Lcd_Write_Data(0x05); 
    Lcd_Write_Data(0x09); 
    Lcd_Write_Data(0x03); 
    Lcd_Write_Data(0x1C); 
    Lcd_Write_Data(0x1A); 
    Lcd_Write_Data(0x00);  
    Lcd_Write_Com(0xF1);   
    Lcd_Write_Data(0x36);   
    Lcd_Write_Data(0x04);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x3C);
    Lcd_Write_Data(0x0F);
    Lcd_Write_Data(0x0F);
    Lcd_Write_Data(0xA4);
    Lcd_Write_Data(0x02);
    Lcd_Write_Com(0xF2);   
    Lcd_Write_Data(0x18);   
    Lcd_Write_Data(0xA3);
    Lcd_Write_Data(0x12);
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x32);
    Lcd_Write_Data(0x12);
    Lcd_Write_Data(0xFF);
    Lcd_Write_Data(0x32);
    Lcd_Write_Data(0x00);
    Lcd_Write_Com(0xF4);   
    Lcd_Write_Data(0x40);   
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x08);
    Lcd_Write_Data(0x91);
    Lcd_Write_Data(0x04);     
    Lcd_Write_Com(0xF8);   
    Lcd_Write_Data(0x21);   
    Lcd_Write_Data(0x04);
    Lcd_Write_Com(0x36);   
    Lcd_Write_Data(0x48);   
    Lcd_Write_Com(0x3A);   
    Lcd_Write_Data(0x55);        
 
    Lcd_Write_Com(0x11);    //Exit Sleep 
    delay(120); 				
    Lcd_Write_Com(0x29);    //Display on 

   Set_Rotation(rotation);
}

void ILI9481_TFT_DISPLAY::Draw_Pixe(int16_t x, int16_t y, uint16_t color)
{
	if((x < 0) || (y < 0) || (x > Get_Width()) || (y > Get_Height()))
	{
		return;
	}
	LCD_CS_LOW;
	  Lcd_Write_Com(0x02c); //write_memory_start	  
	Set_Addr_Window(x, y, x, y);
	Lcd_Write_Data16(color);
	LCD_CS_HIGH;  
}
void ILI9481_TFT_DISPLAY::H_line(int16_t x, int16_t y, int16_t l, uint16_t c)                   
{	
  uint8_t i,j;
  LCD_CS_LOW;
  Lcd_Write_Com(0x02c); //write_memory_start
  
  l=l+x;
  Set_Addr_Window(x,y,l,y);
  while(l--> 0){
        Lcd_Write_Data16(c); 
  }
  LCD_CS_HIGH;   
}

void ILI9481_TFT_DISPLAY::V_line(int16_t x, int16_t y, int16_t l, uint16_t c)                   
{	
  uint8_t i,j;
  LCD_CS_LOW;
  Lcd_Write_Com(0x02c); //write_memory_start
  
  l=l+y;
  Set_Addr_Window(x,y,x,l);
  while(l--> 0){
        Lcd_Write_Data16(c); 
  }
  LCD_CS_HIGH;   
}

void ILI9481_TFT_DISPLAY::Rect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void ILI9481_TFT_DISPLAY::Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{

  int16_t end;
   LCD_CS_LOW;
   Set_Addr_Window(x,y,x+w-1,y+h-1);
  while(h--> 0){
     end=w;
     do{
        Lcd_Write_Data16(c); 
       
     }while(--end>0) ;
  }
   LCD_CS_HIGH; 
}
int RGB(int r,int g,int b)
{return r << 16 | g << 8 | b;
}
void ILI9481_TFT_DISPLAY::LCD_Clear(unsigned int j)                   
{	
  uint8_t i,m;
  int16_t w=320,h=480,end;
   LCD_CS_LOW;
   Set_Addr_Window(0,0,320,480);
  while(h--> 0){
     end=w;
     do{
        Lcd_Write_Data16(j); 
       
     }while(--end>0) ;
  }
   LCD_CS_HIGH; 
  
}
void ILI9481_TFT_DISPLAY::Set_LR(void)
{
	LCD_CS_LOW;
	Lcd_Write_Com_Data(0x04,(width -1)>>8);
	Lcd_Write_Com_Data(0x05,width -1);
	Lcd_Write_Com_Data(0x04,(height -1)>>8);
	Lcd_Write_Com_Data(0x05,height -1);
	LCD_CS_HIGH;
}

void ILI9481_TFT_DISPLAY::Set_Rotation(uint8_t r)
{
    rotation = r & 3;           // just perform the operation ourselves on the protected variables
    width = (rotation & 1) ? HEIGHT : WIDTH;
    height = (rotation & 1) ? WIDTH : HEIGHT;

	  Lcd_Write_Com(0x02c); //write_memory_start
	  //digitalWrite(LCD_RS,HIGH);
	  LCD_CS_LOW;

	uint8_t val;
	switch (rotation) 
	{
	   	case 0:
	     	val = ILI9341_MADCTL_BGR; //0 degree 
	     	break;
	   	case 1:
	     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_MV | ILI9341_MADCTL_ML | ILI9341_MADCTL_BGR ; //90 degree 
	     	break;
	 	case 2:
	    	val = ILI9341_MADCTL_MY | ILI9341_MADCTL_MX |ILI9341_MADCTL_BGR; //180 degree 
	    	break;
	   	case 3:
	     	val = ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; //270 degree
	     	break;
	 }
	 Lcd_Write_Com_Data(MD, val); 
	
	
 	//Set_Addr_Window(0, 0, width - 1, height - 1);
	//Vert_Scroll(0, HEIGHT, 0);
	LCD_CS_HIGH;
}

void ILI9481_TFT_DISPLAY::Vert_Scroll(int16_t top, int16_t scrollines, int16_t offset)
{
    int16_t bfa = HEIGHT - top - scrollines; 
    int16_t vsp;
    int16_t sea = top;
    if (offset <= -scrollines || offset >= scrollines)
    {
		offset = 0; //valid scroll
    }
	vsp = top + offset; // vertical start position
    if (offset < 0)
    {
        vsp += scrollines;          //keep in unsigned range
    }
    sea = top + scrollines - 1;

  		uint8_t d[6];           // for multi-byte parameters
  		d[0] = top >> 8;        //TFA
  		d[1] = top;
  		d[2] = scrollines >> 8; //VSA
  		d[3] = scrollines;
  		d[4] = bfa >> 8;        //BFA
  		d[5] = bfa;
		Push_Command(SC1, d, 6);
		d[0] = vsp >> 8;        //VSP
  		d[1] = vsp;
		Push_Command(SC2, d, 2);
		 if (offset == 0) 
		{
			Push_Command(0x13, NULL, 0);
		}
	
}

void ILI9481_TFT_DISPLAY::Push_Command(uint8_t cmd, uint8_t *block, int8_t N)
{
   LCD_CS_LOW; 
    Lcd_Write_Com(cmd);
    while (N-- > 0) 
	{
        uint8_t u8 = *block++;
        Lcd_Write_Data(u8); 
    }
    LCD_CS_HIGH; 
}

//get lcd width
int16_t ILI9481_TFT_DISPLAY::Get_Width(void) const
{
	return width;
}

//get lcd height
int16_t ILI9481_TFT_DISPLAY::Get_Height(void) const
{
	return height;
}
uint8_t ILI9481_TFT_DISPLAY::Get_Rotation(void) const
{
	return rotation;
}

//Anti color display 
void ILI9481_TFT_DISPLAY::Invert_Display(boolean i)
{
	 Lcd_Write_Com(0x02c); //write_memory_start
  	//digitalWrite(LCD_RS,HIGH);
  	LCD_CS_LOW;
	uint8_t val = VL^i;
	Lcd_Write_Com(val ? 0x21 : 0x20);
	LCD_CS_HIGH;
}
//push color table for 16bits
void ILI9481_TFT_DISPLAY::Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)
{

	uint16_t color;
    uint8_t h, l;
	bool isconst = flags & 1;
//	bool isbigend = (flags & 2) != 0;
    LCD_CS_LOW;
    if (first) 
	{  
		Lcd_Write_Com(CC);		
    }
    while (n-- > 0) 
	{
        //if (isconst) 
	//	{
	//		color = pgm_read_word(block++);		
        //} 
	//	else 
	//	{
			color = (*block++);			

	//	}		
        Lcd_Write_Data16(color);
    }
   LCD_CS_HIGH;

}
int16_t ILI9481_TFT_DISPLAY::Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
   
    uint16_t ret, dummy;
    int16_t n = w * h;
    uint8_t r, g, b, tmp;
    Set_Addr_Window(x, y, x + w - 1, y + h - 1);
    while (n > 0) 
	{
        LCD_CS_LOW;
	Lcd_Write_Com16(RC);
        setReadDir();
    /*
		if(lcd_driver == ID_932X)
		{
			while(n)
			{
				for(int i =0; i< 2; i++)
				{
					read8(r);
					read8(r);
					read8(r);
					read8(g);
				}
				*block++ = (r<<8 | g);
				n--;
			}
			Set_Addr_Window(0, 0, width - 1, height - 1);
		}
		else 
		{*/
			r=Lcd_Read_Data();
        	while (n) 
			{
				//if(R24BIT == 1)
				//{
        			/*r= Lcd_Read_Data();
         			 g= Lcd_Read_Data();
        			b= Lcd_Read_Data();
                                ret = Color_To_565(r, g, b);*/
                                
                                uint16_t high;
                                uint8_t low;
                                high=Lcd_Read_Data();
                                //ead8(low);
                                low=Lcd_Read_Data();
                                ret = (high << 8) | low;
				//}
				//else if(R24BIT == 0)
				//{
				//	read16(ret);
				//}
            	*block++ = ret;
            	n--;
        	}
       // }
        LCD_RD_HIGH;
        LCD_CS_HIGH;
        setWriteDir();
    }
	return 0;
}

void ILI9481_TFT_DISPLAY::Draw_Lin(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  int16_t i;
  int16_t dx,dy;
  int16_t sx,sy;
  int16_t E;
  #ifdef _DEBUG_
  Serial.println("drawLine:x1=" + String(x1) + " x2=" + String(x2) + " y1=" + String(y1) + " y2=" + String(y2));
  #endif
  
  /* distance between two points */
  dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
  dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  sx = ( x2 > x1 ) ? 1 : -1;
  sy = ( y2 > y1 ) ? 1 : -1;

  if (y1 == y2) {
    if (x2 > x1) {
      H_line(x1, y1, x2 - x1 + 1, color);
    } else {
      H_line(x2, y1, x1 - x2 + 1, color);
    }
    return;
  } else if (x1 == x2) {
    if (y2 > y1) {
      V_line(x1, y1, y2 - y1 + 1, color);
    } else {
      V_line(x1, y2, y1 - y2 + 1, color);
    }
    return;
  }

  /* inclination < 1 */
  if ( dx > dy ) {
    E = -dx;
    for ( i = 0 ; i <= dx ; i++ ) {
      Draw_Pixe( x1, y1, color );
      x1 += sx;
      E += 2 * dy;
      if ( E >= 0 ) {
        y1 += sy;
        E -= 2 * dx;
      }
    }
  /* inclination >= 1 */
  } else {
    E = -dy;
    for ( i = 0 ; i <= dy ; i++ ) {
      Draw_Pixe( x1, y1, color );
      y1 += sy;
      E += 2 * dx;
      if ( E >= 0 ) {
        x1 += sx;
        E -= 2 * dy;
      }
    }
  }
}


void ILI9481_TFT_DISPLAY::Fill_Circl(int16_t x, int16_t y, int16_t radius,uint16_t color){
    
    Set_Draw_color( color);
    Fill_Circle( x,  y,  radius);
}
void ILI9481_TFT_DISPLAY::printString(const char *st){
    
    Print_String(st, text_x, text_y);
    
}
void ILI9481_TFT_DISPLAY::printInt(long num){
    
    Print_Number_Int(num, text_x, text_y, 5, ' ', 10);
    
}
void ILI9481_TFT_DISPLAY::printFloat(double num){
    
    Print_Number_Float(num, 2, text_x, text_y, '.', 0, ' ');
}
//void ILI9481_TFT_DISPLAY::print(uint8_t *st){
//    
//    Print_String(*st, text_x, text_y);
//    
//}
//void ILI9481_TFT_DISPLAY::print(const uint8_t *st){
//    Print_String( *st, text_x, text_y);
    
//}
