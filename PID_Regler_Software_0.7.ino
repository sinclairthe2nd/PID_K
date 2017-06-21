#include <TaskScheduler.h>
#include "max6675.h"
#include <PID_v1.h>

// Callback methods prototypes
void t1Callback();
//Define Variables we'll be connecting to
double Setpoint, Input, Output,setvar,bodgefix;
//Tasks add time after which setpoint will change in !milliseconds!
Task t1(1000, 3, &t1Callback);
Scheduler runner;
void t1Callback() {
  if (bodgefix == 1){  
  Serial.println("Setpoint Changed");
  Setpoint = 50;} 
  //Change Setpoint to ?
  else {bodgefix = 1;}
}




//Set Parameters
double farP = 3;
double farI = 4;
double farD = 10;
double cloP = 10;
double cloI = 7;
double cloD = 30;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,farP,farI,farD, DIRECT);
//Initialise Relay and Thermocouple Pins
int Relay = 8;
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;
  
void setup() {
  Serial.begin(9600);
  //Sheduler stuff
  runner.init();
  Serial.println("Initialized scheduler");
   runner.addTask(t1);
  Serial.println("added t1");
    t1.enable();
Serial.println("Enabled t1");
  
  //Define RelayPin as Output
  pinMode(Relay, OUTPUT);
  //initialize the variables we're linked to
  Input = analogRead(0);
  
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  
  
  // use Arduino pins as a power Supply for the Max6675
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  delay(500);
  Setpoint = 100;
}

void loop() {
   runner.execute();
   Serial.println("Termperature is set to");
   Serial.println(Setpoint);
  //Safety First
  if (Setpoint > 301){Setpoint = 300;};
  //Use 5V rail if T < 200°C
  if (Setpoint < 200) {digitalWrite(Relay, HIGH);}
  else {digitalWrite(Relay, LOW);} 
  double gap = abs(Setpoint-setvar); //distance away from setpoint
  if (gap < 20)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(cloP, cloI, cloD);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(farP, farI, farD);
  }

  // basic readout test, just print the current temp
  setvar = thermocouple.readCelsius();
  Serial.println("Termperature is");
   Serial.println(setvar);
     Input = setvar;
  myPID.Compute();
  analogWrite(10,Output);
   delay(200);
}
