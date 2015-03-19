#define FIND_BOARD 0
#define FOLLOW_BOARD 1
#define LOOK_FOR_PATIENT 5
#define LOAD_PATIENT 2
#define RETURN 3
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
BoardEnd = !checkSideDistance();
DriveParallelToBoard();
Timer2++;

case LOOK_FOR_PATIENT:
FrontObstructionDetected = CheckFrontSensor();
DriveParallelToBoard();

case LOAD_PATIENT:
EmitBeep();
FlashLight();
break;

case RETURN:
FrontObstructionDetected = CheckFrontSensor();
NavigateToMU();
break;
case TERMINATE:
break;
} //end of do switch statement


// Switch statement for handling "Entry/Exit" actions
// and state transitions
switch(State){
case FIND_BOARD:
if(FrontObstructionDetected){
StopDriving(); // Exit action
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
}
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
State = RETURN; // State change
StartDriving(); // Entry action
}

break;
case RETURN:
if(FrontObstructionDetected){
StopDriving(); // Exit action
State = TERMINATE; // State change
}
break;
case TERMINATE:
break;
} //end of entry/exit switch statement


} // end of main loop

void StartDriving()
{
  servoRight.writeMicroseconds(1690);  // Robot goes forward
  servoLeft.writeMicroseconds(1300);   // 
  Serial.write("I'm driving!\n");
}
int CheckFrontSensor()
{
int SensorResponse = !irDetect(2, 3, 42000);       // Check for object on right
// Code to poll front sensor and filter out false positives   
  digitalWrite(8, SensorResponse);
return SensorResponse;
}

void DriveParallelToBoard()
{
  Serial.write("Time to drive parallel \n");
  int sideDistance; // 1 is close, 2 is good, 3 is far
  sideDistance = checkSideDistance();
      // Code to poll front sensor and filter out false positives

  if (sideDistance == 2) { 
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

}


int checkSideDistance() { 
  Serial.write ("I'm checking side distance\n");
  int frequency = 40000;
  int distance = 0;
  for (; frequency < 42000; frequency+=1000) {
      distance += irDetect(9, 10, frequency); // check left sensor       //ALIGNMENT
  }
  digitalWrite(7, distance);    //display side sensor status on left LED
  return distance;
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
  Serial.write("I'm turning right \n");  
  servoLeft.writeMicroseconds(1600);  //right turn
  servoRight.writeMicroseconds(1600);
  delay (625);
  }
  else {
  Serial.write("I'm turning right \n");  
  servoLeft.writeMicroseconds(1600);  //right turn
  servoRight.writeMicroseconds(1600);
  delay (625);
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
  Serial.write("Moving around board end \n");  
  forward(100);
  leftTurn();
  forward(1000); //this will probably need to be changed      //ALIGNMENT
  leftTurn();
  forward(250);
}

void leftTurn () {
    servoLeft.writeMicroseconds(1400);  //right turn
   servoRight.writeMicroseconds(1400);
   delay (625);
}

void forward (int time) {
    servoLeft.writeMicroseconds(1690);  //right turn
   servoRight.writeMicroseconds(1300);
   delay (time);
}
