

//********************************************************************************
// IO PINS CONFIGURATION
//********************************************************************************

#define IO_BUTTON0  0
#define IO_BUTTON47 47

//--------------------------------------------------------------------------------
// LCD DISPLAY CONFIG
//--------------------------------------------------------------------------------

#define TFT_MOSI   2  // SDA
#define TFT_MISO   -1
#define TFT_SCLK   3  // SCL
#define TFT_DC     6
#define TFT_CS     5
#define TFT_RST    1
#define TFT_BCK_LT 10 

//--------------------------------------------------------------------------------
// MICROSD CARD
//--------------------------------------------------------------------------------

#define SD_SPI_DRIVER_SELECT 2

#define SOFTSD_MOSI_PIN 35             // [SD MOSI]
#define SOFTSD_MISO_PIN 37             // [SD MISO]
#define SOFTSD_SCK_PIN  36             // [SD SCK]
#define SD_CS_PIN       34             // [CS]

//--------------------------------------------------------------------------------
// NES CONTROLLER CONFIG
//--------------------------------------------------------------------------------
#define NESCTRL_Data    48             // The data pin for the NES controller
#define NESCTRL_Clock   18             // The clock pin for the NES controller ---[ 330R ]--- +3.3V (Must be pulled up to +3.3V)
#define NESCTRL_Latch   17             // The latch pin for the NES controller (STROBE) is pulled up by default on ESP32

//--------------------------------------------------------------------------------
// PWM AUDIO CONFIG
//--------------------------------------------------------------------------------

#define PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO 38
#define PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO 39

//********************************************************************************
//********************************************************************************
#define SD_SPI_HOST SPI3_HOST

  #define SOFTSD_MOSI_PIN 35             // [SD MOSI]
  #define SOFTSD_MISO_PIN 37             // [SD MISO]
  #define SOFTSD_SCK_PIN 36              // [SD SCK]
  #define SD_CS_PIN 34                   // [CS]
  
#define CONFIG_HW_SD_PIN_NUM_MISO SOFTSD_MISO_PIN
#define  CONFIG_HW_SD_PIN_NUM_MOSI SOFTSD_MOSI_PIN
#define  CONFIG_HW_SD_PIN_NUM_CLK SOFTSD_SCK_PIN
#define  CONFIG_HW_SD_PIN_NUM_CS SD_CS_PIN 

#define CONFIG_HW_BUTTON_PIN_NUM_UP 0
#define CONFIG_HW_BUTTON_PIN_NUM_RIGHT 1
#define CONFIG_HW_BUTTON_PIN_NUM_DOWN 2
#define CONFIG_HW_BUTTON_PIN_NUM_LEFT 3
#define CONFIG_HW_BUTTON_PIN_NUM_BUTTON1 4
#define CONFIG_HW_BUTTON_PIN_NUM_BUTTON2 5
