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

int servoSpeed = 5; //ms

// int RY = 0;

// button declarations
bool Grip         = false;
bool Up           = false;
bool Down         = false;
bool idlebutton   = false;
bool Right        = false;
bool ResetPosition = false;
bool StanBY       = false;
bool BreakUp        = false;
bool SpeedUp      = false;
bool SpeedDown    = false;

bool ButtonPressed(){
  return 
    Ps3.data.button.l1 ||
    Ps3.data.button.r2 ||
    Ps3.data.button.r1 ||
    Ps3.data.button.square ||
    Ps3.data.button.cross ||
    Ps3.data.button.r3 ||
    Ps3.data.button.triangle ||
    Ps3.data.button.cross ||
    Ps3.data.analog.stick.ly ||
    Ps3.data.analog.stick.lx ||
    Ps3.data.analog.stick.ry ||
    Ps3.data.analog.stick.rx
  ; 
}

unsigned long lastReposTime = 0;   // waktu terakhir reset
const unsigned long reposDelay = 300; // delay 500 ms antar reset

// Button settings
void updateButton() {\
  Grip           = Ps3.data.button.l1;
  Up             = Ps3.data.button.r2;
  Down           = Ps3.data.button.r1;
  // Left           = Ps3.data.button.l1;
  idlebutton          = Ps3.data.button.select;
  ResetPosition  = Ps3.data.button.square;
  StanBY         = Ps3.data.button.start;
  BreakUp          = Ps3.data.button.r3;
  SpeedUp        = Ps3.data.button.triangle;
  SpeedDown      = Ps3.data.button.cross;
  // RY             = Ps3.data.analog.stick.ry;
}

const int DEADZONE = 20;
int MIN_BRAKE_PWM = 30;

Servo servocapit;
Servo servoangkat;

const int servo_capit = 26;
const int servo_angkat = 27;

int speedlimmit = 128;
int maxspeed, motorFrontLeft, motorFrontRight, motorRearLeft, motorRearRight;

int G1 = ServoUD;
int G2 = ServoGrip;

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
bool STBYStatus = true;
bool sl;
// bool BreakStatus = true;

int frequency = 30000;
int resolution = 8;

// ===== CONFIG =====
const int BAT_PIN = 35;            // ADC1 pin (GPIO34 recommended)
const float R1 = 1000000.0f;        // top resistor (ohm)
const float R2 = 330000.0f;         // bottom resistor (ohm)
const int ADC_BITS = 12;           // 12-bit ADC
const int ADC_MAX = (1 << ADC_BITS) - 1; // 4095
const float VREF = 3.3f;           // nominal reference
const int NUM_SAMPLES = 32;         // moving window size (8 is light & stable)
const unsigned long SAMPLE_INTERVAL_MS = 50; // sample throttle (ms)

// optional calibration (kalibrasi setelah ukur dengan multimeter jika perlu)
float calib_scale = 0.6792453f;
float calib_offset = 3.5660377f;

// ===== internal state =====
static uint16_t samples[NUM_SAMPLES];
static unsigned int sampleIdx = 0;
static uint32_t samplesSum = 0;
static unsigned int sampleCount = 0;
static unsigned long lastSampleMillis = 0;
static float batteryVoltage = 0.0f; // latest computed voltage
#endif