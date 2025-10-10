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
  if (Ps3.data.analog.button.l2 > 254) {
    maxspeed = speedlimmit; }
  else if (Ps3.data.analog.button.l2 > 0) {
    maxspeed = speedlimmit/3*2; }
  else {maxspeed = speedlimmit/2;}

  // speed limmit
  if (SpeedUp) {speedlimmit = 256; delay(300);}
  if (SpeedDown) {speedlimmit = 128; delay(300);}

  // idk wtf is this
  // if (speed >= 160) {maxspeed = 160;} 
  // if (speed <= 65 ) {maxspeed = 64;}
  // else {maxspeed = speed;}

  // gripper/feture
  if (Grip) {capit();}
  if (ResetPosition) {resetposisiongriper();}
  if (ResetYposition) {resetposisiYongriper();}
    
  griperhead1();
  updateButton();

  controlMecanum(-lx, ly, -rx);

  if (!noButtonPressed()) {
    lastButtonTime = millis();
    idle = false;
    mainDisplay();
  } else {
    if (millis() - lastButtonTime >= idleTimeout){
      if (!idle){
        idle = true;
        idleDisplay();
      }
    } else {
      mainDisplay();
    }
  }
}
