#ifndef MY_ILI9486_TFT_H_
#define MY_ILI9486_TFT_H_
// Register names from Peter Barrett's Microtouch code
#define ILI932X_START_OSC          0x00
#define ILI932X_DRIV_OUT_CTRL      0x01
#define ILI932X_DRIV_WAV_CTRL      0x02
#define ILI932X_ENTRY_MOD          0x03
#define ILI932X_RESIZE_CTRL        0x04
#define ILI932X_DISP_CTRL1         0x07
#define ILI932X_DISP_CTRL2         0x08
#define ILI932X_DISP_CTRL3         0x09
#define ILI932X_DISP_CTRL4         0x0A
#define ILI932X_RGB_DISP_IF_CTRL1  0x0C
#define ILI932X_FRM_MARKER_POS     0x0D
#define ILI932X_RGB_DISP_IF_CTRL2  0x0F
#define ILI932X_POW_CTRL1          0x10
#define ILI932X_POW_CTRL2          0x11
#define ILI932X_POW_CTRL3          0x12
#define ILI932X_POW_CTRL4          0x13
#define ILI932X_GRAM_HOR_AD        0x20
#define ILI932X_GRAM_VER_AD        0x21
#define ILI932X_RW_GRAM            0x22
#define ILI932X_POW_CTRL7          0x29
#define ILI932X_FRM_RATE_COL_CTRL  0x2B
#define ILI932X_GAMMA_CTRL1        0x30
#define ILI932X_GAMMA_CTRL2        0x31
#define ILI932X_GAMMA_CTRL3        0x32
#define ILI932X_GAMMA_CTRL4        0x35
#define ILI932X_GAMMA_CTRL5        0x36
#define ILI932X_GAMMA_CTRL6        0x37
#define ILI932X_GAMMA_CTRL7        0x38
#define ILI932X_GAMMA_CTRL8        0x39
#define ILI932X_GAMMA_CTRL9        0x3C
#define ILI932X_GAMMA_CTRL10       0x3D
#define ILI932X_HOR_START_AD       0x50
#define ILI932X_HOR_END_AD         0x51
#define ILI932X_VER_START_AD       0x52
#define ILI932X_VER_END_AD         0x53
#define ILI932X_GATE_SCAN_CTRL1    0x60
#define ILI932X_GATE_SCAN_CTRL2    0x61
#define ILI932X_GATE_SCAN_CTRL3    0x6A
#define ILI932X_PART_IMG1_DISP_POS 0x80
#define ILI932X_PART_IMG1_START_AD 0x81
#define ILI932X_PART_IMG1_END_AD   0x82
#define ILI932X_PART_IMG2_DISP_POS 0x83
#define ILI932X_PART_IMG2_START_AD 0x84
#define ILI932X_PART_IMG2_END_AD   0x85
#define ILI932X_PANEL_IF_CTRL1     0x90
#define ILI932X_PANEL_IF_CTRL2     0x92
#define ILI932X_PANEL_IF_CTRL3     0x93
#define ILI932X_PANEL_IF_CTRL4     0x95
#define ILI932X_PANEL_IF_CTRL5     0x97
#define ILI932X_PANEL_IF_CTRL6     0x98

#define HX8347G_COLADDRSTART_HI    0x02
#define HX8347G_COLADDRSTART_LO    0x03
#define HX8347G_COLADDREND_HI      0x04
#define HX8347G_COLADDREND_LO      0x05
#define HX8347G_ROWADDRSTART_HI    0x06
#define HX8347G_ROWADDRSTART_LO    0x07
#define HX8347G_ROWADDREND_HI      0x08
#define HX8347G_ROWADDREND_LO      0x09
#define HX8347G_MEMACCESS          0x16



#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_MEMORYACCESS       0x36
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_RGBSIGNAL          0xB0
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_INVERSIONCONRTOL   0xB4
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1       0xC5
#define ILI9341_VCOMCONTROL2       0xC7
#define ILI9341_POWERCONTROLB      0xCF
#define ILI9341_POWERCONTROLA      0xCB
#define ILI9341_UNDEFINE0          0xE0
#define ILI9341_UNDEFINE1          0xE1
#define ILI9341_DRIVERTIMINGA      0xE8
#define ILI9341_DRIVERTIMINGB      0xEA
#define ILI9341_POWERONSEQ         0xED
#define ILI9341_ENABLE3G           0xF2
#define ILI9341_INTERFACECONTROL   0xF6
#define ILI9341_RUMPRATIO          0xF7
#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL             0x36

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04



#define HX8357_NOP     0x00
#define HX8357_SWRESET 0x01
#define HX8357_RDDID   0x04
#define HX8357_RDDST   0x09

#define HX8357B_RDPOWMODE  0x0A
#define HX8357B_RDMADCTL  0x0B
#define HX8357B_RDCOLMOD  0x0C
#define HX8357B_RDDIM  0x0D
#define HX8357B_RDDSDR  0x0F

#define HX8357_SLPIN   0x10
#define HX8357_SLPOUT  0x11
#define HX8357B_PTLON   0x12
#define HX8357B_NORON   0x13

#define HX8357_INVOFF  0x20
#define HX8357_INVON   0x21
#define HX8357_DISPOFF 0x28
#define HX8357_DISPON  0x29

#define HX8357_CASET   0x2A
#define HX8357_PASET   0x2B
#define HX8357_RAMWR   0x2C
#define HX8357_RAMRD   0x2E

#define HX8357B_PTLAR   0x30
#define HX8357_TEON  0x35
#define HX8357_TEARLINE  0x44
#define HX8357_MADCTL  0x36
#define HX8357_COLMOD  0x3A

#define HX8357_SETOSC 0xB0
#define HX8357_SETPWR1 0xB1
#define HX8357B_SETDISPLAY 0xB2
#define HX8357_SETRGB 0xB3
#define HX8357D_SETCOM  0xB6

#define HX8357B_SETDISPMODE  0xB4
#define HX8357D_SETCYC  0xB4
#define HX8357B_SETOTP 0xB7
#define HX8357D_SETC 0xB9

#define HX8357B_SET_PANEL_DRIVING 0xC0
#define HX8357D_SETSTBA 0xC0
#define HX8357B_SETDGC  0xC1
#define HX8357B_SETID  0xC3
#define HX8357B_SETDDB  0xC4
#define HX8357B_SETDISPLAYFRAME 0xC5
#define HX8357B_GAMMASET 0xC8
#define HX8357B_SETCABC  0xC9
#define HX8357_SETPANEL  0xCC


#define HX8357B_SETPOWER 0xD0
#define HX8357B_SETVCOM 0xD1
#define HX8357B_SETPWRNORMAL 0xD2

#define HX8357B_RDID1   0xDA
#define HX8357B_RDID2   0xDB
#define HX8357B_RDID3   0xDC
#define HX8357B_RDID4   0xDD

#define HX8357D_SETGAMMA 0xE0

#define HX8357B_SETGAMMA 0xC8
#define HX8357B_SETPANELRELATED  0xE9

#define HX8357B_MADCTL_MY  0x80
#define HX8357B_MADCTL_MX  0x40
#define HX8357B_MADCTL_MV  0x20
#define HX8357B_MADCTL_ML  0x10
#define HX8357B_MADCTL_RGB 0x00
#define HX8357B_MADCTL_BGR 0x08
#define HX8357B_MADCTL_MH  0x04
#define LCD_RD   PA8
#define LCD_WR   PA9     
#define LCD_RS   PA1        
#define LCD_CS   PA0       
#define LCD_REST PA10

#define YP PA0  // must be an analog pin, use "An" notation!
#define XM PA1  // must be an analog pin, use "An" notation!
#define YM PB3   // can be a digital pin
#define XP PA15 

#include <LCDWIKI_GUI.h>
// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGRAY   0xC618      /* 192, 192, 192 */
#define DARKGRAY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F
#define GRAY        0x5AEB

 #define DELAY7        \
    asm volatile(       \
        "nop" "\n\t" \
        "nop" "\n\t" \
        "nop" "\n\t" \
        "nop"      "\n"   \
        ::);
    
 typedef struct _lcd_info
{
	uint16_t lcd_id;
	int16_t lcd_wid;
	int16_t lcd_heg;
}lcd_info;


class MY_ILI9486_TFT:public LCDWIKI_GUI
{

	public:
        MY_ILI9486_TFT(void);
	void Init_LCD(void);
	//void resetlcd(void);
	//void start(uint16_t ID);
	void Draw_Pixe(int16_t x, int16_t y, uint16_t color);
	void Lcd_Write_Com(uint8_t VH);
        void Lcd_Write_Com16(uint16_t VH);
	void Lcd_Write_Data(uint8_t VH);
        void Lcd_Write_Data16(uint16_t VH);
	void Lcd_Write_Com_Data(uint8_t com,uint8_t dat);
	void Lcd_Writ_Bus(uint8_t d);
	//void init_table8(const void *table, int16_t size);
	//void init_table16(const void *table, int16_t size);
	void Push_Command(uint8_t cmd, uint8_t *block, int8_t N);
	uint16_t Color_To_565(uint8_t r, uint8_t g, uint8_t b);
	uint16_t Read_ID(void);
	void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void Set_Rotation(uint8_t r); 
	uint8_t Get_Rotation(void) const;
	void Invert_Display(boolean i);
	uint16_t Read_Reg(uint16_t reg, int8_t index);
	int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
	void Set_Addr_Window(int16_t x1,int16_t y1,int16_t x2,int16_t y2);
	void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags);
	//void Push_Any_Color(uint8_t * block, int16_t n, bool first, uint8_t flags);
        void Vert_Scroll(int16_t top, int16_t scrollines, int16_t offset);
	int16_t Get_Height(void) const;
  	int16_t Get_Width(void) const;
        void LCD_Clear(unsigned int j);
	void Rect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t c);
        void V_line(int16_t x, int16_t y, int16_t l, uint16_t c);                   
        void H_line(int16_t x, int16_t y, int16_t l, uint16_t c);                   
	void Set_LR(void);
        void Draw_Lin(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void Fill_Circl(int16_t x, int16_t y, int16_t radius,uint16_t color);
        uint8_t Lcd_Read_Bus();
        uint8_t Lcd_Read_Data();
        //void print(const uint8_t *st);
        //void print(uint8_t *st);
        void printString(const char *st);
        void printInt(long num);
        void printFloat(double num);
    protected:
        uint16_t WIDTH,HEIGHT,width, height, rotation,lcd_driver,lcd_model;
	private:
	uint16_t XC,YC,CC,RC,SC1,SC2,MD,VL,R24BIT;


};
#endif
