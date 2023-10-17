

#include <f310_app.h>


USB Usb;
LF310 lf310(&Usb);

uint8_t oldX = 128;
uint8_t oldY = 128;
uint8_t oldZ = 128;
uint8_t oldRz = 128;

uint8_t oldBtnA = 0;
uint8_t oldBtnB = 0;
uint8_t oldBtnX = 0;
uint8_t oldBtnY = 0;
uint8_t oldBtnLB = 0;
uint8_t oldBtnRB = 0;
uint8_t oldBtnLT = 0;
uint8_t oldBtnRT = 0;
uint8_t oldDpad = 0;

uint8_t oldBtnStart = 0;
uint8_t oldBtnBack  = 0;

// default left dir 0 -> right 255 , up dir 0 -> down 255

int f310_analogLX=0;
int f310_analogLY=0;
int f310_analogRX=0;
int f310_analogRY=0;

float f310_analogLX_std=0;
float f310_analogLY_std=0;
float f310_analogRX_std=0;
float f310_analogRY_std=0;

// buttonVal 
int f310_crossKey = 0;       // 0,1,2,3 -> Up Left Right Down
int f310_abxyButton = 0;    // 0,1,2,3 -> Tri, Square, O, X
int f310_shoulderButton = 0; // 0,1,2,3 -> LB,RB, LT, RT
int f310_optionButton = 0;   // 0,1,2     -> start, select, PS
const int f310_crossKey_size = 4;
const int f310_abxyButton_size = 4;
const int f310_shoulderButton_size = 4;
const int f310_optionButton_size = 3;


void f310_init(){
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Starting Logitech F310 gamepad");

        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");
                
        // Set this to higher values to enable more debug information
        // minimum 0x00, maximum 0xff, default 0x80
        // UsbDEBUGlvl = 0xff;
}

int analog2val( int analogval ){
  int ret;

  ret = (analogval*100/127);
  if( ret >= 100 ){ ret = 100; }
  else if( ret <= -100 ){ ret = -100; }

  return ret;
}

int setButtonDown( int* buttonVal, int num ){ return *buttonVal |=  (1 << num); }
int setButtonUp(   int* buttonVal, int num ){ return *buttonVal &= ~(1 << num); }

int getButtonState( int buttonVal, int num ){ 
  return  ( (buttonVal &=  (1 << num)) >> num ) & 0x1; 
}

void getButtonStateArr( int buttonVal, int *buttonValArr, int size ){ 
  for( int i=0; i<size; i++  ){
    buttonValArr[i] = getButtonState( buttonVal, i );
  }
}

void getButtonStateArr_shapeButton( int *buttonValArr ){ // size = 4
  getButtonStateArr( f310_abxyButton, buttonValArr, f310_abxyButton_size );
}
void getButtonStateArr_shoulderButton( int *buttonValArr ){ // size = 4
  getButtonStateArr( f310_shoulderButton, buttonValArr, f310_shoulderButton_size );
}
void getButtonStateArr_optionButton( int *buttonValArr ){ // size = 3
  getButtonStateArr( f310_optionButton, buttonValArr, f310_optionButton_size );
}
void getButtonStateArr_crossKey( int *buttonValArr ){ // size = 4
  getButtonStateArr( f310_crossKey, buttonValArr, f310_crossKey_size );
}
void getButtonStateArr_analogStick( int *buttonValArr ){
  *(buttonValArr + 0) = f310_analogLX;
  *(buttonValArr + 1) = f310_analogLY;
  *(buttonValArr + 2) = f310_analogRX;
  *(buttonValArr + 3) = f310_analogRY;

}
void f310_input(){

    f310_analogLX = lf310.lf310Data.X; 
    f310_analogLY = lf310.lf310Data.Y; 
    f310_analogRX = lf310.lf310Data.Z; 
    f310_analogRY = lf310.lf310Data.Rz; 


    if( lf310.buttonClickState.Abutton - oldBtnA > 0 ) setButtonDown( &f310_abxyButton, F310_BTN_A );
    if( lf310.buttonClickState.Bbutton - oldBtnB > 0 ) setButtonDown( &f310_abxyButton, F310_BTN_B );
    if( lf310.buttonClickState.Xbutton - oldBtnX > 0 ) setButtonDown( &f310_abxyButton, F310_BTN_X );
    if( lf310.buttonClickState.Ybutton - oldBtnY > 0 ) setButtonDown( &f310_abxyButton, F310_BTN_Y );
    if( lf310.buttonClickState.Abutton - oldBtnA < 0 ) setButtonUp(   &f310_abxyButton, F310_BTN_A );
    if( lf310.buttonClickState.Bbutton - oldBtnB < 0 ) setButtonUp(   &f310_abxyButton, F310_BTN_B );
    if( lf310.buttonClickState.Xbutton - oldBtnX < 0 ) setButtonUp(   &f310_abxyButton, F310_BTN_X );
    if( lf310.buttonClickState.Ybutton - oldBtnY < 0 ) setButtonUp(   &f310_abxyButton, F310_BTN_Y);

    if( lf310.buttonClickState.LBbutton - oldBtnLB > 0 ) setButtonDown( &f310_shoulderButton, F310_BTN_L1 );
    if( lf310.buttonClickState.RBbutton - oldBtnRB > 0 ) setButtonDown( &f310_shoulderButton, F310_BTN_R1 );
    if( lf310.buttonClickState.LTbutton - oldBtnLT > 0 ) setButtonDown( &f310_shoulderButton, F310_BTN_L2 );
    if( lf310.buttonClickState.RTbutton - oldBtnRT > 0 ) setButtonDown( &f310_shoulderButton, F310_BTN_R2 );
    if( lf310.buttonClickState.LBbutton - oldBtnLB < 0 ) setButtonUp(   &f310_shoulderButton, F310_BTN_L1 );
    if( lf310.buttonClickState.RBbutton - oldBtnRB < 0 ) setButtonUp(   &f310_shoulderButton, F310_BTN_R1 );
    if( lf310.buttonClickState.LTbutton - oldBtnLT < 0 ) setButtonUp(   &f310_shoulderButton, F310_BTN_L2 );
    if( lf310.buttonClickState.RTbutton - oldBtnRT < 0 ) setButtonUp(   &f310_shoulderButton, F310_BTN_R2 );

    if( lf310.lf310Data.btn.dPad == DPAD_UP    && oldDpad != DPAD_UP    ) setButtonDown( &f310_crossKey, F310_KEY_UP );
    if( lf310.lf310Data.btn.dPad == DPAD_LEFT  && oldDpad != DPAD_LEFT  ) setButtonDown( &f310_crossKey, F310_KEY_LEFT );
    if( lf310.lf310Data.btn.dPad == DPAD_RIGHT && oldDpad != DPAD_RIGHT ) setButtonDown( &f310_crossKey, F310_KEY_RIGHT );
    if( lf310.lf310Data.btn.dPad == DPAD_DOWN  && oldDpad != DPAD_DOWN  ) setButtonDown( &f310_crossKey, F310_KEY_DOWN );
    if( lf310.lf310Data.btn.dPad != DPAD_UP    && oldDpad == DPAD_UP    ) setButtonUp( &f310_crossKey,   F310_KEY_UP );
    if( lf310.lf310Data.btn.dPad != DPAD_LEFT  && oldDpad == DPAD_LEFT  ) setButtonUp( &f310_crossKey,   F310_KEY_LEFT );
    if( lf310.lf310Data.btn.dPad != DPAD_RIGHT && oldDpad == DPAD_RIGHT ) setButtonUp( &f310_crossKey,   F310_KEY_RIGHT );
    if( lf310.lf310Data.btn.dPad != DPAD_DOWN  && oldDpad == DPAD_DOWN  ) setButtonUp( &f310_crossKey,   F310_KEY_DOWN );

    if( lf310.buttonClickState.Startbutton - oldBtnStart > 0 ) setButtonDown( &f310_optionButton, F310_BTN_STA );
    if( lf310.buttonClickState.Backbutton  - oldBtnBack  > 0 ) setButtonDown( &f310_optionButton, F310_BTN_SEL );
    if( lf310.buttonClickState.Startbutton - oldBtnStart < 0 ) setButtonUp(   &f310_optionButton, F310_BTN_STA );
    if( lf310.buttonClickState.Backbutton  - oldBtnBack  < 0 ) setButtonUp(   &f310_optionButton, F310_BTN_SEL );

    oldBtnA = lf310.buttonClickState.Abutton; 
    oldBtnB = lf310.buttonClickState.Bbutton; 
    oldBtnX = lf310.buttonClickState.Xbutton; 
    oldBtnY = lf310.buttonClickState.Ybutton; 
    oldBtnLB = lf310.buttonClickState.LBbutton; 
    oldBtnRB = lf310.buttonClickState.RBbutton; 
    oldBtnLT = lf310.buttonClickState.LTbutton; 
    oldBtnRT = lf310.buttonClickState.RTbutton; 
    oldDpad = lf310.lf310Data.btn.dPad;
    oldBtnStart = lf310.buttonClickState.Startbutton;
    oldBtnBack =  lf310.buttonClickState.Backbutton;



}


void f310_loop(){
    /*
     * These four variable hold the "old" values of the joysticks so that action can be taken
     * only if they change.
     */
     
    Usb.Task();
    if (lf310.connected()) {
      f310_input();
    }


    Serial.print(f310_analogLX);
    Serial.print(" : ");
    Serial.print(f310_analogLY );
    Serial.print(" : ");
    Serial.print(f310_analogRX);
    Serial.print(" : ");
    Serial.print(f310_analogRY  );
    Serial.print(" : ");


    Serial.print(f310_crossKey);
    Serial.print(" : ");
    Serial.print(f310_abxyButton);
    Serial.print(" : ");
    Serial.print(f310_shoulderButton);
    Serial.print(" : ");
    Serial.print(f310_optionButton);
    Serial.println("");



        // if (lf310.lf310Data.X != oldX) {
        //   Serial.print("Left Joystick X: ");
        //   Serial.println(lf310.lf310Data.X);
        //   oldX = lf310.lf310Data.X;
        // }

        // if (lf310.lf310Data.Y != oldY) {
        //   Serial.print("Left Joystick Y: ");
        //   Serial.println(lf310.lf310Data.Y);
        //   oldY = lf310.lf310Data.Y;
        // }

        // if (lf310.lf310Data.Z != oldZ) {
        //   Serial.print("Right Joystick X: ");
        //   Serial.println(lf310.lf310Data.Z);
        //   oldZ = lf310.lf310Data.Z;
        // }

        // if (lf310.lf310Data.Rz != oldRz) {
        //   Serial.print("Right Joystick Y: ");
        //   Serial.println(lf310.lf310Data.Rz);
        //   oldRz = lf310.lf310Data.Rz;
        // }

      
        // if (lf310.buttonClickState.Xbutton) {
        //     lf310.buttonClickState.Xbutton= 0; // Clear event
        //     Serial.println(F("X button"));
        // }
      
        // if (lf310.buttonClickState.Abutton) {
        //     lf310.buttonClickState.Abutton= 0; // Clear event
        //     Serial.println(F("A button"));
        // }
      
        // if (lf310.buttonClickState.Bbutton) {
        //     lf310.buttonClickState.Bbutton= 0; // Clear event
        //     Serial.println(F("B button"));
        // }
      
        // if (lf310.buttonClickState.Ybutton) {
        //     lf310.buttonClickState.Ybutton= 0; // Clear event
        //     Serial.println(F("Y button"));
        // }
      
        // if (lf310.buttonClickState.LBbutton) {
        //     lf310.buttonClickState.LBbutton= 0; // Clear event
        //     Serial.println(F("LB button"));
        // }
      
        // if (lf310.buttonClickState.RBbutton) {
        //     lf310.buttonClickState.RBbutton= 0; // Clear event
        //     Serial.println(F("RB button"));
        // }
      
        // if (lf310.buttonClickState.LTbutton) {
        //     lf310.buttonClickState.LTbutton= 0; // Clear event
        //     Serial.println(F("LT button"));
        // }
      
        // if (lf310.buttonClickState.RTbutton) {
        //     lf310.buttonClickState.RTbutton= 0; // Clear event
        //     Serial.println(F("RT button"));
        // }
      
        // if (lf310.buttonClickState.Backbutton) {
        //     lf310.buttonClickState.Backbutton= 0; // Clear event
        //     Serial.println(F("Back button"));
        // }
      
        // if (lf310.buttonClickState.Startbutton) {
        //     lf310.buttonClickState.Startbutton= 0; // Clear event
        //     Serial.println(F("Start button"));
        // }
      
        // if (lf310.buttonClickState.LJSP) {
        //     lf310.buttonClickState.LJSP= 0; // Clear event
        //     Serial.println(F("Left Joystick deprressed"));
        // }
      
        // if (lf310.buttonClickState.RJSP) {
        //     lf310.buttonClickState.RJSP= 0; // Clear event
        //     Serial.println(F("Right Joystick deprressed"));
        // }
        
        // switch (lf310.lf310Data.btn.dPad) {
        //     case DPAD_UP:
        //         Serial.println(F("Up"));
        //         break;
        //     case DPAD_RIGHT:
        //         Serial.println(F("Right"));
        //         break;
        //     case DPAD_DOWN:
        //         Serial.println(F("Down"));
        //         break;
        //     case DPAD_LEFT:
        //         Serial.println(F("Left"));
        //         break;
        //     case DPAD_OFF:
        //         break;
        //     default:
        //         Serial.print(F("Unknown state: "));
        //         PrintHex<uint8_t > (lf310.lf310Data.btn.dPad, 0x80);
        //         Serial.println();
        //         break;
        // }
    

}