uint8_t LCD_BACKLIGHT = 128; //0..255
uint8_t TIME_COUNTER_BREAK = 0;
//================================================================================
uint8_t INDICATOR_MODE = 1; //0=disabled 1=VOLUME 2=BACKGLIGHT 
uint32_t VOLUME_INDICATOR_TIMEOUT = 0;

uint8_t UPDATE_VOLUME_INDICATOR(uint8_t LEVEL) {
  screenmemory_drawfillrectangle_static(106, 24, 10 + 1, 80 + 1, 0);
  screenmemory_drawrectangle_static(106, 23, 10, 80, 0xffff);
  screenmemory_drawfillrectangle_static(108, 24 + 80 - (LEVEL * 5 / 16), 7, (LEVEL * 5 / 16), 0b0000011111000000);
  ///UPDATE_STATUS_BAR();
  VOLUME_INDICATOR_TIMEOUT = millis();
  return 1;
}
uint8_t HIDE_VOLUME_INDICATOR() {
  screenmemory_drawfillrectangle_static(106, 23, 10 + 1, 80 + 2, 0);
  VOLUME_INDICATOR_TIMEOUT = 0;
  return 1;
}
//================================================================================

//================================================================================
uint8_t wakeup_LCD() {
  #if LCD_BACKLIGHT_CONTROL
  #if AUTO_POWER_SAVER
  if (LCD_BACKLIGHT == 0) {
    if (DEBUG > 1) printf("WAKING UP lcd display...\n");
    LCD_BACKLIGHT = BACKLIGHT_PIN_LAST_VALUE;
    BACKLIGHT_PIN_UPDATE(LCD_BACKLIGHT);
    BACKLIGHT_PIN_LAST_VALUE = 0;
  }
  #endif
  #endif
}
//================================================================================

//================================================================================
uint32_t BACKLIGHT_INDICATOR_TIMEOUT = 0;
uint8_t UPDATE_BACKLIGHT_INDICATOR(uint8_t LEVEL) {
  screenmemory_drawfillrectangle_static(106, 24, 10 + 1, 80 + 1, 0);
  screenmemory_drawrectangle_static(106, 24, 10, 80, 0xffff);
  screenmemory_drawfillrectangle_static(108, 24 + 80 - (LEVEL * 5 / 16), 7, (LEVEL * 5 / 16), 0b0000000000011111); //BLUE
  BACKLIGHT_PIN_UPDATE(LCD_BACKLIGHT); // backlight level was changed...
  BACKLIGHT_INDICATOR_TIMEOUT = millis();
  return 1;
}
uint8_t HIDE_BACKLIGHT_INDICATOR() {
  ///RESTORE_BACKGROUND(212, 48, 20 + 1, 160 + 1);
  screenmemory_drawfillrectangle_static(106, 23, 10 + 1, 80 + 2, 0);
  BACKLIGHT_INDICATOR_TIMEOUT = 0;
  return 1;
}
//================================================================================

uint16_t loadedFileNames = 0;
uint16_t onlyFileNames = 0;
uint16_t SELECTED_FILE_POS = 0;
uint8_t LOADED_ROM = 0;

uint8_t MAX_MENU_ITEMS = 2; //PRO VERSION HAS 4 apps but this is LITE version

//********************************************************************************
//   FILE EXPLORE & BROWSE
//********************************************************************************
uint8_t EXIT = 0; //0=No Exit, 1=Exit from Browse, 2=Exit to Main Menu

#define MAXFILES 512
#define MAXFILENAME_LENGTH 64
char * filename[MAXFILES];
char fileext[4];
#define FILESPERPAGE 8
//--------------------------------------------------------------------------------
void sortStrings(char * arr[], int n) {
  char temp[MAXFILENAME_LENGTH];

  // Sorting strings using bubble sort
  for (int j = 0; j < n - 1; j++) {
    for (int i = j + 1; i < n; i++) {
      if (strcmp(arr[j], arr[i]) > 0) {
        strcpy(temp, arr[j]);
        strcpy(arr[j], arr[i]);
        strcpy(arr[i], temp);
      }
    }
  }
}
//--------------------------------------------------------------------------------
void secondsToHMS(const uint32_t seconds, uint16_t &h, uint8_t &m, uint8_t &s) {
  uint32_t t = seconds;
  s = t % 60;
  t = (t - s) / 60;
  m = t % 60;
  t = (t - m) / 60;
  h = t;
}
//--------------------------------------------------------------------------------

#if MICROSD
SdFile dirFile;
SdFile file;
#endif

//################################################################################
char * EXPLORE_MICROSD(uint16_t TYPE, char * PATH, char * MESSAGE) {
  #if MICROSD
  uint8_t num = 0;
  uint8_t loadedFileNames = 0;

  onlyFileNames = 0;
  SELECTED_FILE_POS = 0;

  SDCARD_BUSY = 1;

  //clear memory variables
  for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) memset(filename[tmp], 0, sizeof(filename[tmp]));
  fileext[0] = 0;
  fileext[1] = 0;
  fileext[2] = 0;
  fileext[3] = 0;

  num = 0;
  loadedFileNames = 0;

  //----------------------------------------
  //LOAD FILENAMES INTO MEMORY...
  //----------------------------------------
  if (!dirFile.open(PATH, O_READ)) {
    printf("PATH NOT FOUND: %s\n", PATH);
    screenmemory_fillscreen(0); //black color
    set_font_XY(32, 64);
    draw_string((char * )
      "PATH NOT FOUND: ", 255);
    set_font_XY(32, 64 + 20);
    draw_string(PATH, 255);
    //while (1) {} //HALT.
    //----------------------------------------
    //SWITCH SOURCE     
    INPUTCLEAR();
    delay(1000);
    while (!INPUTREAD()) {
      INPUTCLEAR();
    }
    delay(50);
    if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
      INPUTCLEAR();
      EXIT = 2;
      return MAINPATH;
    }
    if (LOADED_ROM == 1) LOADED_ROM = 11; //USB STORAGE LOAD
    else if (LOADED_ROM == 11) LOADED_ROM = 1; //SD CARD LOAD

    EXIT = 1;
    return MAINPATH;
    //----------------------------------------    
  }
  while (num < MAXFILES && file.openNext( &dirFile, O_READ)) {
    // Skip hidden files.
    if (!file.isHidden()) {
      file.getName(filename[num], MAXFILENAME_LENGTH);
      printf("%s\n", filename[num]);
      if (file.isSubDir()) {
        sprintf(filename[num], "%s/", filename[num]);
        num++;
      } else {
        for (uint8_t i = strlen(filename[num]); i > 3; i--) {
          if (filename[num][i] != 0) {
            fileext[3] = '\0';
            fileext[2] = filename[num][i];
            fileext[1] = filename[num][i - 1];
            fileext[0] = filename[num][i - 2];
            break;
          }
        }
        //................................................................................
        if (TYPE == APP_EMU_NES) {
          //check NES ROM IMAGES File extension, then increase index
          if (((fileext[0] == 'N' || fileext[0] == 'n') &&
              (fileext[1] == 'E' || fileext[1] == 'e') &&
              (fileext[2] == 'S' || fileext[2] == 's'))) {
            num++;
            onlyFileNames++;
          }
        }
        //................................................................................      
        if (TYPE == APP_EMU_GBC) {
          //check GBC ROM IMAGES File extension, then increase index
          if ((((fileext[0] == 'G' || fileext[0] == 'g') &&
              (fileext[1] == 'B' || fileext[1] == 'b') &&
              (fileext[2] == 'C' || fileext[2] == 'c'))) ||
            (((fileext[1] == 'G' || fileext[1] == 'g') &&
              (fileext[2] == 'B' || fileext[2] == 'b')
              ///&& (fileext[2] == 'C' || fileext[2] == 'c')
            ))) {
            num++;
            onlyFileNames++;
          }
        }
        //................................................................................       

        //................................................................................
        if (TYPE == APP_AUD_PLAYER) {
          //check MP3 File extension, then increase index
          if ((fileext[0] == 'M' || fileext[0] == 'm') &&
            (fileext[1] == 'P' || fileext[1] == 'p') &&
            (fileext[2] == '3' || fileext[2] == '3')) {
            num++;
            onlyFileNames++;
          }
          if ((fileext[0] == 'W' || fileext[0] == 'w') &&
            (fileext[1] == 'A' || fileext[1] == 'a') &&
            (fileext[2] == 'V' || fileext[2] == 'v')) {
            num++;
            onlyFileNames++;
          }
        }
        //................................................................................
      }
    }
    loadedFileNames = num;
    file.close();
  }
  dirFile.close();
  #ifdef DEBUG
  printf("--------------------------------------\n");
  printf("Count of loaded File Names: %d", loadedFileNames);
  #endif
  sortStrings(filename, loadedFileNames);

  //DRAW FILENAMES INTO BUFFER
  uint8_t CURSOR = 0;
  uint8_t PAGE = 0;
  bool NamesDisplayed = false;

  while (1) {
    vPortYield(); //we are in infinite loop must yield();
    delay(1);
    PAGE = CURSOR / FILESPERPAGE;
    if (!NamesDisplayed) {
      screenmemory_fillscreen(0); //black color
      set_font_XY(8, 6);
      draw_string(MESSAGE, 0, 96);
      set_font_XY(8, 18);
      draw_string(PATH, 96);

      for (num = PAGE * FILESPERPAGE; num < ((PAGE + 1) * FILESPERPAGE) && num < loadedFileNames; num++) {
        set_font_XY(20, 30 + 10 * (num % FILESPERPAGE));

        if (filename[num][strlen(filename[num]) - 1] == '/') draw_string(filename[num], 23);
        else draw_string(filename[num], 255);
        if (DEBUG > 3) {
          printf("%s, %d\n", filename[num], strlen(filename[num]));
        }
        delay(1);
      }
      NamesDisplayed = true;
    }

    //Draw Cursor
    set_font_XY(0, 30 + (10 * (CURSOR % FILESPERPAGE)));
    draw_string((char * )
      "->", 255);

    if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
    delay(200); //ms

    //PROCESS CURSOR SELECTION
    INPUTCLEAR();
    while (!INPUTREAD()) {
      INPUTCLEAR();
      delay(1);
    };

    //Empty Cursor
    set_font_XY(0, 30 + (10 * (CURSOR % FILESPERPAGE)));
    draw_string((char * )
      "  ", 0);

    if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
      INPUTCLEAR();
      EXIT = 2;
      return MAINPATH;
    }

    if (INPUT_UP == 1) {
      if (CURSOR % FILESPERPAGE == 0) NamesDisplayed = false; //changed page
      if (CURSOR == 0 && loadedFileNames > 0) CURSOR = loadedFileNames - 1;
      else if (CURSOR > 0 && loadedFileNames > 0) CURSOR--;

      while (INPUTREAD()) {
        INPUTCLEAR();
      }
    }
    if (INPUT_DOWN == 1) {
      if (CURSOR % FILESPERPAGE == FILESPERPAGE - 1 || CURSOR == loadedFileNames - 1) NamesDisplayed = false; //changed page
      if (CURSOR == loadedFileNames - 1 && loadedFileNames > 0) CURSOR = 0;
      else if (CURSOR < loadedFileNames - 1 && loadedFileNames > 0) CURSOR++;

      while (INPUTREAD()) {
        INPUTCLEAR();
      }
    }
    if (INPUT_LEFT) {
      if (CURSOR > FILESPERPAGE - 1) CURSOR -= FILESPERPAGE;
      NamesDisplayed = false;
      while (INPUTREAD()) {
        INPUTCLEAR();
      }
    }
    if (INPUT_RIGHT == 1) {
      if (CURSOR / FILESPERPAGE < loadedFileNames / FILESPERPAGE) CURSOR += FILESPERPAGE;
      if (CURSOR > loadedFileNames - 1) CURSOR = loadedFileNames - 1;
      NamesDisplayed = false;

      while (INPUTREAD()) {
        INPUTCLEAR();
      }
    }
    if (INPUT_SELECT == 1) {
      //SWITCH SOURCE     
      while (INPUTREAD()) {
        INPUTCLEAR();
      }
      delay(50);

      if (POWER == APP_AUD_PLAYER) continue; /// DISABLE SELECT SWITCH !

      if (LOADED_ROM == 1) LOADED_ROM = 11; //USB STORAGE LOAD
      else if (LOADED_ROM == 11) LOADED_ROM = 1; //SD CARD LOAD

      EXIT = 1;
      return MAINPATH;
    }
    if (loadedFileNames > 0) { ///cannot select NOTHING!
      if ((INPUT_A == 1 || INPUT_B == 1) && INPUT_SELECT == 0) {
        dirFile.close();

        while (INPUTREAD()) {
          INPUTCLEAR();
        }
        delay(50);

        sprintf(MAINPATH, "%s%s", PATH, filename[CURSOR]);
        SELECTED_FILE_POS = CURSOR;
        sprintf(TRACKNAME, "%s", filename[CURSOR]); //load FILENAME
        return MAINPATH; //START //A
      }
    }
    if (((INPUT_START == 1) && INPUT_SELECT == 0)) {
      while (INPUTREAD()) {
        INPUTCLEAR();
      }
      dirFile.close();
      delay(25);

      sprintf(MAINPATH, "%s", PATH);

      if (strlen(MAINPATH) > 1) {
        MAINPATH[strlen(MAINPATH) - 1] = '\0';
        for (uint8_t strpos = strlen(MAINPATH) - 1; strpos > 0; strpos--) {
          if (MAINPATH[strpos] == '/') break;
          MAINPATH[strpos] = '\0';
        }
      }
      return MAINPATH;
    }

    if (INPUT_SYSTEM == 1) { ///"SYSTEM button" pressed = EXIT
      EXIT = 1;
      POWER = 0;
    }
  }
  delay(300);
  #endif
}
//################################################################################
char * Browse_MICROSD(uint16_t TYPE, char * PATH, char * MESSAGE) {
  #if MICROSD
  SDCARD_BUSY = 1;

  #ifdef DEBUG
  printf("microSD: Browsing in PATH: %s\n", PATH);
  #endif

  if (PATH[strlen(PATH) - 1] != '/')
    if (strlen(PATH) > 1) {
      PATH[strlen(PATH) - 1] = '\0';
      for (uint8_t strpos = strlen(PATH) - 1; strpos > 0; strpos--) {
        if (PATH[strpos] == '/') break;
        PATH[strpos] = '\0';
      }
    }

  EXIT = 0;
  //................................................................................
  if (!filename[0]) {
    printf(">>> alloc filenames\n");
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char * ) malloc(MAXFILENAME_LENGTH + 1); //temporary malloc filenamebuffer
  }

  while (EXIT == 0 && PATH[strlen(PATH) - 1] == '/') {
    PATH = EXPLORE_MICROSD(TYPE, PATH, MESSAGE);
    printf("%s\n", PATH);
  }

  if (EXIT == 0) LOADED_ROM = 2; //SD CARD LOAD SELECTED

  if (filename[0]) {
    printf(">>> free filenames\n");

    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
      free(filename[tmp]); //free temporary malloc filenamebuffer
      filename[tmp] = NULL;
    }
  }
  SDCARD_BUSY = 0;

  //................................................................................
  return PATH;

  #else //NoMicroSD
  LOADED_ROM = 11; //USB LOAD
  #endif

}
//________________________________________________________________________________

//********************************************************************************
//
//********************************************************************************
uint32_t WALLPAPERDATA_SIZE = 0;
uint8_t * WALLPAPER_DATA;

void MENU_TASK_START(int8_t STARTMENU) {
  #if (DEBUG>0)
  printf("Starting MAIN MENU...");
  #endif
  screenmemory_fillscreen(0); //black color

  if (MENU_ITEM == 0) {
    renderJPG_toBUFFER(NULL, (128 - 80) / 2, (128 - 80) / 2, JPG_MP3_ICON, sizeof(JPG_MP3_ICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
  }
  //render START ICON of MENU
  if (MENU_ITEM == 1) {
    renderJPG_toBUFFER(NULL, (128 - 80) / 2, (128 - 80) / 2, JPG_NES_ICON, sizeof(JPG_NES_ICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
  }
  if (MENU_ITEM == 2) {
    renderJPG_toBUFFER(NULL, (128 - 80) / 2, (128 - 80) / 2, JPG_GBC_ICON, sizeof(JPG_GBC_ICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
  }
  if (MENU_ITEM == 3) {
    renderJPG_toBUFFER(NULL, (128 - 80) / 2, (128 - 80) / 2, JPG_WOLF3D_ICON, sizeof(JPG_WOLF3D_ICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
  }
  MENU_ACTIVE = 1;
  #if (DEBUG>0)
  printf("[ OK ]\n");
  #endif
}

//--------------------------------------------------------------------------------
int8_t MENU_TASK(int8_t STARTMENU) {
  if (MENU_ACTIVE == 0) MENU_TASK_START(STARTMENU);
  if (MENU_ACTIVE != 1) return 0;
  INPUTCLEAR();

  vPortYield();
  delay(1); ///we are in infinite loop so yielding
  INPUTREAD();
  if (INDICATOR_MODE > 0 && VOLUME_INDICATOR_TIMEOUT > 0 && VOLUME_INDICATOR_TIMEOUT + 3000 < millis()) HIDE_VOLUME_INDICATOR();
  else if (INDICATOR_MODE > 0 && BACKLIGHT_INDICATOR_TIMEOUT > 0 && BACKLIGHT_INDICATOR_TIMEOUT + 3000 < millis()) HIDE_BACKLIGHT_INDICATOR();
  //--------------------------------------------------------------------------------
  if (INPUT_RIGHT == 1) {

    if (INDICATOR_MODE == 1) HIDE_VOLUME_INDICATOR();
    else if (INDICATOR_MODE == 2) HIDE_BACKLIGHT_INDICATOR();
    //................................................................................
    MENU_ITEM++;
    if (MENU_ITEM > MAX_MENU_ITEMS) MENU_ITEM = 0;
    if (MENU_ITEM == 0) {
      animateJPG(-4, -128 + 40, (128 - 88) / 2, (128 - 80) / 2, JPG_NES_ICON, sizeof(JPG_NES_ICON), NULL, 0, 0x0000);
      animateJPG(-4, +20, 128 - 44 + (128 - 88) / 2, (128 - 80) / 2, JPG_MP3_ICON, sizeof(JPG_MP3_ICON), NULL, 0, 0x0000);
    }
    if (MENU_ITEM == 1) {
      animateJPG(-4, -128 + 40, (128 - 88) / 2, (128 - 80) / 2, JPG_MP3_ICON, sizeof(JPG_MP3_ICON), NULL, 0, 0x0000);
      animateJPG(-4, +20, 128 - 44 + (128 - 88) / 2, (128 - 80) / 2, JPG_NES_ICON, sizeof(JPG_NES_ICON), NULL, 0, 0x0000);
    }

  }
  //--------------------------------------------------------------------------------
  if (INPUT_LEFT == 1) {
    if (INDICATOR_MODE == 1) HIDE_VOLUME_INDICATOR();
    else if (INDICATOR_MODE == 2) HIDE_BACKLIGHT_INDICATOR();
    //................................................................................
    MENU_ITEM--;
    if (MENU_ITEM < 0) MENU_ITEM = MAX_MENU_ITEMS;

    if (MENU_ITEM == 1) {
      animateJPG(+4, 128 + 0, (128 - 88) / 2 + 4, (128 - 80) / 2, JPG_MP3_ICON, sizeof(JPG_MP3_ICON), NULL, 0, 0x0000);
      animateJPG(+4, +20, ((128 - 88) / 2) - 128, (128 - 80) / 2, JPG_NES_ICON, sizeof(JPG_NES_ICON), NULL, 0, 0x0000);
    }
    if (MENU_ITEM == 0) {
      animateJPG(+4, 128 + 0, (128 - 88) / 2 + 4, (128 - 80) / 2, JPG_NES_ICON, sizeof(JPG_NES_ICON), NULL, 0, 0x0000);
      animateJPG(+4, +20, ((128 - 88) / 2) - 128, (128 - 80) / 2, JPG_MP3_ICON, sizeof(JPG_MP3_ICON), NULL, 0, 0x0000);
    }
  }
  //--------------------------------------------------------------------------------

  if (INPUT_UP == 1) {
    delay(20);
    if (INDICATOR_MODE == 1) {
      if (VOLUME < 255 - 8) VOLUME += 8;
      else VOLUME = 255;
      UPDATE_VOLUME_INDICATOR(VOLUME);
      TIME_COUNTER_BREAK = 1;
    } else if (INDICATOR_MODE == 2) {
      if (LCD_BACKLIGHT < 255 - 8) LCD_BACKLIGHT += 8;
      else LCD_BACKLIGHT = 255;
      UPDATE_BACKLIGHT_INDICATOR(LCD_BACKLIGHT);
      TIME_COUNTER_BREAK = 1;
    }
  }
  //--------------------------------------------------------------------------------
  if (INPUT_DOWN == 1) {
    delay(20);
    if (INDICATOR_MODE == 1) {
      if (VOLUME > 8) VOLUME -= 8;
      else VOLUME = 0;
      UPDATE_VOLUME_INDICATOR(VOLUME);
      TIME_COUNTER_BREAK = 1;
    } else if (INDICATOR_MODE == 2) {
      if (LCD_BACKLIGHT > 8) LCD_BACKLIGHT -= 8;
      else LCD_BACKLIGHT = 0;
      UPDATE_BACKLIGHT_INDICATOR(LCD_BACKLIGHT);
      TIME_COUNTER_BREAK = 1;
    }
  }
  //--------------------------------------------------------------------------------
  if (INPUT_SELECT == 1) {
    if (INDICATOR_MODE == 1) INDICATOR_MODE = 2;
    else if (INDICATOR_MODE == 2) INDICATOR_MODE = 1;

    printf("INDICATOR_MODE: %d\n", INDICATOR_MODE);
    while (INPUTREAD()) {
      INPUTCLEAR();
    }
    TIME_COUNTER_BREAK = 1;
  }
  //--------------------------------------------------------------------------------

  if (INPUT_START == 1 || INPUT_A == 1 || INPUT_B == 1) {
    while (INPUTREAD()) {
      INPUTCLEAR();
    }
    MENU_ACTIVE = 2;
  }
  return 0;
}
//********************************************************************************

//================================================================================
#define WINMENU_CURSOR "->"
#define WINMENU_TEXT0 "RESUME"
#define WINMENU_TEXT1 "STATE SAVE"
#define WINMENU_TEXT2 "STATE LOAD"
#define WINMENU_TEXT3 "EXIT"

#define WINMENU_ITEMSCOUNT 4
uint8_t WINMENU_POSITION = 1;

char * STATESAVE_FILENAME;

int NES_state_save(char * filename);
int NES_state_load(char * filename);


uint8_t WM_fontcolor = 0xff;
uint8_t WM_fontbgcolor = 0x00;

//********************************************************************************
// WINDOW MENU FUNCTIONS
//********************************************************************************
void do_window_menu() {
  DRAWREADY = 1;
  WINMENU_POSITION = 1;
  #ifndef NCAT_NO_PSRAM
  STATESAVE_FILENAME = (char * ) ps_malloc(256); ///allocate memory 
  #else
  STATESAVE_FILENAME = (char * ) malloc(256); ///allocate memory 
  #endif
  memset(STATESAVE_FILENAME, '\0', sizeof(STATESAVE_FILENAME));
  uint16_t SCREEN_MENU_XOFFSET = 240;
  if (DRAWMODE == 1 || DRAWMODE == 4) {
    WM_fontcolor = 0xff;
    WM_fontbgcolor = 0b10100100;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2, 128, 112, 0b10100100); //default PAL //GGRR0BB0
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2, 128, 112, 0b11101101); //GGRR0BB0
  } else if (DRAWMODE == 2) {
    WM_fontcolor = 0x20;
    WM_fontbgcolor = 0x00;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 120) / 2, (240 - 112) / 2, 128, 112, 0x00); //NES PAL   
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 120) / 2, (240 - 112) / 2, 128, 112, 0x10);
  } else if (DRAWMODE == 3) {
    WM_fontcolor = 0x1f;
    WM_fontbgcolor = 0x1e;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2, 128, 112, WM_fontbgcolor); //SMS PAL
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2, 128, 112, WM_fontbgcolor);
  }
  draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (WINMENU_POSITION - 1) * 16, WINMENU_CURSOR, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 16 + 0, WINMENU_TEXT0, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 24 + 16, WINMENU_TEXT1, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 32 + 32, WINMENU_TEXT2, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 40 + 48, WINMENU_TEXT3, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  DRAWREADY = 1;
  while (WINMENU_POSITION != 255) {
    INPUTCLEAR();
    if (INPUTREAD()) {
      INPUTREPORT();
      if (INPUT_UP == 1) {
        if (WINMENU_POSITION > 1) WINMENU_POSITION--;
        else WINMENU_POSITION = WINMENU_ITEMSCOUNT;
      }
      if (INPUT_DOWN == 1) {
        if (WINMENU_POSITION < WINMENU_ITEMSCOUNT) WINMENU_POSITION++;
        else WINMENU_POSITION = 1;
      }
      for (uint8_t tmp = 1; tmp < WINMENU_ITEMSCOUNT + 1; tmp++) {
        if (tmp == WINMENU_POSITION)
          draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (tmp - 1) * (16 + 8), WINMENU_CURSOR, DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
        else
          draw_string_xy_double((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (tmp - 1) * (16 + 8), "  ", DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
      }
      if (INPUT_A || INPUT_B) {
        INPUTCLEAR();
        if (WINMENU_POSITION == 1) {
          //RESUME to GAME
          WINMENU_POSITION = 255;
        } else if (WINMENU_POSITION == 2) {
          #if MICROSD
          //STATE SAVE
          if (POWER == APP_EMU_NES) {
            strcpy(STATESAVE_FILENAME, "/NES/");
            strcpy( & STATESAVE_FILENAME[5], TRACKNAME);
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 4] = '.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 3] = 'S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 2] = 'A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 1] = 'V';
            printf("SAVE STATE FILENAME: %s\n", STATESAVE_FILENAME);
            #if NES_EMULATOR
            NES_state_save(STATESAVE_FILENAME);
            #endif
          } 
          #endif
          WINMENU_POSITION = 255;
        } else if (WINMENU_POSITION == 3) {
          //LOAD STATE
          #if MICROSD
          if (POWER == APP_EMU_NES) {
            strcpy(STATESAVE_FILENAME, "/NES/");
            strcpy( & STATESAVE_FILENAME[5], TRACKNAME);
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 4] = '.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 3] = 'S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 2] = 'A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME) - 1] = 'V';
            printf("LOAD STATE FILENAME: %s\n", STATESAVE_FILENAME);
            #if NES_EMULATOR
            NES_state_load(STATESAVE_FILENAME);
            #endif
          } 
          #endif
          WINMENU_POSITION = 255;
        } else if (WINMENU_POSITION == 4) {
          //EXIT GAME
          POWER = 0;
          WINMENU_POSITION = 255;
        }
      }
    }
    DRAWREADY = 1;
    xSemaphoreGive(draw_mutex); //refresh SOUND
    vPortYield(); ///
    delay(120); //200ms
  }
  free(STATESAVE_FILENAME);
}
