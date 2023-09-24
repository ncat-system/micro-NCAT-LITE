//                          ▄▀▀█▄                ▄██▀▀▄       
//                         ▐█░░ ▀██▄          ▄███▀░░░█       
//                         ▐█░░░░░▀█████████████▀▒░░░░█       
//                 ▄▄████████░░▒▒▒▒░██████████████░▒░░█       
//             ▄█████████████▄▒▒▒▒▒▄████████████████░▐        
//           ▄████████████████▄▄▄█████████  ▀████████▌        
//          ▐█████████▀▀▀▀▀█████████████▀    ▀███████▌        
//         ▐█▀███▀▀▄       ▐██████████▀        ▀█████▌        
//         ▓▌               ░████▀▀▀   ▀▄    ▄▀   ▀██         
//        ▄██              ▐ ▀▌                    ▄▀         
//       ▓███▒             ▐  ▀▓▄▒         ▄    ▒▄▀▀          
//       █████▒▒         ▒▒▌             ▀▀▒▀▀▀ ▀█            
//       ▓████████████████████▒▒         ▄ ▄▄ ▒▄▒▄█▄▄         
//     ▄▄███████████████████████████████████▀██████████▄▄     
//  ▄█▀▒▒▀▒▒▒▀█▓▀▒█▓▒▒▓█▀▒▒▀█▄▒▒▒▒█▄████▓▓▓███▒███▓▓████▓██   
// ▐█▒▒▒▒▒▒▒▀▒▄▒▀▒▄▄▒▒▀█▄▒▒▒▄█▒▓▄█████▓▓███████▓▓▒▒███▀▒▓▒▒██▄
//  ▀▀█▄▒▒▒▒▄▄▒▒▒▄▒▒█▄▒▒▒▒▀▒▒▒▒▒▄███▄██▀▒██▒██▒▓▓▓▓▓▓▓▓▒▄█▀▀  
//      ▀▀█▄▒▒██▒▒█████████▒▒▒▒██▀▄██▒▒▄▓▓▓▓▓▒▒▓▓▓▓▒▄█▀▀      
//          ▀▀████▓████▓████▒▒▒▒▒▒▒▄▒▀▄█▓██▒▒▓▓▒▄█▀▀          
//              ▀▀███████▀▀▀▒▒▄▄▀▀▒▄▓█▓▒▒▓█▒██▀▀              
//                  ▀███▀████▄▒▓▓█▌▒▒▓█▒██▀▀                  
//                      ▀██▒▒▒▒▓▓▓▓▒███▀                      
//                          ▀▀▀▀▀▀▀▀                          

//********************************************************************************
//*                                                                              *
//*   (micro) μNCAT SYSTEM 1.0 LITE VERSION for ESP32S3 by Nathalis              *
//*                                                                              *
//*   Programs: 1. MP3 player                                                    *
//*             2. NES EMULATOR                                                  *
//*                                                                              *
//*   Pro version programs:                                                      *
//*             3. GBC EMULATOR                                                  *
//*             4. WOLFENSTEIN 3D                                                *
//*                                                                              *
//*   Hardware: 1. LILYGO T-QT                                                   *
//*             2. LILYGO T-QT PRO                                               *
//*                                                                              *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Features:                                                                  *
//*   __________________________________________________________________________ *
//*                                                                              *
//*   - MicroSD card support                                                     *
//*   - 128*128 0.85" LCD GC9107 display                                         *
//*   - PWM AUDIO support (optional)                                             *
//*   - InGame Menu with State Save and State load options                       *
//*   - POWER SAVER (display off when inactivity)                                *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Only for personal & educational use!                                       *
//*   [GPL-3.0 License]                                                          *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Please take care on correct settings and connections of parts              *
//*   in schematics, it was tested many times but nothing guaranteed.            *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Recommended Arduino core for the ESP32 IDF 2.0.9                           *
//*                                                                              *
//*   LINK FOR ARDUINO IDE:                                                      *
//*   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   When you change code, you need create new save state file because          *
//*   it contains memory pointers and they have been changed... so old state     *
//*   load cause crash...                                                        *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   This is really huge code, so please have a patience to messy sections      *
//*   of code...                                                                 *
//*                                                                              *
//********************************************************************************

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_wifi.h"
#include "esp_bt.h"

#define DEBUG 1
#define MICROSD 2
#define NCAT_NO_PSRAM                   // Comment out when you are using T-QT PRO [ESP32-S3FN4R2]
#define LCD_ENABLED 1
#define SOUND_ENABLED 1
#define PWMAUDIO_ENABLED 1
#define LCD_BACKLIGHT_CONTROL 1
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define JPG_BUFFER_SIZE 8192

#define USB_HOST_ESP32S3 0
#define NES_CONTROLLER 1

#define NES_EMULATOR 1
#define GBC_EMULATOR 1

#define MAX_FLASH_FILESIZE 1048576     // max. 1MB files for load to FLASH
#define MAX_PSRAM_FILESIZE 2097152     // max. 2MB files for load to PSRAM

#define LCD_WIDTH 128                  // LCD real width
#define LCD_HEIGHT 128                 // LCD real height
#define SCREEN_WIDTH 320               // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256              // BUFFER MAX HEIGHT

#define IN_GAME_MENU 1                 

#include "CONFIG.h"
#include "src/LGFX/LovyanGFX.hpp"

//--------------------------------------------------------------------------------
uint8_t* JPGOUTPUT_BUFFER;              
//--------------------------------------------------------------------------------
uint8_t DRAWREADY = 0;                    // DRAW: 0=not ready, 1=ready, 2=busy
uint8_t AUDIOREADY = 0;                   // AUDIO: 0=not ready, 1=ready, 2=busy
//--------------------------------------------------------------------------------
SemaphoreHandle_t draw_mutex;             // Semaphore handle variable for DRAW
SemaphoreHandle_t audio_mutex;            // Semaphore handle variable for AUDIO
//--------------------------------------------------------------------------------
uint8_t VOLUME = 128;                      // Audio Volume 1..255, 0=not detected (OFF)
//--------------------------------------------------------------------------------
uint8_t* SCREENMEMORY;                    // 64kB (SCREEN_WIDTH=256)*(SCREEN_HEIGHT=256) bytes //do not change this!!!
uint8_t* SCREENMEMORY_LINE[SCREEN_HEIGHT]; // SCREEN_HEIGHT = 256 lines mapped to SCREENMEMORY.
uint16_t SCREENBUFFER[1024] = {0};        // 4kB ...
//--------------------------------------------------------------------------------
uint8_t DRAWMODE=0;
uint32_t AUDIO_TIMER = 0;
//--------------------------------------------------------------------------------
uint8_t SDCARD_BUSY = 0;                  // 0=SDCARD inactive, 1=SDCARD is BUSY
uint8_t USBSTORAGE_BUSY = 0;              // USB STORAGE: 0=not inited, 1=inited, 2=BUSY
//--------------------------------------------------------------------------------
uint32_t PSRAM_BASE_SIZE = 0;             // PSRAM BASE SIZE in bytes
//--------------------------------------------------------------------------------
int8_t MENU_POS = -1;                     // MENU POSITION: -1=inactive, 0..127 = active positions
int8_t MENU_ACTIVE = -2;                  // MENU REQUEST: -1=inactive, 0=activate request, 1=active
int8_t MENU_ITEM = 0;                     // SELECTED MENU ITEM
//--------------------------------------------------------------------------------
uint8_t BATTERY = 1;                      // 0..255 or 0=not detected
uint8_t BATTERY_METER_ACTIVE = 1;         // 0=inactive, 1=active (do not check battery level in APPLICATIONS)
//--------------------------------------------------------------------------------
char* MAINPATH;                           // malloc 256bytes
char* TEXTBUFFER;                         // malloc 256bytes
char* TRACKNAME;                          // malloc 256bytes
//--------------------------------------------------------------------------------
uint8_t MULTITASK_MODE=1; 
//================================================================================
uint8_t visualyze_READY = 1;
//--------------------------------------------------------------------------------
uint32_t DATA_SIZE=0;
uint8_t* DATA;
//================================================================================
// APP
//================================================================================
uint16_t POWER = 0;
//--------------------------------------------------------------------------------
#define APP_EMU_NONE 0               
#define APP_AUD_PLAYER 1             
#define APP_EMU_NES 2                
#define APP_EMU_GBC 4                
#define APP_EMU_WOLF 8                
//--------------------------------------------------------------------------------
void MEMORY_STATUS() {
  printf("========================================\n");
  printf("Internal RAM  size   : %u \n", ESP.getHeapSize());
  printf("Internal RAM  free   : %u \n", ESP.getFreeHeap());
  printf("========================================\n");  
  printf("PSRAM first 4MB size : %u \n", ESP.getPsramSize());
  printf("PSRAM first 4MB free : %u \n", ESP.getMaxAllocPsram());
  printf("========================================\n");  
}
//--------------------------------------------------------------------------------


//██████████████████████████████████████████████████
//█ LCD DISPLAY
//██████████████████████████████████████████████████

#define SPI_MODE 0

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9107 _panel_instance;
  lgfx::Bus_SPI _bus_instance; 
  lgfx::Light_PWM  _light_instance;
  
public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST; 
      cfg.spi_mode = SPI_MODE;          
      cfg.freq_write = 80000000;
      cfg.freq_read = 16000000;  
      cfg.spi_3wire = true;      
      cfg.use_lock = true;        
      cfg.dma_channel = SPI_DMA_CH_AUTO; 
      cfg.pin_sclk = TFT_SCLK; 
      cfg.pin_mosi = TFT_MOSI; 
      cfg.pin_miso = TFT_MISO; 
      cfg.pin_dc = TFT_DC;    

      _bus_instance.config(cfg);              
      _panel_instance.setBus(&_bus_instance); 
    }
    {
      auto cfg = _panel_instance.config(); 

      cfg.pin_cs = TFT_CS;    
      cfg.pin_rst = TFT_RST;  
      cfg.pin_busy = -1; 
      cfg.panel_width = LCD_WIDTH;    
      cfg.panel_height = LCD_HEIGHT;   
      cfg.offset_rotation = 0;  
      cfg.dummy_read_pixel = 8; 
      cfg.dummy_read_bits = 1;
      cfg.offset_x = 0;
      cfg.offset_y = +32;  //correction
      cfg.readable = true;      
      cfg.invert = false;       
      cfg.rgb_order = false;    
      cfg.dlen_16bit = false;   
      cfg.bus_shared = true;    

      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = TFT_BCK_LT;              
      cfg.invert = false;           
      cfg.freq   = 44100;           
      cfg.pwm_channel = 7;          

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  
      }

    setPanel(&_panel_instance); // Set the panel to be used.
  }
};

static LGFX lcd;                
static LGFX_Sprite sprite(&lcd);
//██████████████████████████████████████████████████
//█ MICROSD CARD
//██████████████████████████████████████████████████
#if MICROSD
  #include "src/sdfat-beta/src/SdFat.h" // Use SDFAT-beta...
  #include "src/sdfat-beta/src/SpiDriver/SdSpiSoftDriver.h" //this is required for SOFTWARE SPI!
  SoftSpiDriver < SOFTSD_MISO_PIN, SOFTSD_MOSI_PIN, SOFTSD_SCK_PIN > softSpi; //Soft SPI definition
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(5), &softSpi) //Soft SPI is SLOWER

  SdFat SD;
  File fp;
#endif

//██████████████████████████████████████████████████
//█ JPG DECODER
//██████████████████████████████████████████████████
//--------------------------------------------------------------------------------
// JPG DECODER SUPPORT
//--------------------------------------------------------------------------------
#if JPG_DECODER
#include "src/JPG_SUPPORT/tjpgd.h"

JDEC jpgdec;
JRESULT jpgres;
uint8_t * JPGDATA;
#endif
//--------------------------------------------------------------------------------



//██████████████████████████████████████████████████
//█ MAIN LIBRARIES INCLUDES
//██████████████████████████████████████████████████

#include "ICONS.h"
#include "FONTS_PAL.h"
#include "INPUT.h"
#include "FUNCTIONS.h"
#include "MENU.h"


//██████████████████████████████████████████████████
//█ FILE LOADING FUNCTIONS (into FLASH / PSRAM)
//██████████████████████████████████████████████████
const void *ROM; //constant data pointer for direct access
uint8_t flashdata[8192] = {0}; //8kB buffer (do not use malloc!)

//********************************************************************************
#ifdef NCAT_NO_PSRAM

#include "esp_spi_flash.h"

#define SPI_FLASH_ADDRESS 0x00220000 //start of empty FLASH
#define SPI_FLASH_SECTOR_SIZE 0x1000 //4kB = better not change

uint32_t FILE_ROM_SIZE = 0;
uint32_t FLASH_ROM_SIZE = 0;

spi_flash_mmap_handle_t handle1;

//================================================================================
// LOAD DATA TO FLASH (SOLUTION FOR NO PSRAM DEVICES)
//================================================================================
char loadmessage[64];
IRAM_ATTR uint8_t* FLASH_load_sdcard(char * filename, uint32_t* len) {
  fp = SD.open(filename);
  if (DEBUG) printf("FILE SIZE: %d B\n",fp.size());
  FILE_ROM_SIZE = fp.size();
  *len = FILE_ROM_SIZE;

  if (FILE_ROM_SIZE>MAX_FLASH_FILESIZE) {
    printf("Selected file size is out of range: %d Bytes");
    return NULL;
  }

  // Read DATA rom to SPI FLASH!
  uint32_t i = 0;
  for (i = 0; i < fp.size() + SPI_FLASH_SECTOR_SIZE; i++) {
    if (DEBUG>3 && i % SPI_FLASH_SECTOR_SIZE == 0) {
      printf("ROM loaded 4kB: %d\n",i / 0x1000);
    }
    if (i > 0 && i % SPI_FLASH_SECTOR_SIZE == 0) {
      //spi_flash_erase_sector(SPI_FLASH_ADDRESS/SPI_FLASH_SECTOR_SIZE + (i/SPI_FLASH_SECTOR_SIZE)-SPI_FLASH_SECTOR_SIZE);

      delayMicroseconds(300);
      spi_flash_erase_range(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, sizeof(flashdata));
      delayMicroseconds(300);
      spi_flash_write(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, flashdata, sizeof(flashdata));
      delayMicroseconds(300);

      sprintf(loadmessage, " %d / %d", i, FILE_ROM_SIZE);
      set_font_XY(8, 8);
      draw_string("Loaded:");
      set_font_XY(8, 16);
      draw_string(loadmessage);
      screenmemory_drawfillrectangle(0, 24, ((float)LCD_WIDTH / (float)(fp.size() + SPI_FLASH_SECTOR_SIZE))* i, 8, 0xff);
    }
    if (fp.available()) flashdata[i % SPI_FLASH_SECTOR_SIZE] = fp.read();
  }
  fp.close();

  FLASH_ROM_SIZE = i; //Size of File and Offset Align

  if (DEBUG) printf("FLASH SIZE: %d\n",FLASH_ROM_SIZE);

  ROM = 0;

  printf("Mapping %x (+%x)\n", SPI_FLASH_ADDRESS, FLASH_ROM_SIZE);
  ESP_ERROR_CHECK( spi_flash_mmap(SPI_FLASH_ADDRESS, FLASH_ROM_SIZE, SPI_FLASH_MMAP_DATA, &ROM, &handle1) );
  printf("mmap_res: handle=%d ptr=%p\n", handle1, ROM);
  printf("[DATA MAPPED ON FLASH!]\n");
  return (unsigned char *)ROM;    
}
#else
//================================================================================
// LOAD DATA TO PSRAM 
//================================================================================
uint8_t* RAM_load_file(char * filename, uint32_t* len) {
  printf("*** PSRAM_load_file_MICROSD *** \n");  
#if MICROSD
  SDCARD_BUSY=1; 
  uint8_t* data;
  fp = SD.open(filename);
  if (DEBUG) printf("FILE SIZE: %d B\n",fp.size());
  *len = fp.size();

  DRAWMODE = 1;

  uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
  if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
  uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

  if (FILE_ROM_SIZE>MAX_PSRAM_FILESIZE) {
    printf("Selected file size is out of range: %d Bytes");
    return 0;
  }

  data = (uint8_t *)malloc(fp.size() + 1 );
  uint32_t i = 0;
  for (i = 0; i < fp.size(); i++) {
    ((uint8_t*)data)[i] = fp.read();
    if (i % 1024 == 0) {
      sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
      set_font_XY(8, 8);
      draw_string((char*)"Loaded:");
      draw_string(TEXTBUFFER);
      screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 192);
      if (i % 20480 == 0) {
        if (DEBUG>1) {
          printf("█");
        }
      }
    }
  }
  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  fp.close();
  SDCARD_BUSY=0;
  return data;
#endif  
}
//--------------------------------------------------------------------------------
#endif
//================================================================================


//██████████████████████████████████████████████████
//________________________________________________________________________________
//
// AUDIO PLAYER SECTION
//________________________________________________________________________________
bool PAUSED = false;
#if MICROSD
#include "src/AUDIO_PLAYER/src/Audio.h"

Audio audio;
bool player_I2S_enabled = false;
#endif

#include "VISUALIZATIONS.h"
#include "AUDIO.h"


//██████████████████████████████████████████████████
//________________________________________________________________________________
//
// NES EMU SECTION
//________________________________________________________________________________

// Define this if running on little-endian (x86) systems
#define HOST_LITTLE_ENDIAN //!!! important for Arduino
#define ZERO_LENGTH 0
#define NES_UNUSED(x)((x) = (x)) // quell stupid compiler warnings

#ifdef PAL
#define NES_REFRESH_RATE 50 //PAL = 50Hz
#else
#define NES_REFRESH_RATE 60 //NTSC = 60Hz
#endif

//AUDIO_SETUP
#define DEFAULT_SAMPLERATE  22080
#define DEFAULT_FRAGSIZE (DEFAULT_SAMPLERATE / NES_REFRESH_RATE)

//VIDEO_SETUP
#define NES_VISIBLE_HEIGHT 240
#define NES_SCREEN_WIDTH 256
#define NES_SCREEN_HEIGHT 240

#define MAX_MEM_HANDLERS 32

#define DEFAULT_WIDTH NES_SCREEN_WIDTH
#define DEFAULT_HEIGHT NES_VISIBLE_HEIGHT

#define NES_CLOCK_DIVIDER 12 //default: 12
//#define  NES_MASTER_CLOCK     21477272.727272727272
#define NES_MASTER_CLOCK (236250000 / 11)
#define NES_SCANLINE_CYCLES (1364.0 / NES_CLOCK_DIVIDER)
#define NES_FIQ_PERIOD (NES_MASTER_CLOCK / NES_CLOCK_DIVIDER / 60)

#define NES_RAMSIZE 0x800

#define NES6502_NUMBANKS 16
#define NES6502_BANKSHIFT 12
#define NES6502_BANKSIZE (0x10000 / NES6502_NUMBANKS)
#define NES6502_BANKMASK (NES6502_BANKSIZE - 1)

//--------------------------------------------------------------------------------
void (*audio_callback)(void *buffer, int length) = NULL; //for NES
void do_audio_frame();                                   //for NES
void play_audio_frame();                                 //for NES
//--------------------------------------------------------------------------------

uint32_t tickcnt = 0;

#include "NES.h"

//██████████████████████████████████████████████████


//██████████████████████████████████████████████████
//█  DRAW TASKS
//██████████████████████████████████████████████████
//********************************************************************************
//*  DEFAULT DRAW FRAME (... and GBC DRAW FRAME)                                                          *
//********************************************************************************
static inline void lcd_write_frame_default() {
  uint16_t* dest;
  uint8_t* src;
  int16_t Y_POS = -1;
  uint16_t X_POS = 0;


#if DEBUG>1
  uint32_t DRAWTIME_METER = millis();
#endif
//................................................................................
  while (++Y_POS < LCD_HEIGHT) {
    dest = &SCREENBUFFER[0];
    if (DRAWMODE == 4) src = SCREENMEMORY_LINE[Y_POS*2];
    else src = SCREENMEMORY_LINE[Y_POS];
    
    X_POS = LCD_WIDTH;

    if (DRAWMODE == 4) while (--X_POS) {
        *dest++ = default_16bit[*src++];
        *src++;
    } else while (--X_POS) {
        *dest++ = default_16bit[*src++];
      }

    if (DRAWMODE == 4) if (AUDIO_TIMER + 0 <= millis()) AUDIOTask(); //in GBC we need do AUDIOTASK try every 1ms...
//................................................................................
#if LCD_ENABLED
    lcd.pushImage(0, Y_POS, 128, 1, (uint16_t *)(&SCREENBUFFER[0]), true);
#endif
  }
#if DEBUG>1
  printf("DRAWTIME: %d\n", (uint32_t)(millis() - DRAWTIME_METER));
#endif
}
//********************************************************************************
//*  NES EMU DRAW FRAME                                                          *
//********************************************************************************
uint32_t DRAWTIME_METER=0;
IRAM_ATTR void lcd_write_frame_NES() {
  uint16_t* dest;
  uint8_t* src;
  int16_t Y_POS = 0; // + 1 vertical pixel for drawing (NES SCROLLING BUFFER FIX)
  uint16_t X_POS = 0;

#if DEBUG>1
  uint32_t DRAWTIME_METER = millis();
#endif
  while (++Y_POS < LCD_HEIGHT) {

    AUDIOTask(); //in NES we need do AUDIOTASK more than every 25ms... (every 16ms)
    
    dest = &SCREENBUFFER[0];
    src = &SCREENMEMORY_LINE[Y_POS*2][0]; ///center NES
    X_POS = LCD_WIDTH;
    while (X_POS--) {
      *dest++ = nes_16bit[*src++];
      *src++;
    }
#if LCD_ENABLED
    lcd.pushImage(0, Y_POS+8, 128, 1, (uint16_t *)(&SCREENBUFFER[0]), true); // true = enable DMA
#endif
  }
#if DEBUG>1
  printf("DRAWTIME: %d\n", (uint32_t)(millis() - DRAWTIME_METER));
#endif
}


//██████████████████████████████████████████████████
//█  AUDIO TASKS
//██████████████████████████████████████████████████
//--------------------------------------------------------------------------------

void sound_mix(); //for GBC

static inline void AUDIOTask() {
  if (AUDIOREADY == 1) {
    AUDIOREADY = 2;
    if (SOUND_ENABLED && audio_callback && POWER == APP_EMU_NES) { //NES AUDIO FRAME
#if NES_EMULATOR
      do_audio_frame();
      play_audio_frame();
#endif
    } 
    AUDIOREADY = 0;
    AUDIO_TIMER = millis();
  }
}
//--------------------------------------------------------------------------------
//██████████████████████████████████████████████████
//█  SYSTEM TASK
//██████████████████████████████████████████████████
void SYSTEM_TASK() {

  if (AUDIO_TIMER + 0 <= millis()) {                 //in GBC we need do AUDIOTASK try every ~1ms...
    AUDIOTask();
  }
  
  DRAWREADY = 0;

  if (POWER == APP_AUD_PLAYER) {
    visualyze();                                     //visualyze when playing audio...
  } 

  //DRAW and AUDIO tasks
  if (DRAWMODE == 1) lcd_write_frame_default();      // DEFAULT DRAW MODE & NO AUDIO
  else if (DRAWMODE == 2) lcd_write_frame_NES();     // NES DRAW MODE + AUDIO
  else if (DRAWMODE == 4) lcd_write_frame_default(); // GBC DRAW MODE + AUDIO

  DRAWREADY = 1;
}
//--------------------------------------------------------------------------------
IRAM_ATTR void SYSTEMTASK(void * arg) {
  while (1) {
#if USB_HOST_ESP32S3
  usbHost.task();
#endif 
    
    if (!xSemaphoreTake(draw_mutex, 5)) { //30ms wait for draw_mutex
#if (DEBUG>1)      
      printf("DRAW: waiting for frame\n");
#endif
      if (DRAWMODE == 5) {
          INPUTREAD();
      }
            
      continue;
    }
    SYSTEM_TASK();
  }
}
//--------------------------------------------------------------------------------



//██████████████████████████████████████████████████
//█  SETUP
//██████████████████████████████████████████████████
void setup(void) {

   //Serial.begin(115200);

//--------------------------------------------------------------------------------
// INIT LCD
//--------------------------------------------------------------------------------
#if LCD_ENABLED
  printf("Initializing LCD...");
  lcd.init();
  lcd.setRotation(0);
  lcd.setBrightness(128);
  lcd.fillScreen(0);
  printf("[ OK ]\n");
#endif  
//--------------------------------------------------------------------------------
// INIT microSD CARD
//--------------------------------------------------------------------------------
#if MICROSD
  printf("Initializing microSD CARD...");

  if (SD_CS_PIN != -1) {
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, 0);
  }

  if (!SD.begin(SD_CONFIG)) {
    printf("[fail]\n");
    delay(250);
  } else {
    printf("[ OK ]\n");
  }
#if(MICROSD == 2)
  while (!SD.begin(SD_CONFIG)) {
    printf("No MicroSD card detected!\n");
    delay(1000); //wait 250ms for confirm
  }
  printf("[ OK ]\n");
#endif
#endif

//--------------------------------------------------------------------------------
#if USB_HOST_ESP32S3
  usbHost.begin();
#endif  
//--------------------------------------------------------------------------------

//████████████████████████████████████████
  printf("Allocating VIDEO memory 64kB RAM...");
  SCREENMEMORY = (uint8_t * ) heap_caps_malloc(SCREEN_WIDTH * SCREEN_HEIGHT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
  
  memset(SCREENMEMORY, 0, (SCREEN_WIDTH * SCREEN_HEIGHT)); 
  for (uint32_t tmp = 0; tmp < SCREEN_HEIGHT; tmp++) { 
    SCREENMEMORY_LINE[tmp] = &SCREENMEMORY[tmp * SCREEN_WIDTH];
  }
  printf("[ OK ]\n");
  set_font(font_CP437_4x8); //default font 4*8 pixels
//████████████████████████████████████████
#if JPG_DECODER
  printf("Allocating JPG Decoding buffer ~4kB PSRAM...");
  JPGOUTPUT_BUFFER = (uint8_t * ) malloc(JPG_BUFFER_SIZE);
  printf("[ OK ]\n");
#endif
//████████████████████████████████████████
#if JPG_DECODER
  renderJPG_toBUFFER(NULL, (128 - 64) / 2, (128 - 64) / 2 - 16, JPG_NCATSYSICON, sizeof(JPG_NCATSYSICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif
  set_font_XY((128 - 11*4) /2, 100); 
  draw_string_static((char*)"NCAT SYSTEM", 0xffff);

  delay(500);
  
//--------------------------------------------------------------------------------
#if NES_CONTROLLER
   printf("Initializing NES CONTROLLER...");
   //NES Controller init
   pinMode(NESCTRL_Data, INPUT);
   pinMode(NESCTRL_Clock, OUTPUT); 
   pinMode(NESCTRL_Latch, OUTPUT); //GPIO2 is pulled up...
   digitalWrite(NESCTRL_Clock, 0);
   digitalWrite(NESCTRL_Latch, 0);
#endif
//--------------------------------------------------------------------------------
  USBKEYBOARD=(uint8_t*)malloc(256);
  memset(USBKEYBOARD,0,256);

  TEXTBUFFER = (char * ) malloc(64);
  MAINPATH = (char * ) malloc(64);
  TRACKNAME = (char * ) malloc(64);
  
  draw_mutex = xSemaphoreCreateBinary();
  xSemaphoreTake(draw_mutex, 0);

  audio_mutex = xSemaphoreCreateBinary();
  xSemaphoreTake(audio_mutex, 0);
  
  disableLoopWDT();
  disableCore0WDT(); //Disable Watchdog on CORE 0 (I think it is not good idea... maybe cause blutooth)

  MEMORY_STATUS();

  INIT_SOUND();

  xTaskCreatePinnedToCore( &SYSTEMTASK, "SYSTEMTASK", 4096, NULL, 20 | portPRIVILEGE_BIT, NULL, 0); //Core0
  
  delay(300);

  DRAWMODE=1;

  screenmemory_fillscreen(0);
  
  delay(100);

  DRAWMODE=0;
}


void PLAY_MUSIC();
void PLAY_NES();
//void PLAY_GBC();

//█████████████████████████████████████████████████
//█ LOOP TASK
//█████████████████████████████████████████████████


void loop(void) {    
    if (MENU_ACTIVE == 0 || MENU_ACTIVE == 1) {
      MENU_TASK(MENU_ITEM);
    }
    if (MENU_ACTIVE < 0) MENU_ACTIVE = 0; //MENU TASK Activation Request
    if (MENU_ACTIVE == 2) {
      MENU_ACTIVE = -1;

      EXIT = 0;
      POWER = 0;
      AUDIOREADY=0; 

      switch (MENU_ITEM) {
      case 0:
        PLAY_MUSIC();
        break;
      case 1:
        PLAY_NES();
        break;
 //     case 2:
//        PLAY_GBC();
//        break;
//      case 3:
//        play_wolf3d(); 
//        break;        
      default:
        break;
      }
    }
}

//█████████████████████████████████████████████████
//█ PLAY NES
//█████████████████████████████████████████████████

void PLAY_NES() {
        printf("================================================================================\n");
        printf("Starting NES EMULATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if NES_EMULATOR
        DRAWMODE = 1;
        set_font(font_CP437_4x8); //default font
        screenmemory_fillscreen(0); //black color       
//--------------------------------------------------------------------------------
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield();       
          if (LOADED_ROM == 1) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, (char * )"/NES/"); ///must be allocated 256 Bytes
              Browse_MICROSD(APP_EMU_NES, MAINPATH, (char * )"microSD: Select NES ROM image");
          }
        }
//--------------------------------------------------------------------------------
        if (EXIT) return; ///

        if (LOADED_ROM == 2) LOADED_ROM = 3;
        else if (LOADED_ROM == 12) LOADED_ROM = 13;
        else if (LOADED_ROM == 22) LOADED_ROM = 23;

        screenmemory_fillscreen(0); //black color

        if (DEBUG) printf("> SELECTED NES: %s\n",MAINPATH);

        delay(300);        

#ifdef NCAT_NO_PSRAM
        DATA = FLASH_load_sdcard(MAINPATH, &DATA_SIZE); 
#else
        DATA = RAM_load_file(MAINPATH, &DATA_SIZE);
#endif


printf("FLASH_load_sdcard done.\n");

        null_page = (uint8_t * ) malloc(NES6502_BANKSIZE); ///4kB memory
        audio_frame[0] = (int16_t * ) malloc(DEFAULT_FRAGSIZE * 4); ///
        audio_frame[1] = (int16_t * ) malloc(DEFAULT_FRAGSIZE * 4); ///

        AUDIO_SET_PARAMS(DEFAULT_SAMPLERATE, 16, 2, 0);
        init_sound(); //START AUDIO

        if (DEBUG) printf("NES machine CREATE\n");

        //NES machine CREATE
        if (NULL == NESmachine) NESmachine = nes_create();
        if (NULL == NESmachine) {
          if (DEBUG) printf("Failed to create NES instance.\n");
          while (1) {} //FREEZE
        }

        if (DEBUG) printf("NES Inserting cartridge.\n");      
//--------------------------------------------------------------------------------
        if (NULL == NESmachine -> rominfo) 
        NESmachine -> rominfo = (rominfo_t * ) malloc(sizeof(rominfo_t));
        if (NULL == NESmachine -> rominfo) goto rom_load_fail;
        
        memset(NESmachine -> rominfo, 0, sizeof(rominfo_t));
        
        // Get the header and stick it into rominfo struct
        if (rom_getheader( & DATA, NESmachine -> rominfo)) goto rom_load_fail;
        
        // Make sure we really support the mapper
        if (false == mmc_peek(NESmachine -> rominfo -> mapper_number)) {
          if (DEBUG) {
            printf("Mapper not yet implemented:");
            printf("%d\n",NESmachine -> rominfo -> mapper_number);
          }
          goto rom_load_fail;
        }
        
        MEMORY_STATUS();
        
        // iNES format doesn't tell us if we need SRAM, so we have to always allocate it -- bleh!* UNIF, TAKE ME AWAY!  AAAAAAAAAA!!!
        if (rom_allocsram( & NESmachine -> rominfo)) goto rom_load_fail; ///NEED FOR example: SMB2-LostLevels
        rom_loadtrainer( & DATA, NESmachine -> rominfo);
        if (rom_loadrom( & DATA, NESmachine -> rominfo)) goto rom_load_fail;        
        goto rom_load_success;        
//................................................................................
      rom_load_fail:
        if (DEBUG) {
          printf("NES ROMLOAD FAIL.\n");
        }                
        goto nes_exit; // continue to menu.     
//................................................................................
      rom_load_success:
        if (DEBUG) {
          printf("NES ROMLOAD SUCCESS.\n");
        }
        // map cart's SRAM to CPU $6000-$7FFF
        if (NESmachine -> rominfo -> sram) {
          NESmachine -> cpu -> mem_page[6] = NESmachine -> rominfo -> sram;
          NESmachine -> cpu -> mem_page[7] = NESmachine -> rominfo -> sram + 0x1000;
        }
        // mapper
        NESmachine -> mmc = mmc_create(NESmachine -> rominfo);
        if (NULL == NESmachine -> mmc) goto inscart_fail;
        // if there's VRAM, let the PPU know
        if (NULL != NESmachine -> rominfo -> vram) NESmachine -> ppu -> vram_present = true;
        if (SOUND_ENABLED) apu_setext(NESmachine -> apu, NESmachine -> mmc -> intf -> sound_ext);
        if (SOUND_ENABLED) osd_setsound(NESmachine -> apu -> process);
        build_address_handlers(NESmachine);
        nes_setcontext(NESmachine);

        //NES RESET:
        nes_reset();
        goto inscart_success;
//................................................................................
      inscart_fail:
        if (DEBUG) {
          printf("Insert Cartridge Fail!\n");
        }
        goto nes_exit; // continue to menu.
//................................................................................
      inscart_success:
        if (DEBUG) {
            printf("NES Insert Cartridge OK.\n");
        }
        DRAWMODE = 1;
        screenmemory_fillscreen(0); //black color
        xSemaphoreGive(draw_mutex);
        delay(100);

        POWER = APP_EMU_NES;   
        DRAWMODE = 2;
        ///SETPAL(APP_EMU_NES);

        //NES START!
        AUDIOREADY=1; 


        printf("NES START!\n");
//--------------------------------------------------------------------------------
        while (POWER == APP_EMU_NES) { //NES EMULATION LOOP
          tickcnt = micros();
          INPUTCLEAR();                    
          if (INPUTREAD()) {
              //INPUTREPORT();
          }
       
          nes_renderframe(true);
                                      
          tickcnt = micros() - tickcnt;

          if (AUDIOREADY == 0) AUDIOREADY = 1;          

          if (LCD_ENABLED) if (MULTITASK_MODE == 1) xSemaphoreGive(draw_mutex); //refresh LCD         
          if (DRAWREADY == 0) DRAWREADY = 1;
                    
          if (tickcnt < 1000000 / NES_REFRESH_RATE) delay(1000 / NES_REFRESH_RATE - tickcnt / 1000); //delay before next frame
          if (((INPUT_START == 1 && INPUT_SELECT == 1) || INPUT_SYSTEM == 1) && !INPUT_A && !INPUT_B) { //EXIT NES EMULATOR CALL
            if (IN_GAME_MENU) {
              delay(100);
              do_window_menu();
              INPUTCLEAR();
            } else {
              AUDIOREADY=0;
              POWER = 0; //exit
            }
          }
//----------------------------------------------------------------------------------------
        }
        
        DRAWMODE=0;       
      nes_exit:
        memset(&SCREENBUFFER[0],0,1024*2);

        POWER = 0;
        delay(500);
        osd_stopsound(); ///must be delayed because crash...
        AUDIO_SILENCE();
        free(audio_frame[0]);
        free(audio_frame[1]);
        free(null_page); ///
        if (NESmachine -> rominfo -> sram != NULL) free(NESmachine -> rominfo -> sram); ///free SRAM

#ifdef NCAT_NO_PSRAM
#else
        DATA = RAM_load_file(MAINPATH, &DATA_SIZE);
#endif        

#endif
        screenmemory_fillscreen_static(0);
}

//█████████████████████████████████████████████████
//█ PLAY MUSIC
//█████████████████████████████████████████████████


void PLAY_MUSIC() {
        printf("================================================================================\n");
        printf("Starting AUDIO PLAYER APPLICATION\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
        
        if (PWM_AUDIO_STOPPED) {
          PWM_AUDIO_STOPPED=0;
          pwm_audio_start();
        }
#if MICROSD
        DRAWMODE = 1;

        player_I2S_enabled = false;

        screenmemory_fillscreen(0); //black color
        sprintf(MAINPATH, "/AUDIO/");
        //malloc FILENAME array
        for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
          filename[tmp] = (char * ) calloc(MAXFILENAME_LENGTH, sizeof(char));
        }
        
        audio.m_outBuff = (int16_t * ) malloc(2048 * 2 * 2); //malloc 16bit * 2*2048
        //audio.setPinout(I2S_bck_io_num, I2S_ws_io_num, I2S_data_out_num);
        audio.setPinout(-1, -1, -1);
        audio.setVolume(VOLUME * 100 / 1163); // 0..255 => 0...21  

//................................................................................
        while (!EXIT) {
//--------------------------------------------------------------------------------
          LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
          while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
            vPortYield();          
            if (LOADED_ROM == 1) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, "/AUDIO/"); ///must be allocated 256 Bytes
              Browse_MICROSD(APP_AUD_PLAYER, MAINPATH, (char * ) "microSD: Select AUDIO file");
            }  
          }

          
          PLAYINGFILE = SELECTED_FILE_POS; ///play selected file           
          TOTALFILES = onlyFileNames; //without DIRS!
//--------------------------------------------------------------------------------
          if (EXIT) {
              AUDIO_SILENCE();
              delay(200);
              INPUTCLEAR(); ///
              POWER = 0;
              DRAWMODE=1; ///
              break;
          }
          delay(300);
            
          printf("%s\n%d/%d\n",MAINPATH,PLAYINGFILE + 1,TOTALFILES);
          
          audio.connecttoFS(SD, MAINPATH);
          
          screenmemory_fillscreen(0); //black color
          set_font_XY(0, 6);
          sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
          draw_string(TEXTBUFFER, 255);
          
          set_font_XY(0, 128 - 16);
          draw_string(TRACKNAME, 255);
          
          set_font_XY(0, 6 + 8);
          sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH ", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
          draw_string(TEXTBUFFER, 255);
          
          uint32_t prevtime = millis();
          
          POWER = APP_AUD_PLAYER; //Playing Flag
          PAUSED = false;
          
          visualyze_READY = 1;

          uint32_t AUDIO_TIME_COUNTER = millis();
          uint32_t AUDIO_TIME_COUNTER2 = millis();
//................................................................................
          while (POWER == APP_AUD_PLAYER) {
            audio.loop();
            audio.loop(); //twice is good for performance...
                        
            if (AUDIO_TIME_COUNTER + 40 < millis()) { 
              if (/*DRAWREADY == 0 &&*/ POWER == APP_AUD_PLAYER && visualyze_READY == 1) {
                DRAWREADY = 1;
                //visualyze(); 
                xSemaphoreGive(draw_mutex);               
                if (AUDIOREADY == 0) AUDIOREADY = 1;
              }              
              AUDIO_TIME_COUNTER = millis();
              //continue;
            }

            audio.loop(); //triple is better for performance...
            audio.loop(); //quad is best for performance...

            if (AUDIO_TIME_COUNTER + 40 < millis()) { 
              if (/*DRAWREADY == 0 &&*/ POWER == APP_AUD_PLAYER && visualyze_READY == 1) {
                DRAWREADY = 1;
                //visualyze(); 
                xSemaphoreGive(draw_mutex);               
                if (AUDIOREADY == 0) AUDIOREADY = 1;
              }              
              AUDIO_TIME_COUNTER = millis();
              //continue;
            }


              if (INPUTREAD()) {
                //INPUTREPORT();
              }

                                                                          
            if (AUDIO_TIME_COUNTER2 + 500 < millis()) {
              uint16_t h = 0;
              uint8_t m = 0;
              uint8_t s = 0;
              secondsToHMS(audio.getAudioCurrentTime(), h, m, s);
              set_font_XY(128 - 80, 6);
              sprintf(TEXTBUFFER, "%02d:%02d:%02d / ", h, m, s);
              draw_string(TEXTBUFFER, 0xff);
              secondsToHMS(audio.getAudioFileDuration(), h, m, s);
              sprintf(TEXTBUFFER, "%02d:%02d:%02d", h, m, s);
              draw_string(TEXTBUFFER, 0xff);
              AUDIO_TIME_COUNTER2 = millis();
              //continue;
            }


            
            if (INPUT_A || INPUT_B || INPUT_SELECT || INPUT_START || INPUT_UP || INPUT_DOWN || INPUT_LEFT || INPUT_RIGHT) delay(1);
            //PAUSE
            if ((INPUT_SELECT == 0 && INPUT_START == 1) || INPUT_A == 1) {
              audio.pauseResume();
              if (PAUSED == false) PAUSED = true;
              else PAUSED = false;
              AUDIO_SILENCE();
              delay(200);
              INPUTCLEAR();
            }
            //VOLUME+
            if ((INPUT_UP == 1) && prevtime + 200 < millis()) {
              if (VOLUME <= 255 - 8) VOLUME += 8;
              else VOLUME = 255;
              audio.setVolume(VOLUME * 100 / 1163); // 0...21
              set_font_XY(0, 6 + 8);
                  
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              prevtime = millis();
              INPUTCLEAR();
            }
            //VOLUME-
            if ((INPUT_DOWN == 1) && prevtime + 200 < millis()) {
              if (VOLUME >= 8) VOLUME -= 8;
              else VOLUME = 0;
              audio.setVolume(VOLUME * 100 / 1163); // 0...21
              set_font_XY(0, 6 + 8);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              prevtime = millis();
              INPUTCLEAR();
            }
            //PLAY NEXT FILE AFTER END
            if (PLAYINGFILE < TOTALFILES - 1 && !audio.isRunning() && PAUSED == false) {
              if (PLAYINGFILE < TOTALFILES - 1) PLAYINGFILE++;
              AUDIO_SILENCE();
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                  printf("%d/%d\n%s\n",PLAYINGFILE + 1,TOTALFILES,MAINPATH);
              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              screenmemory_fillscreen(0); //black color
              set_font_XY(0, 6);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(0, 128 - 12);
              draw_string(TRACKNAME, 255);
              set_font_XY(0, 6 + 8);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              delay(300);
            }
            //NEXT TRACK
            if (INPUT_RIGHT == 1) {
              if (PLAYINGFILE < TOTALFILES - 1) PLAYINGFILE++;
              AUDIO_SILENCE();
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                  printf("%d/%d\n%s\n",PLAYINGFILE + 1,TOTALFILES,MAINPATH);
              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              screenmemory_fillscreen(0); //black color
              set_font_XY(0, 6);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(0, 128 - 12);
              draw_string(TRACKNAME, 255);
              set_font_XY(0, 6 + 8);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              INPUTCLEAR();
              delay(200);
            }
            //PREV TRACK
            if (INPUT_LEFT == 1) {
              if (PLAYINGFILE > 0) PLAYINGFILE--;
              AUDIO_SILENCE();                  
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                  printf("%d/%d\n%s\n",PLAYINGFILE + 1,TOTALFILES,MAINPATH);

              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              delay(200);
              screenmemory_fillscreen(0); //black color
              set_font_XY(0, 6);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(0, 128 - 12);
              draw_string(TRACKNAME, 255);
              set_font_XY(0, 6 + 8);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              INPUTCLEAR();
            }
            //EXIT
            if ((INPUT_SELECT == 1 && INPUT_START == 1) || (INPUT_SYSTEM == 1)) {
              audio.stopSong();
              AUDIO_SILENCE();
              delay(200);
              INPUTCLEAR(); ///
              POWER = 0;
              EXIT = true;
            }
          }
        }
//................................................................................
        printf("Exiting Player application.\n");
        POWER = 0;
        audio.Uninstall();
        screenmemory_fillscreen(0); //black color
        delay(200);
        DRAWMODE=0;
#endif
        delay(200);  
}
