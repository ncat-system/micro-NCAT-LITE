
uint8_t Multiplayer_dev_input_support=0;

//================================================================================
uint8_t BUTTONS=0;                        // MOUSE & JOYSTICK (0bRLZYXCBA)
uint8_t BUTTONS_LAST=0; 
//================================================================================
uint8_t* USBKEYBOARD;                     // malloc/ps_malloc
//================================================================================
//GPIO BUTTONS: 9pcs 
//--------------------------------------------------------------------------------
uint8_t BUTTON_UP=0;
uint8_t BUTTON_DOWN=0;
uint8_t BUTTON_LEFT=0;
uint8_t BUTTON_RIGHT=0;
uint8_t BUTTON_A=0;
uint8_t BUTTON_B=0; 
uint8_t BUTTON_START=0;
uint8_t BUTTON_SELECT=0;
uint8_t BUTTON_SYSTEM=0;
//================================================================================
//NES CONTROLLER BUTTONS: 8 
//--------------------------------------------------------------------------------
uint8_t NESCTRL_UP=0;
uint8_t NESCTRL_DOWN=0;
uint8_t NESCTRL_LEFT=0;
uint8_t NESCTRL_RIGHT=0;
uint8_t NESCTRL_A=0;
uint8_t NESCTRL_B=0; 
uint8_t NESCTRL_START=0;
uint8_t NESCTRL_SELECT=0;
//================================================================================
//--------------------------------------------------------------------------------
uint8_t USB_UP=0;
uint8_t USB_DOWN=0;
uint8_t USB_LEFT=0;
uint8_t USB_RIGHT=0;
uint8_t USB_A=0;
uint8_t USB_B=0; 
uint8_t USB_START=0;
uint8_t USB_SELECT=0;
//================================================================================
//================================================================================
///GENERAL BUTTONS
//================================================================================
uint8_t INPUT_UP=0;                       /// ↑
uint8_t INPUT_DOWN=0;                     /// ↓
uint8_t INPUT_LEFT=0;                     /// ←
uint8_t INPUT_RIGHT=0;                    /// →
uint8_t INPUT_A=0;                        /// X = ALT = Z
uint8_t INPUT_B=0;                        /// □ = CTRL = X
uint8_t INPUT_X=0;                        /// O
uint8_t INPUT_Y=0;                        /// Δ
uint8_t INPUT_START=0;                    /// ◄ START = ENTER
uint8_t INPUT_SELECT=0;                   /// ► SELECT = BACKSPACE
uint8_t INPUT_SYSTEM=0;                   /// ■ SYSTEM = ESCAPE
//===============================================================================


//================================================================================
#if USB_HOST_ESP32S3

#define USBHID_TIMEOUT 50

#include "usbjoy/EspUsbHost.h"
#include "usbjoy/EspUsbHost.cpp"

uint32_t USB_counter=0;

class MyEspUsbHost : public EspUsbHost {
  
};

MyEspUsbHost usbHost;

IRAM_ATTR void EspUsbHost::_onReceive(usb_transfer_t *transfer) {
#if 0
  printf("%02X, ",transfer->data_buffer[0]);
  printf("%02X, ",transfer->data_buffer[1]);
  printf("%02X, ",transfer->data_buffer[2]);
  printf("%02X, ",transfer->data_buffer[3]);
  printf("%02X, ",transfer->data_buffer[4]);
  printf("%02X, ",transfer->data_buffer[5]);
  printf("%02X, ",transfer->data_buffer[6]);
  printf("%02X \n",transfer->data_buffer[7]);
#endif

if (USB_counter<millis()) {
  if ((transfer->data_buffer[0]<0x7F) && USB_LEFT==0) USB_LEFT=1; else if ((transfer->data_buffer[0]>=0x7F) && USB_LEFT==1) USB_LEFT=0;
  if ((transfer->data_buffer[0]>0x7F) && USB_RIGHT==0) USB_RIGHT=1; else if ((transfer->data_buffer[0]<=0x7F) && USB_RIGHT==1) USB_RIGHT=0;
  if ((transfer->data_buffer[1]<0x7F) && USB_UP==0) USB_UP=1; else if ((transfer->data_buffer[1]>=0x7F) && USB_UP==1) USB_UP=0;
  if ((transfer->data_buffer[1]>0x7F) && USB_DOWN==0) USB_DOWN=1; else if ((transfer->data_buffer[1]<=0x7F) && USB_DOWN==1) USB_DOWN=0;
  
//  if ((transfer->data_buffer[5]&0x10) && USB_X==0) USB_X=1; else if (!(transfer->data_buffer[5]&0x10) && USB_X==1) USB_X=0;
  if ((transfer->data_buffer[5]&0x20) && USB_A==0) USB_A=1; else if (!(transfer->data_buffer[5]&0x20) && USB_A==1) USB_A=0;
  if ((transfer->data_buffer[5]&0x40) && USB_B==0) USB_B=1; else if (!(transfer->data_buffer[5]&0x40) && USB_B==1) USB_B=0;
//  if ((transfer->data_buffer[5]&0x80) && USB_Y==0) USB_Y=1; else if (!(transfer->data_buffer[5]&0x80) && USB_Y==1) USB_Y=0;

  if ((transfer->data_buffer[6]&0x10) && USB_SELECT==0) USB_SELECT=1; else if (!(transfer->data_buffer[6]&0x10) && USB_SELECT==1) USB_SELECT=0;
  if ((transfer->data_buffer[6]&0x20) && USB_START==0) USB_START=1; else if (!(transfer->data_buffer[6]&0x20) &&  USB_START==1) USB_START=0;
  
//  if ((transfer->data_buffer[6]&0x01) && USB_L==0) USB_L=1; else if (!(transfer->data_buffer[6]&0x01) && USB_L==1) USB_L=0;
//  if ((transfer->data_buffer[6]&0x02) && INPUT_R==0) INPUT_R=1; else if (!(transfer->data_buffer[6]&0x02) && USB_R==1) USB_R=0;

USB_counter=millis()+USBHID_TIMEOUT;
}
}

#endif
//================================================================================



//--------------------------------------------------------------------------------
void BACKLIGHT_PIN_UPDATE(uint8_t BACKLIGHT_LEVEL) {
#if LCD_BACKLIGHT_CONTROL
  lcd.setBrightness(255-(BACKLIGHT_LEVEL));  
#endif
}


uint32_t NES_CONTROLLER_TIME=0;
void CLEAR_NES_CONTROLLER() {
#if NES_CONTROLLER  
  NESCTRL_UP=0;
  NESCTRL_DOWN=0;
  NESCTRL_LEFT=0;
  NESCTRL_RIGHT=0;
  NESCTRL_A=0;
  NESCTRL_B=0; 
  NESCTRL_START=0;
  NESCTRL_SELECT=0;
#endif   
}

void NES_CONTROLLER_READ() {
#define NES_CONTROLLER_REFRESH_TIME 40 //every 40ms read JOYSTICK  
#if NES_CONTROLLER  
  if (millis() - NES_CONTROLLER_TIME > NES_CONTROLLER_REFRESH_TIME) { 
    NES_CONTROLLER_TIME=millis();
    CLEAR_NES_CONTROLLER();

    //NES Controller strobe
    digitalWrite(NESCTRL_Latch, 1);
    digitalWrite(NESCTRL_Latch, 0);

    delayMicroseconds(1); //ESP32 is too fast for NES CONTROLLER...

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_A=1; else NESCTRL_A=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_B=1; else NESCTRL_B=0; 
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_SELECT=1; else NESCTRL_SELECT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_START=1; else NESCTRL_START=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_UP=1; else NESCTRL_UP=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_DOWN=1; else NESCTRL_DOWN=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_RIGHT=1; else NESCTRL_RIGHT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_LEFT=1; else NESCTRL_LEFT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);  
    delayMicroseconds(1);
  }
#endif  
}

extern uint32_t ON_SECONDS;               //TOTAL ON SCEONDS
extern uint32_t LAST_ACTIVITY_SECONDS;    //LAST ACTIVITY SECONDS TIME
extern uint8_t LCD_BACKLIGHT;
extern uint8_t BACKLIGHT_PIN_LAST_VALUE;

uint8_t wakeup_LCD();
uint8_t INPUTREPORT();

uint8_t INPUTREAD() {

  NES_CONTROLLER_READ();

 /*
  if (BUTTON_UP==1 || JOY_UP==1 || USB_JOY_UP==1 || NESCTRL_UP==1 || USBKEYBOARD[0x57] ||  TOUCH_UP==1 ) INPUT_UP=1;
  if (BUTTON_DOWN==1 || JOY_DOWN==1 || USB_JOY_DOWN==1 || NESCTRL_DOWN==1 || USBKEYBOARD[0x62] || TOUCH_DOWN==1) INPUT_DOWN=1;
  if (BUTTON_LEFT==1 || JOY_LEFT==1 || USB_JOY_LEFT==1 || NESCTRL_LEFT==1 || USBKEYBOARD[0x61] || TOUCH_LEFT==1 ) INPUT_LEFT=1;
  if (BUTTON_RIGHT==1 || JOY_RIGHT==1 || USB_JOY_RIGHT==1 || NESCTRL_RIGHT==1 || USBKEYBOARD[0x63] || TOUCH_RIGHT==1 ) INPUT_RIGHT=1;
  if (BUTTON_A==1 || JOY_B==1 || USB_JOY_A==1 || NESCTRL_A==1 || USBKEYBOARD[0x5d] || USBKEYBOARD[0x4c] || TOUCH_A==1) INPUT_A=1;
    
  if (JOY_X==1 || USB_JOY_Y==1 || USBKEYBOARD[0x3c] ) INPUT_Y=1; 
  if (BUTTON_B==1 || JOY_A==1 || USB_JOY_B==1 || NESCTRL_B==1 || USBKEYBOARD[0x5c] || USBKEYBOARD[0x4d]) INPUT_B=1;
    
  if (JOY_C==1 || USB_JOY_X==1 || USBKEYBOARD[0x3d] ) INPUT_X=1;
  
  if (BUTTON_START==1 || USB_JOY_START==1 || JOY_START || NESCTRL_START==1 || USBKEYBOARD[0x47] ) INPUT_START=1;
  if (BUTTON_SELECT==1 || USB_JOY_SELECT==1 || JOY_SELECT || NESCTRL_SELECT==1 || USBKEYBOARD[0x1e]) INPUT_SELECT=1;
  if (JOY_SYSTEM || USB_JOY_SYSTEM || BUTTON_SYSTEM || USBKEYBOARD[0x01] ||  TOUCH_SYSTEM==1 ) INPUT_SYSTEM=1;
*/

  if (DEBUG>2) INPUTREPORT();

 if (digitalRead(IO_BUTTON0)==0) INPUT_START= 1;
 if (digitalRead(IO_BUTTON47)==0) INPUT_SELECT= 1;
 
  if ((NESCTRL_LEFT || USB_LEFT) && INPUT_LEFT==0) INPUT_LEFT=1; else if ((!NESCTRL_LEFT && !USB_LEFT) && INPUT_LEFT==2) INPUT_LEFT=3;
  if ((NESCTRL_RIGHT || USB_RIGHT) && INPUT_RIGHT==0) INPUT_RIGHT=1; else if ((!NESCTRL_RIGHT && !USB_RIGHT) && INPUT_RIGHT==2) INPUT_RIGHT=3;
  if ((NESCTRL_UP || USB_UP) && INPUT_UP==0) INPUT_UP=1; else if ((!NESCTRL_UP && !USB_UP) && INPUT_UP==2) INPUT_UP=3;
  if ((NESCTRL_DOWN || USB_DOWN) && INPUT_DOWN==0) INPUT_DOWN=1; else if ((!NESCTRL_DOWN && !USB_DOWN) && INPUT_DOWN==2) INPUT_DOWN=3;
  
  if ((NESCTRL_A || USB_A) && INPUT_A==0) INPUT_A=1; else if ((!NESCTRL_A && !USB_A) && INPUT_A==2) INPUT_A=3;
  if ((NESCTRL_B || USB_B) && INPUT_B==0) INPUT_B=1; else if ((!NESCTRL_B && !USB_B) && INPUT_B==2) INPUT_B=3;

  if ((NESCTRL_SELECT || USB_SELECT) && INPUT_SELECT==0) INPUT_SELECT=1; else if ((!NESCTRL_SELECT && !USB_SELECT) && INPUT_SELECT==2) INPUT_SELECT=3;
  if ((NESCTRL_START || USB_START) && INPUT_START==0) INPUT_START=1; else if ((!NESCTRL_START && !USB_START) && INPUT_START==2) INPUT_START=3;
  
 
//................................................................................
  //return 1=keypressed, 0=no change
  if (INPUT_UP==1||INPUT_DOWN==1||INPUT_LEFT==1||INPUT_RIGHT==1
  ||INPUT_A==1||INPUT_B==1||INPUT_X==1||INPUT_Y==1
  ||INPUT_START==1||INPUT_SELECT==1||INPUT_SYSTEM==1) {
    
    return 1; 
  } else return 0;  
}
//================================================================================
uint8_t INPUTCLEAR() {
  INPUT_UP=0;         /// ↑
  INPUT_DOWN=0;       /// ↓
  INPUT_LEFT=0;       /// ←
  INPUT_RIGHT=0;      /// →
  INPUT_A=0;          /// X = CTRL = X
  INPUT_B=0;          /// □ = ALT = Z
  INPUT_X=0;          /// O
  INPUT_Y=0;          /// Δ
  INPUT_START=0;      /// ◄ START =  ENTER
  INPUT_SELECT=0;     /// ► SELECT = BACKSPACE  
  INPUT_SYSTEM=0;     /// ■ SYSTEM = ESCAPE
//................................................................................
/*  if (Multiplayer_dev_input_support==2) {
    Player2_INPUT_UP=0;
    Player2_INPUT_DOWN=0;
    Player2_INPUT_LEFT=0;
    Player2_INPUT_RIGHT=0; 
    Player2_INPUT_A=0;    
    Player2_INPUT_Y=0; 
    Player2_INPUT_B=0;   
    Player2_INPUT_X=0;
    Player2_INPUT_START=0;
    Player2_INPUT_SELECT=0;
    Player2_INPUT_SYSTEM=0; 
  }*/
//................................................................................  
  return 1;
}
//================================================================================
uint8_t INPUTREPORT() {
  if (INPUT_UP) printf("↑"); else printf(" ");
  if (INPUT_DOWN) printf("↓"); else printf(" ");
  if (INPUT_LEFT) printf("←"); else printf(" ");
  if (INPUT_RIGHT) printf("→"); else printf(" ");
  printf(" ");
  if (INPUT_A) printf("A"); else printf(" ");
  if (INPUT_B) printf("B"); else printf(" ");
  if (INPUT_X) printf("X"); else printf(" ");
  if (INPUT_Y) printf("Y"); else printf(" ");
  printf(" ");
  if (INPUT_START) printf("◄"); else printf(" ");
  if (INPUT_SYSTEM) printf("■"); else printf(" ");
  if (INPUT_SELECT) printf("►"); else printf(" ");
  printf("\n");

  return 1;
}
//================================================================================
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒

//--------------------------------------------------------------------------------
uint8_t BUTTONSINPUT() {   
  return 1;
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒

//********************************************************************************
