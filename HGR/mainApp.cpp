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
    cvNamedWindow( "Final_image",1);


	return 0;
}

//--------------------------------------------------------------
void mainApp::update()
{	
	if (numerKlatki == 1 && rawImage)
	{
		finalImage = cvCreateImage(cvGetSize(rawImage), 8, 1);
		back_subtractor.setup(rawImage);
		skin_detector.setup(rawImage);
	}
	if(rawImage)
	{
		//background_mask = back_subtractor.subtract_background(skin_detector.getNRGB(rawImage),numerKlatki);
		skin_mask = skin_detector.mask_skin(rawImage);
		
		//cvAnd(skin_mask,background_mask,finalImage);
	}

}
//--------------------------------------------------------------
void mainApp::draw() 
{
	 cvShowImage( "Raw",rawImage);
	 cvShowImage( "Skin_mask",skin_mask);
	// cvShowImage( "Background_mask", background_mask);
	// cvShowImage( "Final_image", finalImage);
}
//--------------------------------------------------------------