#include "stdafx.h"
#include "mainApp.h"
using namespace cv;

//--------------------------------------------------------------
void mainApp::run() {

	rawImage = 0;
	capture = cvCaptureFromAVI("vid.avi");
	//capture = cvCaptureFromCAM( 0 );
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
    cvNamedWindow( "Skin_mask",1);
	
	return 0;
}

//--------------------------------------------------------------
void mainApp::update()
{	
	if (numerKlatki == 1 && rawImage)
	{
		final_mask = cvCreateImage(cvGetSize(rawImage), 8, 1);
		final_image = cvCreateImage(cvGetSize(rawImage), rawImage->depth, rawImage->nChannels);
		storage	= cvCreateMemStorage(0);
		back_subtractor.setup(rawImage);
		skin_detector.setup(rawImage);
		hand_detector.setup(rawImage);

	}
	if(rawImage)
	{
		background_mask = back_subtractor.subtract_background(rawImage,numerKlatki);
		skin_mask = skin_detector.mask_skin(rawImage);
		cvAnd(skin_mask,background_mask,final_mask);
		
		
		 // 132 170
		 //  105 168
	
		int conNum = cvFindContours(final_mask, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0)); //find contour
	
		CvSeq* current_contour = contours;

		largestArea = 0;
		largest_contour = NULL;

		while (current_contour != NULL)
		{
			double area = fabs(cvContourArea(current_contour));

		if(area > largestArea)
		{
        largestArea = area;
        largest_contour = current_contour;
		
		}
		current_contour = current_contour->h_next;
		}
		cvZero(final_mask);
		cvDrawContours(final_mask, largest_contour, CV_RGB(255,255,255), CV_RGB(255,255,255),0,CV_FILLED);
	
		cvErode(final_mask,final_mask,NULL,3);
		cvDilate(final_mask,final_mask,NULL,3); 

		IplImage* refImg = cvCreateImage(cvGetSize(rawImage), rawImage->depth, rawImage->nChannels);
		cvZero(final_image);
		cvZero(refImg);
		cvOr(rawImage, refImg, final_image, final_mask);
		cvReleaseImage(&refImg);

		if (back_subtractor.calibrated)
			hand_detector.detect_hand(final_image);
	
	}
}
//--------------------------------------------------------------
void mainApp::draw() 
{
	 cvShowImage( "Raw",rawImage);
	 //cvShowImage( "CrCb_mask",skin_detector.CrCbMask);
	 //cvShowImage( "cov_mask",skin_detector.covMask);
	 //cvShowImage( "Background_mask", background_mask);
	 cvShowImage( "Skin_mask",final_mask);
	 cvShowImage("final",final_image);
}
//--------------------------------------------------------------

