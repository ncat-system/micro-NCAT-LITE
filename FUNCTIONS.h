//********************************************************************************
//   VIDEO DRAW SYSTEM:
//********************************************************************************
void screenmemory_fillscreen(uint8_t COLOR) {
  for (uint16_t Ypos = 0; Ypos < SCREEN_HEIGHT; Ypos++)
    for (uint16_t Xpos = 0; Xpos < 256 /*< SCREEN_WIDTH*/ ; Xpos++) {
      SCREENMEMORY_LINE[Ypos][Xpos] = COLOR;
    }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
}
//--------------------------------------------------------------------------------
void screenmemory_drawpixel(int16_t X, int16_t Y, uint8_t COLOR) {
  if (Y >= 0 && Y < SCREEN_HEIGHT && X >= 0 && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = COLOR;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void screenmemory_drawpixel_static(uint16_t X, uint16_t Y, uint16_t COLOR) {
  #if LCD_ENABLED
  if (Y < SCREEN_HEIGHT && X < SCREEN_WIDTH) lcd.drawPixel(X, Y, COLOR); //draw 1 pixel into LCD
  #endif
  uint8_t R = (COLOR & 0b1111100000000000) >> (11 + 2);
  uint8_t G = (COLOR & 0b0000011111100000) >> (6 + 3);
  uint8_t B = (COLOR & 0b0000000000011111) >> (0 + 2);
  //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
  uint8_t c = ((G << 6) | (R << 3) | (B << 0)); //RGB
  if (Y < SCREEN_HEIGHT && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = c; //draw 1 pixel into MEMORY
}
//--------------------------------------------------------------------------------
void screenmemory_line(int startx, int starty, int endx, int endy, uint8_t color) {
  int t, distance;
  int xerr = 0, yerr = 0, delta_x, delta_y;
  int incx, incy;
  // compute the distances in both directions
  delta_x = endx - startx;
  delta_y = endy - starty;
  // Compute the direction of the increment, an increment of 0 means either a horizontal or vertical line.
  if (delta_x > 0) incx = 1;
  else if (delta_x == 0) incx = 0;
  else incx = -1;

  if (delta_y > 0) incy = 1;
  else if (delta_y == 0) incy = 0;
  else incy = -1;

  // determine which distance is greater
  delta_x = abs(delta_x);
  delta_y = abs(delta_y);
  if (delta_x > delta_y) distance = delta_x;
  else distance = delta_y;

  // draw the line
  for (t = 0; t <= distance + 1; t++) {
    screenmemory_drawpixel(startx, starty, color);

    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      startx += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      starty += incy;
    }
  }
}
//--------------------------------------------------------------------------------
void screenmemory_line_static(int startx, int starty, int endx, int endy, uint16_t color) {
  int t, distance;
  int xerr = 0, yerr = 0, delta_x, delta_y;
  int incx, incy;
  // compute the distances in both directions
  delta_x = endx - startx;
  delta_y = endy - starty;
  // Compute the direction of the increment, an increment of 0 means either a horizontal or vertical line.
  if (delta_x > 0) incx = 1;
  else if (delta_x == 0) incx = 0;
  else incx = -1;

  if (delta_y > 0) incy = 1;
  else if (delta_y == 0) incy = 0;
  else incy = -1;

  // determine which distance is greater
  delta_x = abs(delta_x);
  delta_y = abs(delta_y);
  if (delta_x > delta_y) distance = delta_x;
  else distance = delta_y;

  // draw the line
  for (t = 0; t <= distance + 1; t++) {
    screenmemory_drawpixel_static(startx, starty, color);

    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      startx += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      starty += incy;
    }
  }
}
//--------------------------------------------------------------------------------
void screenmemory_drawrectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t COLOR) {
  screenmemory_line(X, Y, X + Width, Y, COLOR);
  screenmemory_line(X, Y, X, Y + Height, COLOR);
  screenmemory_line(X + Width, Y, X + Width, Y + Height, COLOR);
  screenmemory_line(X, Y + Height, X + Width, Y + Height, COLOR);
}
//--------------------------------------------------------------------------------
void screenmemory_drawrectangle_static(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint16_t COLOR) {
  screenmemory_line_static(X, Y, X + Width, Y, COLOR);
  screenmemory_line_static(X, Y, X, Y + Height, COLOR);
  screenmemory_line_static(X + Width, Y, X + Width, Y + Height, COLOR);
  screenmemory_line_static(X, Y + Height, X + Width, Y + Height, COLOR);
}
//--------------------------------------------------------------------------------
void screenmemory_drawfillrectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t COLOR) {
  for (uint16_t Ypos = Y; Ypos < Y + Height; Ypos++)
    for (uint16_t Xpos = X; Xpos < X + Width; Xpos++) {
      screenmemory_drawpixel(Xpos, Ypos, COLOR);
    }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex);
}
//--------------------------------------------------------------------------------
void screenmemory_drawfillrectangle_static(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint16_t COLOR) {
  uint8_t R = (COLOR & 0b1111100000000000) >> (11 + 2);
  uint8_t G = (COLOR & 0b0000011111100000) >> (6 + 3);
  uint8_t B = (COLOR & 0b0000000000011111) >> (0 + 2);
  //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
  uint8_t c = ((G << 6) | (R << 3) | (B << 0)); //RGB

  for (int16_t Ypos = Y; Ypos < Y + Height; Ypos++)
    for (int16_t Xpos = X; Xpos < X + Width; Xpos++) {
      screenmemory_drawpixel(Xpos, Ypos, c);
    }
  #if LCD_ENABLED
  lcd.fillRect(X, Y, Width, Height, COLOR);
  #endif
}
//--------------------------------------------------------------------------------
void screenmemory_fillscreen_static(uint16_t COLOR) {
  screenmemory_drawfillrectangle_static(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint8_t color = 255, uint8_t bgcolor = 0) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel(Main_x + Xpos + 0, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 0, Main_y + Ypos, bgcolor);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel(Main_x + Xpos + 1, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 1, Main_y + Ypos, bgcolor);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel(Main_x + Xpos + 2, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 2, Main_y + Ypos, bgcolor);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel(Main_x + Xpos + 3, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 3, Main_y + Ypos, bgcolor);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel(Main_x + Xpos + 4, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 4, Main_y + Ypos, bgcolor);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel(Main_x + Xpos + 5, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 5, Main_y + Ypos, bgcolor);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel(Main_x + Xpos + 6, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 6, Main_y + Ypos, bgcolor);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel(Main_x + Xpos + 7, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 7, Main_y + Ypos, bgcolor);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------//--------------------------------------------------------------------------------
uint8_t draw_char_xy_double(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint8_t color = 255, uint8_t bgcolor = 0) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;

        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2 + 1, color);
        } else {
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2 + 1, bgcolor);
          screenmemory_drawpixel(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2 + 1, bgcolor);
        }
      }
  return (XcharSize * 2);
}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_static(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint16_t color = 0xffff, uint16_t bgcolor = 0) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, bgcolor);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, bgcolor);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, bgcolor);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, bgcolor);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, bgcolor);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, bgcolor);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, bgcolor);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, bgcolor);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_static_transparent(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint16_t color = 0xffff) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, color);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, color);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, color);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, color);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, color);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, color);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, color);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, color);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_transparent_double(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint32_t color) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos += 1)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 0, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 0 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 2, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 2 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 4, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 4 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 6, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 6 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 8, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 8 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 10, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 10 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 12, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 12 + 1, Main_y + Ypos * 2 + 1, color);
        }
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) {
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 14, Main_y + Ypos * 2 + 1, color);
          screenmemory_drawpixel_static(Main_x + Xpos * 2 + 14 + 1, Main_y + Ypos * 2 + 1, color);
        }
      }
  return XcharSize * 2;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
uint16_t draw_string_xy(uint16_t x, uint16_t y,
  const char * c,
    const char * font, uint8_t color = 0xff,
      uint8_t bgcolor = 0) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while ( * c) {
    width = draw_char_xy(x, y, * c, font, color, bgcolor);
    textwidth += (width);
    x += (width);
    c++;
  }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex);
  return textwidth;
}
//--------------------------------------------------------------------------------
uint16_t draw_string_xy_double(uint16_t x, uint16_t y,
  const char * c,
    const char * font, uint8_t color = 0xff,
      uint8_t bgcolor = 0) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while ( * c) {
    width = draw_char_xy_double(x, y, * c, font, color, bgcolor);
    textwidth += (width);
    x += (width);
    c++;
  }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex);
  return textwidth;
}
//--------------------------------------------------------------------------------
uint16_t draw_string_xy_static(uint16_t x, uint16_t y,
  const char * c,
    const char * font, uint16_t color = 0xffff,
      uint16_t bgcolor = 0) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while ( * c) {
    width = draw_char_xy_static(x, y, * c, font, color, bgcolor);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}

//--------------------------------------------------------------------------------
uint16_t draw_string_xy_transparent(uint16_t x, uint16_t y,
  const char * c,
    const char * font, uint16_t color = 0xffff) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while ( * c) {
    width = draw_char_xy_static_transparent(x, y, * c, font, color);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}
//--------------------------------------------------------------------------------
uint16_t draw_string_xy_transparent_double(uint16_t x, uint16_t y,
  const char * c,
    const char * font, uint16_t color = 0xffff) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while ( * c) {
    width = draw_char_xy_transparent_double(x, y, * c, font, color);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}
//--------------------------------------------------------------------------------
const char * DisplayFontSet = NULL;
uint16_t XPOS_CHAR = 0;
uint16_t YPOS_CHAR = 0;
//--------------------------------------------------------------------------------
void set_font(const char * font) {
  DisplayFontSet = font;
}
//--------------------------------------------------------------------------------
void set_font_XY(uint16_t x, uint16_t y) {
  XPOS_CHAR = x;
  YPOS_CHAR = y;
}
//--------------------------------------------------------------------------------
void draw_string(char * c, uint8_t color = 255, uint8_t bgcolor = 0) {
  if (c[strlen(c) - 1] == '\n') {
    draw_string_xy(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color);
    YPOS_CHAR += (uint8_t)(DisplayFontSet[1]);
    XPOS_CHAR = 0;
  } else {
    XPOS_CHAR += draw_string_xy(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color, bgcolor);
  }
}

//--------------------------------------------------------------------------------
void draw_string_static(char * c, uint16_t color = 0xffff, uint16_t bgcolor = 0) {
  if (c[strlen(c) - 1] == '\n') {
    draw_string_xy_static(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color);
    YPOS_CHAR += (uint8_t)(DisplayFontSet[1]);
    XPOS_CHAR = 0;
  } else {
    XPOS_CHAR += draw_string_xy_static(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color, bgcolor);
  }
}

//--------------------------------------------------------------------------------

#if JPG_DECODER
//********************************************************************************
// JPG RENDER FUNCTIONS
//********************************************************************************
uint32_t dataIdx = 0; //actual input buffer read index
uint32_t dataLen = 0; //size of input data
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint32_t loadMem(JDEC * jdec, uint8_t * buf, uint32_t len) {
  vPortYield();
  if (dataIdx + len > dataLen) len = dataLen - dataIdx;
  if (buf) memcpy(buf, JPGDATA + dataIdx, len);
  //if (buf) printf("JPG BUF %d: %d\n", dataIdx, ((uint8_t*)buf)[dataIdx]);
  dataIdx += len;
  return len;
}
//--------------------------------------------------------------------------------
uint32_t renderindex = 0;
int16_t jpgXPOS = 0;
int16_t jpgYPOS = 0;
int16_t jpgtmpX = 0;
int16_t jpgtmpY = 0;
//--------------------------------------------------------------------------------
uint8_t jpg_Red;
uint8_t jpg_Green;
uint8_t jpg_Blue;
uint16_t jpg_COLOR;
//********************************************************************************
int renderLCD(JDEC * jdec, uint16_t * bitmap, JRECT * jrect) {
  // uint16_t jpgwidth = jdec->width;
  // uint16_t jpgheight = jdec->height;
  uint16_t x = jrect -> left + 0;
  uint16_t y = jrect -> top + 0;
  uint16_t w = jrect -> right + 1 - jrect -> left;
  uint16_t h = jrect -> bottom + 1 - jrect -> top;
  #if DEBUG > 2
  printf("JPG renderindex: %d\n", renderindex);
  #endif
  for (jpgtmpY = 0; jpgtmpY < h; jpgtmpY++) {
    for (jpgtmpX = 0; jpgtmpX < w; jpgtmpX++) {
      jpg_Red = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 0];
      jpg_Green = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 1];
      jpg_Blue = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 2];

      jpg_Red = (jpg_Red & 0b11111000) >> 3;
      jpg_Green = (jpg_Green & 0b11111000) >> 3;
      jpg_Blue = (jpg_Blue & 0b11111000) >> 3;

      jpg_COLOR = ((jpg_Red << 11) | (jpg_Green << 6) | (jpg_Blue << 0)); //RGB

      SCREENMEMORY_LINE[jpgYPOS + y + jpgtmpY][jpgXPOS + x + jpgtmpX] = (((jpg_Green >> 3) << 6) | ((jpg_Red >> 2) << 3) | ((jpg_Blue >> 3) << 0)); //RGB 256colors
      ((uint16_t * ) SCREENBUFFER)[jpgtmpX % 512] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8); //max 512 WORDS pre line
    }
    if (w < 512) {
      //schema: 0bGGBBBBBRRRRRGGG, R=RED, G=GREEN, B=BLUE
      #if LCD_ENABLED
      lcd.pushImage(x + jpgXPOS, y + jpgYPOS + jpgtmpY, w, 1, &SCREENBUFFER[0]); //draw line
      #endif
    }
  }
  renderindex++;
  return 1;
}
//--------------------------------------------------------------------------------
void renderJPG(int XPOS, int YPOS,
  const uint8_t * inputdata, uint32_t inputdataLen, uint8_t * decodeBuffer, uint32_t decodeBufferLen, int jpgScale) {
  dataIdx = 0;
  dataLen = inputdataLen;
  jpgXPOS = XPOS;
  jpgYPOS = YPOS;

  JPGDATA = (uint8_t * ) inputdata;

  DRAWMODE = 0; //this is important!

  jpgres = jd_prepare( &jpgdec, loadMem, decodeBuffer, decodeBufferLen, 0);
  #if DEBUG > 2
  printf("JPG PREPARE STATUS: %d\n", jpgres);
  #endif
  if (jpgres == JDR_OK) jpgres = jd_decomp( &jpgdec, renderLCD, jpgScale);
  #if DEBUG > 2
  printf("JPG DECODE STATUS: %d\n", jpgres);
  #endif
}
//--------------------------------------------------------------------------------
uint16_t * tmpJPGRenderbuffer;
int renderBUFFER(JDEC * jdec, uint16_t * bitmap, JRECT * jrect) {
  uint16_t x = jrect -> left + 0;
  uint16_t y = jrect -> top + 0;
  uint16_t w = jrect -> right + 1 - jrect -> left;
  uint16_t h = jrect -> bottom + 1 - jrect -> top;

  DRAWMODE = 0; //this is important!

  #if DEBUG > 2
  printf("JPG renderindex: %d\n", renderindex);
  #endif
  for (jpgtmpY = 0; jpgtmpY < h; jpgtmpY++) {
    for (jpgtmpX = 0; jpgtmpX < w; jpgtmpX++) {

      jpg_Red = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 0];
      jpg_Green = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 1];
      jpg_Blue = ((uint8_t * ) bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 2];

      jpg_Red = (jpg_Red & 0b11111000) >> 3;
      jpg_Green = (jpg_Green & 0b11111000) >> 3;
      jpg_Blue = (jpg_Blue & 0b11111000) >> 3;

      jpg_COLOR = ((jpg_Red << 11) | (jpg_Green << 6) | (jpg_Blue << 0)); //RGB
      SCREENMEMORY_LINE[jpgYPOS + y + jpgtmpY][jpgXPOS + x + jpgtmpX] = (((jpg_Green >> 3) << 6) | ((jpg_Red >> 2) << 3) | ((jpg_Blue >> 2) << 0)); //RGB
      if (tmpJPGRenderbuffer) { //save to BUFFER pixel
        ((uint16_t * ) tmpJPGRenderbuffer)[(jpgYPOS + y + jpgtmpY) * LCD_WIDTH + (jpgXPOS + x + jpgtmpX)] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8);
      }
      ((uint16_t * ) SCREENBUFFER)[jpgtmpX % 512] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8); //max 512 WORDS pre line
    }
    if (w < 512) {
      //schema: 0bGGBBBBBRRRRRGGG, R=RED, G=GREEN, B=BLUE
      #if LCD_ENABLED
      lcd.pushImage(x + jpgXPOS, y + jpgYPOS + jpgtmpY, w, 1, &SCREENBUFFER[0]); //draw line
      #endif
    }
  }
  renderindex++;
  return 1;
}
//----------------------------------------------------------------------------------------
void renderJPG_toBUFFER(uint16_t * renderJPGbuf, int XPOS, int YPOS,
  const uint8_t * inputdata, uint32_t inputdataLen, uint8_t * decodeBuffer, uint32_t decodeBufferLen, int jpgScale) {
  dataIdx = 0;
  dataLen = inputdataLen;
  jpgXPOS = XPOS;
  jpgYPOS = YPOS;

  JPGDATA = (uint8_t * ) inputdata;

  DRAWMODE = 0; //this is important!

  tmpJPGRenderbuffer = renderJPGbuf; //set pointer to output buffer

  jpgres = jd_prepare( &jpgdec, loadMem, decodeBuffer, decodeBufferLen, 0);
  #if DEBUG > 2
  printf("JPG PREPARE STATUS: %d\n", jpgres);
  #endif
  if (jpgres == JDR_OK) {
    #if DEBUG > 2
    printf("jpgdec.width: %d jpgdec.height: %d\n", jpgdec.width, jpgdec.height);
    #endif

    if (jpgdec.height >= LCD_HEIGHT) jpgdec.height = LCD_HEIGHT;
    if (jpgdec.width >= LCD_WIDTH) jpgdec.width = LCD_WIDTH;
    jpgres = jd_decomp( &jpgdec, renderBUFFER, jpgScale);
  }
  #if DEBUG > 2
  printf("JPG DECODE STATUS: %d\n", jpgres);
  #endif
}
//********************************************************************************
#endif

//--------------------------------------------------------------------------------
void animateTEXT(int offset_direction, int stopXpos, int XPOS, int YPOS, char * TEXT, uint16_t COLOR) {
  DRAWMODE = 0; //this is important!
  int16_t XPOS_save = XPOS;
  int16_t XoffsetINC = offset_direction; //-8 to left //+8 to right
  for (int16_t animateX = XPOS_save;
    ((animateX <= stopXpos && XoffsetINC > 0) || (animateX >= stopXpos && XoffsetINC < 0)) && (animateX >= 0 - LCD_WIDTH) && (animateX <= LCD_WIDTH * 2); animateX += XoffsetINC) {
    vPortYield();
    #if DEBUG > 2
    printf("animateX:%d\n", animateX);
    #endif
    XPOS = animateX;
    screenmemory_drawfillrectangle_static(XPOS - 8, YPOS, strlen(TEXT) * 4 + 16, 16, 0);
    draw_string_xy_transparent(XPOS, YPOS, (char * ) TEXT, font_CP437_4x8, COLOR);
    delay(40);
  }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void animateJPG(int offset_direction, int stopXpos, int XPOS, int YPOS, const uint8_t * inputdata, uint32_t inputdataLen, uint8_t * decodeBuffer, uint32_t decodeBufferLen, uint16_t transparentCOLOR) {

  DRAWMODE = 0; //this is important!

  uint16_t WIDTH = 80;
  uint16_t HEIGHT = 80;

  int16_t XPOS_save = XPOS;
  int16_t XoffsetINC = offset_direction; //-8 to left //+8 to right

  for (int16_t animateX = XPOS_save;((animateX <= stopXpos && XoffsetINC > 0) || (animateX >= stopXpos && XoffsetINC < 0)) && (animateX >= 0 - LCD_WIDTH) && (animateX <= LCD_WIDTH * 2); animateX += XoffsetINC) {
    vPortYield();
    #if DEBUG > 2
    printf("animateX:%d\n", animateX);
    #endif
    XPOS = animateX;

    renderJPG_toBUFFER(NULL, XPOS, YPOS, inputdata, inputdataLen, JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); 
  }
}
//--------------------------------------------------------------------------------
