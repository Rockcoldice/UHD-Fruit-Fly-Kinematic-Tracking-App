#pragma once
#ifndef VIDEOPROCESSING
#define VIDEOPROCESSING
#endif // !VIDEOPROCESSING

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <iostream>
#include <ctime>
using namespace std;
using namespace cv;


class VideoProcessing
{
	
private:
	
	const static int SENSITIVITY_VALUE = 20;
	//size of blur used to smooth the intensity image output from absdiff() function
	const static int BLUR_SIZE = 10;
	//we'll have just one object to search for
	//and keep track of its position.
	int theObject[2] = { 0,0 };
	//bounding rectangle of the object, we will use the center of this as its position.
	Rect objectBoundingRectangle = Rect(0, 0, 0, 0);


	string intToString(int number) {
		//this function has a number input and string output
		std::stringstream ss;
		ss << number;
		return ss.str();
	}

	void searchForMovement(Mat thresholdImage, Mat &cameraFeed) {
		//notice how we use the '&' operator for objectDetected and cameraFeed. This is because we wish
		//to take the values passed into the function and manipulate them, rather than just working with a copy.
		//eg. we draw to the cameraFeed to be displayed in the main() function.
		bool objectDetected = false;
		Mat temp;
		thresholdImage.copyTo(temp);

		//these two vectors needed for output of findContours
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		//find contours of filtered image using openCV findContours function
		//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
		findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);// retrieves external contours

																						  //if contours vector is not empty, we have found some objects
		if (contours.size()>0)objectDetected = true;
		else objectDetected = false;

		if (objectDetected) {
			//the largest contour is found at the end of the contours vector
			//we will simply assume that the biggest contour is the object we are looking for.
			vector< vector<Point> > largestContourVec;
			largestContourVec.push_back(contours.at(contours.size() - 1));
			//make a bounding rectangle around the largest contour then find its centroid
			//this will be the object's final estimated position.
			objectBoundingRectangle = boundingRect(largestContourVec.at(0));
			int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
			int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;

			//update the objects positions by changing the 'theObject' array values
			theObject[0] = xpos, theObject[1] = ypos;
		}
		//make some temp x and y variables so we dont have to type out so much
		int x = theObject[0];
		int y = theObject[1];

		//draw some crosshairs around the object
		circle(cameraFeed, Point(x, y), 20, Scalar(0, 255, 0), 2);
		line(cameraFeed, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
		line(cameraFeed, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
		line(cameraFeed, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
		line(cameraFeed, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
		Point(x, y);

		//write the position of the object to the screen
		putText(cameraFeed, "Tracking object at (" + intToString(x) + "," + intToString(y) + ")", Point(x, y), 1, 1, Scalar(255, 0, 0), 2);


		FrameDataStruct.X_Coord = x;
		FrameDataStruct.Y_Coord = y;

		FrameVector.push_back(FrameDataStruct);
		return;

	}

	

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
		float DistanceFromCenter;
	};

	//		Minute Data Set

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
	string NameofVideo;
	bool Processing() {
		VideoCapture cap(NameofVideo);

		Mat frame1, frame2;
		//their grayscale images (needed for absdiff() function)
		Mat grayImage1, grayImage2;
		//resulting difference image
		Mat differenceImage;
		//thresholded difference image (for use in findContours() function)
		Mat thresholdImage;
		cout << endl << endl;
		char key;
		bool Circle = true;
		cap >> frame1;
		cap >> frame2;
		Mat clone = frame1.clone();

		int k1 = x_center - ArenaRadius - 5;
		int m1 = y_center - ArenaRadius - 5;
		int height = 2 * ArenaRadius + 10;
		int width = 2 * ArenaRadius + 10;
		x_center = width / 2;
		y_center = height / 2;
		Rect CropRect = Rect(k1, m1, width, height);
		Mat CropFrame1 = frame1(CropRect);
		Mat CropFrame2 = frame2(CropRect);
		system("CLS");
		unsigned int framecounter = 0;
		time_t StartTime = time(NULL);
		while (1) {

			// If the frame is empty, break immediately
			if (frame1.empty() || frame2.empty())
				break;
			cv::cvtColor(CropFrame1, grayImage1, COLOR_BGR2GRAY);
			//copy second frame

			//convert frame2 to gray scale for frame differencing
			cv::cvtColor(CropFrame2, grayImage2, COLOR_BGR2GRAY);
			//perform frame differencing with the sequential images. This will output an "intensity image"
			//do not confuse this with a threshold image, we will need to perform thresholding afterwards.
			cv::absdiff(grayImage1, grayImage2, differenceImage);
			//threshold intensity image at a given sensitivity value
			cv::threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
			//blur the image to get rid of the noise. This will output an intensity image
			cv::blur(thresholdImage, thresholdImage, cv::Size(BLUR_SIZE, BLUR_SIZE));
			//threshold again to obtain binary image from blur output
			cv::threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

			searchForMovement(thresholdImage, CropFrame1);
			cv::circle(CropFrame1,cv::Point(height/2, width/2), 2, (255, 0, 0), 2);
			FrameVector[framecounter].FrameNumber = framecounter + 1;
			// Display the resulting frame
			imshow("Frame", CropFrame1);
			framecounter++;
			int numofMin = framecounter / FPM;
			float numofSec = (framecounter % FPM) / (FPM / 60);
			cout << NameofVideo << "\t" << numofMin << ":" << numofSec << "\tFrameCounter: " << framecounter << endl;
			frame1.empty();
			frame1.release();
			frame1 = frame2.clone();
			cap >> frame2;
			if (frame2.empty()) break;
			CropFrame1 = frame1(CropRect);
			CropFrame2 = frame2(CropRect);

			// Press  ESC on keyboard to exit
			char c = (char)waitKey(25);
			if (c == 27)
				break;
		}
		time_t FinalTime = time(NULL);
		double dTime = FinalTime - StartTime;
		cout << "dTime (RAW): " << dTime << endl;
		dTime = dTime / 60;
		cout << "dTime (Min): " << dTime << endl;

		// When everything done, release the video capture object
		cap.release();

		// Closes all the frames
		destroyAllWindows();
		cout << endl << endl;
		InitMinVector();
		CalcFrameDistance();
		CalcFrameVel();
		CalcMinDistance();
		CalcMinDistance();
		CalcMinVel();
		ActivityCalc();
		InnerTimeCalc();
		//DisplayVectors();
		system("CLS");
		return true;
	}

	int ArenaRadius;
	int x_center;
	int y_center;
	Arena ArenaStruct;
	vector<Arena> ArenaCoord;			//Store the Data Points for the Circle Calculations

	FrameData FrameDataStruct;
	vector<FrameData> FrameVector;		//Store the Frame Data for futher Analysis

	MinuteData MinuteDataStruct;
	vector<MinuteData> MinuteVector;	//Store the Minute Data for Displaying;

	const int FPM = 1800;
	const float ConvertSec = 0.0333;
	float InputArenaRadius; // Store the Arena Radius
	float ConvertCenti;
	const int ActivityVelocity = 1;

	double ExplorationDistance = 0;
	double SponteneousActivity = 0;
	double ActiveTimeTotal = 0;
	double InActiveTimeTotal = 0;

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
	bool copyCoord(vector<unsigned short int>);
	VideoProcessing();
	~VideoProcessing();
};

