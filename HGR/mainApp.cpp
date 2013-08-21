#include "stdafx.h"
#include "mainApp.h"

using namespace cv;

//--------------------------------------------------------------
void mainApp::run() {

	if( setup() == -1 ) {
		std::cout << "Setup failed" << std::endl;
		return;
	}
	else {
		std::cout << "Setup complete" << std::endl;
	}

	while(1) { //main loop

		bool bSuccess = videoCapture.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			std::cout << "Cannot read a frame from video file" << std::endl;
			break;
		}

		update();
		draw();

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			std::cout << "esc key is pressed by user" << std::endl;
			destroyAllWindows();
			break;
		}
	}
	return;
}

//--------------------------------------------------------------
int mainApp::setup()
{
	videoCapture = VideoCapture(0); // open the video camera no. 0
	if (!videoCapture.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the video device" << std::endl;
		return -1;
	}
	double dWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	std::cout << "Frame size : " << dWidth << " x " << dHeight << std::endl;

	win_name = "HGR";
	namedWindow(win_name, CV_WINDOW_AUTOSIZE);

	HSV_converter = HSV_Converter();
	namedWindow(win_name+"_HSV", CV_WINDOW_AUTOSIZE);
	return 0;
}

//--------------------------------------------------------------
void mainApp::update()
{
	HSV_converter.Convert(frame);
}
//--------------------------------------------------------------
void mainApp::draw() {
	imshow(win_name,frame);
	imshow(win_name+"_HSV", HSV_converter.HSV_frame); //show the frame in "MyVideo" window
	
}
//--------------------------------------------------------------