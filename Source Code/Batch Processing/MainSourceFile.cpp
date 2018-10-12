#include <stdio.h>
#include <stdlib.h>
#include "dirent.h"
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "VideoProcessing.h"
#include <deque>
using namespace cv;
using namespace std;
static int find_directory(const char *dirname);
void ParseForFolderPath(string PATH);
void GetAllVideoFiles();
vector<string> FileNames;
vector<unsigned short int> coord;
unsigned short int x_global, y_global;
double ArenaRadius;
void ArenaCalc(int event, int x, int y, int flags, void* userdata) {

	if (coord.size() < 6) {
		if (event == EVENT_LBUTTONDOWN) {
			cout << "x: " << x << "\t y: " << y << endl;
			circle((*(Mat*)userdata), Point(x, y), 2, CV_RGB(0, 255, 0), 3);
			coord.push_back(x);
			coord.push_back(y);
		}
	}
	else if (coord.size() == 6) {
		circle((*(Mat*)userdata), Point(x_global, y_global), ArenaRadius, CV_RGB(0, 255, 0), 3);
	}


	imshow("img", (*(Mat*)userdata));
}
void setArena();
bool WritetoFile();
bool WritetoFileXCL();
deque<VideoProcessing> VideoDataSet;
int main() {
	GetAllVideoFiles();
	setArena();
	unsigned short int counter = 0;
	while (!VideoDataSet.empty()) {
		cout << VideoDataSet[0].NameofVideo << endl;
		bool done = VideoDataSet[0].Processing();
		if (done == true) {
			cout << "Process A video" << endl;
			counter++;
		}
		cout << "Writting to files" << endl;
		WritetoFile();
		WritetoFileXCL();
		cout << "x_center: " << VideoDataSet[0].x_center << "\ty_center: " << VideoDataSet[0].x_center << endl;
		VideoDataSet.pop_front();
		cout << "Size: " << VideoDataSet.size() << endl;
		cout << endl << endl;
	}
	cout << "Amount of Video Processed: " << counter << endl;
	system("pause");
	return 0;
}

static int find_directory(const char *dirname)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = dirname;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(dirname);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				//printf("%s\n", buffer);
				FileNames.push_back(buffer);
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		/* Could not open directory */
		printf("Cannot open directory %s\n", dirname);
		ok = 0;
	}

	return ok;
}
void GetAllVideoFiles() {
	string FolderPath;
	cout << "Fruit Fly Tracking App Bulk Processing" << endl;
	cout << "Enter Folder Path: " << endl;
	getline(cin, FolderPath);
	const char *dir = FolderPath.c_str();
	//ParseForFolderPath(FolderPath);
	//CreateDirectory(OutputFilePath, NULL);	//Creates Output Folder
	find_directory(dir);
	for (int x = 0; x <= FileNames.size() - 1; x++) {
		string nameofFile = FileNames[x];
		if (nameofFile.find(".mp4") == nameofFile.npos) {
			FileNames.erase(FileNames.begin() + x);
			x--;
		}
	}
	cout << "Number of Video Files: " << FileNames.size() << endl << endl;
}
void ParseForFolderPath(string PATH) {
	int slash;
	for (int i = PATH.size() - 1; i >=0 ; i--) {
		if (PATH[i] == '\\')
			slash = i;
	}
	string something = PATH.substr(0, slash);
	cout << "Path: " << something << endl;
}
void setArena() {
	int NumOfVideos = FileNames.size() - 1;

	//Initilized Deque for Videos
	for (int i = 0; i <= NumOfVideos; i++) {
		VideoProcessing newVideo;
		newVideo.NameofVideo = FileNames[i];
		VideoDataSet.push_back(newVideo);
	}
	for (int i = 0; i <= NumOfVideos; i++) {
		cout << "Areana for: " << FileNames[i] << endl;
		VideoDataSet[i].setInputArenaRadius();
		VideoCapture cap(FileNames[i]);
		Mat frame1;
		cout << endl << endl;
		char key;
		bool Circle = true;
		bool set = false;
		cap >> frame1;
		Mat clone = frame1.clone();
		cout << "Please Select Point around Cimcumference of the Arena" << endl;
		cout << "Press ESC to once done" << endl;
		cout << "Press r to reset" << endl << endl << endl;
		system("pause");
		cout << endl << endl;
		do {

			namedWindow("img", 0);
			setMouseCallback("img", ArenaCalc, &clone);
			imshow("img", clone);
			if (coord.size() == 6 && !set) {
				set = VideoDataSet[i].copyCoord(coord);
				VideoDataSet[i].ArenaCalc();
				x_global = VideoDataSet[i].x_center;
				y_global = VideoDataSet[i].y_center;
				ArenaRadius = VideoDataSet[i].ArenaRadius;
			}
			switch (key = waitKey(10))
			{
			case 27:
				if (coord.size() == 6) {
					Circle = false;
				}
				break;
			case 82:
				clone.empty();
				clone.release();
				clone = frame1.clone();
				VideoDataSet[i].ArenaCoord.clear();
				x_global = 0;
				y_global = 0;
				ArenaRadius = 0;
				coord.clear();
				set = false;
				break;
			case 114:
				clone.empty();
				clone.release();
				clone = frame1.clone();
				VideoDataSet[i].ArenaCoord.clear();
				x_global = 0;
				y_global = 0;
				ArenaRadius = 0;
				coord.clear();
				set = false;
				break;
			}
		} while (Circle);
		destroyAllWindows();
		x_global = 0;
		y_global = 0;
		ArenaRadius = 0;
		coord.clear();
		set = false;
		cout << endl << endl;
	}

}
bool WritetoFile() {
	string  outputfile = VideoDataSet[0].NameofVideo.substr(0, VideoDataSet[0].NameofVideo.size() - 4);
	outputfile = outputfile + ".csv";
	
	unsigned short int MinSize = VideoDataSet[0].MinuteVector.size() - 1;
	unsigned short int FrameSize = VideoDataSet[0].FrameVector.size() - 1;

	ofstream myFile(outputfile);

	if (myFile.is_open() && MinSize >= 0 && FrameSize > 0) {
		myFile << "Video File Name: ,";
		myFile << VideoDataSet[0].NameofVideo << endl;
		myFile << "Frame Per Minute: ,";
		myFile << VideoDataSet[0].FPM << endl;
		myFile << "Time per Frame: ,";
		myFile << VideoDataSet[0].ConvertSec << endl;
		myFile << "Arena Radius in Pixels: ,";
		myFile << VideoDataSet[0].ArenaRadius << endl;
		myFile << "Areana Radius in Centimeter: ,";
		myFile << VideoDataSet[0].InputArenaRadius << endl;
		myFile << "Convertion Factor for Pixels to Centimenter: ,";
		myFile << VideoDataSet[0].ConvertCenti << endl;
		myFile << "Arena Center:" << endl;
		myFile << "X_Center:," << VideoDataSet[0].x_center << endl;
		myFile << "Y_Center:," << VideoDataSet[0].y_center << endl << endl;

		myFile << "Video Data" << endl << endl;
		myFile << "Total Time Active in Seconds: ,";
		myFile << VideoDataSet[0].ActiveTimeTotal << endl;
		myFile << "Total Time InActive in Seconds: ,";
		myFile << VideoDataSet[0].InActiveTimeTotal << endl;
		myFile << "Spontenous Activity in [cm]: ,";
		myFile << VideoDataSet[0].SponteneousActivity << endl;
		myFile << "Exploration Distance in [cm]: ,";
		myFile << VideoDataSet[0].ExplorationDistance << endl;
		myFile << endl << endl << endl;


		myFile << "Data by Minute" << endl << endl;
		myFile << "Minutes,Distance [cm],Avg Velocity [cm/s],Time in Inner Arena [s],Active Time [s],InActive Time [s],Shift Counts" << "\n";
		for (int i = 0; i <= MinSize; i++) {
			myFile << i << ",";
			myFile << VideoDataSet[0].MinuteVector[i].Distance << ",";
			myFile << VideoDataSet[0].MinuteVector[i].Velocity << ",";
			myFile << VideoDataSet[0].MinuteVector[i].InnerTime << ",";
			myFile << VideoDataSet[0].MinuteVector[i].ActiveTime << ",";
			myFile << VideoDataSet[0].MinuteVector[i].InActiveTime << ",";
			myFile << VideoDataSet[0].MinuteVector[i].Shifts << "\n";
		}

		myFile << endl << endl << endl;
		myFile << "Data by the Frame" << endl << endl;
		myFile << "Frame Number, X-Coordinate [pxl],Y-Coordinate [pxl],Distance [cm], Velocity [cm/s],";
		myFile << "\n";
		for (int i = 0; i <= FrameSize; i++) {
			myFile << VideoDataSet[0].FrameVector[i].FrameNumber << ",";
			myFile << VideoDataSet[0].FrameVector[i].X_Coord << ",";
			myFile << VideoDataSet[0].FrameVector[i].Y_Coord << ",";
			myFile << VideoDataSet[0].FrameVector[i].Distance << ",";
			myFile << VideoDataSet[0].FrameVector[i].Velocity << "\n";
		}
	}
	
	else {
		cout << "Error: File Cannot Be Created" << endl << endl;
	}
	myFile.close();
	cout << "Finish Writing to File" << endl << endl;
	return true;
}
bool WritetoFileXCL() {
	string  outputfile = VideoDataSet[0].NameofVideo.substr(0, VideoDataSet[0].NameofVideo.size() - 4);
	outputfile = outputfile + "_XCL.txt";

	unsigned short int MinSize = VideoDataSet[0].MinuteVector.size() - 1;
	unsigned short int FrameSize = VideoDataSet[0].FrameVector.size() - 1;

	ofstream myFile(outputfile);

	if (myFile.is_open() && MinSize >= 0 && FrameSize > 0) {
		myFile << "Video File Name: \t";
		myFile << VideoDataSet[0].NameofVideo << endl;
		myFile << "Frame Per Minute: \t";
		myFile << VideoDataSet[0].FPM << endl;
		myFile << "Time per Frame: \t";
		myFile << VideoDataSet[0].ConvertSec << endl;
		myFile << "Arena Radius in Pixels: \t";
		myFile << VideoDataSet[0].ArenaRadius << endl;
		myFile << "Areana Radius in Centimeter: \t";
		myFile << VideoDataSet[0].InputArenaRadius << endl;
		myFile << "Convertion Factor for Pixels to Centimenter: \t";
		myFile << VideoDataSet[0].ConvertCenti << endl;
		myFile << "Arena Center:" << endl;
		myFile << "X_Center:\t" << VideoDataSet[0].x_center << endl;
		myFile << "Y_Center:\t" << VideoDataSet[0].y_center << endl << endl;

		myFile << "Video Data" << endl << endl;
		myFile << "Total Time Active in Seconds:\t=SUM(E22:E"<<MinSize+22<<")\n";
		myFile << "Total Time InActive in Seconds:\t=SUM(F22:F" << MinSize + 22 << ")\n";
		myFile << "Spontenous Activity in [cm]:\t";
		if(MinSize>=5)
			myFile << "= SUM(B22:B" <<MinSize + 22 - 5 << ")\n";
		else
			myFile << VideoDataSet[0].SponteneousActivity << endl;

		myFile << "Exploration Distance in [cm]:\t=B22";
		myFile << endl << endl << endl;


		myFile << "Data by Minute" << endl << endl;
		myFile << "Minutes\tDistance [cm]\tAvg Active Velocity [cm/s]\tTime in Inner Arena [s]\tActive Time [s]\tInActive Time [s]\tShift Counts" << "\n";
		unsigned short int StartIndex = MinSize + 29;
		unsigned short int FPM = VideoDataSet[0].FPM;
		unsigned short int EndIndex = StartIndex + FPM;
		for (int i = 0; i <= MinSize; i++) {
			myFile << i << "\t";
			myFile << "=SUM(D" << StartIndex <<":D"<<EndIndex<<")" <<"\t";					//Distance 
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
		StartIndex = MinSize+29;
		EndIndex = StartIndex -1;
		for (int i = 0; i <= FrameSize; i++) {
			myFile << VideoDataSet[0].FrameVector[i].FrameNumber << "\t";
			myFile << VideoDataSet[0].FrameVector[i].X_Coord << "\t";
			myFile << VideoDataSet[0].FrameVector[i].Y_Coord << "\t";
			myFile << VideoDataSet[0].FrameVector[i].Distance << "\t";
			myFile << VideoDataSet[0].FrameVector[i].Velocity << "\t";
			myFile << "=IF(E"<<StartIndex<<">=1,1,0)" << "\t";
			myFile << "=IF(E" << StartIndex << "<=1,1,0)" << "\t";
			myFile << "=IF(OR(AND(E"<<StartIndex<<"<=1, E"<<StartIndex+1<<">=1),AND(E"<<StartIndex<<">=1,E"<<StartIndex+1<<"<=1)),1,0)"<< "\t";
			myFile << VideoDataSet[0].FrameVector[i].DistanceFromCenter << "\t";
			myFile << "=IF(I" << StartIndex << "<=($B$4/2),1,0)" << "\t";
			myFile << "=IF(E"<<StartIndex<<">=1,E"<<StartIndex<<",0)" << "\n";
		
			EndIndex = StartIndex;
			StartIndex++;
		}
	}

	else {
		cout << "Error: File Cannot Be Created" << endl << endl;
	}
	myFile.close();
	cout << "Finish Writing to Excel File" << endl << endl;
	return true;
}