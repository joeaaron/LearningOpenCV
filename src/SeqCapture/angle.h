#ifndef ZOOMLION_ANGLE_H
#define ZOOMLION_ANGLE_H

#include <string>
class AngleProc
{
public:
    virtual int Open(int carmarIdx, const std::string inputCameraDataFile) = 0;
    virtual int Angle(double& angle) = 0;
    static AngleProc* Create();
};
#endif