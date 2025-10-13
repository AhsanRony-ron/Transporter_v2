#include "Settings.h"

void onConnect() {
  Serial.println("Connected.");
}

void setup() {
  Serial.begin(115200);
  
  Wire.setClock(400000);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin(MacAddress);

  pinMode(Am1, OUTPUT);
  pinMode(Am2, OUTPUT); 
  pinMode(Bm1, OUTPUT);
  pinMode(Bm2, OUTPUT); 
  pinMode(Cm1, OUTPUT); //mod
  pinMode(Cm2, OUTPUT); //mod
  pinMode(Dm1, OUTPUT); //mod
  pinMode(Dm2, OUTPUT); //mod

  pinMode(STBY, OUTPUT); //mod

  pinMode(PinPWMa, OUTPUT);
  pinMode(PinPWMb, OUTPUT); 
  pinMode(PinPWMc, OUTPUT); //mod
  pinMode(PinPWMd, OUTPUT); //mod

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servocapit.setPeriodHertz(50);
  servoangkat.setPeriodHertz(50);

  servocapit.attach(servo_capit, 500, 2400);
  servoangkat.attach(servo_angkat, 500, 2400);

  ledcSetup(PWMa, frequency, resolution);
  ledcSetup(PWMb, frequency, resolution);
  ledcSetup(PWMc, frequency, resolution); //mod
  ledcSetup(PWMd, frequency, resolution); //mod

  ledcAttachPin(PinPWMa, PWMa);
  ledcAttachPin(PinPWMb, PWMb);
  ledcAttachPin(PinPWMc, PWMc); //mod
  ledcAttachPin(PinPWMd, PWMd); //mod

  servocapit.write(ServoGrip);
  servoangkat.write(G1);
  // servoputar.write(G2);
  u8g2.begin();
  battery_begin();


  digitalWrite(STBY, HIGH); //mod
}


void loop() {
  // if(!Ps3.isConnected()) return;

  // Default joystick
  int ly  = Ps3.data.analog.stick.ly;
  int lx  = Ps3.data.analog.stick.lx;
  int rx  = Ps3.data.analog.stick.rx;
  int ry  = Ps3.data.analog.stick.ry;  

  // speed control
  if (Ps3.data.analog.button.l2 > 240) {
    maxspeed = speedlimmit; }
  else if (Ps3.data.analog.button.l2 > 0) {
    maxspeed = speedlimmit*3/4; }
  else {maxspeed = speedlimmit/2;}

  // speed limmit
  if (SpeedUp) {speedlimmit = 255; delay(300);}
  if (SpeedDown) {speedlimmit = 127; delay(300);}
  if (StanBY) {STBYStatus = !STBYStatus; digitalWrite(STBY, STBYStatus ? HIGH : LOW); delay(300);}
  // if (BreakUp) {BreakStatus = !BreakStatus; MIN_BRAKE_PWM = BreakStatus ? 30 : 255; delay(300);}

  // idk wtf is this
  // if (speed >= 160) {maxspeed = 160;} 
  // if (speed <= 65 ) {maxspeed = 64;}
  // else {maxspeed = speed;}

  // gripper/feture
  if (Grip) {capit();}
  if (ResetPosition) {ReposisionGripper();}
  // if (ResetYposition) {resetposisiYongriper();}
    
  griper();
  updateButton();
  battery_update();

  controlMecanum(-lx, ly, -rx);

  if (idlebutton) {sl = !sl; delay(200);}
  if (sl) {idleDisplay();}
  else {mainDisplay();}
}

// ===== API =====
void battery_begin() {
  analogReadResolution(ADC_BITS);
  analogSetPinAttenuation(BAT_PIN, ADC_11db); // ADC1, extend input range
  // init buffer with current reading
  uint16_t v = analogRead(BAT_PIN);
  for (int i = 0; i < NUM_SAMPLES; ++i) samples[i] = v;
  sampleIdx = 0;
  samplesSum = (uint32_t)v * NUM_SAMPLES;
  sampleCount = NUM_SAMPLES;

  // compute initial voltage
  float v_adc = ((float)v / (float)ADC_MAX) * VREF;
  batteryVoltage = v_adc * ((R1 + R2) / R2);
  batteryVoltage = batteryVoltage * calib_scale + calib_offset;
}

void battery_update() {
  unsigned long now = millis();
  if (now - lastSampleMillis < SAMPLE_INTERVAL_MS) return; // non-blocking throttle
  lastSampleMillis = now;

  uint16_t raw = analogRead(BAT_PIN); // very fast

  // circular buffer sum update
  samplesSum -= samples[sampleIdx];
  samples[sampleIdx] = raw;
  samplesSum += raw;
  sampleIdx = (sampleIdx + 1) % NUM_SAMPLES;
  if (sampleCount < NUM_SAMPLES) sampleCount++;

  float avgRaw = (float)samplesSum / (float)sampleCount;
  float v_adc = (avgRaw / (float)ADC_MAX) * VREF;
  float v_batt = v_adc * ((R1 + R2) / R2);

  // apply small smoothing (light)
  const float EMA_ALPHA = 0.25f; // 0..1 (bigger = faster)
  v_batt = v_batt * calib_scale + calib_offset;
  batteryVoltage = (EMA_ALPHA * v_batt) + ((1.0f - EMA_ALPHA) * batteryVoltage);
}

// Ambil tegangan terakhir (V)
float getBatteryVoltage() {
  return batteryVoltage;
}