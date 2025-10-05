#ifndef Settings_H
#define Settings_H

#include <Ps3Controller.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// MacAddres
const char* MacAddress = "12:34:56:78:90:ab";

// Servo position Default
int ServoGrip = 100;
int ServoRL = 88;
int ServoUD = 0;

int servoSpeed = 15; //ms

// int RY = 0;

// button declarations
bool Grip         = false;
bool Up           = false;
bool Down         = false;
bool Left         = false;
bool Right        = false;
bool ResetPosition = false;
bool ResetXposition = false;
bool ResetYposition = false;
bool SpeedUp      = false;
bool SpeedDown    = false;

// Button settings
void updateButton() {\
  Grip           = Ps3.data.button.l1;
  Up             = Ps3.data.button.r2;
  Down           = Ps3.data.button.r1;
  // Left           = Ps3.data.button.l1;
  // Right          = Ps3.data.button.r1;
  ResetPosition  = Ps3.data.button.square;
  ResetXposition = Ps3.data.button.cross;
  ResetYposition = Ps3.data.button.r3;
  SpeedUp        = Ps3.data.button.triangle;
  SpeedDown      = Ps3.data.button.cross;
  // RY             = Ps3.data.analog.stick.ry;
}

Servo servocapit;
Servo servoangkat;

const int servo_capit = 26;
const int servo_angkat = 27;

int speedlimmit = 128;
int maxspeed;

int G1 = ServoUD;
int G2 = ServoRL;

int PWMa = 3;
int PWMb = 4;
int PWMc = 5; // placeholder
int PWMd = 6; // placeholder

const int PinPWMc = 15;
const int PinPWMd = 5;
const int PinPWMa = 18; // placeholder
const int PinPWMb = 25; // placeholder

const int Cm2 = 0;
const int Cm1 = 2;
const int Dm2 = 16;
const int Dm1 = 17;
const int Am2 = 23; // placeholder
const int Am1 = 19; // placeholder
const int Bm2 = 32; // placeholder
const int Bm1 = 33; // placeholder

const int STBY = 4;

bool DefaultControl = true;
bool capiton = false;

int frequency = 30000;
int resolution = 8;
#endif
