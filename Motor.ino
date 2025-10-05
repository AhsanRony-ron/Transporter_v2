int applyDeadZone(int val, int dz) {
  return (abs(val) < dz) ? 0 : val;
}

void controlMecanum(int forward, int strafe, int rotate) {
  // deadzone
  int vy = applyDeadZone(forward, 15);
  int vx = applyDeadZone(strafe, 15);
  int w  = applyDeadZone(rotate, 10);

  // mapping joystick ke speed
  vy = map(vy, -128, 127, -maxspeed, maxspeed);
  vx = map(vx, -128, 127, -maxspeed, maxspeed);
  w  = map(w,  -128, 127, -maxspeed, maxspeed);

  // hitung motor
  int mFL = vx - vy - w;  // front left  (A)
  int mFR = vx + vy + w;  // front right (B)
  int mRL = vx + vy - w;  // rear left   (C)
  int mRR = vx - vy + w;  // rear right  (D)

  // normalisasi
  int maxCalc = max(max(abs(mFL), abs(mFR)), max(abs(mRL), abs(mRR)));
  if (maxCalc > maxspeed) {
    mFL = mFL * maxspeed / maxCalc;
    mFR = mFR * maxspeed / maxCalc;
    mRL = mRL * maxspeed / maxCalc;
    mRR = mRR * maxspeed / maxCalc;
  }

  // kirim ke motor
  setMotor(0, mFL); // A
  setMotor(1, mFR); // B
  setMotor(2, mRL); // C
  setMotor(3, mRR); // D
}

void setMotor(int id, int speed) {
  int pin1, pin2, pwm;

  switch(id) {
    case 0: // Front Left (A)
      pin1 = Am1; pin2 = Am2; pwm = PWMa; break;
    case 1: // Front Right (B)
      pin1 = Bm1; pin2 = Bm2; pwm = PWMb; break;
    case 2: // Rear Left (C)
      pin1 = Cm1; pin2 = Cm2; pwm = PWMc; break;
    case 3: // Rear Right (D)
      pin1 = Dm1; pin2 = Dm2; pwm = PWMd; break;
    default:
      return;
  }

  if (speed > 0) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else if (speed < 0) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }

  ledcWrite(pwm, abs(speed));
}
