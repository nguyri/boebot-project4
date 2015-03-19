#define FIND_BOARD 0
#define FOLLOW_BOARD 1
#define LOOK_FOR_PATIENT 5
#define LOAD_PATIENT 2
#define FAR_SIDE 3
#define NEAR_SIDE 6
#define TO_MU 7
#define TERMINATE 4
int State;
int PatientLoaded = 0;
int Timer1 = 0;
int Timer2 = 0;

#include <Servo.h>                           // Include servo library
 
Servo servoLeft;                             // Declare left and right servos
Servo servoRight;

void setup()
{
  Serial.begin(9600);
  Serial.write("I'm in setup! \n");
  tone(4, 3000, 1000);                       // Play tone for 1 second
  delay(500);                               // Delay to finish tone

  pinMode(10, INPUT);  pinMode(9, OUTPUT);   // Left IR LED & Receiver
  pinMode(3, INPUT);  pinMode(2, OUTPUT);    // Right IR LED & Receiver
  pinMode(8, OUTPUT); pinMode(7, OUTPUT);    // Indicator LEDs
  
  servoLeft.attach(13);                      // Attach left signal to pin 13
  servoRight.attach(12);                     // Attach right signal to pin 12
  
  delay (1000);
  
State = FIND_BOARD;
StartDriving();

}


//Project 4 Sample FSM and Skeleton Code Page 1
void loop()
{
  
int FrontObstructionDetected, TimerStart, PatientLoaded, BoardEnd;
// Code to initialize the above int variables appropriately
// Switch statement for handling the "Do" actions
switch(State){
case FIND_BOARD:
FrontObstructionDetected = CheckFrontSensor();
Timer1++;
break;

case FOLLOW_BOARD:
BoardEnd = checkSide();
//DriveParallelToBoard();
Timer2++;
break;

case LOOK_FOR_PATIENT:
FrontObstructionDetected = CheckFrontSensor();
//DriveParallelToBoard();

case LOAD_PATIENT:
EmitBeep();
FlashLight();
break;

case FAR_SIDE:
BoardEnd = checkSide();
//DriveParallelToBoard();

case NEAR_SIDE:
//DriveParallelToBoard();
Timer2--;

break;
case TO_MU:
Timer1--;
break;
} //end of do switch statement


// Switch statement for handling "Entry/Exit" actions
// and state transitions
switch(State){
case FIND_BOARD:
if(FrontObstructionDetected){
stopped(500); // Exit action
forward(500); // shuffle
PivotTurn(); // Exit action
State = FOLLOW_BOARD; // State change
DriveParallelToBoard(); // Entry action   //ALIGNMENT
}

break;

case FOLLOW_BOARD:
if(BoardEnd){
StopDriving(); // Exit action
CornerManoeuvre();
State = LOOK_FOR_PATIENT; // State change
Serial.write ("I'm looking for the patient. \n");
DriveParallelToBoard(); //Entry Action
}

/* 
case FOLLOW_BOARD
if(FrontObstructionDetected) {
  StopDriving();      //found the patient, reel 'er in
  State = LOAD_PATIENT;
  TimerStart = millis();  //reference 0 time for the timer
}
break;
*/
break;

case LOOK_FOR_PATIENT:
if(FrontObstructionDetected) {
  StopDriving();      //found the patient, reel 'er in
  State = LOAD_PATIENT;
  TimerStart = millis();  //reference 0 time for the timer
}

//Project 4 Sample FSM and Skeleton Code Page 2
case LOAD_PATIENT:

if(CheckTimer(TimerStart) >= 10000){
PatientLoaded = 1;
State = FAR_SIDE; // State change
DriveParallelToBoard(); // Entry action
}

break;
case FAR_SIDE:
if(BoardEnd){
  StopDriving(); // Exit action
  CornerManoeuvre();
  State = NEAR_SIDE; // State change
  DriveParallelToBoard(); //Entry Action
}

break;
case NEAR_SIDE:
if(Timer2<=0) {
  PivotTurn(); //Exit Action
  State = TO_MU; // State Change 
  StartDriving(); //Entry Action
}

break;
case TO_MU:
if (Timer1<=0) {
  StopDriving(); // done!
break;
}
} //end of entry/exit switch statement


} // end of main loop


int CheckFrontSensor()
{
int SensorResponse = !irDetect(2, 3, 42000);       // Check for object on right
// Code to poll front sensor and filter out false positives   
  digitalWrite(7, SensorResponse);
return SensorResponse;
}

void StartDriving()
{
  if (PatientLoaded) {
    
  servoRight.writeMicroseconds(1300);  // Robot goes forward
  servoLeft.writeMicroseconds(1690);   // 
  Serial.write("I'm driving backwards!\n"); //
  }
  
  else {
  servoRight.writeMicroseconds(1690);  // Robot goes forward
  servoLeft.writeMicroseconds(1300);   // 
  Serial.write("I'm driving!\n");
  }
}

void DriveParallelToBoard()
{
  if(PatientLoaded) {
  Serial.write("Driving parallel backwards \n");
  int sideDistance;
  sideDistance = !checkSide();
      // Code to poll front sensor and filter out false positives

  if (sideDistance) {  //editted for getting side distance to work
  Serial.write("Good distance \n");
  servoRight.writeMicroseconds(1300);  // Robot goes forward
  servoLeft.writeMicroseconds(1690);   // 
  }
/*  else if (sideDistance == 3) {
  Serial.write("too far! ");
  servoRight.writeMicroseconds(1350);  // right wheel slower than usual, will turn to the right towards board
  servoLeft.writeMicroseconds(1690);   // 
  }
  
  else if (sideDistance == 1) {
  Serial.write("too close ");
  servoRight.writeMicroseconds(1350);  // left wheel slower than usual, will turn to the left away from board
  servoLeft.writeMicroseconds(1640);   // 
  }
*/} // end of if(patientloaded)
    
  else {
  Serial.write("Time to drive parallel \n");
  int sideDistance; // 1 is close, 2 is good, 3 is far
  sideDistance = !checkSide();
      // Code to poll front sensor and filter out false positives

  if (sideDistance) { 
  Serial.write("Good distance \n");
  servoRight.writeMicroseconds(1690);  // Robot goes forward
  servoLeft.writeMicroseconds(1300);   // 
  }
/*  else if (sideDistance == 3) {
  Serial.write("too far! ");
  servoRight.writeMicroseconds(1690);  // left wheel slower than usual, will turn to the left towards board
  servoLeft.writeMicroseconds(1350);   // 
  }
  
  else if (sideDistance == 1) {
  Serial.write("too close ");
  servoRight.writeMicroseconds(1640);  // right wheel slower than usual, will turn to the right away from board
  servoLeft.writeMicroseconds(1300);   // 
  }
*/
  } //end of else
}


int checkSideDistance() { 
  Serial.write ("I'm checking side distance\n");
  
  int distance = 0;
  for ( int frequency = 42000; frequency <= 44000; frequency+=1000) {
      distance += irDetect(9, 10, frequency); // check left sensor       //ALIGNMENT
      Serial.write ("In for loop\n");
      Serial.write ("%i",distance);
  }
  digitalWrite(8, distance);    //display side sensor status on left LED
  return distance;
}

int checkSide() { 
  Serial.write ("I'm checking side bool\n");
  
  int SensorResponse = irDetect(9, 10, 44000);       // Check for object on right
// Code to poll front sensor and filter out false positives   
  digitalWrite(8, SensorResponse);
  return SensorResponse;
  
}  


//Project 4 Sample FSM and Skeleton Code Page 3
int CheckTimer(int TimerStart)
{
  Serial.write("What time is it Mr. Wolf? \n");
  return (millis() - TimerStart);  //return how much time has passed from TimerStart
}


void EmitBeep()
{
  tone(4, 4000, 300);                       // Play tone for a bit
  delay (50);
  tone(4, 2000, 300);
  delay (350);
}

void FlashLight()
{
  digitalWrite(8, 1);   //left is 7, right is 8
  digitalWrite(7, 1);
  delay (500);
  digitalWrite(8, 0);
  digitalWrite(7, 0);
  delay (500);
}
void NavigateToMU()
{
// Code to navigate back to MU, noting that robot is already
// driving before it enters this function and does not need
// to stop driving here. Previously-used functions can be
// re-used here if desired by making use of the PatientLoaded
// variable.
}
void StopDriving()
{
  Serial.write("I stopped.\n");
  servoRight.writeMicroseconds(1500);  // robot stops
  servoLeft.writeMicroseconds(1500);   // 
}
void PivotTurn()
{
  if(PatientLoaded) {
  Serial.write("I'm turning left since I'm going backwards \n");  
  servoLeft.writeMicroseconds(1400);  //left turn
  servoRight.writeMicroseconds(1400);
  delay (585);
  }
  else {
  Serial.write("I'm turning right \n");  
  servoLeft.writeMicroseconds(1600);  //right turn
  servoRight.writeMicroseconds(1600);
  delay (585);
  }
  
}



int irDetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency, 8);              
  delay(1);                                  
  int ir = digitalRead(irReceiverPin);       
  delay(1);                                  
  return ir;                                 
}

void CornerManoeuvre() {
  
  if(PatientLoaded) {
  Serial.write("BoardEndBackwards \n");  
  backwards(100);
  rightTurn();
  backwards(1000); //this will probably need to be changed      //ALIGNMENT
  rightTurn();
  backwards(250);
  }
  
  else {
  Serial.write("Moving around board end \n");  
  forward(1000);
  leftTurn();
  forward(2000); //this will probably need to be changed      //ALIGNMENT
  leftTurn();
  forward(1000);
  }
  
}

void leftTurn () {
    servoLeft.writeMicroseconds(1400);  //left turn
   servoRight.writeMicroseconds(1400);
   delay (585);
}

void rightTurn () {
    servoLeft.writeMicroseconds(1600);  //right turn
   servoRight.writeMicroseconds(1600);
   delay (585);
}

void backwards (int time) {
    servoLeft.writeMicroseconds(1690);  // forwards, aligned for 2 meters straight
   servoRight.writeMicroseconds(1300);
   delay (time);
}

void forward (int time) {
    servoLeft.writeMicroseconds(1300);  // reverse, aligned for 2 meters straight
   servoRight.writeMicroseconds(1690);
   delay (time);
}

void stopped (int time) {
    servoLeft.writeMicroseconds(1500);  // reverse, aligned for 2 meters straight
   servoRight.writeMicroseconds(1500);
   delay (time);
}
