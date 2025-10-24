
#include <Arduino.h>
#include <controller.h>
#include <iostream>
using namespace std;



controller::controller() {
  speedValue = 0.0;
  directionInput = 0;
  turnValue = 0.0;
  LTValue = 0.0;
  RTValue = 0.0;
  joystickValue = 0; 
  LTValue = 0; 
  RTValue = 0; 
}


int controller::getDirectionInput()
{
    
  float LT = LTValue/255;
  float RT = RTValue/255;


  if (RT > .1 && LT <= .1) directionInput = 1; // forward
  else if (LT > .1 && RT <= .1) directionInput = 2; // backward
  else if (RT > .1 && LT > .1 )directionInput = 0; // nothing or both pressed
  else if (RT < .1 && LT < .1) directionInput = 0;
  return directionInput;
}




void controller::setTurnValues() // left –32768 to 32767 right
{
  float joy = joystickValue / 32767; 

  if (joy <= .2 && joy >= -.2) {leftWheelSpeed = 1; rightWheelSpeed = 1;}
  //straight

  if (joy < -.2 && joy > -.5) // slight left turning
  {
    leftWheelSpeed =  .75;   // ENA
    rightWheelSpeed = 1;     // ENB
  }

if (joy <= -.5 && joy > -.75) // regular left turning
  {
    leftWheelSpeed =  .5;   // ENA
    rightWheelSpeed = 1;     // ENB
  }

  if (joy <= -.75 && joy >= -1 ) // sharp left turning
  {
    leftWheelSpeed =  .2;   // ENA
    rightWheelSpeed = 1;     // ENB
  }


  // RIGHT TURN JOYSTICK

  if (joy > .2 && joy < .5) // slight right turning
  {
    leftWheelSpeed =  1;   // ENA
    rightWheelSpeed = .75;     // ENB
  }

if (joy >= .5 && joy < .75) // regular right turning
  {
    leftWheelSpeed =  1;   // ENA
    rightWheelSpeed = .5;     // ENB
  }

  if (joy > .75 && joy < 1 ) // sharp right turning
  {
    leftWheelSpeed =  1;   // ENA
    rightWheelSpeed = .2;     // ENB
  }
}




////////








