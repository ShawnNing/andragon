#include <Servo.h>
#include <Scheduler.h>

int noseLed = 11;

int eyeLedR = 4;
int eyeLedG = 12;
int eyeLedB = 8;

int eyebrowLPin = 5;
int eyebrowRPin = 6;
int earPin = 10;

int buttonL = 2;
int buttonR = 3;

Servo eyebrowServoL;
Servo eyebrowServoR;
Servo earServo;

boolean buttonLeftPressed = false;
boolean buttonRightPressed = false;

int eyeJobId = 0;
int earJobId = 0;
int eyebrowLJobId = 0;
int eyebrowRJobId = 0;

int eyebrow_left_close = -20;
int eyebrow_left_open = 80;
int eyebrow_right_close = 70;
int eyebrow_right_open = 0;

int eyebrow_blink_time = 1500;

void setup() {       
  Serial.begin(9600);  

  pinMode(noseLed, OUTPUT);
  pinMode(eyeLedR, OUTPUT);
  pinMode(eyeLedB, OUTPUT);
  pinMode(eyeLedG, OUTPUT);

  pinMode(buttonL, INPUT);
  digitalWrite(buttonL, HIGH);

  pinMode(buttonR, INPUT);
  digitalWrite(buttonR, HIGH);

  //  attachInterrupt(0, debounce_left, CHANGE);
  //  attachInterrupt(1, debounce_right, CHANGE);

  eyebrowServoL.attach(eyebrowLPin);
  eyebrowServoR.attach(eyebrowRPin);
  earServo.attach(earPin);

  digitalWrite(eyeLedR, HIGH);
  digitalWrite(eyeLedG, HIGH);
  digitalWrite(eyeLedB, HIGH);
  
  Sch.init();

  // eyeJobId = Sch.addTask(eyeJob, 0, 1000, 1);
  earJobId = Sch.addTask(earJob, 0, 2000, 1);
  eyebrowLJobId = Sch.addTask(eyebrowLJob, 0, eyebrow_blink_time, 1);
  eyebrowRJobId = Sch.addTask(eyebrowRJob, 0, eyebrow_blink_time, 1);

  Sch.addTask(noseJob, 0, 10, 1);
  Sch.addTask(timerJob, 0, 1, 1);
  Sch.addTask(buttonLJob, 0, 10, 1);
  Sch.addTask(buttonRJob, 0, 10, 1);

  Sch.start();
}

void noseJob()
{
  static int noseRedness = 0;
  static boolean noseState = false;
  
  analogWrite(noseLed, noseRedness);
  if (noseState){
    noseRedness++;
    if (noseRedness > 255) {
      noseState = !noseState;
      noseRedness = 255;
    }
  }
  else{
    noseRedness--;
    if (noseRedness < 0) {
      noseState = !noseState;
      noseRedness = 0;
    }
  }
}

void eyeJob()
{
  static boolean eyeState = false;
  int signal = eyeState ? HIGH : LOW ;

  digitalWrite(eyeLedR, signal);
  digitalWrite(eyeLedG, signal);
  digitalWrite(eyeLedB, signal);

  eyeState = !eyeState;
}
/*
//Ear was drven by a stepper, it may be overheat after a few hours, so we changed to continus servo
void earJob_stepper()
{
  static boolean earState = false;
  int angleL = earState ? -30 : 330;
  int angleR = earState ? -30 : 330;

  earServoL.write(angleL);
  earServoR.write(angleR);
  earState = !earState;
}
*/
//Ear is driven by continnus servo
//90 is stop (varieies for each individual servo, this perticular is 94), 180 is fastest forward, 0 is fastest backword
//speed 85 is normal run
//speed 70 is faster run when button pressed
void earJob()
{
  int speed = 85;  

  if (buttonLeftPressed || buttonRightPressed){
    speed = 70;  
  }

  earServo.write(speed);
}


void eyebrowAction(char left_right, char open_close)
{
  //left eye open
  if (open_close == 'O' && left_right == 'L'){
    eyebrowServoL.write(eyebrow_left_open);
  }
  //right eye open
  if (open_close == 'O' && left_right == 'R'){
    eyebrowServoR.write(eyebrow_right_open);
  }
  //left eye close
  if (open_close == 'C' && left_right == 'L'){
    eyebrowServoL.write(eyebrow_left_close);
  }
  //right eye close
  if (open_close == 'C' && left_right == 'R'){
    eyebrowServoR.write(eyebrow_right_close);
  }
  //left eye blink
  if (open_close == 'B' && left_right == 'L'){
    eyebrowServoL.write(eyebrow_left_open);
    Sch.addTask(eyebrowLBlinkJob, 300, 0, 1);
  }
  //right eye blink
  if (open_close == 'B' && left_right == 'R'){
    eyebrowServoR.write(eyebrow_right_open);
    Sch.addTask(eyebrowRBlinkJob, 300, 0, 1);
  }
}

void eyebrowLBlinkJob()
{
  eyebrowAction('L', 'C');
}

void eyebrowRBlinkJob()
{
  eyebrowAction('R', 'C');
}

void eyebrowLJob()
{
  if (buttonRightPressed){
    eyebrowAction('L', 'C');
  }
  else{
    eyebrowAction('L', 'B');
  }
}

void eyebrowRJob()
{
  if (buttonLeftPressed){
    eyebrowAction('R', 'C');
  }
  else{
    eyebrowAction('R', 'B');
  }
}

unsigned long sch_tm;
void timerJob() {
  sch_tm++;
}

void debounce_left() {
  static unsigned long last_interrupt_time_left = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time_left > 100)
  {
    buttonLeftPressed = !buttonLeftPressed;
  }
  last_interrupt_time_left = interrupt_time;
}

void debounce_right() {
  static unsigned long last_interrupt_time_right = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time_right > 100)
  {
    buttonRightPressed = !buttonRightPressed;
  }
  last_interrupt_time_right = interrupt_time;
}

void jobReschedule(){
  int eyeSpeed = 1000;
  int earSpeed = 1000;
  int leftEyebrowSpeed = eyebrow_blink_time;
  int rightEyebrowSpeed = eyebrow_blink_time;
  
    if (buttonLeftPressed){
      if (buttonRightPressed){
        //both pressed
        eyeSpeed = 250;
        earSpeed = 250;
      }
      else{
        //left pressed
        eyeSpeed = 250;
        earSpeed = 250;
        leftEyebrowSpeed = eyebrow_blink_time / 2;
        rightEyebrowSpeed = eyebrow_blink_time / 2;
      }
    }
    else{
      if (buttonRightPressed){
        //right pressed
        eyeSpeed = 250;
        earSpeed = 250;
        leftEyebrowSpeed = eyebrow_blink_time / 2;
        rightEyebrowSpeed = eyebrow_blink_time / 2;
      }
      else{
        //none pressed
      }
    }
//  Sch.editTask(eyeJobId, eyeJob, 0, eyeSpeed, 1);
  Sch.editTask(earJobId, earJob, 0, earSpeed, 1);
  Sch.editTask(eyebrowLJobId, eyebrowLJob, 0, leftEyebrowSpeed, 1);
  Sch.editTask(eyebrowRJobId, eyebrowRJob, 0, rightEyebrowSpeed, 1);
}

void buttonLJob(){
  static unsigned long last_tm = 0;
  static int last_state = 0;

  int state = digitalRead(buttonL);
  if (last_state != state){

    if ((sch_tm-last_tm)>100){
      buttonLeftPressed =! buttonLeftPressed;
      Serial.println("trigged left");
      Serial.println(buttonLeftPressed);    
      jobReschedule();  
    }
    last_tm = sch_tm;
    last_state = state;
  }
}

void buttonRJob(){
  static unsigned long last_tm = 0;
  static int last_state = 0;

  int state = digitalRead(buttonR);
  if (last_state != state){

    if ((sch_tm-last_tm)>100){
      buttonRightPressed =! buttonRightPressed;
      Serial.println("trigged right");
      Serial.println(buttonRightPressed);      
      jobReschedule();  
    }
    last_tm = sch_tm;
    last_state = state;
  }
}

void loop()
{
  Sch.dispatchTasks(); 
}



