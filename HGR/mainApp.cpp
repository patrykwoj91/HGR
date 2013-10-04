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

		bool bSuccess = videoCapture.read(raw_frame); // read a new frame from video

		//KOD DO TESTOWANIA NA OBRAZKU
		raw_frame = imread("rgb.jpg", CV_LOAD_IMAGE_COLOR); 
		if(! raw_frame.data )                           
		{
			cout <<  "Could not open or find the image" << std::endl ;
			break;
		}

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

	skin_detector = skin_detection();
	namedWindow(win_name+"_Mask", CV_WINDOW_AUTOSIZE);
	return 0;
}

//--------------------------------------------------------------
void mainApp::update()
{	
	skin_detector.set_frame(raw_frame);
	medianBlur(skin_detector.mask, skin_detector.mask, 3);
	skin_detector.get_bootstrap();
	skin_detector.calc_hist();
	//final_frame=skin_detector.train();
	skin_detector.predict(skin_detector.mask);

}
//--------------------------------------------------------------
void mainApp::draw() {
	imshow(win_name,raw_frame);
	imshow(win_name+"_Mask", skin_detector.mask); //show the frame in "MyVideo" window
	//imshow(win_name+"_Final", final_frame);
}
//--------------------------------------------------------------