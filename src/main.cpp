#include <Arduino.h>
#include <WiFi.h>
#include <controller.h>
#include <autonomous.h>
#include <WiFiUdp.h>
#include <pwmWrite.h>



const char* AP_SSID     = "hachdearborn4";
const char* AP_PASSWORD = "12345678";
const uint16_t PORT = 8080;
WiFiServer server(PORT);

WiFiUDP udp;
const int UDP_PORT = 4210;        // must match PC sender port
char udpBuffer[64];               // buffer to receive incoming data

#define LED 2

#define EN_A 4 //for speed
#define EN_B 23 //temp pin
#define EN_C 13 //temp pin
#define EN_D 32 //temp pin


#define frontIN1 22 // Front Left forward
#define frontIN2 21 // Front Left back

#define frontIN3 19 // Front Right forward
#define frontIN4 18 // Front Right back

#define rearIN1 14// Back Wheels Left forward
#define rearIN2 27// Back Wheels Left backwards

#define rearIN3 26//Back Wheels Right forward
#define rearIN4 25// Bac kWheels right backwards


#define echoFront 39   // ALL TEMP PINS
#define triggerFront 17
#define echoLeft 34
#define triggerLeft 2
#define echoRight 36
#define triggerRight 33
#define echoRear 35 
#define triggerRear 16


float distFront;
long durationFront;
float distLeft;
long durationLeft;
float distRight;
long durationRight;
float distRear;
long durationRear;

int mode=0; //1 auto //0 controller
int framecount=0;

controller myController; 
autonomous walle;
Pwm pwm;

void setup()
{
  Serial.begin(115200);



  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress ip = WiFi.softAPIP();   //wifi shi
  server.begin();

  udp.begin(4210);   // must match your PC's destination port
  Serial.println("UDP listening on port 4210");


  int controllerDirection;


  pinMode(frontIN1, OUTPUT);  //GPIO setups
  pinMode(frontIN2, OUTPUT);
  pinMode(frontIN3, OUTPUT);
  pinMode(frontIN4, OUTPUT);
  pinMode(rearIN1, OUTPUT); 
  pinMode(rearIN2, OUTPUT);
  pinMode(rearIN3, OUTPUT);
  pinMode(rearIN4, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(triggerFront, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(triggerLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(triggerRight, OUTPUT);
  pinMode(echoRear, INPUT);
  pinMode(triggerRear, OUTPUT);

  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(EN_C, OUTPUT);
  pinMode(EN_D, OUTPUT);

  analogWrite(EN_C, 255);
  analogWrite(EN_D, 255);


}


void forward()
{

  digitalWrite(frontIN1, HIGH); // left forward
  digitalWrite(frontIN2, LOW); 

  digitalWrite(frontIN3, HIGH); // right forward
  digitalWrite(frontIN4, LOW); 


  //REARS FORWARD
  digitalWrite(rearIN1, HIGH); 
  digitalWrite(rearIN2, LOW); 

  digitalWrite(rearIN3, HIGH);
  digitalWrite(rearIN4, LOW);
}


void backward()
{
  digitalWrite(frontIN1, LOW);  // left reverse
  digitalWrite(frontIN2, HIGH); 

  digitalWrite(frontIN3, LOW);  // right reverse
  digitalWrite(frontIN4, HIGH); 

  //REARS Reverse
  digitalWrite(rearIN1, LOW);  
  digitalWrite(rearIN2, HIGH); 

  digitalWrite(rearIN3, LOW); 
  digitalWrite(rearIN4, HIGH);
}


void stop()
{
  digitalWrite(frontIN1, LOW);  
  digitalWrite(frontIN2, LOW); 

  digitalWrite(frontIN3, LOW);  
  digitalWrite(frontIN4, LOW); 

  //REARS stop
  digitalWrite(rearIN1, LOW);  
  digitalWrite(rearIN2, LOW); 

  digitalWrite(rearIN3, LOW); 
  digitalWrite(rearIN4, LOW);
}


float getDistance(int triggerPin, int echoPin) {

  // ultrasonic pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);


  // Measure response
  long duration = pulseIn(echoPin, HIGH, 25000); // 25ms timeout

  // Handle timeout, so car doesnt think it crashed
  if (duration < 1.5) return 999.0; 

  // Convert to cm 
  float distance = (duration * 0.0343) / 2;
  return distance;
}


void controllerMode()
{
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
      int len = udp.read(udpBuffer, sizeof(udpBuffer) - 1);
      if (len > 0) udpBuffer[len] = 0;  // null-terminate

      // Parse raw floats: LT, RT, joystick
      float LT, RT, LX;
      sscanf(udpBuffer, "%f,%f,%f", &LT, &RT, &LX);

      // Update controller object
      myController.LTValue = LT;
      myController.RTValue = RT;
      myController.joystickValue = LX;

      myController.setTurnValues(); // update wheel speeds based on joystick

      static unsigned long lastPrint3 = 0;
  if (millis() - lastPrint3 >= 1000) { // every 1 second
      Serial.print("LT: "); Serial.print(myController.LTValue);
      Serial.print(" RT: "); Serial.print(myController.RTValue);
      Serial.print(" LX: "); Serial.println(myController.joystickValue);
      Serial.print("\n");
      lastPrint3 = millis();
    }
  }

  static unsigned long lastPrint2 = 0;
  if (millis() - lastPrint2 >= 1000) { // every 1 second
        Serial.print("right wheel speed: ");
        Serial.println(myController.rightWheelSpeed*250);
        Serial.print("leftt wheel speed: ");
        Serial.println(myController.leftWheelSpeed*250);
        Serial.print("\n");
        lastPrint2 = millis();
  }
  pwm.write(EN_B, myController.rightWheelSpeed*250);
  pwm.write(EN_A, myController.leftWheelSpeed*250);

  //1 = rtclicked, 2= ltclicked , 3= none
  if (myController.getDirectionInput() == 1) forward();


  if (myController.getDirectionInput() == 2) backward();


  if (myController.getDirectionInput() == 0) stop();





  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) { // every 1 second
        Serial.print("Direction: ");
        Serial.println(myController.getDirectionInput());
        Serial.print("\n");
        lastPrint = millis();
  }

}


void autonomousMode()
{
  analogWrite(EN_B, 200);
  analogWrite(EN_A, 200);
  
  distFront = getDistance(triggerFront, echoFront);
  distRear  = getDistance(triggerRear,  echoRear);
  delay(40);
  distLeft  = getDistance(triggerLeft,  echoLeft);
  distRight = getDistance(triggerRight, echoRight);


  walle.sendDist(distFront, distLeft, distRight, distRear); 

  if (walle.clearForward() == true) forward();
  else
  {
    if (walle.lookBothWays()  == 1) //go left
    {
      digitalWrite(frontIN1, LOW);  // left front reverse
      digitalWrite(frontIN2, HIGH); 

      digitalWrite(frontIN3, HIGH); // right front forward
      digitalWrite(frontIN4, LOW);

      //rears forward
      digitalWrite(rearIN1, HIGH); 
      digitalWrite(rearIN2, LOW); 

      digitalWrite(rearIN3, HIGH);
      digitalWrite(rearIN4, LOW);

      }

    if (walle.lookBothWays()  == 2) // go right
    {
      digitalWrite(frontIN1, HIGH); // left front forward
      digitalWrite(frontIN2, LOW);

      digitalWrite(frontIN3, LOW);  // right front reverse
      digitalWrite(frontIN4, HIGH);

      //rears forward
      digitalWrite(rearIN1, HIGH); 
      digitalWrite(rearIN2, LOW); 

      digitalWrite(rearIN3, HIGH);
      digitalWrite(rearIN4, LOW);

    }
  }
  delay(1000); //give time for slight turn

  if (walle.collisionCheck() == true)
  {
    stop();
    delay(500);

    if (walle.rearCheck() == true) backward();
    else 
    {  // take clearer route from right and left 
      int LorR = walle.lookBothWays();
      if (LorR == 1) // turn left 90
      {
        digitalWrite(frontIN1, LOW);  // left front reverse
        digitalWrite(frontIN2, HIGH); 

        digitalWrite(frontIN3, HIGH); // right front forward
        digitalWrite(frontIN4, LOW); 

        digitalWrite(rearIN1, LOW);  // left rear reverse
        digitalWrite(rearIN2, HIGH); 

        digitalWrite(rearIN3, HIGH); // right  rear forward
        digitalWrite(rearIN4, LOW); 

      }

      else if (LorR == 2) // turn right 90
      {
          digitalWrite(frontIN1, HIGH); // left front forward
          digitalWrite(frontIN2, LOW);

          digitalWrite(frontIN3, LOW);  // right front reverse
          digitalWrite(frontIN4, HIGH);


          //REARS FORWARD
          digitalWrite(rearIN1, HIGH); // left rear forward
          digitalWrite(rearIN2, LOW); 

          digitalWrite(rearIN3, LOW); // right rear reverse
          digitalWrite(rearIN4, HIGH);

      }
    }
    delay(1000);

  } // collision end 

  
}


void loop()
{


  if (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
        case '0':
            mode = 0; 
            Serial.println("Switched to CONTROLLER mode");
            break;
        case '1':
            mode = 1;
            Serial.println("Switched to AUTONOMOUS mode");
            break;
        default:
            mode = 0; 
            Serial.println("Switched to CONTROLLER mode");
    }
  }

  if (mode == 0) //controller
  {
   controllerMode();
  }

  if(mode == 1) //autonomous
  {
    autonomousMode();
  }


  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) { // every 1 second
      Serial.printf("Current mode: %d\n", mode);
      Serial.print("\n");
      lastPrint = millis();
  }



}

