#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <conio.h>
#include <ctime>
#include "VideoData.h"

using namespace std;
using namespace cv;

VideoData FlyData;
//our sensitivity value to be used in the absdiff() function
const static int SENSITIVITY_VALUE = 20;
//size of blur used to smooth the intensity image output from absdiff() function
const static int BLUR_SIZE = 10;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = { 0,0 };
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);

//int to string helper function
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


	FlyData.FrameDataStruct.X_Coord = x;
	FlyData.FrameDataStruct.Y_Coord = y;
	
	FlyData.FrameVector.push_back(FlyData.FrameDataStruct);
	return;

}


void ArenaCalc(int event, int x, int y, int flags, void* userdata) {
	int size = FlyData.ArenaCoord.size();
	if (size < 3) {
		if (event == EVENT_LBUTTONDOWN) {
			cout << "x: " << x << "\t y: " << y << endl;
			circle((*(Mat*)userdata), Point(x, y), 2, CV_RGB(0, 255, 0), 3);

			FlyData.ArenaStruct.x_coord = x;
			FlyData.ArenaStruct.y_coord = y;

			FlyData.ArenaCoord.push_back(FlyData.ArenaStruct);
		}
	}
		else if (size == 3) {
			FlyData.ArenaCalc();
			circle((*(Mat*)userdata), Point(FlyData.x_center,FlyData.y_center), FlyData.ArenaRadius, CV_RGB(0, 255, 0), 3);
			rectangle((*(Mat*)userdata),Point(FlyData.x_center - FlyData.ArenaRadius -5,FlyData.y_center -FlyData.ArenaRadius -5), Point(FlyData.x_center + FlyData.ArenaRadius + 5, FlyData.y_center + FlyData.ArenaRadius + 5), Scalar(0, 255, 255),3);
			
		}
		

		imshow("img", (*(Mat*)userdata));
}


string VideoFileNameParser(string videoName) {
	string outputFileName;
	int size = videoName.size() - 1;
	if (size <= 0) {
		cout << "Error: Character Size of VideoName is less than 0" << endl;
		return "ERRROR";
	}
	else {
		int DotLocation = -1;
		int SlashLocation = -1;
		for (int i = size; i >= 0; i--) {
			if (videoName[i] == '.')
				DotLocation = i;
			else if (videoName[i] == '\\')
				SlashLocation = i;
			if (SlashLocation != -1 && DotLocation != -1)
				break;
		}
		if (DotLocation == -1 || SlashLocation == -1) {
			cout << "Error Parsing the The String" << endl;
			return"ERROR";
		}
		for (int i = SlashLocation+1; i <= DotLocation-1; i++)
			outputFileName = outputFileName + videoName[i];
	}
	cout << "Output File Name is: " << outputFileName << endl << endl;
	return outputFileName;
}

int main() {

	// Create a VideoCapture object and open the input file
	// If the input is the web camera, pass 0 instead of the video file name

	string Videoname;
	do {
		cout << "Enter Name of Video File" << endl << endl;
		getline(cin, Videoname);
		cout << endl << endl;
	} while (Videoname.empty());

	VideoCapture cap(Videoname);
	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		system("pause");
		return -1;
	}
	string FileName;
	char CheckFileName;
	do {
		cout << "Output File Name Same as Video Name? " << endl;
		cout << "Y or N" << endl;
		cin >> CheckFileName;
	} while (CheckFileName != 'Y' && CheckFileName != 'y' && CheckFileName != 'N' && CheckFileName != 'n');

	if (CheckFileName == 'Y' || CheckFileName == 'y')
		FileName = VideoFileNameParser(Videoname);
	else {
		do {
			cout << "Enter Name for the File" << endl;
			getline(cin, FileName);
			cout << endl << endl << endl;

		} while (FileName.empty() || FileName[0] == 32 || FileName[0] == 13);

	}


	Mat frame1, frame2;
	//their grayscale images (needed for absdiff() function)
	Mat grayImage1, grayImage2;
	//resulting difference image
	Mat differenceImage;
	//thresholded difference image (for use in findContours() function)
	Mat thresholdImage;
	//video capture object.
	cout << "Set Arena" << endl << endl << endl << endl << endl;
	bool areana = FlyData.setInputArenaRadius();
	//cout << "Areana: " << areana << endl;
	if (!areana) {
		cout << "Please Reset the program" << endl;
		return 0;
	}
	cout<< endl << endl << endl << endl;
	char key;
	bool Circle = true;
	cap >> frame1;
	cap >> frame2;
	Mat clone = frame1.clone();
	cout << "Please Select Point around Cimcumference of the Arena" << endl;
	cout << "Press ESC to once done" << endl;
	cout << "Press r to reset" << endl << endl << endl;
	system("pause");
	cout << endl << endl;
	int size;
	do {
		namedWindow("img", 0);
		setMouseCallback("img", ArenaCalc, &clone);
		imshow("img", clone);
		switch (key = waitKey(10))
		{
		case 27:
			size = FlyData.ArenaCoord.size();
			if (size==3) {
				Circle = false;
			}
			break;
		case 82:
			clone.empty();
			clone.release();
			clone = frame1.clone();
			FlyData.ArenaCoord.clear();
			break;   
		case 114:
			clone.empty();
			clone.release();
			clone = frame1.clone();
			FlyData.ArenaCoord.clear();
			break;
		}
	} while (Circle);
	destroyAllWindows();

	int k1 = FlyData.x_center - FlyData.ArenaRadius - 5;
	int m1 = FlyData.y_center - FlyData.ArenaRadius - 5;
	int height = 2 * FlyData.ArenaRadius + 10;
	int width = 2 * FlyData.ArenaRadius + 10;
	FlyData.x_center = width / 2;
	FlyData.y_center = height / 2;
	Rect CropRect = Rect(k1, m1, width, height);
	Mat CropFrame1 = frame1(CropRect);
	Mat CropFrame2 = frame2(CropRect);
	cout << endl << endl << endl;
	cout << "Video Processing About to Start press Esc key to stop video process" << endl;
	cout << endl << endl<<endl;
	system("pause");

	double framecounter = 0;
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
		FlyData.FrameVector[framecounter].FrameNumber = framecounter + 1;
		// Display the resulting frame
		imshow("Frame", CropFrame1);

		framecounter++;
		cout << "FrameCounter: " << framecounter << endl;
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
	
	system("pause");
	cout << endl << endl;
	FlyData.InitMinVector();
	FlyData.CalcFrameDistance();
	FlyData.CalcFrameVel();
	FlyData.CalcMinDistance();
	FlyData.CalcMinDistance();
	FlyData.CalcMinVel();
	FlyData.ActivityCalc();
	FlyData.InnerTimeCalc();
	//FlyData.DisplayVectors();
	FlyData.WriteToFile(FileName,Videoname);
	FlyData.WriteToXCL(FileName, Videoname);

	cout << endl << endl;
	system("pause");
	return 0;
}