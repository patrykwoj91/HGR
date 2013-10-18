#include "StdAfx.h"
#include "hand_detection.h"


hand_detection::hand_detection(void)
{
	storage	= cvCreateMemStorage(0);

}


hand_detection::~hand_detection(void)
{
}

void hand_detection::detect_hand(IplImage* skin_mask, IplImage* rawImage)
{
	int conNum = cvFindContours(skin_mask, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0)); //find
	CvSeq* current_contour = contours;

	double largestArea = 0;
	CvSeq* largest_contour = NULL;

	while (current_contour != NULL)
	{
	 double area = fabs(cvContourArea(current_contour,CV_WHOLE_SEQ, 0));    
		if(area > largestArea)
		{
        largestArea = area;
        largest_contour = current_contour;
		}
    current_contour = current_contour->h_next;
	}
//	bounding_rect=cvBoundingRect(&largest_contour); // Find the bounding rectangle for biggest contour

	CvScalar color= cvScalar(255.0,255.0,255.0,0.0);
	CvScalar color2 = cvScalar(0.0,0.0,0.0,0.0);
	cvDrawContours(rawImage,largest_contour,color,color2,CV_FILLED,1,8);// Draw the largest contour using previously stored index.
}