///////////////////////////////////////////////////
/* credits to http://www.technoblogy.com/show?L6I*/
/* original library adapted to our needs		 */
/* 08.2020 GRN									 */
///////////////////////////////////////////////////

////////////////////////////////////////
/*
 *	add this part to your main.c file
////////////////////////////////////////

#define BACKLIGHT_ON PORTB |= (1<<PB2)
#define BACKLIGHT_OFF PORTB &= ~(1<<PB2)						

#define LED_OFF PORTC &= ~(1<<PC3)
#define LED_ON PORTC |= (1<<PC3)

// Pins already defined in st7735.c
extern int const DC;
extern int const MOSI;
extern int const SCK;
extern int const CS;
// Text scale and plot colours defined in st7735.c
extern int fore; // White
extern int back;      // Black
extern int scale;     // Text scale

*/

#include "st7735.h"

extern void SPI_MasterTransmit(char cData);

// Adafruit 1.44" 128x128 display
 int const ysize = 128, xsize = 128, yoff = 1, xoff = 2, invert = 0, rotate = 3;

// ATtiny85 pins
 int const DC = PB1;
 int const MOSI = PB3;
 int const SCK = PB5;
 int const CS = PB0;

// Character set for text - stored in program memory
const uint8_t CharMap[96][6] PROGMEM = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, 
{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, 
{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, 
{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, 
{ 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 }, 
{ 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 }, 
{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, 
{ 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 }, 
{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 }, 
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 }, 
{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, 
{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, 
{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 }, 
{ 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 }, 
{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, 
{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, 
{ 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 }, 
{ 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 }, 
{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 }, 
{ 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 }, 
{ 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 }, 
{ 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, 
{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 }, 
{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, 
{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 }, 
{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, 
{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, 
{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 }, 
{ 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 }, 
{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, 
{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, 
{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 }, 
{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, 
{ 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 }, 
{ 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 }, 
{ 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00 }, 
{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, 
{ 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00 }, 
{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, 
{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x00, 0x03, 0x07, 0x08, 0x00, 0x00 }, 
{ 0x20, 0x54, 0x54, 0x78, 0x40, 0x00 }, 
{ 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x28, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00 }, 
{ 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 }, 
{ 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00 }, 
{ 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00 }, 
{ 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00 }, 
{ 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 }, 
{ 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00 }, 
{ 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 }, 
{ 0x48, 0x54, 0x54, 0x54, 0x24, 0x00 }, 
{ 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00 }, 
{ 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 }, 
{ 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 }, 
{ 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 }, 
{ 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 }, 
{ 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00 }, 
{ 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 }, 
{ 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x77, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 }, 
{ 0x00, 0x06, 0x09, 0x06, 0x00, 0x00 },  // degree symbol = '~'
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }
};

// TFT colour display **********************************************

 int const CASET = 0x2A; // Define column address
 int const RASET = 0x2B; // Define row address
 int const RAMWR = 0x2C; // Write to display RAM

// Current plot position and colours
 int x_0, y_0;
 int fore = 0xFFFF; // White
 int back = 0;      // Black
 int scale = 1;     // Text scale

// Send a byte to the display
/*void Data (uint8_t d) 
{
  SPI_MasterTransmit(d);
}*/
// Send a byte to the display
/*void Data (uint8_t d) 
{
  for (uint8_t bit = 0x80; bit; bit >>= 1) {
    PINB = 1<<SCK;                         // sck low
    if (d & bit) PORTB = PORTB | (1<<MOSI); else PORTB = PORTB & ~(1<<MOSI);
    PINB = 1<<SCK;                         // sck high
  }
}*/

// Send a command to the display
void Command (uint8_t c) 
{
    PINB = 1<<DC;                          // dc low
    SPI_MasterTransmit(c);
    PINB = 1<<DC;                          // dc high again
}

// Send a command followed by four data bytes
void Command4 (uint8_t c, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) 
{
    PINB = (1<<DC);                          // dc low
    SPI_MasterTransmit(c);
    PINB = 1<<DC;                          // dc high again
    SPI_MasterTransmit(d1); SPI_MasterTransmit(d2); SPI_MasterTransmit(d3); SPI_MasterTransmit(d4);
}
  
void InitDisplay (void) 
{
  //DDRB = 1<<dc | 1<<cs | 1<<mosi | 1<<sck; // All outputs
  //PORTB = 1<<sck | 1<<cs | 1<<dc;          // clk, dc, and cs high
  PINB = 1<<CS;                            // cs low
  Command(0x01);                           // Software reset
  _delay_ms(150);                              // _delay_ms 150 ms
  Command(0x11);                           // Out of sleep mode
  _delay_ms(500);                              // _delay_ms 500 ms
	Command(0x3A); SPI_MasterTransmit(0x05);               // Set color mode - 16-bit color (rgb 5-6-5)
   Command(0x20+invert);                    // Invert
  Command(0x36); SPI_MasterTransmit(rotate<<5);          // Set orientation
  PINB = 1<<CS;                            // cs high
}

void DisplayOn (void) 
{
  PINB = 1<<CS;                            // cs low
  Command(0x29);                           // Display on
  _delay_ms(100);
  PINB = 1<<CS;                            // cs high
}

void ClearDisplay (void) 
{
  PINB = 1<<CS;                            // cs low
  Command4(CASET, yoff>>8, yoff, 0, yoff + ysize - 1);
  Command4(RASET, xoff>>8, xoff, 0, xoff + xsize - 1);
  Command(0x3A); SPI_MasterTransmit(0x03);               // 12-bit colour
  Command(RAMWR);
  for (int i=0; i<xsize/2; i++) {
    for (int j=0; j<ysize * 3; j++) 
    {
      SPI_MasterTransmit(0);
    }
  }
  Command(0x3A); SPI_MasterTransmit(0x05);               // Back to 16-bit colour
  //8-bit data bus for 16-bit/pixel (RGB 5-6-5-bit input), 65K-Colors, 3AH= “05h”
  PINB = 1<<CS;                            // cs high
}

unsigned int Colour (int b, int g, int r) 
{
  return (r & 0xf8)<<8 | (g & 0xfc)<<3 | b>>3;
  //return (r & 0xf8)<<8 | (g & 0xfc)<<3 | b>>3;//original 
}

// Move current plot position to x,y
void MoveTo (int x, int y) 
{
  x_0 = x; y_0 = y;
}

// Plot point at x,y
void PlotPoint (int x, int y) 
{
  PINB = 1<<CS;                            // cs low
  Command4(CASET, 0, yoff+y, 0, yoff+y);
  Command4(RASET, 0, xoff+x, 0, xoff+x);
  Command(RAMWR); SPI_MasterTransmit(fore>>8); SPI_MasterTransmit(fore & 0xff);
  PINB = 1<<CS;                            // cs high
}

// Draw a line to x,y
void DrawTo (int x, int y) 
{
  int sx, sy, e2, err;
  int dx = abs(x - x_0);
  int dy = abs(y - y_0);
  if (x_0 < x) sx = 1; else sx = -1;
  if (y_0 < y) sy = 1; else sy = -1;
  err = dx - dy;
  for (;;) {
    PlotPoint(x_0, y_0);
    if (x_0==x && y_0==y) return;
    e2 = err<<1;
    if (e2 > -dy) { err = err - dy; x_0 = x_0 + sx; }
    if (e2 < dx) { err = err + dx; y_0 = y_0 + sy; }
  }
}

void FillRect (int w, int h) 
{
  PINB = 1<<CS;                            // cs low
  Command4(CASET, 0, y_0+yoff, 0, y_0+yoff+h-1);
  Command4(RASET, 0, x_0+xoff, 0, x_0+xoff+w-1);
  Command(RAMWR);
  for (int p=0; p<w*h*2; p++) {
    SPI_MasterTransmit(fore>>8); SPI_MasterTransmit(fore & 0xff);
  }
  PINB = 1<<CS;                            // cs high
}

// Plot an ASCII character with bottom left corner at x,y
void PlotChar (char c) 
{
  int colour;
  PINB = 1<<CS;                            // cs low
  Command4(CASET, 0, yoff+y_0, 0, yoff+y_0+8*scale-1);
  Command4(RASET, 0, xoff+x_0, 0, xoff+x_0+6*scale-1);
  Command(RAMWR);
  for (int xx=0; xx<6; xx++) 
  {
    int bits = pgm_read_byte(&CharMap[c-32][xx]);
    for (int xr=0; xr<scale; xr++) 
    {
      for (int yy=0; yy<8; yy++) 
      {
        if (bits>>(7-yy) & 1) colour = fore; else colour = back;
        for (int yr=0; yr<scale; yr++) 
        {
          SPI_MasterTransmit(colour>>8); SPI_MasterTransmit(colour & 0xFF);
        }
      }
    }
  }
  PINB = 1<<CS;                            // cs high
  x_0 = x_0 + 6*scale;
}

// Plot text starting at the current plot position
void PlotText(PGM_P p) 
{
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) return;
    PlotChar(c);
  }
}

void TestChart (void) 
{
  MoveTo(0,0);
  DrawTo(xsize-1, 0); DrawTo(xsize-1, ysize-1);
  DrawTo(0, ysize-1); DrawTo(0, 0);
  scale=8;
  MoveTo((xsize-40)/2, (ysize-64)/2); PlotChar('A');
}

// Setup **********************************************

void setup(void) 
{
  InitDisplay();
  ClearDisplay();
  DisplayOn();
  MoveTo(0,0);
}

void PlotString(const char str[]) 
{
	while (*str) PlotChar(*str++);
}



void glcd_draw_circle(uint8_t x0, uint8_t y0, uint8_t r)
{                  
          int8_t f = 1 - r;
          int8_t ddF_x = 1;
          int8_t ddF_y = -2 * r;
          int8_t x = 0;
          int8_t y = r;
         
         //  glcd_update_bbox(x0-r, y0-r, x0+r, y0+r);
           
         PlotPoint(x0, y0+r);
          PlotPoint(x0, y0-r);
           PlotPoint(x0+r, y0);
          PlotPoint(x0-r, y0);
          
           while (x<y) {
                   if (f >= 0) {
                           y--;
                           ddF_y += 2;
                           f += ddF_y;
                   }
                   x++;
                   ddF_x += 2;
                   f += ddF_x;
                   
                   PlotPoint(x0 + x, y0 + y);
                   PlotPoint(x0 - x, y0 + y);
                   PlotPoint(x0 + x, y0 - y);
                   PlotPoint(x0 - x, y0 - y);
                   
                   PlotPoint(x0 + y, y0 + x);
                   PlotPoint(x0 - y, y0 + x);
                   PlotPoint(x0 + y, y0 - x);
                   PlotPoint(x0 - y, y0 - x);
                   
           }
   }//end of draw circle




