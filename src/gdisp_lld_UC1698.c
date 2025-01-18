#include "gfx.h"
#include "types.h"
#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT GDISPVMT_UC1698
#include "gdisp_lld_config.h"
#include "../ugfx/src/gdisp/gdisp_driver.h"
#if defined(STM32F407xx) || defined(STM32F103xB)
#include "stm32_board_UC1698.h"
#elif defined(ESP32)
#include "esp32_board_UC1698.h"
#else
#error Unsupported board!
#endif

#ifndef GDISP_SCREEN_HEIGHT
#define GDISP_SCREEN_HEIGHT 160
#endif
#ifndef GDISP_SCREEN_WIDTH
#define GDISP_SCREEN_WIDTH 160
#endif

typedef unsigned short WORD;
typedef unsigned long DWORD;
u8 display_copy_buffer[((GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT) >> 3)];

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay* g) {
  // Initialise the board interface
  if (!init_board(g)) {
    return FALSE;
  }

  // Finish Init
  post_init_board(g);

  // Initialise the GDISP structure
  g->g.Width = GDISP_SCREEN_WIDTH;
  g->g.Height = GDISP_SCREEN_HEIGHT;
  g->g.Orientation = GDISP_ROTATE_0;
  g->g.Powermode = powerOn;

  reset();

  /*power control*/
  write_command(0xe9);  // Bias Ratio:1/10 bias
  write_command(0x2b);  // power control set as internal power
  write_command(0x24);  // set temperate compensation as 0%

  u8 Contrast_level = 0xbf;
  write_command(0x81);
  write_command(Contrast_level);

  /*display control*/
  write_command(0xa4);  // all pixel off
  write_command(0xa6);  // inverse display off

  /*lcd control*/
  write_command(0xc0 | 0);  // write_command(0xc4);     			//Set LCD Maping Control (MY=1, MX=0)
  write_command(0xa1);      // line rate 15.2klps
  write_command(0xd1);      // rgb-rgb
  write_command(0xd5);      // 4k color mode
  write_command(0x84);      // 12:partial display control disable

  /*n-line inversion*/
  write_command(0xc8);
  write_command(0x10);  // enable NIV, 11 lines

  /*com scan fuction*/
  write_command(0xda);  // enable FRC,PWM,LRM sequence

  /*window*/
  write_command(0xf4);  // wpc0:column
  write_command(0x25);  // start from 112
  write_command(0xf6);  // wpc1
  write_command(0x5A);  // end:272

  write_command(0xf5);  // wpp0:row
  write_command(0x00);  // start from 0
  write_command(0xf7);  // wpp1
  write_command(0x9F);  // end 160

  write_command(0xf8);  // inside mode

  write_command(0x89);  // RAM control

  /*scroll line*/
  write_command(0x40);  // low bit of scroll line
  write_command(0x50);  // high bit of scroll line

  write_command(0x90);  // 14:FLT,FLB set
  write_command(0x00);

  /*partial display*/
  write_command(0x84);  // 12,set partial display control:off
  write_command(0xf1);  // com end
  write_command(0x9f);  // 160
  write_command(0xf2);  // display start
  write_command(0);     // 0
  write_command(0xf3);  // display end
  write_command(159);   // 160

  write_command(0xad);  // display on,select on/off mode.Green Enhance mode disable

  return TRUE;
}

#if GDISP_HARDWARE_FLUSH
void Data_processing(u8 temp)  // turns 1byte B/W data to 4k-color data(RRRR-GGGG-BBBB)
{
  unsigned char temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
  unsigned char h11, h12, h13, h14, h15, h16, h17, h18, d1, d2, d3, d4;

  temp1 = temp & 0x80;
  temp2 = (temp & 0x40) >> 3;
  temp3 = (temp & 0x20) << 2;
  temp4 = (temp & 0x10) >> 1;
  temp5 = (temp & 0x08) << 4;
  temp6 = (temp & 0x04) << 1;
  temp7 = (temp & 0x02) << 6;
  temp8 = (temp & 0x01) << 3;
  h11 = temp1 | temp1 >> 1 | temp1 >> 2 | temp1 >> 3;
  h12 = temp2 | temp2 >> 1 | temp2 >> 2 | temp2 >> 3;
  h13 = temp3 | temp3 >> 1 | temp3 >> 2 | temp3 >> 3;
  h14 = temp4 | temp4 >> 1 | temp4 >> 2 | temp4 >> 3;
  h15 = temp5 | temp5 >> 1 | temp5 >> 2 | temp5 >> 3;
  h16 = temp6 | temp6 >> 1 | temp6 >> 2 | temp6 >> 3;
  h17 = temp7 | temp7 >> 1 | temp7 >> 2 | temp7 >> 3;
  h18 = temp8 | temp8 >> 1 | temp8 >> 2 | temp8 >> 3;
  d1 = h11 | h12;
  d2 = h13 | h14;
  d3 = h15 | h16;
  d4 = h17 | h18;

  write_data(d1);
  write_data(d2);
  write_data(d3);
  write_data(d4);
}
LLDSPEC void gdisp_lld_flush(GDisplay* g) {
  // Acquire the bus to communicate with the display controller
  acquire_bus(g);

  write_command(0x60);  // row address LSB
  write_command(0x70);  // row address MSB
  write_command(0x12);  // column address MSB
  write_command(0x05);  // column address LSB

  uint k = ((GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT) >> 3);
  uint i, j;
  u8 temp;
  for (i = 0; i < 160; i++)  // 160*160 B/W picture for example
  {
    for (j = 0; j < 20; j++)  // 160 dot/ 8 bite=20 byte
    {
      temp = display_copy_buffer[k--];
      Data_processing(temp);
    }
    write_data(0x00);  // There are 162-160=2 segment need to write data*/
  }

  // Release the bus
  release_bus(g);
}
#endif

#if GDISP_HARDWARE_DRAWPIXEL
static void translate_coordinates(GDisplay* g, coord_t* _x, coord_t* _y) {
  coord_t x, y;
  // Handle the different possible orientations
#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
  switch (g->g.Orientation) {
    default:
    case GDISP_ROTATE_0:
#if FB_REVERSEAXIS_X
      x = GDISP_SCREEN_WIDTH - 1 - g->p.x;
#else
      x = g->p.x;
#endif
#if FB_REVERSEAXIS_Y
      y = GDISP_SCREEN_HEIGHT - 1 - g->p.y;
#else
      y = g->p.y;
#endif
      break;
    case GDISP_ROTATE_90:
#if FB_REVERSEAXIS_X
      x = GDISP_SCREEN_WIDTH - 1 - g->p.y;
#else
      x = g->p.y;
#endif
#if FB_REVERSEAXIS_Y
      y = g->p.x;
#else
      y = GDISP_SCREEN_HEIGHT - 1 - g->p.x;
#endif
      break;
    case GDISP_ROTATE_180:
#if FB_REVERSEAXIS_X
      x = g->p.x;
#else
      x = GDISP_SCREEN_WIDTH - 1 - g->p.x;
#endif
#if FB_REVERSEAXIS_Y
      y = g->p.y;
#else
      y = GDISP_SCREEN_HEIGHT - 1 - g->p.y;
#endif
      break;
    case GDISP_ROTATE_270:
#if FB_REVERSEAXIS_X
      x = g->p.y;
#else
      x = GDISP_SCREEN_WIDTH - 1 - g->p.y;
#endif
#if FB_REVERSEAXIS_Y
      y = GDISP_SCREEN_HEIGHT - 1 - g->p.x;
#else
      y = g->p.x;
#endif
      break;
  }
#else
#if FB_REVERSEAXIS_X
  x = GDISP_SCREEN_WIDTH - 1 - g->p.x;
#else
  x = g->p.x;
#endif
#if FB_REVERSEAXIS_Y
  y = GDISP_SCREEN_HEIGHT - 1 - g->p.y;
#else
  y = g->p.y;
#endif
#endif

  *_x = x;
  *_y = y;
}

LLDSPEC void gdisp_lld_draw_pixel(GDisplay* g) {
  coord_t x_coord, y_coord;
  translate_coordinates(g, &x_coord, &y_coord);

  DWORD address = 1;
  WORD w_temp;
  u8* p_local_ram_buffer;
  u8 display_working_byte_value;
  u8 bitmap_mask;

  // The driver always works with coordinate 0,0 in top left corner.
  // This function converts the required coordinates to the display address and then writes the pixel.
  // Adjust this function as requried for new screen models and orientations.
  //
  // UC1698U with monochrome LCD MCCOG240160B6W
  //	2 bytes per 3 pixels [R4,R3,R2,R1,R0,G5,G4,G3 | G2,G1,G0,B4,B3,B2,B1,B0]
  //	R, G and B are each a seperate pixel and both bytes need to be written to update the pixels.
  //	Even though we're monochrome the driver is greyscale based and you just have to put a >50%ish value to turn the
  //pixel on. 	Byte1:Byte0 control the top left 3 pixels 	Byte3:Byte2 control the top left 3 pixels 1 row down 	Pixel
  //displayed ordering is G|B|R As we're lots of memory we have a local ram buffer (could read from the screen but not
  // implemented that)

  // ENSURE X AND Y COORDINATES ARE IN RANGE
  if (x_coord >= GDISP_SCREEN_WIDTH)
    return;
  if (y_coord >= GDISP_SCREEN_HEIGHT)
    return;

  //------------------------------------------------------
  //----- UPDATE THE LOCAL RAM BUFFER WITH THE PIXEL -----
  //------------------------------------------------------

  //----- READ THE CURRENT DISPLAY BYTE VALUE -----
  p_local_ram_buffer =
      &display_copy_buffer[0];  // We use a pointer to allow for compilers that can't access large arrays directly
#if (GDISP_SCREEN_HEIGHT & 0x0007)
  p_local_ram_buffer += x_coord * ((GDISP_SCREEN_HEIGHT >> 3) +
                                   1);  // DISPLAY_WIDTH_PIXELS is not a multiple of 8 so need to add 1 for remainder
#else
  p_local_ram_buffer += x_coord * (GDISP_SCREEN_HEIGHT >> 3);  // DISPLAY_WIDTH_PIXELS is a multiple of 8
#endif
  p_local_ram_buffer += (y_coord >> 3);  // 8 pixels per byte

  display_working_byte_value = *p_local_ram_buffer;

  //----- APPLY THE MASK -----
  // Get a mask for the pixel within the byte
  bitmap_mask = 0x01;
  w_temp = y_coord & 0x07;
  while (w_temp--)
    bitmap_mask <<= 1;

  display_working_byte_value &= ~bitmap_mask;
  if (g->p.color == White)
    display_working_byte_value |= (0xff & bitmap_mask);

  // Store it back to the buffer
  *p_local_ram_buffer = display_working_byte_value;
}

#endif

#if GDISP_NEED_CONTROL
LLDSPEC void gdisp_lld_control(GDisplay* g) {
  switch (g->p.x) {
    case GDISP_CONTROL_POWER:
      if (g->g.Powermode == (powermode_t)g->p.ptr)
        return;
      switch ((powermode_t)g->p.ptr) {
        case powerOff:
          break;
        case powerSleep:
          break;
        case powerDeepSleep:
          break;
        case powerOn:
          break;
        default:
          return;
      }
      g->g.Powermode = (powermode_t)g->p.ptr;
      return;

    case GDISP_CONTROL_ORIENTATION:
      if (g->g.Orientation == (orientation_t)g->p.ptr)
        return;
      switch ((orientation_t)g->p.ptr) {
        case GDISP_ROTATE_0:
        case GDISP_ROTATE_180:
          if (g->g.Orientation == GDISP_ROTATE_90 || g->g.Orientation == GDISP_ROTATE_270) {
            coord_t tmp;

            tmp = g->g.Width;
            g->g.Width = g->g.Height;
            g->g.Height = tmp;
          }
          break;
        case GDISP_ROTATE_90:
        case GDISP_ROTATE_270:
          if (g->g.Orientation == GDISP_ROTATE_0 || g->g.Orientation == GDISP_ROTATE_180) {
            coord_t tmp;

            tmp = g->g.Width;
            g->g.Width = g->g.Height;
            g->g.Height = tmp;
          }
          break;
        default:
          return;
      }
      g->g.Orientation = (orientation_t)g->p.ptr;
      return;

    case GDISP_CONTROL_BACKLIGHT:
      if ((unsigned)g->p.ptr > 100) {
        g->p.ptr = (void*)100;
      }
      board_backlight(g, (unsigned)g->p.ptr);
      g->g.Backlight = (unsigned)g->p.ptr;
      return;

    case GDISP_CONTROL_CONTRAST:
      return;
  }
}
#endif /* GDISP_NEED_CONTROL */

#endif /* GFX_USE_GDISP */
