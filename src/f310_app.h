#ifndef F310_APP_H
#define F310_APP_H

#include <Arduino.h>
#include "lf310.h"



#define F310_KEY_UP    (0)
#define F310_KEY_LEFT  (1)
#define F310_KEY_RIGHT (2)
#define F310_KEY_DOWN  (3)
#define F310_BTN_A  (0)
#define F310_BTN_B  (1)
#define F310_BTN_X  (2)
#define F310_BTN_Y  (3)
#define F310_BTN_L1  (0)
#define F310_BTN_R1  (1)
#define F310_BTN_L2  (2)
#define F310_BTN_R2  (3)
#define F310_BTN_STA  (0)
#define F310_BTN_SEL  (1)
#define F310_BTN_PS   (2)

void f310_init();
void f310_loop();

// ボタンのONOFFを配列で取得　
// int buttonArr[4];
// getButtonStateArr_shapeButton( buttonArr );
// みたいな使い方　
void getButtonStateArr_shapeButton( int *buttonValArr );
void getButtonStateArr_shoulderButton( int *buttonValArr );
void getButtonStateArr_optionButton( int *buttonValArr );
void getButtonStateArr_crossKey( int *buttonValArr );
void getButtonStateArr_analogStick( int *buttonValArr );



#endif 