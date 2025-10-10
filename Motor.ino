// ======== HELPERS ========
int applyDeadZone(int val){
  return (abs(val) < DEADZONE) ? 0 : val;
}

// ======== CONTROL MECANUM (panggil tiap loop, input joystick -128..127) ========
void controlMecanum(int forward, int strafe, int rotate){
  int vy = applyDeadZone(forward);
  int vx = applyDeadZone(strafe);
  int w  = applyDeadZone(rotate);

  vy = map(vy, -128, 127, -maxspeed, maxspeed);
  vx = map(vx, -128, 127, -maxspeed, maxspeed);
  w  = map(w,  -128, 127, -maxspeed, maxspeed);

  int mFL = vx - vy - w;
  int mFR = vx + vy + w;
  int mRL = vx + vy - w;
  int mRR = vx - vy + w;

  int maxCalc = max(max(abs(mFL), abs(mFR)), max(abs(mRL), abs(mRR)));
  if (maxCalc > maxspeed) {
    mFL = mFL * maxspeed / maxCalc;
    mFR = mFR * maxspeed / maxCalc;
    mRL = mRL * maxspeed / maxCalc;
    mRR = mRR * maxspeed / maxCalc;
  }

  motorFrontLeft  = mFL;
  motorFrontRight = mFR;
  motorRearLeft   = mRL;
  motorRearRight  = mRR;

  int moveMag = abs(vx) + abs(vy) + abs(w);

  setMotor(0, mFL, moveMag);
  setMotor(1, mFR, moveMag);
  setMotor(2, mRL, moveMag);
  setMotor(3, mRR, moveMag);
}

// ======== SETMOTOR: RIGID BRAKE ONLY (SIMPLIFIED) ========
void setMotor(int id, int speed, int moveMag){
  int pin1, pin2, pwm;
  switch(id){
    case 0: pin1 = Am1; pin2 = Am2; pwm = PWMa; break;
    case 1: pin1 = Bm1; pin2 = Bm2; pwm = PWMb; break;
    case 2: pin1 = Cm1; pin2 = Cm2; pwm = PWMc; break;
    case 3: pin1 = Dm1; pin2 = Dm2; pwm = PWMd; break;
    default: return;
  }

  // Jika ada perintah gerak -> drive normal
  if (speed != 0){
    if (speed > 0){
      digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW);
    } else {
      digitalWrite(pin1, LOW); digitalWrite(pin2, HIGH);
    }
    ledcWrite(pwm, abs(speed));
    return;
  }

  // speed == 0 -> RIGID BRAKE: selalu short-brake dengan level tetap (MIN_BRAKE_PWM)
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  ledcWrite(pwm, MIN_BRAKE_PWM);
}

// ======== PRESET HELPERS ========
void forward(int s){ controlMecanum(s, 0, 0); }
void backward(int s){ controlMecanum(-s, 0, 0); }
void strafeRight(int s){ controlMecanum(0, s, 0); }
void strafeLeft(int s){ controlMecanum(0, -s, 0); }
void rotateCW(int s){ controlMecanum(0, 0, s); }
void rotateCCW(int s){ controlMecanum(0, 0, -s); }
