#ifndef VIDEODATA_H
#define VIDEODATA_H
#include <vector>
#include <fstream>
#include <math.h>


class VideoData
{
private:
    const unsigned int FPM = 1800;
    const float ConvertSec = 0.066;
    float ConvertCenti;
    struct Arena
    {
      int x_coord;
      int y_coord;
    };
    struct FrameData
    {
       unsigned int FrameNumber;
       unsigned int X_Coord;
       unsigned int Y_Coord;
       float Distance;
       float Velocity;
    };
    double ExplorationDistance = 0;
    double SponteneousActivity = 0;
    double ActiveTimeTotal = 0;
    double InActiveTimeTotal = 0;
    struct MinuteData
    {
       float Distance;
       float Velocity;
       float InnerTime;
       float ActiveTime = 0;
       float InActiveTime = 0;
    };

public:
    int ArenaRadius;
    int x_center;
    int y_center;
    Arena ArenaStruct;
    std::vector<Arena> ArenaCoord;

    FrameData FrameDataStruct;
    std::vector<FrameData> FrameVector;
    MinuteData MinuteDataStruct;
    std::vector<MinuteData> MinuteVector;

    void DistanceCalc();
    void VelocityCalc();
    void ArenaCalc();
    void WriteToFile(std::string,std::string);

    VideoData();
    ~VideoData();
};

#endif // VIDEODATA_H
