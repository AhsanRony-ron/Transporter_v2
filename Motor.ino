bool invertMotor[4] = { true, true, false, false }; // A,B terbalik (sesuaikan)

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
    float f = (float)v / 127.0f;      // 127 -> 1.0, -128 -> sedikit < -1
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

  // hitung kombinasi motor (FL, FR, RL, RR)
  long mFLl = (long)vx - (long)vy - (long)w;
  long mFRl = (long)vx + (long)vy + (long)w;
  long mRLl = (long)vx + (long)vy - (long)w;
  long mRRl = (long)vx - (long)vy + (long)w;

  // normalisasi jika melebihi maxspeed
  long maxCalc = max( max(labs(mFLl), labs(mFRl)), max(labs(mRLl), labs(mRRl)) );
  if (maxCalc > maxspeed) {
    float scale = (float)maxspeed / (float)maxCalc;
    mFLl = (long)roundf(mFLl * scale);
    mFRl = (long)roundf(mFRl * scale);
    mRLl = (long)roundf(mRLl * scale);
    mRRl = (long)roundf(mRRl * scale);
  }

  int target[4];
  target[0] = (int)mFLl;
  target[1] = (int)mFRl;
  target[2] = (int)mRLl;
  target[3] = (int)mRRl;

  // deadband kecil utk target juga
  const int OUT_DEADBAND = 3;
  for (int i=0;i<4;i++){
    if (abs(target[i]) <= OUT_DEADBAND) target[i] = 0;
  }

  // compute move magnitude (bisa gunakan target atau current; pakai target untuk respons)
  int moveMag = abs(vx) + abs(vy) + abs(w);

  // ramping berbasis waktu (HANYA saat magnitude "naik")
  unsigned long now = millis();
  unsigned long dt = 0;
  if (lastRampMillis == 0) {
    dt = 0;
  } else {
    dt = now - lastRampMillis;
  }
  lastRampMillis = now;

  // maxStep sebagai float (bisa fractional), diukur dalam unit PWM
  float maxStepF = accelPerMs * (float)dt; // unit perubahan magnitude maksimum selama dt

  for (int i=0;i<4;i++){
    int cur = currentPWMval[i];
    int tgt = target[i];

    // jika dt==0 atau maxStepF sangat kecil, langsung lompat ke target
    if (dt == 0 || maxStepF < 0.0001f){
      currentPWMval[i] = tgt;
      setMotor(i, currentPWMval[i], moveMag);
      continue;
    }

    int absCur = abs(cur);
    int absTgt = abs(tgt);
    int signCur = (cur > 0) ? 1 : (cur < 0 ? -1 : 0);
    int signTgt = (tgt > 0) ? 1 : (tgt < 0 ? -1 : 0);

    // Kondisi untuk melakukan RAMP:
    //  - magnitude target lebih besar dari sekarang (absTgt > absCur)
    //  - dan arah target sama dengan arah current, atau current==0
    bool shouldRamp = (absTgt > absCur) && ( (signCur == signTgt) || (absCur == 0) );

    if (!shouldRamp){
      // magnitude turun atau arah berbeda => langsung set ke target (brake akan tangani)
      currentPWMval[i] = tgt;
    } else {
      // magnitude naik: naikkan magnitude sampai maxStepF (pertahankan tanda target)
      float deltaMagF = (float)absTgt - (float)absCur; // >0
      if (deltaMagF <= maxStepF){
        currentPWMval[i] = tgt;
      } else {
        int stepInt = (int)roundf(maxStepF); // konversi ke int untuk PWM
        int newAbs = absCur + stepInt;
        if (newAbs > absTgt) newAbs = absTgt; // safety
        currentPWMval[i] = (signTgt >= 0) ? newAbs : -newAbs;
      }
    }

    // kirim output ramped ke motor
    setMotor(i, currentPWMval[i], moveMag);
  }

  // update motor status vars supaya monitoring tetap konsisten
  motorFrontLeft  = currentPWMval[0];
  motorFrontRight = currentPWMval[1];
  motorRearLeft   = currentPWMval[2];
  motorRearRight  = currentPWMval[3];
}

// ======== SETMOTOR: RIGID BRAKE ONLY (SAMA DENGAN SEBELUMNYA) ========
void setMotor(int id, int speed, int moveMag){
  int pin1, pin2, pwm;
  switch(id){
    case 0: pin1 = Am1; pin2 = Am2; pwm = PWMa; break;
    case 1: pin1 = Bm1; pin2 = Bm2; pwm = PWMb; break;
    case 2: pin1 = Cm1; pin2 = Cm2; pwm = PWMc; break;
    case 3: pin1 = Dm1; pin2 = Dm2; pwm = PWMd; break;
    default: return;
  }

  int out = speed; // speed di sini adalah nilai ramped
  if (invertMotor[id]) out = -out;

  if (out != 0){
    if (out > 0){
      digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW);
    } else {
      digitalWrite(pin1, LOW); digitalWrite(pin2, HIGH);
    }
    int pwmVal = constrain(abs(out), 0, 255);
    // gunakan ledcWrite untuk ESP32 (pastikan channel sudah disetup di setup())
    ledcWrite(pwm, pwmVal);
    return;
  }

  // out == 0 -> RIGID BRAKE: short-brake dengan level tetap (MIN_BRAKE_PWM)
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
