/***********************************************************************************
*This program is a demo of drawing
*This demo was made for 3.5" LCD module  8bit with ILI9486 controller. 
*MY_ILI9486_TFT.h library is created by refering to LCDWIKI_KBV.h, LCDWIKI_KBV.cpp. 
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
#include <TouchScreen_STM.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library

#include <MY_ILI9486_TFT.h>
MY_ILI9486_TFT my_lcd;


//param calibration from touch

#define TS_MINX 130 
#define TS_MAXX 894

#define TS_MINY 80
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


uint16_t color_mask[] = {0xF800,0xFFE0,0x07E0,0x07FF,0x001F,0xF81F}; //color select

#define COLORBOXSIZE my_lcd.Get_Display_Width()/6
#define PENBOXSIZE my_lcd.Get_Display_Width()/4

int16_t old_color, current_color,flag_colour;
int16_t old_pen,current_pen,flag_pen;
boolean show_flag = true;

void show_string(const char/*uint8_t*/ *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//show color select menu
void show_color_select_menu(void)
{
   uint16_t i;
   for(i = 0;i<6;i++)
   {
       my_lcd.Set_Draw_color(color_mask[i]);
       my_lcd.Fill_Rectangle(i*COLORBOXSIZE, 0, (i+1)*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
   }  
   my_lcd.Set_Draw_color(GREEN);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, COLORBOXSIZE/2+2, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("OK",CENTER,COLORBOXSIZE/2+4,2,RED, BLACK,1);
}

//show pen size select menu
void show_pen_size_select_menu(void)
{
  uint16_t i;
  my_lcd.Set_Text_Mode(1);
  my_lcd.Set_Text_Size(2);
  my_lcd.Set_Text_colour(GREEN);
  my_lcd.Set_Text_Back_colour(BLACK);
  for(i = 0;i<4;i++)
  {
      my_lcd.Print_Number_Int(i+1, 5+PENBOXSIZE*i, (COLORBOXSIZE/2-16)/2, 0, ' ',10);
      my_lcd.Set_Draw_color(RED);
      my_lcd.Fill_Rectangle(25+PENBOXSIZE*i, COLORBOXSIZE/2/2-i, PENBOXSIZE*(i+1)-10, COLORBOXSIZE/2/2+i);
  }
   my_lcd.Set_Draw_color(GREEN);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, COLORBOXSIZE/2+2, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("OK",CENTER,COLORBOXSIZE/2+4,2,RED, BLACK,1);
}

//show main menu
void show_main_menu(void)
{
   my_lcd.Set_Draw_color(YELLOW);
   my_lcd.Fill_Round_Rectangle(5, 0, (my_lcd.Get_Display_Width()-20)/3+5,COLORBOXSIZE/2+20, 5);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3*2+15, 0, (my_lcd.Get_Display_Width()-20)/3*3+15,COLORBOXSIZE/2+20, 5);
   my_lcd.Set_Draw_color(MAGENTA);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, 0, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("COLOUR",5+((my_lcd.Get_Display_Width()-20)/3-72)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
   show_string("CLEAR",(my_lcd.Get_Display_Width()-20)/3+10+((my_lcd.Get_Display_Width()-20)/3-60)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,WHITE, BLACK,1);
   show_string("PENSIZE",(my_lcd.Get_Display_Width()-20)/3*2+15+((my_lcd.Get_Display_Width()-20)/3-84)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
   my_lcd.Set_Draw_color(GREEN);
   
 }
uint8_t data[512];uint16_t sdblockno=0,sdblocklen=64;


void setup(void) 
{
  Serial.begin(9600);
  my_lcd.Init_LCD();
  my_lcd.Fill_Screen(BLACK);
  show_main_menu();
  current_color = RED;
  current_pen = 0;
  pinMode(13, OUTPUT);
  pinMode(SS, OUTPUT); //sd_src
  digitalWrite(SS, LOW);
  delay(10);
 

}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
comme:
  digitalWrite(PC13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(PC13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
      p.x = map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(),0);
     if(p.y < COLORBOXSIZE/2+45 && p.y > COLORBOXSIZE/2+20) 
         {

              if(((p.x>5)&&(p.x < ((my_lcd.Get_Display_Width()-20)/3+5)))&&!flag_pen) //select color
              {
                my_lcd.Set_Draw_color(GREEN);
                my_lcd.Fill_Round_Rectangle(5, COLORBOXSIZE/2+23, (my_lcd.Get_Display_Width()-20)/3+5,20+COLORBOXSIZE, 5);
              }
         }
      
         if(p.y < COLORBOXSIZE/2+20) 
         {

              if(((p.x>5)&&(p.x < ((my_lcd.Get_Display_Width()-20)/3+5)))&&!flag_pen) //select color
              {
                flag_colour = 1;
                if(show_flag)
                {
                    my_lcd.Set_Draw_color(BLACK);
                    my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                    show_color_select_menu();
                }
                show_flag = false;
                switch(current_color)
                {
                  case RED:
                  {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    break;  
                  }
                  case YELLOW:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                  }
                  case GREEN:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                   }
                  case CYAN:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;                 
                  }
                  case BLUE:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                  }
                  case MAGENTA:  
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                  }
                  default:
                      break;
                }
             }
             if(flag_colour)
             {
                 if(p.y < COLORBOXSIZE/2)
                 {
                    old_color = current_color;
                    if (p.x < COLORBOXSIZE) 
                    { 
                        current_color = RED; 
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (p.x < COLORBOXSIZE*2) 
                    {
                        current_color = YELLOW;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (p.x < COLORBOXSIZE*3) 
                    {
                        current_color = GREEN;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (p.x < COLORBOXSIZE*4) 
                    {
                        current_color = CYAN;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (p.x < COLORBOXSIZE*5) 
                    {
                        current_color = BLUE;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (p.x < COLORBOXSIZE*6) 
                    {
                        current_color = MAGENTA;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    }
                    if(old_color != current_color)
                    {
                        switch(old_color)
                        {
                            case RED:
                            {
                              my_lcd.Set_Draw_color(RED);
                              my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                              break;  
                            }
                            case YELLOW:
                            {
                                 my_lcd.Set_Draw_color(YELLOW);
                                 my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break; 
                            }
                            case GREEN:
                            {
                                 my_lcd.Set_Draw_color(GREEN);
                                 my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break; 
                             }
                            case CYAN:
                            {
                                 my_lcd.Set_Draw_color(CYAN);
                                 my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;                 
                            }
                            case BLUE:
                            {
                                 my_lcd.Set_Draw_color(BLUE);
                                 my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;   
                            }
                            case MAGENTA:  
                            {
                                 my_lcd.Set_Draw_color(MAGENTA);
                                 my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;   
                            }
                            default:
                                break;                        
                        }
                    }
                }
                else if(p.y < COLORBOXSIZE/2+20)
                {
                     if((p.x>(my_lcd.Get_Display_Width()-20)/3+10)&&(p.x<(my_lcd.Get_Display_Width()-20)/3*2+10))
                     {
                         my_lcd.Set_Draw_color(BLACK);
                         my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                         show_main_menu();
                         flag_colour = 0;
                         show_flag = true;
                         goto comme;
                     }
                }
            }
            if(((p.x>((my_lcd.Get_Display_Width()-20)/3*2+15))&&(p.x < (((my_lcd.Get_Display_Width()-20)/3*3+15))))&&!flag_colour) //select pen size
            {
                flag_pen = 1;
                if(show_flag)
                {
                    my_lcd.Set_Draw_color(BLACK);
                    my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                    show_pen_size_select_menu();
                }
                show_flag = false;
                switch(current_pen)
                {
                   case 0:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;
                   }
                   case 1:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                   }
                   case 2:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                   }
                   case 3:  
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;                
                   }
                   default:
                       break;
                  }              
              }
              if(flag_pen)
              {
                   if(p.y < COLORBOXSIZE/2)
                   {
                      old_pen = current_pen;
                      if(p.x < PENBOXSIZE)
                      {
                          current_pen = 0;
                          my_lcd.Set_Draw_color(WHITE);
                          my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                      }
                      else if(p.x < 2*PENBOXSIZE)
                      {
                          current_pen = 1;
                          my_lcd.Set_Draw_color(WHITE);
                          my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       }
                       else if(p.x < 3*PENBOXSIZE) 
                       {
                           current_pen = 2;
                           my_lcd.Set_Draw_color(WHITE);
                           my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       }
                       else if(p.x < 4*PENBOXSIZE)
                       {
                           current_pen = 3;
                           my_lcd.Set_Draw_color(WHITE);
                           my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);               
                       }
                       if(old_pen != current_pen)
                       {
                           switch(old_pen)
                           {
                                 case 0:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;
                                 }
                                 case 1:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break; 
                                 }
                                 case 2:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;   
                                 }
                                 case 3:  
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;                
                                 }
                                 default:
                                     break;           
                           }      
                       }
                   }
                   else if(p.y < COLORBOXSIZE/2+20)
                   {
                       if((p.x>(my_lcd.Get_Display_Width()-20)/3+10)&&(p.x<(my_lcd.Get_Display_Width()-20)/3*2+10))
                       {
                          my_lcd.Set_Draw_color(BLACK);
                          my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                          show_main_menu();
                          flag_pen = 0;
                          show_flag = true;
                          goto comme;
                       }  
                   }
              }
              if(((p.x>((my_lcd.Get_Display_Width()-20)/3+10))&&(p.x < ((my_lcd.Get_Display_Width()-20)/3*2+10)))&&!flag_colour&&!flag_pen)
              {
                  my_lcd.Set_Draw_color(BLACK);  
                  my_lcd.Fill_Rectangle(0,COLORBOXSIZE+25,my_lcd.Get_Display_Width()-1,my_lcd.Get_Display_Height()-1);
              }
         }
      if (((p.y-current_pen) > COLORBOXSIZE+25) && ((p.y+current_pen) < my_lcd.Get_Display_Height()))  //drawing
      {
        my_lcd.Set_Draw_color(current_color);
       // if(1 == current_pen)
     //   {
      //      my_lcd.Draw_Pixel(p.x,  p.y);
     //   }
     //   else 
     //   {
          my_lcd.Fill_Circle(p.x,  p.y,current_pen);
      //  }
    }
  }
}
