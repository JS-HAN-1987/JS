#include "Bounce2.h"
#include <Math.h>
#include "Servo.h"
Servo myservo;  // create servo object to control a servo


#define JOG_LEFT 4
#define JOG_UP 5
#define JOG_RIGHT 6
#define JOG_DOWN 7
#define PUSH_BTN 11
#define COIN_INS 12 // TRUE
#define X_LIMIT 3
#define Y_LIMIT 14
#define Z_LIMIT 18


#define GRIP_SERVO  13
#define Z_MOTOR1 9
#define Z_MOTOR2 10

enum eBtn{
  eBtnLeft,
  eBtnUp,
  eBtnRight,
  eBtnDown,
  eBtnPush,
  eBtnCoin,
  eBtnXLimit,
  eBtnYLimit,
  eBtnZLimit,
  eBtnMax
};

enum eMotorDir{
  eLeft,
  eUp,
  eRight,
  eDown,
  eLU,
  eUR,
  eRD,
  eDL
};

const uint8_t BUTTON_PINS[eBtnMax] = {JOG_LEFT, JOG_UP, JOG_RIGHT, JOG_DOWN, PUSH_BTN, COIN_INS, X_LIMIT, Y_LIMIT, Z_LIMIT};
Bounce * buttons = new Bounce[eBtnMax];


// NORMAL MODE PARAMETERS (MAXIMUN SETTINGS)
#define MAX_SPEED 550

#define ZERO_SPEED 65535
#define MAX_ACCEL 100      // Maximun motor acceleration (MAX RECOMMENDED VALUE: 20) (default:14)
#define MICROSTEPPING 16   // 8 or 16 for 1/8 or 1/16 driver microstepping (default:16)

// AUX definitions
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define RAD2GRAD 57.2957795
#define GRAD2RAD 0.01745329251994329576923690768489
#define ROT 45.0 * GRAD2RAD

#define ENABLE1  38
#define ENABLE2  56

#define DIR1  55
#define DIR2  61
#define STEP1 54
#define STEP2 60

#define X_MAX_POS 280
#define Y_MAX_POS 290
#define DEFAULT_AXIS_STEPS_PER_UNIT   78.7402


int16_t speed_M1, speed_M2;        // Actual speed of motors
int8_t  dir_M1, dir_M2;            // Actual direction of steppers motors
long timer_old;
float setPointOld = 0;
volatile int32_t steps1;
volatile int32_t steps2;
volatile int32_t posX;
volatile int32_t posY;
long timer_value;
float dt;

float max_speed = MAX_SPEED;
float control_output;
int16_t speed;
bool g_bMotorEnabled = false;


// position control
int32_t target_steps1;
int32_t target_steps2;
int16_t motor1_control;
int16_t motor2_control;
int g_JoggingVel = 75;
int g_HommingVel = 75;


void stop()
{
  digitalWrite(ENABLE1, HIGH);
  digitalWrite(ENABLE2, HIGH);
  g_bMotorEnabled = false;
  
  setMotorSpeedM1(0);
  setMotorSpeedM2(0);
}

#define DEAD_MARGIN 5
void move( eMotorDir aeMotorDir, int aSpeed)
{
  posX = cos(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT - sin(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT;
  posY = (sin(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT + cos(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT) + Y_MAX_POS;
  Serial.print(" steps1:");
  Serial.print(steps1);
  Serial.print(" x:");
  Serial.print(posX);
  Serial.print("\tsteps2:");
  Serial.print(steps2);
  Serial.print(" y:");
  Serial.println(posY);
  
  int dir1=0, dir2=0;
  
  switch( aeMotorDir){
    case eLeft:
      if( posX <= DEAD_MARGIN)
      {
        stop();
        return;
      }
      dir1 = 1;
      dir2 = -1;  
    break;
    case eUp:
      if( posY >= Y_MAX_POS - DEAD_MARGIN)
      {
        stop();
        return;
      }
      dir1 = -1;
      dir2 = -1;
    break;
    case eRight:
      if( posX >= X_MAX_POS - DEAD_MARGIN)
      {
        stop();
        return;
      }
      
      dir1 = -1;
      dir2 = 1;
    break;
    case eDown:
      Serial.print("posY <= DEAD_MARGIN ");
      Serial.println(posY <= DEAD_MARGIN);
      if( posY <= DEAD_MARGIN)
      {
        stop();
        return;
      }
        
      dir1 = 1;
      dir2 = 1;
    break;
    case eLU:
      if( posX <= DEAD_MARGIN && posY >= Y_MAX_POS - DEAD_MARGIN)
      {
        stop();
        return;
      }
      dir2 = -1;
      if( posX <= DEAD_MARGIN)
        dir1 = -1; // UP
      else if( posY >= Y_MAX_POS - DEAD_MARGIN)
        dir1 = 1;
    break;
    case eUR:
      if( posY <= Y_MAX_POS - DEAD_MARGIN && posX >= X_MAX_POS - DEAD_MARGIN)
      {
        stop();
        return;
      }
      dir1 = -1;
      if( posY >= Y_MAX_POS - DEAD_MARGIN)
        dir2 = 1;
      else if( posX >= X_MAX_POS - DEAD_MARGIN)
        dir2 = -1;
    break;
    case eRD:
      if( posX >= X_MAX_POS - DEAD_MARGIN && posY <= DEAD_MARGIN)
      {
        stop();
        return;
      }
      dir2 = 1;
      if( posX >= X_MAX_POS - DEAD_MARGIN)
        dir1 = 1;
      else if( posY <= DEAD_MARGIN)
        dir1 = -1;
    break;
    case eDL:
      if( posY <= DEAD_MARGIN && posX <= DEAD_MARGIN)
      {
        stop();
        return;
      }
      
      dir1 = 1;
      if( posY <= DEAD_MARGIN)
      {
        Serial.println("dl -> left");
        dir2 = -1;
      }
      else if( posX <= DEAD_MARGIN)
      {
        Serial.println("dl -> down");
        dir2 = 1;
      }
    break;
    default:
    break;
  }

  if( !g_bMotorEnabled)
  {
    digitalWrite(ENABLE1, LOW);   // Enable stepper drivers
    digitalWrite(ENABLE2, LOW);   // Enable stepper drivers
    g_bMotorEnabled = true;
  }
  
  int16_t motor1 = aSpeed*dir1;
  int16_t motor2 = aSpeed*dir2;
  // Limit max speed (control output)
  motor1 = constrain(motor1, -MAX_SPEED, MAX_SPEED);
  motor2 = constrain(motor2, -MAX_SPEED, MAX_SPEED);
  // NOW we send the commands to the motors
  setMotorSpeedM1(motor1);
  setMotorSpeedM2(motor2);
}

void home()
{
  Serial.println("homing...");
  
  if( digitalRead(Z_LIMIT))
    Serial.println("touched z sensor!");
    
  MotorZUp();
  while( 1)
  {
    buttons[eBtnZLimit].update();
    if( buttons[eBtnZLimit].pressed())
      break;
    Serial.println("z sensor is not touched!");
  }
  Serial.println(digitalRead(Z_LIMIT));
  MotorZStop();
  
  int i=0;
  for( i=0; i<2; i++)
  {
    Serial.println("homing...1");
    int homingVel = g_HommingVel;
    if( i==1)
    {
      homingVel = 25;
      delay(300);
    }
    Serial.println(homingVel);
    int t1 = millis();

    digitalWrite(ENABLE1, LOW);   // Enable stepper drivers
    digitalWrite(ENABLE2, LOW);   // Enable stepper drivers
    setMotorSpeedM1(0);
    setMotorSpeedM2(homingVel);
    while( millis() < (t1 + 200));
    stop();
    Serial.println("homing...2");
    digitalWrite(ENABLE1, LOW);   // Enable stepper drivers
    digitalWrite(ENABLE2, LOW);   // Enable stepper drivers
    setMotorSpeedM1(0);
    setMotorSpeedM2(-homingVel);
    Serial.println("homing...3");
    while( 1){
      if( !digitalRead(X_LIMIT) && digitalRead(Y_LIMIT))
        break;
      if( !digitalRead(X_LIMIT))
      {
        Serial.println("homing...4");
        stop();
        digitalWrite(ENABLE1, LOW);   // Enable stepper drivers
        digitalWrite(ENABLE2, LOW);   // Enable stepper drivers
        setMotorSpeedM1(-homingVel);
        setMotorSpeedM2(-homingVel);
      }
      if( digitalRead(Y_LIMIT))
      {
        Serial.println("homing...5");
        stop();
        digitalWrite(ENABLE1, LOW);   // Enable stepper drivers
        digitalWrite(ENABLE2, LOW);   // Enable stepper drivers
        setMotorSpeedM1(homingVel);
        setMotorSpeedM2(-homingVel);
      }
    }
    Serial.println("homing...6");
  }
  Serial.println("homing...7");
  
  steps1 = 0;
  steps2 = 0;

  posX = cos(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT - sin(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT;
  posY = (sin(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT + cos(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT) + Y_MAX_POS;
  /*
  Serial.print("steps1:");
  Serial.print(steps1);
  Serial.print(" x:");
  Serial.print(posX);
  Serial.print("\tsteps2:");
  Serial.print(steps2);
  Serial.print(" y:");
  Serial.println(posY);
  */
  stop( );
  myservo.write(0);
  Serial.println("homing done...");
}


// INITIALIZATION
void setup()
{
  // STEPPER PINS ON JJROBOTS BROBOT BRAIN BOARD
  pinMode(ENABLE1, OUTPUT); // ENABLE MOTORS
  pinMode(ENABLE2, OUTPUT); // ENABLE MOTORS
  
  pinMode(Z_MOTOR1, OUTPUT); // 
  pinMode(Z_MOTOR2, OUTPUT); // 

  
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  digitalWrite(ENABLE1, HIGH);  // Disbale motors
  digitalWrite(ENABLE2, HIGH);  // Disbale motors

  Serial.begin(115200); // Serial output to console
  
  // STEPPER MOTORS INITIALIZATION
  //Serial.println("Stepers init");
  // MOTOR1 => TIMER1
  TCCR1A = 0;                       // Timer1 CTC mode 4, OCxA,B outputs disconnected
  TCCR1B = (1 << WGM12) | (1 << CS11); // Prescaler=8, => 2Mhz
  OCR1A = ZERO_SPEED;               // Motor stopped
  dir_M1 = 0;
  TCNT1 = 0;

  // MOTOR2 => TIMER3
  TCCR3A = 0;                       // Timer3 CTC mode 4, OCxA,B outputs disconnected
  TCCR3B = (1 << WGM32) | (1 << CS31); // Prescaler=8, => 2Mhz
  OCR3A = ZERO_SPEED;   // Motor stopped
  dir_M2 = 0;
  TCNT3 = 0;
  delay(200);

  // Enable TIMERs interrupts
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 interrupt
  TIMSK3 |= (1 << OCIE1A); // Enable Timer1 interrupt

  pinMode(X_LIMIT,INPUT_PULLUP);
  pinMode(Y_LIMIT,INPUT_PULLUP);
  //pinMode(Z_LIMIT,INPUT_PULLUP);
  
  for (int i = 0; i < eBtnMax; i++) {
    buttons[i].attach( BUTTON_PINS[i]);       //setup the bounce instance for the current button
    buttons[i].interval(50);              // interval in ms
  }

  buttons[eBtnXLimit].setActiveHigh();
  buttons[eBtnZLimit].setActiveHigh();
  buttons[eBtnCoin].setActiveHigh();

  posX = cos(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT - sin(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT;
  posY = (sin(ROT)*steps1/DEFAULT_AXIS_STEPS_PER_UNIT + cos(ROT)*steps2/DEFAULT_AXIS_STEPS_PER_UNIT) + Y_MAX_POS;
  myservo.attach(GRIP_SERVO);
  //home();
  timer_old = micros();
}

void initRobot( )
{  
    setMotorSpeedM1(0);
    setMotorSpeedM2(0);

    // RESET steps
    steps1 = 0;
    steps2 = 0;
}



// MAIN LOOP
void loop()
{
  int16_t motor1 = 0;
  int16_t motor2 = 0;
  
  boolean changed = false;
  for (int i = 0; i < eBtnMax; i++)  {
    // Update the Bounce instance :
    buttons[i].update();
  }
  
  if( buttons[eBtnLeft].pressed() && buttons[eBtnUp].pressed()) {
    move(eLU, g_JoggingVel);
  }
  else if( buttons[eBtnUp].pressed() && buttons[eBtnRight].pressed()) {
    move(eUR, g_JoggingVel);
  }
  else if( buttons[eBtnRight].pressed() && buttons[eBtnDown].pressed()) {
    move(eRD, g_JoggingVel);
  }
  else if( buttons[eBtnDown].pressed() && buttons[eBtnLeft].pressed()) {
    move(eDL, g_JoggingVel);
  }
  else if( buttons[eBtnLeft].pressed()){
    move(eLeft, g_JoggingVel);
  }
  else if( buttons[eBtnUp].pressed()){
    move(eUp, g_JoggingVel);
  }
  else if( buttons[eBtnRight].pressed()){
    move(eRight, g_JoggingVel);
  }
  else if( buttons[eBtnDown].pressed()){
    move(eDown, g_JoggingVel);
  }
  else if( buttons[eBtnPush].pressed()){
    Serial.println("push");
    GripMotion();
  }
  else if( buttons[eBtnCoin].pressed()){
    Serial.println("coin");
  }
  else{
    //Serial.println("stop");
    stop();
  }
}

void GripMotion()
{
  // Z down until sensor is pressed.
  Serial.println("MotorZDown");
  MotorZDown();
  delay(1000);
  while( 1)
  {
    //buttons[eBtnZLimit].update();
    buttons[eBtnPush].update();
    //if( buttons[eBtnZLimit].pressed() || buttons[eBtnPush].released())
    if( buttons[eBtnPush].released())
      break;
  }
  MotorZStop();
  Serial.println("gripping");

  // grip
  delay(300);
  myservo.write(90);
  delay(300);

  Serial.println("MotorZUp");
  MotorZUp();
  delay(1000);
  while( 1)
  {
    buttons[eBtnZLimit].update();
    if( buttons[eBtnZLimit].pressed())
      break;
  }
  MotorZStop();

  // move to drop box
  while(1)
  {
    move(eDL, g_JoggingVel);
    if( posY <= DEAD_MARGIN && posX <= DEAD_MARGIN)
      break;
  }
  stop();

  Serial.println("arrived at drop box");
  Serial.println("release grip");
  // grip
  delay(300);
  myservo.write(0);
  delay(300);
}

void MotorZDown()
{
  digitalWrite(Z_MOTOR2, LOW);   // Enable stepper drivers
  digitalWrite(Z_MOTOR1, HIGH);   // Enable stepper drivers
}

void MotorZUp()
{
  digitalWrite(Z_MOTOR1, LOW);   // Enable stepper drivers
  digitalWrite(Z_MOTOR2, HIGH);   // Enable stepper drivers
}

void MotorZStop()
{
  digitalWrite(Z_MOTOR1, LOW);   // Enable stepper drivers
  digitalWrite(Z_MOTOR2, LOW);   // Enable stepper drivers
}
