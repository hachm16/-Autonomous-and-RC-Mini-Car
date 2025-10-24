#ifndef CONTROLLER_H
#define CONTROLLER_H

class controller{
  public:
  float speedValue;
  int directionInput;
  float turnValue;

  float leftWheelSpeed, rightWheelSpeed;

  float LTValue, RTValue;
  float joystickValue;

  int getDirectionInput(); //LT or RT?
  float getRTValue();
  float getLTValue();
  
  void setTurnValues(); // left –32768 to 32767 right

  controller();
};

#endif
