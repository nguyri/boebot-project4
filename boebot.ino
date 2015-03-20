#define FIND_BOARD 0
#define FOLLOW_BOARD 1
#define LOAD_PATIENT 3

#define TO_MU 6
#define TERMINATE 7
int State;
int PatientLoaded = 0;
int Timer1 = 0;
int Timer2 = 0;
int TimerStart= 0;

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
  
int FrontObstructionDetected, CloseToBoard;
// Code to initialize the above int variables appropriately
// Switch statement for handling the "Do" actions
switch(State){
case FIND_BOARD:
FrontObstructionDetected = CheckFrontSensor();
Timer1++;
break;

case FOLLOW_BOARD:
CloseToBoard = checkSide();
FrontObstructionDetected = CheckFrontSensor();
Timer2++;
if (PatientLoaded) 
  Timer2--;
break;

case LOAD_PATIENT:
EmitBeep();
FlashLight();
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

PivotTurn(); // Exit action
State = FOLLOW_BOARD; // State change
DriveParallelToBoard(); // Entry action   //ALIGNMENT
}

break;

case FOLLOW_BOARD:
if(FrontObstructionDetected) { // going to patient
  StopDriving(); // Exit action
  State = LOAD_PATIENT; // State change
  Serial.write ("I'm loading the patient \n");
  TimerStart = millis();
}
if(Timer2<=0) { // coming back to MU
  StopDriving();
  delay(1000);
  PivotTurn();
  State = TO_MU;
  StartDriving();
}
break;

case LOAD_PATIENT:
Serial.write("I'm waiting for the patient.\n");
if(CheckTimer(TimerStart) >= 10000){
  Serial.write("10 seconds are up.\n");
PatientLoaded = 1;
State = FOLLOW_BOARD; // State change
DriveParallelToBoard(); // Entry action
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
  if (!PatientLoaded) {
    Serial.write("Time to drive parallel forwards \n");
    int closeToBoard; // closeToBoard = 0 if nothing is there, 1 if there's something. 
    closeToBoard = !checkSide();
      // Code to poll front sensor and filter out false positives
      
  if (closeToBoard) {
  Serial.write("The board is close \n");
  servoRight.writeMicroseconds(1550);  // Right slightly slower than usual, arcs right
  servoLeft.writeMicroseconds(1400);   // 
  }
  
  else {
    Serial.write("The board is far \n");
    servoRight.writeMicroseconds(1400);  // Right slightly slower than usual, arcs right
    servoLeft.writeMicroseconds(1550);   // 
  }
  
  } // backwards drive parallel
  
  else {Serial.write("Time to drive parallel BACKWARDS\n");
    int closeToBoard; // closeToBoard = 0 if nothing is there, 1 if there's something. 
    closeToBoard = !checkSide();
      // Code to poll front sensor and filter out false positives
      
  if (closeToBoard) {
  Serial.write("The board is close \n");
  servoRight.writeMicroseconds(1400);  // backwards and Right slightly slower than usual, arcs right away from board
  servoLeft.writeMicroseconds(1550);   // 
  }
  
  else {
    Serial.write("The board is far \n");
    servoRight.writeMicroseconds(1550);  // backwards and left slightly slower than usual, arcs left towards board
    servoLeft.writeMicroseconds(1400);   // 
}
  } //end of patient loaded else
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
