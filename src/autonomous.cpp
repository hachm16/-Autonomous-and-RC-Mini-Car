#include <autonomous.h>
#include <Arduino.h>

autonomous::autonomous()
{
    distFront = 0.0;
    distLeft = 0.0;
    distRight = 0.0;
    distRear = 0.0;

    stopFlag = 0;

}

void autonomous::sendDist(float distA, float distB, float distC, float distD) //get dist from main
{
    distFront = distA;
    distLeft = distB;
    distRight = distC;
    distRear = distD;


}


bool autonomous::collisionCheck()
{
    if (distFront <=15 || distLeft <=15 || distRight<= 15) return true;

    else return false;
}


bool autonomous::rearCheck()
{
        if (distRear > 30)
        {
        return true; //rear is clear
        }

        else  return false; // if rear not clear, check left and right for turning
        
            
        
} 


int autonomous::lookBothWays()
{
    if (distLeft > distRight) return 1;
    else return 2;
}


bool autonomous::clearForward()
{
    if (distFront >= 45) return true;
    else return false;

}
 //is clear for at least 45cm


