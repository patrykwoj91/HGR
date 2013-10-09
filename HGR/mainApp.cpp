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
		finalImage = cvCreateImage(cvGetSize(rawImage), 8, 3);
		final_mask = cvCreateImage(cvGetSize(rawImage), 8, 1);
		back_subtractor.setup(rawImage);
		skin_detector.setup(rawImage);
		createTrackbar( "Cr_min", "Raw", &skin_detector.alpha_slider, skin_detector.alpha_slider_max, NULL );
		createTrackbar( "Cr_max", "Raw", &skin_detector.beta_slider, skin_detector.alpha_slider_max, NULL );
		createTrackbar( "Cb_min", "Raw", &skin_detector.alpha_slider_2, skin_detector.alpha_slider_max, NULL );
		createTrackbar( "Cb_max", "Raw", &skin_detector.beta_slider_2, skin_detector.alpha_slider_max, NULL );
	}
	if(rawImage)
	{
		background_mask = back_subtractor.subtract_background(rawImage,numerKlatki);
		cvErode(background_mask,background_mask,NULL,5);
		cvDilate(background_mask,background_mask,NULL,5);

		skin_mask = skin_detector.mask_skin(rawImage);
		cvErode(skin_mask,skin_mask,NULL,5);
		cvDilate(skin_mask,skin_mask,NULL,5);

		cvAnd(skin_mask,background_mask,final_mask);
		cvErode(final_mask,final_mask,NULL,5);
		cvDilate(final_mask,final_mask,NULL,5);

		cvZero(finalImage);
		cvCopy(rawImage,finalImage,final_mask);
	}

}
//--------------------------------------------------------------
void mainApp::draw() 
{
	 cvShowImage( "Raw",rawImage);
	 cvShowImage( "Skin_mask",skin_mask);
	 cvShowImage( "Background_mask", background_mask);
	 cvShowImage( "Final_image", finalImage);
}
//--------------------------------------------------------------

