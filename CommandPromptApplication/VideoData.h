#ifndef VIDEODATA
#define VIDEODATA
#endif 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

class VideoData
{
private:

	const int FPM = 1800;
	const float ConvertSec = 0.066;
	float ConvertCenti;

	//		Arena Data
	struct Arena
	{	//Struct to Store the Points

		int x_coord;	//Store the X-Coordinate 
		int y_coord;	//Store the Y-Coordinate 
	};
	
	//		FrameData
	struct FrameData
	{	//Struct to store the Frame Data

		unsigned int FrameNumber;	//Store The Frame Number
		unsigned int X_Coord;		//Store the x-coordinate
		unsigned int Y_Coord;		//Store the y-coordinate
		float Distance;				//Store the distance between Frames
		float Velocity;				//Store the velocity between Frames
	};
	
	//		Minute Data Set
	double ExplorationDistance = 0;
	double SponteneousActivity = 0;
	double ActiveTimeTotal = 0;
	double InActiveTimeTotal = 0;
	struct MinuteData
	{//Struct to store the data for Minute calculations;
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
	vector<Arena> ArenaCoord;			//Store the Data Points for the Circle Calculations

	FrameData FrameDataStruct;
	vector<FrameData> FrameVector;		//Store the Frame Data for futher Analysis

	MinuteData MinuteDataStruct;
	vector<MinuteData> MinuteVector;	//Store the Minute Data for Displaying;


	void DistanceCalc();
	void VelocityCalc();
	void ArenaCalc();
	void DisplayVectors();
	void WriteToFile(string,string);

	VideoData();
	~VideoData();
};

