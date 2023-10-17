#ifndef _M5SONIC_H_
#define _M5SONIC_H_

#include <Arduino.h>
#include <M5Stack.h>


void IRAM_ATTR sonic_isr();
void sonic_init( int _trigpin, int _echopin );
void sonic_loop();

float sonic_getDistance();
int   sonic_getDistance_int();
unsigned long  sonic_getDuration();





#endif