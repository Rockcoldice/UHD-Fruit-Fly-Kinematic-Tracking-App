#include "videodata.h"

bool VideoData::ArenaCalc(){

    float k, l, m, n, p, q;
    float a_1, a_2, b_2, b_1, g_1, g_2;

    //Point (k,l)
    k = ArenaCoord[0].x_coord;
    l = ArenaCoord[0].y_coord;

    //Point (m,n)
    m = ArenaCoord[1].x_coord;
    n = ArenaCoord[1].y_coord;

    //Point (p,q)
    p = ArenaCoord[2].x_coord;
    q = ArenaCoord[2].y_coord;


    a_1 = 2 * (k - m);
    a_2 = 2 * (k - p);

    b_1 = 2 * (l - n);
    b_2 = 2 * (l - q);

    g_1 = (k*k) + (l*l) - (m*m) - (n*n);
    g_2 = (k*k) + (l*l) - (p*p) - (q*q);

    float v = a_2 * (-1);
    v = v / a_1;

    y_center = (v*g_1) + g_2;
    y_center = y_center / ((v*b_1) + b_2);

    x_center = (g_1 - (y_center*b_1)) / a_1;
    ArenaRadius = ((k - x_center)*(k - x_center)) + (l - y_center)*(l - y_center);
    ArenaRadius = sqrt(ArenaRadius);
    ConvertCenti = 4.1 / ArenaRadius;
    if(ConvertCenti==0) return false;
    else return true;
}
bool VideoData::DistanceCalc(){
    bool Finish = false;
    MinuteData min;
    min.Distance = 0;
    int size = FrameVector.size();
    size = size - 1;
    float x0, y0, x1, y1, x, y, distance;
    int j = 0;
    FrameVector[j].Distance = 0;

    int Minutes = (size / FPM) + 1;

    //initialization of final "minute" vector
    for (int i = 0; i <= Minutes-1; i++) {
        MinuteVector.push_back(min);
    }
    float TotDist = 0;
    int MinCounter = 0;
    for (int i = 1; i <= size; i++) {
        j = i - 1;

        x0 = FrameVector[j].X_Coord;
        x1 = FrameVector[i].X_Coord;
        x = x1 - x0;
        x = pow(x, 2);

        y0 = FrameVector[j].Y_Coord;
        y1 = FrameVector[i].Y_Coord;
        y = y1 - y0;
        y = pow(y, 2);

        distance = x + y;
        distance = sqrt(distance);
        distance = distance*ConvertCenti;
        FrameVector[i].Distance = distance;
        TotDist=TotDist + distance;
        if (i%FPM == 0) {
            MinuteVector[MinCounter].Distance = TotDist;
            if (MinCounter <= Minutes) {
                TotDist = 0;
            }
            MinCounter++;
        }
    }
    MinuteVector[MinCounter].Distance = TotDist;
    ExplorationDistance = MinuteVector[0].Distance;
    MinCounter = 0;

    float SmallRad;
    SmallRad = ArenaRadius*0.5;
    float framecounter = 0;
    for (int i = 0; i <= size; i++) {
        x0 = x_center;
        x1 = FrameVector[i].X_Coord;
        x = x1 - x0;
        x = pow(x, 2);
        y0 = y_center;
        y1 = FrameVector[i].Y_Coord;
        y = y1 - y0;
        y = pow(y, 2);
        distance = x + y;
        distance = sqrt(distance);
        if (distance <= SmallRad)
        {
            framecounter++;
        }
        if (i%FPM == 0 && i != 0)
        {
            //cout << "Inner Circle Minute " << MinCounter << ": " << framecounter*ConvertSec << '\t';
            MinuteVector[MinCounter].InnerTime = framecounter * ConvertSec;
            //cout << "Vector Value: " << MinuteVector[MinCounter].InnerTime << "\n";
            framecounter = 0;
            MinCounter++;
        }

    }
    if (Minutes> 5) {
        for (int i = 4; i <= Minutes - 1; i++) {
            SponteneousActivity = SponteneousActivity + MinuteVector[i].Distance;
        }
    }
    MinuteVector[MinCounter].InnerTime = framecounter * ConvertSec;
    Finish = true;
    return Finish;

}
bool VideoData::VelocityCalc(){
    bool Finish = false;
    int size = FrameVector.size();
    size = size - 4;
    float const dem = 5 * 0.033;	//denominator
    float d_2, d_1, d, d1, d2, velocity;
    float totalVelocity =0;
    int Minutes = MinuteVector.size();
    int MinCounter = 0;
    for (int i = 2; i <= size; i++) {
        d_2 = FrameVector[i - 2].Distance;
        d_1 = FrameVector[i - 1].Distance;
        d = FrameVector[i].Distance;
        d1 = FrameVector[i + 1].Distance;
        d2 = FrameVector[i + 2].Distance;
        velocity = (d_1 + d_2 + d + d1 + d2);
        velocity = velocity / dem;
        totalVelocity = velocity + totalVelocity;
        FrameVector[i].Velocity = velocity;
        if (i%FPM == 0) {
            MinuteVector[MinCounter].Velocity = totalVelocity/(FPM-2);
            if (MinCounter <= Minutes) {
                totalVelocity = 0;
            }
            MinCounter++;
        }
    }
    MinuteVector[MinCounter].Velocity = totalVelocity / (FPM - 2);
    int ActiveCounter = 0;
    int InactiveCounter = 0;
    MinCounter = 0;

    for (int i = 0; i <= size; i++) {
        if (FrameVector[i].Velocity >= 0.01) {
            ActiveCounter++;
        }
        else {
            InactiveCounter++;
        }
        if (i % FPM == 0 && i>0) {
            MinuteVector[MinCounter].ActiveTime = ActiveCounter * ConvertSec;
            MinuteVector[MinCounter].InActiveTime = InactiveCounter*ConvertSec;
            ActiveCounter = 0;
            InactiveCounter = 0;
            MinCounter++;
        }
    }
    MinuteVector[MinCounter].ActiveTime = ActiveCounter * ConvertSec;
    MinuteVector[MinCounter].InActiveTime = InactiveCounter*ConvertSec;
    if (Minutes> 5) {
        for (int i = 4; i <= Minutes-1; i++) {
            ActiveTimeTotal = ActiveTimeTotal + MinuteVector[i].ActiveTime;
            InActiveTimeTotal = InActiveTimeTotal + MinuteVector[i].InActiveTime;
        }
    }
    Finish = true;
    return Finish;
}
bool VideoData::WriteToFile(std::string FileName, std::string VideoName){
    bool Finish = false;
    int MinSize = MinuteVector.size();
        MinSize = MinSize - 1;
        int FrameSize = FrameVector.size();
        FrameSize = FrameSize - 1;

        FileName = FileName + ".csv";
        std::ofstream myFile(FileName);
        if (myFile.is_open() && MinSize >= 0 && FrameSize > 0)
        {
            myFile << "Video File Name: ,";
            myFile << VideoName << "\n";
            myFile << "Frame Per Minute: ,";
            myFile << FPM << "\n";
            myFile << "Arena Redious in Pixels: ,";
            myFile << ArenaRadius << "\n";
            myFile << "Convertion Factor for Pixels to Centimenter: ,";
            myFile << ConvertCenti << "\n";
            myFile << "Time per Frame: ,";
            myFile << ConvertSec << "\n" << "\n";
            myFile << "Video Data" << "\n" << "\n";
            myFile << "Total Time Active in Seconds: ,";
            myFile << ActiveTimeTotal << "\n";
            myFile << "Total Time InActive in Seconds: ,";
            myFile << InActiveTimeTotal << "\n";
            myFile << "Spontenous Activity in [cm]: ,";
            myFile << SponteneousActivity << "\n";
            myFile << "Exploration Distance in [cm]: ,";
            myFile << ExplorationDistance << "\n";
            myFile << "\n" << "\n" << "\n";


            myFile << "Data by Minute" << "\n" << "\n";
            myFile << "Minutes,Distance [cm],Avg Velocity [cm/s],Time in Inner Arena [s],Active Time [s],InActive Time [s]" << "\n";
            for (int i = 0; i <= MinSize; i++) {
                myFile << i << ",";
                myFile << MinuteVector[i].Distance << ",";
                myFile << MinuteVector[i].Velocity << ",";
                myFile << MinuteVector[i].InnerTime << ",";
                myFile << MinuteVector[i].ActiveTime << ",";
                myFile << MinuteVector[i].InActiveTime << "\n";
            }

            myFile << "\n" << "\n" << "\n";
            myFile << "Data by the Frame" << "\n" << "\n";
            myFile << "Frame Number, X-Coordinate [pxl],Y-Coordinate [pxl],Distance [cm], Velocity [cm/s]\n";
            for (int i = 0; i <= FrameSize; i++) {
                FrameVector[i].FrameNumber=i;
                myFile << FrameVector[i].FrameNumber << ",";
                myFile << FrameVector[i].X_Coord << ",";
                myFile << FrameVector[i].Y_Coord << ",";
                myFile << FrameVector[i].Distance << ",";
                myFile << FrameVector[i].Velocity << "\n";
            }
        }
        else {
            Finish = false;
            return Finish;
        }
        myFile.close();
        //cout << "Finish Writing to File" << "\n" << "\n";
        Finish = true;
        return Finish;
}

VideoData::VideoData()
{

}
VideoData::~VideoData(){
    ArenaRadius = 0;
    x_center =0;
    y_center=0;
    ArenaCoord.clear();
    FrameVector.clear();
    MinuteVector.clear();

}
