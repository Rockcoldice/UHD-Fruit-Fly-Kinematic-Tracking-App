#include "VideoData.h"

bool VideoData::setInputArenaRadius() {
	bool sizeset = false;
	float arenasize;
	cout << "Enter Size of Arena Inner Diameter (in Centimeter): ";
	cin >> arenasize;
	arenasize = arenasize / 2;
	if (arenasize <= 0) {

		int counter = 0;
		do {
			cout << "Error: Invalid Input: " << endl << endl;
			cout << "Enter Size of Arena Diameter (in Millimeters): ";
			cin >> arenasize;
			arenasize = arenasize / 2;
			if (counter >= 10)
				break;
			counter++;

		} while (arenasize <= 0);
	}
	if (arenasize >= 0) {
		sizeset = true;
		InputArenaRadius = arenasize;
	}


	return sizeset;
}

void VideoData::ArenaCalc() {
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
	ConvertCenti = InputArenaRadius / ArenaRadius;
}

void VideoData::InitMinVector() {
	//Initilized the Minute Vector for further uses;
	MinuteData min;
	int size = FrameVector.size() - 1;
	int AmountofMin = size / FPM;
	for (int i = 0; i <= AmountofMin; i++)
		MinuteVector.push_back(min);
	cout << "Finish Initilizing the Vector" << endl;
};

void VideoData::CalcFrameDistance() {
	int size = FrameVector.size() - 1;
	float x0, y0, x1, y1, x, y, distance;
	int j = 0;
	FrameVector[j].Distance = 0;
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
	}
	cout << "Finish Distance Calc : Frame" << endl;
};
void VideoData::CalcFrameVel() {
	int size = FrameVector.size() - 4;
	float const dem = 5 * ConvertSec;
	float d_2, d_1, d, d1, d2, velocity;
	for (int i = 2; i <= size; i++) {
		d_2 = FrameVector[i - 2].Distance;
		d_1 = FrameVector[i - 1].Distance;
		d = FrameVector[i].Distance;
		d1 = FrameVector[i + 1].Distance;
		d2 = FrameVector[i + 2].Distance;
		velocity = (d_1 + d_2 + d + d1 + d2);
		velocity = velocity / dem;
		FrameVector[i].Velocity = velocity;
	}
	cout << "Finish Velocity Calc : Frame" << endl;
};
void VideoData::CalcMinDistance() {
	int size = MinuteVector.size() - 1;
	int StartIndex = 0;
	int EndIndex = FPM;
	
	for (int i = 0; i <= size; i++) {
		float distance = 0;
		for (int j = StartIndex; j <= EndIndex; j++) {
			distance = distance + FrameVector[j].Distance;
		}
		MinuteVector[i].Distance = distance;
		StartIndex = EndIndex + 1;
		EndIndex = StartIndex + FPM;
		if (EndIndex >= (FrameVector.size() - 1))
			EndIndex = FrameVector.size() - 1;
		if (i >= 5)
			SponteneousActivity = SponteneousActivity + MinuteVector[i].Distance;
	}
	ExplorationDistance = MinuteVector[0].Distance;
	cout << "Finish Distance Calc : Minute" << endl;
};
void VideoData::CalcMinVel() {
	int size = MinuteVector.size() - 1;
	int StartIndex = 0;
	int EndIndex = FPM;
	for (int i = 0; i <= size; i++) {
		float velocity = 0;
		for (int j = StartIndex; j <= EndIndex; j++) {
			if(FrameVector[j].Velocity >=ActivityVelocity)
				velocity = velocity + FrameVector[j].Velocity;
		}
		MinuteVector[i].Velocity = velocity / double(FPM);
		StartIndex = EndIndex + 1;
		EndIndex = StartIndex + FPM;
		if (EndIndex >= (FrameVector.size() - 1))
			EndIndex = FrameVector.size() - 1;
	}
	cout << "Finish Velocity Calc : Minute" << endl;
};
void VideoData::ActivityCalc() {
	int size = MinuteVector.size() - 1;
	int StartIndex = 1;
	int EndIndex = FPM;
	float velocity = 0;
	for (int i = 0; i <= size; i++) {
		int ActiveCounter = 0;
		int InActiveCounter = 0;
		int ShiftCounter = 0;
		for (int j = StartIndex; j <= EndIndex; j++) {
			velocity = FrameVector[j].Velocity;
			if (velocity >= ActivityVelocity) {
				ActiveCounter++;
				FrameVector[j].Active = 1;
			}
			else {
				InActiveCounter++;
				FrameVector[j].Inactive = 1;
			}
			if ((FrameVector[j].Velocity <= ActivityVelocity && FrameVector[j + 1].Velocity >= ActivityVelocity) ||
				(FrameVector[j].Velocity >= ActivityVelocity && FrameVector[j + 1].Velocity <= ActivityVelocity)) {
				ShiftCounter++;
				FrameVector[j].ShiftChange = 1;
			}

		}
		MinuteVector[i].ActiveTime = ActiveCounter * ConvertSec;
		ActiveTimeTotal = ActiveTimeTotal + MinuteVector[i].ActiveTime;
		MinuteVector[i].InActiveTime = InActiveCounter * ConvertSec;
		InActiveTimeTotal = InActiveTimeTotal + MinuteVector[i].InActiveTime;
		MinuteVector[i].Shifts = ShiftCounter;
		StartIndex = EndIndex + 1;
		EndIndex = StartIndex + FPM;
		if (EndIndex >= (FrameVector.size() - 1)) {
			EndIndex = FrameVector.size() - 2;
		}
	}
	cout << "Finish Activity Calc : Minute" << endl;
}
void VideoData::InnerTimeCalc() {
	int FrameSize = FrameVector.size() - 1;
	int MinSize = MinuteVector.size() - 1;
	float halfArena = ArenaRadius / 2;
	float x0, y0, x1, y1, x, y, distance;
	int k = 0;
	int InnerTime = 0;
	int MinuteCounter = 0;
	int modVal = -1;
	for (int i = 0; i <= FrameSize; i++) {
		k = i - 1;

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
		FrameVector[i].DistanceFromCenter = distance;
		if (distance <= halfArena) {
			InnerTime++;
		}
		modVal = i%FPM;
		if (modVal== 0 &&i!=0) {
			MinuteVector[MinuteCounter].InnerTime = InnerTime * ConvertSec;
			InnerTime = 0;
			MinuteCounter++;
		}
	}

	MinuteVector[MinuteCounter].InnerTime = InnerTime * ConvertSec;

	cout << "Finish Inner Distance Calc : Minute" << endl;
};

void VideoData::WriteToFile(string FileName, string VideoName) {
	int MinSize = MinuteVector.size();
	MinSize = MinSize - 1;
	int FrameSize = FrameVector.size();
	FrameSize = FrameSize - 1;

	FileName = FileName + ".csv";
	ofstream myFile(FileName);
	if (myFile.is_open() && MinSize >= 0 && FrameSize > 0)
	{
		myFile << "Video File Name: ,";
		myFile << VideoName << endl;
		myFile << "Frame Per Minute: ,";
		myFile << FPM << endl;
		myFile << "Time per Frame: ,";
		myFile << ConvertSec << endl << endl;
		myFile << "Arena Radius in Pixels: ,";
		myFile << ArenaRadius << endl;
		myFile << "Areana Radius in Centimeter: ,";
		myFile << InputArenaRadius << endl;
		myFile << "Convertion Factor for Pixels to Centimenter: ,";
		myFile << ConvertCenti << endl;
		myFile << "Arena Center:" << endl;
		myFile << "X_Center:," << x_center << endl;
		myFile << "Y_Center:," << y_center << endl << endl;
		

		myFile << "Video Data" << endl << endl;
		myFile << "Total Time Active in Seconds: ,";
		myFile << ActiveTimeTotal << endl;
		myFile << "Total Time InActive in Seconds: ,";
		myFile << InActiveTimeTotal << endl;
		myFile << "Spontenous Activity in [cm]: ,";
		myFile << SponteneousActivity << endl;
		myFile << "Exploration Distance in [cm]: ,";
		myFile << ExplorationDistance << endl;
		myFile << endl << endl << endl;


		myFile << "Data by Minute" << endl << endl;
		myFile << "Minutes,Distance [cm],Avg Velocity [cm/s],Time in Inner Arena [s],Active Time [s],InActive Time [s],Shift Counts" << "\n";
		for (int i = 0; i <= MinSize; i++) {
			myFile << i << ",";
			myFile << MinuteVector[i].Distance << ",";
			myFile << MinuteVector[i].Velocity << ",";
			myFile << MinuteVector[i].InnerTime << ",";
			myFile << MinuteVector[i].ActiveTime << ",";
			myFile << MinuteVector[i].InActiveTime << ",";
			myFile << MinuteVector[i].Shifts << "\n";
		}

		myFile << endl << endl << endl;
		myFile << "Data by the Frame" << endl << endl;
		myFile << "Frame Number, X-Coordinate [pxl],Y-Coordinate [pxl],Distance [cm], Velocity [cm/s]\n";
		for (int i = 0; i <= FrameSize; i++) {
			myFile << FrameVector[i].FrameNumber << ",";
			myFile << FrameVector[i].X_Coord << ",";
			myFile << FrameVector[i].Y_Coord << ",";
			myFile << FrameVector[i].Distance << ",";
			myFile << FrameVector[i].Velocity << "\n";
		}
	}
	else {
		cout << "Error: File Cannot Be Created" << endl << endl;
	}
	myFile.close();
	cout << "Finish Writing to File" << endl << endl;
}

void VideoData::WriteToXCL(string FileName, string VideoName) {
	int MinSize = MinuteVector.size();
	MinSize = MinSize - 1;
	int FrameSize = FrameVector.size();
	FrameSize = FrameSize - 1;

	FileName = FileName + "_XCL.txt";
	ofstream myFile(FileName);
	if (myFile.is_open() && MinSize >= 0 && FrameSize > 0)
	{
		myFile << "Video File Name: \t";
		myFile << VideoName << endl;
		myFile << "Frame Per Minute: \t";
		myFile << FPM << endl;
		myFile << "Time per Frame: \t";
		myFile << ConvertSec << endl << endl;
		myFile << "Arena Radius in Pixels: \t";
		myFile << ArenaRadius << endl;
		myFile << "Areana Radius in Centimeter: \t";
		myFile << InputArenaRadius << endl;
		myFile << "Convertion Factor for Pixels to Centimenter: \t";
		myFile << ConvertCenti << endl;
		myFile << "Arena Center:" << endl;
		myFile << "X_Center:\t" << x_center << endl;
		myFile << "Y_Center:\t" << y_center << endl << endl;


		myFile << "Video Data" << endl << endl;
		myFile << "Total Time Active in Seconds:\t";
		myFile << ActiveTimeTotal << endl;
		myFile << "Total Time InActive in Seconds:\t";
		myFile << InActiveTimeTotal << endl;
		myFile << "Spontenous Activity in [cm]:\t";
		myFile << SponteneousActivity << endl;
		myFile << "Exploration Distance in [cm]:\t";
		myFile << ExplorationDistance << endl;
		myFile << endl << endl << endl;


		myFile << "Data by Minute" << endl << endl;
		myFile << "Minutes\tDistance [cm]\tAvg Velocity [cm/s]\tTime in Inner Arena [s]\tActive Time [s]\tInActive Time [s]\tShift Counts" << "\n";
		unsigned short int StartIndex = MinSize + 31;
		unsigned short int EndIndex = StartIndex + FPM;
		for (int i = 0; i <= MinSize; i++) {
			myFile << i << "\t";
			myFile << "=SUM(D" << StartIndex << ":D" << EndIndex << ")" << "\t";			//Distance 
			myFile << "=AVERAGE(K" << StartIndex << ":K" << EndIndex << ")" << "\t";		//Avg Active Velocity 
			myFile << "=SUM(J" << StartIndex << ":J" << EndIndex << ")*$B$3" << "\t";		//Time Inner Arena
			myFile << "=SUM(F" << StartIndex << ":F" << EndIndex << ")*$B$3" << "\t";		//Active Time
			myFile << "=SUM(G" << StartIndex << ":G" << EndIndex << ")*$B$3" << "\t";		//InActive Time
			myFile << "=SUM(H" << StartIndex << ":H" << EndIndex << ")" << "\n";			//Shifts

			StartIndex = EndIndex + 1;
			EndIndex = StartIndex + FPM;
		}

		myFile << endl << endl << endl;
		myFile << "Data by the Frame" << endl << endl;
		myFile << "Frame Number\t X-Coordinate [pxl]\tY-Coordinate [pxl]\tDistance [cm]\t Velocity [cm/s]\t";
		myFile << "Active Time Marker\t Inactive Time Mark\t Shift Marker\t Distance from center\tDistance Marker\tVelocity Marker\n";
		StartIndex = MinSize + 31;
		EndIndex = StartIndex - 1;
		for (int i = 0; i <= FrameSize; i++) {
			myFile << FrameVector[i].FrameNumber << "\t";
			myFile << FrameVector[i].X_Coord << "\t";
			myFile << FrameVector[i].Y_Coord << "\t";
			myFile << FrameVector[i].Distance << "\t";
			myFile << FrameVector[i].Velocity << "\t";
			myFile << "=IF(E" << StartIndex << ">=1,1,0)" << "\t";
			myFile << "=IF(E" << StartIndex << "<=1,1,0)" << "\t";
			myFile << "=IF(OR(AND(E" << StartIndex << "<=1, E" << StartIndex + 1 << ">=1),AND(E" << StartIndex << ">=1,E" << StartIndex + 1 << "<=1)),1,0)" << "\t";
			myFile << FrameVector[i].DistanceFromCenter << "\t";
			myFile << "=IF(I" << StartIndex << "<=($B$5/2),1,0)" << "\t";
			myFile << "=IF(E" << StartIndex << ">=1,E" << StartIndex << ",0)" << "\n";

			EndIndex = StartIndex;
			StartIndex++;
		}
	}
	else {
		cout << "Error: File Cannot Be Created" << endl << endl;
	}
	myFile.close();
	cout << "Finish Writing to File" << endl << endl;
}

void VideoData::DisplayVectors() {

	int size = FrameVector.size();
	for (int i = 0; i <= size - 1; i++) {
		cout << "Frame Number: " << FrameVector[i].FrameNumber << "\t" << "\t";
		cout << "X - Coord: " << FrameVector[i].X_Coord << "\t" << "\t";
		cout << "Y - Coord: " << FrameVector[i].Y_Coord << "\t" << "\t";
		cout << "Distance: " << FrameVector[i].Distance << "\t" << "\t";
		cout << "Velocity: " << FrameVector[i].Velocity << endl;
	}
	size = MinuteVector.size();
	for (int i = 0; i <= size - 1; i++) {
		cout << "Minute Number:" << i << "\t";
		cout << "Distance: " << MinuteVector[i].Distance << "\t";
		cout << "Velocity: " << MinuteVector[i].Velocity << "\t";
		cout << "Inner Time: " << MinuteVector[i].InnerTime << "\t";
		cout << "Active Time: " << MinuteVector[i].ActiveTime << "\t";
		cout << "InActive Time: " << MinuteVector[i].InActiveTime << "\t";
		cout << "Shifts: " << MinuteVector[i].Shifts << endl;
	}
}

VideoData::VideoData()
{
}

VideoData::~VideoData()
{
	ArenaCoord.clear();
	FrameVector.clear();
	MinuteVector.clear();
}
