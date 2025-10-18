// ======== HELPERS ========
int applyDeadZone(int val){
  return (abs(val) < DEADZONE) ? 0 : val;
}

// ======== CONTROL MECANUM (panggil tiap loop, input joystick -128..127) ========
void controlMecanum(int forward, int strafe, int rotate){
  int vy_in = applyDeadZone(forward);
  int vx_in = applyDeadZone(strafe);
  int w_in  = applyDeadZone(rotate);

  // normalisasi ke [-1..1] menggunakan float; tangani -128 dengan clipping
  auto norm = [](int v)->float {
    float f = (float)v / 127.0f;      // bagi 127 sehingga 127 -> 1.0, -128 -> ~-1.007 -> nanti diclip
    if (f > 1.0f) f = 1.0f;
    if (f < -1.0f) f = -1.0f;
    return f;
  };

  float fy = norm(vy_in);
  float fx = norm(vx_in);
  float fw = norm(w_in);

  // skala ke maxspeed (float) lalu round ke int
  float fMax = (float)maxspeed;
  int vy = (int)roundf(fy * fMax);
  int vx = (int)roundf(fx * fMax);
  int w  = (int)roundf(fw * fMax);

  // gunakan long untuk penjumlahan supaya aman
  long mFLl = (long)vx - (long)vy - (long)w;
  long mFRl = (long)vx + (long)vy + (long)w;
  long mRLl = (long)vx + (long)vy - (long)w;
  long mRRl = (long)vx - (long)vy + (long)w;

  // cari nilai absolut maksimum dari kalkulasi
  long maxCalc = max( max(labs(mFLl), labs(mFRl)), max(labs(mRLl), labs(mRRl)) );

  // scaling float jika melebihi maxspeed => lebih presisi
  if (maxCalc > maxspeed) {
    float scale = (float)maxspeed / (float)maxCalc;
    mFLl = (long)roundf(mFLl * scale);
    mFRl = (long)roundf(mFRl * scale);
    mRLl = (long)roundf(mRLl * scale);
    mRRl = (long)roundf(mRRl * scale);
  }

  // cast ke int; sekarang range dijamin dalam [-maxspeed, maxspeed]
  int mFL = (int)mFLl;
  int mFR = (int)mFRl;
  int mRL = (int)mRLl;
  int mRR = (int)mRRl;

  // deadband kecil utk keluaran motor (boleh diubah, mis. gunakan persentase jika perlu)
  const int OUT_DEADBAND = 3;
  if (abs(mFL) <= OUT_DEADBAND) mFL = 0;
  if (abs(mFR) <= OUT_DEADBAND) mFR = 0;
  if (abs(mRL) <= OUT_DEADBAND) mRL = 0;
  if (abs(mRR) <= OUT_DEADBAND) mRR = 0;

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
    // pastikan PWM ter-constrain ke 0..255 (esp32 ledc 8-bit)
    int pwmVal = constrain(abs(speed), 0, 255);
    ledcWrite(pwm, pwmVal);
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
