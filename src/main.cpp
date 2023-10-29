/*
 Example sketch for the Logitech F310 gamepad
 */
#include <Arduino.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include "f310_app.h"


#include "mcp_can.h"
#include "m5can_app.h"
#include <eep.h>
#include <m5wifi_f3102can.h>


TFT_eSprite sprite(&M5.Lcd);

int can_tx_test = 0;
int can_show_mode = 0;

int  sens_val  = 0;
int  sens_id   = 0x63; 
char sens_dlc   = 6;
int  sens_cycle = 50;

// 値はデフォルト値でEEPROMの保存値やWifi経由の設定で上書きされる
int eep_can_id    = 0; 
int eep_can_cycle = 0;
int eep_can_para[2];

unsigned char sens_data[] = { 0,0,0,0, 0,0,0,0 };

// CAN IDと周期設定のためのwifi出力が不要なときに止めるためのフラグ
int wifi_setup_flag = 0;
int use_wifi = 0;

// タイマー割り込み
const int timerInterval = 1000; // タイマーの間隔
hw_timer_t *timer = NULL;



void M5_setup(){
    M5.begin();
    M5.Power.begin();

    M5.Speaker.begin();
    M5.Speaker.setVolume(4);

    sprite.setTextColor(GREEN);
    sprite.setColorDepth(8);
    sprite.setTextSize(1);
    sprite.createSprite(M5.Lcd.width(), M5.Lcd.height());
    delay(5);

}

int M5_loop_BtnA(){
    int ret_flag = 0;
    static int prevBtnA = 0;

    static int vol = 0;
    if (M5.BtnA.isPressed() && prevBtnA == 0 ) {

        can_tx_test++;
        if( can_tx_test >= 2 ){ can_tx_test = 0; }
        ret_flag = 1;

         M5.Speaker.tone(1000, 250); // 周波数1000 Hzで0.5秒間音を�?�生

    }
    prevBtnA = M5.BtnA.isPressed();

    return ret_flag;

}

int  M5_loop_BtnB(){
    int ret_flag = 0;
    static int prevBtnB = 0;
    if (M5.BtnB.isPressed() && prevBtnB == 0 ) {

        can_show_mode++;
        if( can_show_mode >= 3 ){ can_show_mode = 0; }
        ret_flag = 1;
         M5.Speaker.tone(1300, 250); // 周波数1000 Hzで0.5秒間音を�?�生
    }    
    prevBtnB = M5.BtnB.isPressed();
    return ret_flag;
    
}

int  M5_loop_BtnC(){
    int ret_flag = 0;
    static int prevBtnC = 0;
    if (M5.BtnC.wasPressed()) {
        wifi_setup_flag = 1;
        ret_flag = 1;
         M5.Speaker.tone(1500, 250); // 周波数1000 Hzで0.5秒間音を�?�生

    }
    return ret_flag;

}


void main_f310Show( TFT_eSprite* sprite ){
    //sens_val = mb1013_getVal();

    (*sprite).setTextSize(1); 
    (*sprite).printf(" F310 GamePad : \n\n");
 
    if( can_show_mode == 2 ){
        Serial.println( " F310 GamePad : " );
    }

}

int M5_btn_loop(){
    int retflag = 0;
    retflag += M5_loop_BtnA();
    retflag += M5_loop_BtnB();
    retflag += M5_loop_BtnC();    

    return retflag;

}
void M5_LCD_loop(){

    sprite.fillScreen(BLACK);
    sprite.setCursor(0, 0);

    sprite.printf("Can Tx Test Msg (press A btn): %d", can_tx_test);
    if( can_tx_test != 0 ){  sprite.printf(" TestMsg sending");
    }else{                   sprite.printf(" No TestMsg"); }
    sprite.printf("\n");

    sprite.printf("Can Show Mode   (press B btn): %d", can_show_mode);
    if( can_show_mode == 0 ){ sprite.printf(" No show"); }
    if( can_show_mode == 1 ){ sprite.printf(" show LCD");}
    if( can_show_mode == 2 ){ sprite.printf(" show LCD & Serial"); }     
    sprite.printf("\n");

    sprite.printf("Wifi start (press C btn): %d", use_wifi);
    if( use_wifi != 0 ){  sprite.printf(" Wifi On for CAN setting ");
    }else{                   sprite.printf(" Wifi Off"); }
    sprite.printf("\n");


    main_f310Show(  &sprite  );
    M5_CanShowLCD( &sprite );

    sprite.pushSprite(0, 0); 
    
}


void main_getParaFromEep(){
  int temp_id    = eep_read_num( EEP_ADDR_ID );
  int temp_cycle = eep_read_num( EEP_ADDR_CYCLE );

  if( 0x00 < temp_id && temp_id < 0x800 ){
    eep_can_id = temp_id;
  }
  if( 5 <= temp_cycle && temp_cycle < 5000 ){
    eep_can_cycle = temp_cycle;
  }

  eep_can_para[0] = eep_can_id;
  eep_can_para[1] = eep_can_cycle;
  Serial.println( "[ EEP Parameter ]" );
  Serial.print( "can_id: " );
  Serial.println( eep_can_id );
  Serial.print( "can_cycle: " );
  Serial.println( eep_can_cycle );

}




void setup() {
    Serial.begin(115200);
    Serial.println("[ Start ]");

    M5_setup();
    eep_init();
    can_init();    
    f310_init();

    main_getParaFromEep();
    Serial.println("");
    Serial.println("[ SetUp End ]");    

    M5.update();     
    M5_btn_loop();
    M5_LCD_loop();
    delay(200);
}


void main_setGamepadToCan_loop(){

    // 0,1,2,3 -> Up Left Right Down
   // 0,1,2,3 -> Tri, Square, O, X
    // 0,1,2,3 -> LB,RB, LT, RT
    // 0,1,2     -> start, select, PS
    // CAN Message struct 
    //  data0: analogLX, 
    //  data1: analogLY, 
    //  data2: analogRX, 
    //  data3: analogRY
    //  data4 [0,1,2,3,4,5,6,7]:[up, left, right, down, Tri, square, o, x]
    //  data5 [0,1,2,3,4,5,6,7]:[L1, R1, L2, R2, start, select, f310btn]

  int f310input0_3[4];
  int f310input4_7[4];

  unsigned char candata[8];
  unsigned char can1byte=0;

  getButtonStateArr_analogStick( f310input0_3 );    //
  for( int i=0; i<4; i++ ){
    candata[i] = f310input0_3[i];// + 128;
  }

  for( int i=0; i<4; i++){ 
    f310input0_3[i] = 0; 
    f310input4_7[i] = 0; 
  }

  getButtonStateArr_crossKey(    f310input0_3 );   //
  getButtonStateArr_shapeButton( f310input4_7 ); //
  can1byte = 0;
  for( int i=0; i<4; i++ ){
    can1byte |= ( f310input0_3[i] <<  i );
    can1byte |= ( f310input4_7[i] << (i+4) );
  } 
  candata[4] = can1byte;

  for( int i=0; i<4; i++){ 
    f310input0_3[i] = 0; 
    f310input4_7[i] = 0; 
  }

  getButtonStateArr_shoulderButton( f310input0_3 ); //
  getButtonStateArr_optionButton(  f310input4_7 );   //
  can1byte = 0;
  for( int i=0; i<4; i++ ){
    can1byte |= ( f310input0_3[i] <<  i );
    can1byte |= ( f310input4_7[i] << (i+4) );
  } 
  candata[5] = can1byte;

  canTxbuf_set( eep_can_id, sens_dlc, eep_can_cycle, candata, 1 );

}
 
void loop() {
    static unsigned long curr_prev=0;
    static int lcd_show_temp = 1;
    unsigned long curr = millis(); // 現在時刻を更新


    if( curr - curr_prev < 1 ){
        return;
    }
    curr_prev = curr;

    M5.update(); 

    can_setTestFlag( can_tx_test , can_show_mode );
    f310_loop();
    //main_sensorGetVal();
    //canTxbuf_set( eep_can_id, sens_dlc, eep_can_cycle, sens_data, 1 );
    main_setGamepadToCan_loop();
    can_loop();
    //sonic_loop();

    int btn_changed = M5_btn_loop();

    if( lcd_show_temp == 1 || btn_changed > 0 || can_show_mode > 0){
        M5_LCD_loop();
        lcd_show_temp = 0;
    }

    if( use_wifi ){
        wifi_loop();
    }    

    if( wifi_setup_flag ){
        wifi_setup( "" );
        wifi_setPara_int( eep_can_para );
        wifi_setup_flag = 0;
        use_wifi = 1;
    }


}
