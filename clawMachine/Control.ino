// BROBOT EVO 2 by JJROBOTS
// SELF BALANCE ARDUINO ROBOT WITH STEPPER MOTORS
// License: GPL v2
// Control functions (PID controls, Steppers control...)

float positionPDControl(long actualPos, long setPointPos, float Kpp, float Kdp, int16_t speedM)
{
  float output;
  float P;

  P = constrain(Kpp * float(setPointPos - actualPos), -115, 115); // Limit command
  output = P + Kdp * float(speedM);
  return (output);
}

// TIMER 1 : STEPPER MOTOR1 SPEED CONTROL
ISR(TIMER1_COMPA_vect)
{
  if (dir_M1 == 0) // If we are not moving we dont generate a pulse
    return;
  // We generate 1us STEP pulse
  //SET(PORTE, 6); // STEP MOTOR 1
  digitalWrite(STEP1, HIGH);
  //delay_1us();
  if (dir_M1 > 0)
    steps1--;
  else
    steps1++;
  //CLR(PORTE, 6);
  digitalWrite(STEP1, LOW);
}
// TIMER 3 : STEPPER MOTOR2 SPEED CONTROL
ISR(TIMER3_COMPA_vect)
{
  if (dir_M2 == 0) // If we are not moving we dont generate a pulse
    return;
  // We generate 1us STEP pulse
  //SET(PORTD, 6); // STEP MOTOR 2
  digitalWrite(STEP2, HIGH);
  //delay_1us();
  if (dir_M2 > 0)
    steps2--;
  else
    steps2++;
  //CLR(PORTD, 6);
  digitalWrite(STEP2, LOW);
}


// Set speed of Stepper Motor1
// tspeed could be positive or negative (reverse)
void setMotorSpeedM1(int16_t tspeed)
{
  long timer_period;
  int16_t speed;

  // Limit max speed?

  // WE LIMIT MAX ACCELERATION of the motors
  if ((speed_M1 - tspeed) > MAX_ACCEL)
    speed_M1 -= MAX_ACCEL;
  else if ((speed_M1 - tspeed) < -MAX_ACCEL)
    speed_M1 += MAX_ACCEL;
  else
    speed_M1 = tspeed;

#if MICROSTEPPING==16
  speed = speed_M1 * 50; // Adjust factor from control output speed to real motor speed in steps/second
#else
  speed = speed_M1 * 25; // 1/8 Microstepping
#endif

  if (speed == 0)
  {
    timer_period = ZERO_SPEED;
    dir_M1 = 0;
  }
  else if (speed > 0)
  {
    timer_period = 2000000 / speed; // 2Mhz timer
    dir_M1 = 1;
    //SET(PORTB, 4); // DIR Motor 1 (Forward)
    digitalWrite(DIR1, HIGH);
  }
  else
  {
    timer_period = 2000000 / -speed;
    dir_M1 = -1;
    //CLR(PORTB, 4); // Dir Motor 1
    digitalWrite(DIR1, LOW);
  }
  if (timer_period > 65535)   // Check for minimun speed (maximun period without overflow)
    timer_period = ZERO_SPEED;

  OCR1A = timer_period;
  // Check  if we need to reset the timer...
  if (TCNT1 > OCR1A)
    TCNT1 = 0;
}

// Set speed of Stepper Motor2
// tspeed could be positive or negative (reverse)
void setMotorSpeedM2(int16_t tspeed)
{
  long timer_period;
  int16_t speed;

  // Limit max speed?

  // WE LIMIT MAX ACCELERATION of the motors
  if ((speed_M2 - tspeed) > MAX_ACCEL)
    speed_M2 -= MAX_ACCEL;
  else if ((speed_M2 - tspeed) < -MAX_ACCEL)
    speed_M2 += MAX_ACCEL;
  else
    speed_M2 = tspeed;

#if MICROSTEPPING==16
  speed = speed_M2 * 50; // Adjust factor from control output speed to real motor speed in steps/second
#else
  speed = speed_M2 * 25; // 1/8 Microstepping
#endif

  if (speed == 0)
  {
    timer_period = ZERO_SPEED;
    dir_M2 = 0;
  }
  else if (speed > 0)
  {
    timer_period = 2000000 / speed; // 2Mhz timer
    dir_M2 = 1;
    //CLR(PORTC, 6);   // Dir Motor2 (Forward)
    digitalWrite(DIR2, LOW);
  }
  else
  {
    timer_period = 2000000 / -speed;
    dir_M2 = -1;
    //SET(PORTC, 6);  // DIR Motor 2
    digitalWrite(DIR2, HIGH);
  }
  if (timer_period > 65535)   // Check for minimun speed (maximun period without overflow)
    timer_period = ZERO_SPEED;

  OCR3A = timer_period;
  // Check  if we need to reset the timer...
  if (TCNT3 > OCR3A)
    TCNT3 = 0;
}
