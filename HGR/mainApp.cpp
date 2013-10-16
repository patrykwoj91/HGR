#include "stdafx.h"
#include "mainApp.h"
using namespace cv;

//--------------------------------------------------------------
void mainApp::run() {

	rawImage = 0; 
	capture = cvCaptureFromCAM( 0 );
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
	
	if( !capture )
    {
        printf( "Can not initialize video capturing\n\n" );
        return;
    }
	else {
		std::cout << "Capturing initialized\n\n" << std::endl;
	}

	if (setup() == 0) 
		std::cout << "Setup Complete\n\n" <<std::endl;

	while(1) { //main loop
		rawImage = cvQueryFrame( capture );
		//rawImage = cvLoadImage("rgb2.jpg", CV_LOAD_IMAGE_COLOR);
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
	
    cvNamedWindow( "Raw", 1 );
    cvNamedWindow( "Final_image",1);
	cvNamedWindow( "cov_mask",1);
	cvNamedWindow( "CrCb_mask",1);
	cvNamedWindow( "Prob_mask",1);
	
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
		
	}
	if(rawImage)
	{
		background_mask = back_subtractor.subtract_background(rawImage,numerKlatki);
		//cvErode(background_mask,background_mask,NULL,5);
		//cvDilate(background_mask,background_mask,NULL,5);

		skin_mask = skin_detector.mask_skin(rawImage);
		//cvErode(skin_mask,skin_mask,NULL,5);
		//cvDilate(skin_mask,skin_mask,NULL,5);

		cvAnd(skin_mask,background_mask,final_mask);

		//cvErode(final_mask,final_mask,NULL,5);
		//cvDilate(final_mask,final_mask,NULL,5);

		cvZero(finalImage);
		cvCopy(rawImage,finalImage,background_mask);
	}

}
//--------------------------------------------------------------
void mainApp::draw() 
{
	 cvShowImage( "Raw",rawImage);
	 cvShowImage( "Skin_mask",skin_mask);


	// cvShowImage( "CrCb_mask",skin_detector.CrCbMaskI);
	// cvShowImage( "cov_mask",skin_detector.covMaskI);
	 cvShowImage( "Prob_mask",final_mask);

	 cvShowImage( "Background_mask", background_mask);
	 cvShowImage( "Final_image", finalImage);
}
//--------------------------------------------------------------

