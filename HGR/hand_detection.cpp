#include "StdAfx.h"
#include "hand_detection.h"


hand_detection::hand_detection(void)
{

}


hand_detection::~hand_detection(void)
{
}

void hand_detection::setup(IplImage * rawImage)
{
	skin = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
	cont = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
}

void hand_detection::detect_hand(IplImage* skin_mask, IplImage* rawImage)
{
	cvCopy(skin_mask,skin);
	storage	= cvCreateMemStorage(0);
	int conNum = cvFindContours(skin, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0)); //find contours
	CvSeq* current_contour = contours;

	largestArea = 0;
	largest_contour = NULL;

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

	cvZero(cont);
	cvDrawContours(cont, largest_contour, CV_RGB(255,0,0), CV_RGB(255,255,255), 0, 2, 8); // Draw the largest contour using previously stored index.
	
	find_defects();
	cvShowImage("contour", cont);
}

void hand_detection::find_defects()
{

	CvMemStorage* storage1 = cvCreateMemStorage(0);
	CvMemStorage* storage2 = cvCreateMemStorage(0);
	CvMemStorage* storage3 = cvCreateMemStorage(0);

	if (largestArea > 5000)
	{
		largest_contour = cvApproxPoly( largest_contour, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
		CvPoint pt0;
		CvSeq* ptseq = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour),sizeof(CvPoint), storage1 );

		for(int i = 0; i < largest_contour->total; i++ )
		{ 
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, largest_contour, i );
			pt0.x = p->x;
			pt0.y = p->y;
			cvSeqPush( ptseq, &pt0 );
		}

		hull = cvConvexHull2( ptseq, 0, CV_CLOCKWISE, 0 );
		int hullcount = hull->total;
		defects= cvConvexityDefects(ptseq,hull,storage2 );

		CvConvexityDefect* defectArray;
		int j=0;

		// This cycle marks all defects of convexity of current contours.
		for(;defects;defects = defects->h_next)
		{
			int nomdef = defects->total; // defect amount
			if(nomdef == 0)
				continue;
			// Alloc memory for defect set.
			defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);
			// Get defect set.
			cvCvtSeqToArray(defects,defectArray, CV_WHOLE_SEQ);
			// Draw marks for all defects.
			for(int i=0; i<nomdef ; i++)
			{ 	
				cvLine(cont, *(defectArray[i].start), *(defectArray[i].depth_point),CV_RGB(255,255,0),1, CV_AA, 0 );
				cvCircle( cont, *(defectArray[i].depth_point), 5, CV_RGB(0,0,164), 2, 8,0);
				cvCircle( cont, *(defectArray[i].start), 5, CV_RGB(0,0,164), 2, 8,0);
				cvLine(cont, *(defectArray[i].depth_point), *(defectArray[i].end),CV_RGB(255,255,0),1, CV_AA, 0 );
			}
			j++;
			// Free memory.
			free(defectArray);
		}
	}
	cvReleaseMemStorage( &storage1 );
	cvReleaseMemStorage( &storage2 );
	cvReleaseMemStorage( &storage3 );
}