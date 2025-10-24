#ifndef AUTONOMOUS_H
#define AUTONOMOUS_H

class autonomous{
    public:
    float distFront, distLeft, distRight, distRear;

    bool stopFlag;

    void sendDist(float distA, float distB, float distC, float distD);
    bool collisionCheck();
    bool rearCheck();
    int lookBothWays();
    bool clearForward();

    autonomous();
};


#endif
