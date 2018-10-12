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
	const float ConvertSec = 0.0333;
	float InputArenaRadius; // Store the Arena Diameter
	float ConvertCenti;
	const int ActivityVelocity = 1;

	//		Arena Data
	struct Arena
	{	//Struct to Store the Points and the areana diameter
		
		int x_coord;		//Store the X-Coordinate 
		int y_coord;		//Store the Y-Coordinate 
	};
	
	//		FrameData
	struct FrameData
	{	//Struct to store the Frame Data

		unsigned int FrameNumber;	//Store The Frame Number
		unsigned int X_Coord;		//Store the x-coordinate
		unsigned int Y_Coord;		//Store the y-coordinate
		float Distance;				//Store the distance between Frames
		float Velocity;				//Store the velocity between Frames
		unsigned short int Active = 0;		//Hold 1 if the frame is active
		unsigned short int Inactive = 0;	//Hold 1 od the frame is inactive
		unsigned short int ShiftChange = 0;	//Hold 1 if the frame has shifted from active to inactive
		float DistanceFromCenter = 0;
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
		unsigned int Shifts = 0;
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

	bool setInputArenaRadius();
	void InitMinVector();
	void CalcFrameDistance();
	void CalcFrameVel(); 
	void CalcMinDistance();
	void CalcMinVel();
	void ActivityCalc();
	void InnerTimeCalc();
	void ArenaCalc();
	void DisplayVectors();
	void WriteToFile(string,string);
	void WriteToXCL(string, string);

	VideoData();
	~VideoData();
};

