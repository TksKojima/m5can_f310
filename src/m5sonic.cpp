#include "m5sonic.h"

int sonic_echoPin = 21; //init で上書き
int sonic_trigPin = 22;
int sonic_flag = 0;
int sonic_prevEcho = 0;
int sonic_err = 0;
unsigned long sonic_start_time = 0;
unsigned long sonic_end_time = 0;
unsigned long sonic_duration_us = 0;
float sonic_distance_mm = 0;
int  sonic_distance_int = 0;

unsigned long sonic_timeout_cnt = 0;


void IRAM_ATTR sonic_isr(){
    int now_echo = digitalRead( sonic_echoPin );

    if( sonic_flag == 1 ){
        if( sonic_prevEcho == LOW && now_echo == HIGH ){
            sonic_start_time = micros();
            sonic_flag = 2;
        }
    }

    if( sonic_flag == 2 ){
        if( sonic_prevEcho == HIGH && now_echo == LOW ){
            sonic_end_time = micros();
            sonic_duration_us = sonic_end_time - sonic_start_time;
            sonic_flag = 3;
        }
    }

    sonic_prevEcho = now_echo; 
}

void sonic_init( int _trigpin, int _echopin ){
    pinMode(_trigpin, OUTPUT);  // Sets the trigPin as an Output. 将 TrigPin 设置为输出
    pinMode(_echopin, INPUT);  // Sets the echoPin as an Input. 将 echoPin 设置为输入  
    sonic_echoPin = _echopin;
    sonic_trigPin = _trigpin;
    sonic_flag = 0;
    attachInterrupt( sonic_echoPin, sonic_isr, CHANGE);
}

void sonic_loop(){
    unsigned long timeout_us = 150000;

    if( sonic_flag == 0 ){
        sonic_timeout_cnt = micros();
        digitalWrite( sonic_trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite( sonic_trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite( sonic_trigPin, LOW);
        sonic_flag = 1;
    }

    if( sonic_flag == 3 ){
        sonic_distance_mm = (float)sonic_duration_us * 0.34 / 2;
        // Serial.print("[ Sonic: ");
        // Serial.print( sonic_duration_us/*sonic_distance_mm*/ );
        sonic_flag = 0;
    }

    // Error Time out
    if( micros() - sonic_timeout_cnt > timeout_us ){
        sonic_flag = 0;
        sonic_err = 1;
    }

}

float sonic_getDistance(){
    return sonic_distance_mm;
}

int sonic_getDistance_int(){
    sonic_distance_int = sonic_distance_mm;
    return sonic_distance_int;
}


unsigned long sonic_getDuraion(){
   return sonic_duration_us;    
}
