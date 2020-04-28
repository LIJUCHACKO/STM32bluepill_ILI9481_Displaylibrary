/*
 An example analogue clock using a TFT LCD screen to show the time
 use of some of the drawing commands with the Adafruit_GFX library.
 For a more accurate clock, it would be better to use the RTC library.
 But this is just a demo. 
 
 Gilchrist 6/2/2014 1.0
 Updated by Alan Senior 18/1/2015
 Updated by nopnop2002 5/1/2018
 */

#include "RTClock.h"
RTClock rtclock (RTCSEL_LSE);
tm_t mtt;
char s[100]; // for sprintf
char s2[100]; // for sprintf
//#include <Adafruit_GFX.h>

#include <ILI9481_TFT_DISPLAY.h>
ILI9481_TFT_DISPLAY tft;
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
uint16_t x00=0, x11=0, y00=0, y11=0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh,mm,ss;
boolean initial = 1;
//-----------------------------------------------------------------------------
const char * weekdays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char * months[] = {"Dummy", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
//-----------------------------------------------------------------------------
uint8_t str2month(const char * d)
{
    uint8_t i = 13;
    while ( (--i) && strcmp(months[i], d)!=0 );
    return i;
}
//-----------------------------------------------------------------------------
const char * delim = " :";
//-----------------------------------------------------------------------------
void ParseBuildTimestamp(tm_t & mt)
{
    // Timestamp format: "Dec  8 2017, 22:57:54"
    sprintf(s, "Timestamp: %s, %s\n", __DATE__, __TIME__);
    //Serial.print(s);
    char * token = strtok(s, delim); // get first token
    // walk through tokens
    while( token != NULL ) {
        uint8_t m = str2month((const char*)token);
        if ( m>0 ) {
            mt.month = m;
            //Serial.print(" month: "); Serial.println(mt.month);
            token = strtok(NULL, delim); // get next token
            mt.day = atoi(token);
            //Serial.print(" day: "); Serial.println(mt.day);
            token = strtok(NULL, delim); // get next token
            mt.year = atoi(token) - 1970;
            //Serial.print(" year: "); Serial.println(mt.year);
            token = strtok(NULL, delim); // get next token
            mt.hour = atoi(token);
            //Serial.print(" hour: "); Serial.println(mt.hour);
            token = strtok(NULL, delim); // get next token
            mt.minute = atoi(token);
            //Serial.print(" minute: "); Serial.println(mt.minute);
            token = strtok(NULL, delim); // get next token
            mt.second = atoi(token);
            //Serial.print(" second: "); Serial.println(mt.second);
        }
        token = strtok(NULL, delim);
    }
}
void setup(void) {


  
  delay(1000);

  Serial.begin(9600);
  delay(1000);
  
  tft.Init_LCD();
  tft.Set_Rotation(1);
  ParseBuildTimestamp(mtt);
  time_t  tt; 
  /* Uncomment below two lines to set rtc clock time*/
  //tt = rtclock.makeTime(mtt);
  //rtclock.setTime(tt);

  uint16_t wid = tft.Get_Width();
  uint16_t ht = tft.Get_Height();
  Serial.println("width=" + String(wid) + " height=" + String(ht));

  if (wid < ht) {
    tft.Set_Rotation(0);
  } else {
    tft.Set_Rotation(1);
  }
  tft.Fill_Screen(RED);
  tft.Set_Text_Back_colour(RED);
  tft.Set_Text_colour(WHITE);//, GRAY);  // Adding a background colour erases previous text automatically
  
  // Draw clock face
  tft.Fill_Circl(120, 120, 118, GREEN);
  tft.Fill_Circl(120, 120, 110, BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x00 = sx*114+120;
    y00 = sy*114+120;
    x11 = sx*100+120;
    y11 = sy*100+120;

    tft.Draw_Lin(x00, y00, x11, y11, GREEN);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x00 = sx*102+120;
    y00 = sy*102+120;
    // Draw minute markers
    tft.Draw_Pixe(x00, y00, WHITE);
    
    // Draw main quadrant dots
    if(i==0 || i==180) tft.Fill_Circl(x00, y00, 2, WHITE);
    if(i==90 || i==270) tft.Fill_Circl(x00, y00, 2, WHITE);
  }

  tft.Fill_Circl(120, 121, 3, WHITE);
  
  targetTime = millis() + 1000; 
}

void loop() {
  struct tm * tx; 
  rtclock.breakTime(rtclock.now(), mtt);
    sprintf(s, "%s %u %u, %s",
      months[mtt.month], mtt.day, mtt.year+1970, weekdays[mtt.weekday]);
      sprintf(s2, "%u/%u/%u,  %02u:%02u:%02u\n",
      mtt.day,mtt.month,  mtt.year+1970,  mtt.hour, mtt.minute, mtt.second);

  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss=mtt.second;
    mm=mtt.minute;
    hh=mtt.hour;
    

    Serial.print("hh=");
    Serial.print(hh);
    Serial.print(" mm=");
    Serial.print(mm);
    Serial.print(" ss=");
    Serial.println(ss);
    Serial.println(tft.Read_ID(),HEX);

    
    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx =  cos((hdeg-90)*0.0174532925);    
    hy =  sin((hdeg-90)*0.0174532925);
    mx =  cos((mdeg-90)*0.0174532925);    
    my =  sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy =  sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.Draw_Lin(ohx, ohy, 120, 121, BLACK);
      ohx = hx*62+121;    
      ohy = hy*62+121;
      tft.Draw_Lin(omx, omy, 120, 121, BLACK);
      omx = mx*84+120;    
      omy = my*84+121;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      tft.Draw_Lin(osx, osy, 120, 121, BLACK);
      osx = sx*90+121;    
      osy = sy*90+121;
      tft.Draw_Lin(osx, osy, 120, 121, RED);
      tft.Draw_Lin(ohx, ohy, 120, 121, WHITE);
      tft.Draw_Lin(omx, omy, 120, 121, WHITE);
      tft.Draw_Lin(osx, osy, 120, 121, RED);

    tft.Fill_Circl(120, 121, 3, RED);
    tft.Set_Text_Cousur(20, 260);
    tft.Set_Text_Size(2);
    tft.Print_String(s2,20,260);
  }
  delay(1000);
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
