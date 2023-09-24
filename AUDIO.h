#include "pwm_audio/pwm_audio.c"
#include "driver/i2s.h"

uint8_t PWM_AUDIO_STOPPED=0;

uint32_t BUFSIZE[8]={0};

static int16_t* audio_frame[2];     //8 buffers available for EMULATORS
uint32_t BUFIDX1=0;
uint32_t BUFIDX2=0;

uint8_t pcm_busy=0;
uint8_t gbcaudio_busy=0;

uint8_t AUDIOBUF0_READY;
uint8_t AUDIOBUF1_READY;

#define I2S_DMA_BUFFER_COUNT    2
#define I2S_DMA_BUFFER_SAMPLES  512

#define PWM_RINGBUFFER_SIZE     2048*4 //STEREO

#define INITIAL_RATE            44100
#define INITIAL_CHANNELS        2 //2
#define INITIAL_BITS            16 

//--------------------------------------------------------------------------------
pwm_audio_config_t pac;
uint8_t PWM_AUDIO_INITED=0;
//--------------------------------------------------------------------------------
uint16_t PWM_SAMPRATE_REQUEST=0;  
uint16_t PWM_BITS_REQUEST=0;  
uint16_t PWM_CHANELS_REQUEST=0;  

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//uint8_t VOLUME = 64; //1..255 0=not detected
//--------------------------------------------------------------------------------
int16_t Gain(int16_t s) {
  int32_t v;    
  v=(s * VOLUME)/256; 
  return (int16_t)(v&0xffff);    
}  
//--------------------------------------------------------------------------------
IRAM_ATTR
uint16_t AUDIO_WRITE(uint8_t* buffer_audio,uint16_t bytes_to_write,uint32_t AUDIO_delay) { ///portMAX_DELAY


  #if SOUND_ENABLED

//--------------------------------------------------------------------------------
                        //CORE 0 PWM audio PWM_SAMPRATE_REQUEST:
                        #if PWMAUDIO_ENABLED

                            
                            if (PWM_SAMPRATE_REQUEST) {
                                printf("PWM setting parameters REQUEST: %dHz %dbit %dCH \n",PWM_SAMPRATE_REQUEST,PWM_BITS_REQUEST,PWM_CHANELS_REQUEST);
                                /*if (PWM_AUDIO_INITED) {
                                  pwm_audio_stop();
                                  pwm_audio_deinit();
                                  PWM_AUDIO_INITED=0;
                                } */         

                                //THIS MUST BE CALLED FROM SAME CORE WHERE WAS PWM AUDIO INITED... !!!
                                pwm_audio_deinit();
                                pac.duty_resolution = LEDC_TIMER_10_BIT;
                                pac.gpio_num_left = PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO;
                                pac.ledc_channel_left = LEDC_CHANNEL_0;
                                pac.gpio_num_right = PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO;
                                pac.ledc_channel_right = LEDC_CHANNEL_1;
                                pac.ledc_timer_sel = LEDC_TIMER_0;
                                pac.tg_num = TIMER_GROUP_0; ///must be differt
                                pac.timer_num = TIMER_0;
                                pac.ringbuf_len = PWM_RINGBUFFER_SIZE; ///
                                pwm_audio_init( & pac);
                                if (PWM_CHANELS_REQUEST==1)                       
                                  pwm_audio_set_param(PWM_SAMPRATE_REQUEST, (ledc_timer_bit_t) PWM_BITS_REQUEST, PWM_CHANELS_REQUEST); ///NEEDS CORRECTION
                                else 
                                  pwm_audio_set_param(PWM_SAMPRATE_REQUEST, (ledc_timer_bit_t) PWM_BITS_REQUEST, PWM_CHANELS_REQUEST); ///NEEDS CORRECTION
                                  
                                pwm_audio_start();
                                pwm_audio_set_volume(0.99);
                                PWM_SAMPRATE_REQUEST = 0;
                            }
                        #endif
//--------------------------------------------------------------------------------
  
    uint32_t tmp=0;
    size_t i2s_bytes_written=0;
    uint32_t pwm_bytes_written=0;
    int16_t* dest=(int16_t*)buffer_audio;

    // VOLUME ADJUST OF BUFFER
    for (tmp=0; tmp<bytes_to_write/2;tmp++) {  //STEREO
      int16_t SAMPLE=Gain(*dest);    
      *dest=SAMPLE;
      dest++;  
    }
   

    #if PWMAUDIO_ENABLED
    dest=(int16_t*)buffer_audio;
      for (tmp=0; tmp<bytes_to_write/2;tmp++) {  //STEREO
/*      #ifndef NCAT_TWATCH_V2
        int16_t SAMPLE=(*dest)*8; // *16 = too loud
        *dest=SAMPLE;
        dest++;
      #else */
        int16_t SAMPLE=(*dest)*2;    
        *dest=SAMPLE;
        dest++;
///      #endif
    }

    if (POWER == APP_AUD_PLAYER) {
      if (AUDIO_delay==0 || AUDIO_delay==portMAX_DELAY) AUDIO_delay=200;  ///AUDIO PLAYER requires more time 
    } else {
      if (AUDIO_delay==0 || AUDIO_delay==portMAX_DELAY) AUDIO_delay=5;  ///we can not do wait more than 5ms because delay kill bluetooth...?      
    }
     
    pwm_audio_write((uint8_t *)buffer_audio, bytes_to_write, &pwm_bytes_written, AUDIO_delay); ///
    
  #endif

  #if PWMAUDIO_ENABLED
    return pwm_bytes_written;
  #endif  
#endif  
}  

uint8_t AUDIO_SET_PARAMS(uint16_t sampRate, uint8_t bits, uint8_t channels, uint8_t AUDIOCORE=0) { 
  //default: 44100Hz, 16bit, 2 channels

///  sampRate = HERTZ___(sampRate);


  
  #if SOUND_ENABLED
    // this must be called from core from which was PWM audio inited !
    #if PWMAUDIO_ENABLED



      if (AUDIOCORE==1) {  
        PWM_SAMPRATE_REQUEST=sampRate;
        PWM_BITS_REQUEST=bits;
        PWM_CHANELS_REQUEST=channels;
        ///xSemaphoreGive(audio_mutex); //refresh SOUND
        delay(100);
      } else {    
     
        printf("PWM setting parameters: %dHz 16bit 2CH \n",sampRate);
        if (PWM_AUDIO_INITED) {
          pwm_audio_stop();
          pwm_audio_deinit();
          PWM_AUDIO_INITED=0;
        }          
        pac.duty_resolution    = LEDC_TIMER_8_BIT; ///LEDC_TIMER_10_BIT = crackling sound
        pac.gpio_num_left      = PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO;
        pac.ledc_channel_left  = LEDC_CHANNEL_0;
        pac.gpio_num_right     = PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO;
        pac.ledc_channel_right = LEDC_CHANNEL_1;
        pac.ledc_timer_sel     = LEDC_TIMER_0;
        pac.tg_num             = TIMER_GROUP_1;         // must be differt from previous INIT why?
        pac.timer_num          = TIMER_0;
        pac.ringbuf_len        = PWM_RINGBUFFER_SIZE;  
        pwm_audio_init(&pac);       
        if (channels==1)  
          pwm_audio_set_param(sampRate*2, (ledc_timer_bit_t)bits, channels); 
        else 
          pwm_audio_set_param(sampRate, (ledc_timer_bit_t)bits, channels);   
        pwm_audio_start();
        pwm_audio_set_volume(0.99);   
        PWM_AUDIO_INITED=1;
      }     
    #endif
  #endif
  return 1;      
}
//--------------------------------------------------------------------------------
void AUDIO_SILENCE() {
  #if SOUND_ENABLED   
    #if PWMAUDIO_ENABLED
   ///   pwm_audio_stop();
    #endif  
  #endif      
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void DEInit_audio() {
  #if PWMAUDIO_ENABLED
    pwm_audio_stop();
    pwm_audio_deinit();
  #endif  
  }  
//--------------------------------------------------------------------------------
  
void Init_PWM_audio() {
  #if PWMAUDIO_ENABLED
    if (PWM_AUDIO_INITED) {
      pwm_audio_deinit();
      PWM_AUDIO_INITED=0;
    }  
    printf("Initializing PWM AUDIO...");
    pac.duty_resolution    = LEDC_TIMER_8_BIT; //LEDC_TIMER_10_BIT = crackling sound...
    pac.gpio_num_left      = PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO;
    pac.ledc_channel_left  = LEDC_CHANNEL_0;
    pac.gpio_num_right     = PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO;
    pac.ledc_channel_right = LEDC_CHANNEL_1;
    pac.ledc_timer_sel     = LEDC_TIMER_0;
    pac.tg_num             = TIMER_GROUP_0;        
    pac.timer_num          = TIMER_0;
    pac.ringbuf_len        = PWM_RINGBUFFER_SIZE;   /// default: 1024*8
    pwm_audio_init(&pac);         
    pwm_audio_set_param(INITIAL_RATE, (ledc_timer_bit_t)INITIAL_BITS, INITIAL_CHANNELS);
    pwm_audio_start();
    pwm_audio_set_volume(0.99);       
    PWM_AUDIO_INITED=1;
    printf("[ OK ]\n"); 
  #endif  
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void INIT_SOUND() {
#if SOUND_ENABLED
  printf("Initializing AUDIO...");
  Init_PWM_audio();
  printf("[ OK ]\n");
#endif
}
//--------------------------------------------------------------------------------
