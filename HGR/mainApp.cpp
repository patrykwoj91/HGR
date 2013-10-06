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
		
	//rawImage = cvLoadImage("rgb.jpg");


	while(1) { //main loop
		rawImage = cvQueryFrame( capture );
        ++numerKlatki;
            
		if(!rawImage)
		{
			std::cout << "Cannot read a frame from video file" << std::endl;
            break;
		}

		update();
		draw();

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			std::cout << "esc key is pressed by user" << std::endl;
			cvReleaseCapture( &capture );
			destroyAllWindows();
			break;
		}
	}
	return;
}

//--------------------------------------------------------------
int mainApp::setup()
{
	rawImage = 0; 
	capture = cvCaptureFromCAM( 0 );
	
	if( !capture )
    {
        printf( "Can not initialize video capturing\n\n" );
        return -1;
    }
    cvNamedWindow( "Raw", 1 );
    cvNamedWindow( "Mask",1);
	return 0;
}

//--------------------------------------------------------------
void mainApp::update()
{	
	if (numerKlatki == 1 && rawImage)
	{
		back_subtractor.setup(rawImage);
		skin_detector.setup(rawImage);
	}
	if(rawImage)
	{
		//back_subtractor.subtract_background(rawImage,numerKlatki);
		skin_detector.detect_skin(rawImage);
	}

}
//--------------------------------------------------------------
void mainApp::draw() 
{
	 cvShowImage( "Raw",rawImage);
	 cvShowImage( "Mask",skin_detector.maskImage);
}
//--------------------------------------------------------------