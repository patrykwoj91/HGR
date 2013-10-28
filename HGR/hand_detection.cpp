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
	skin = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
	conts = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
	thresh = 25;
	max_thresh = 255;
	cont_max_thresh = 5000;
	cont_thresh = 300;
	cvNamedWindow( "Canny",1);
	createTrackbar( " Canny thresh:", "Canny", &thresh, max_thresh );
	createTrackbar( " Cont thresh:", "Canny", &cont_thresh, cont_max_thresh );
	
}

void hand_detection::detect_hand(IplImage* skin_image,IplImage* skin_mask)
{
	cvCopy(skin_image,skin);
	handstorage	= cvCreateMemStorage(0);
	filtcontstorage	= cvCreateMemStorage(0);

	IplImage * grey_scale = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 1 );
	IplImage * hsv = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 3 );
	IplImage * h = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 1 );
	IplImage * canny_out = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 1 );

	cvCvtColor( skin_image, grey_scale, CV_BGR2GRAY );
	cvSmooth( grey_scale, grey_scale,CV_MEDIAN,3 );

	cvCanny( grey_scale, canny_out, thresh, thresh*2, 3 );
	cvDilate(canny_out,canny_out);

	cvShowImage("Canny", canny_out);

	filtered_contour = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2,sizeof(CvContour),sizeof(CvPoint),filtcontstorage);
	CvContourScanner contours = cvStartFindContours(canny_out,handstorage, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	CvSeq* c;

	while( (c = cvFindNextContour( contours )) != NULL ) { 
		Rect r = cvBoundingRect(c,0);
		if( r.area() < cont_thresh ) 
		{
			cvSubstituteContour( contours, NULL );
		} 
	} 

	filtered_contour = cvEndFindContours(&contours);

	mark_features2(skin_mask);
}

void hand_detection::mark_features() //te metody do przerobienia
{
	handstorage = cvCreateMemStorage(0);
	fingerstorage = cvCreateMemStorage(0);
	minimumDistanceDepthPointToEndPointRatio = 0.0;  // adjust this to find best distance to detect finger

	if (filtered_contour)
	{
		CvMemStorage* storage2 = cvCreateMemStorage(0);
		CvMemStorage* storage3 = cvCreateMemStorage(0);
		hand = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),handstorage); 
		fingers = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),fingerstorage); 

		filtered_contour = cvApproxPoly( filtered_contour, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
		for(int i = 0; i < filtered_contour->total; i++ )
		{ 
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, filtered_contour, i );
			pt0.x = p->x;
			pt0.y = p->y;
			cvSeqPush( hand, &pt0 );
		}

		hull = cvConvexHull2( hand, 0, CV_CLOCKWISE, 0 );

		pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 ); 
		defects= cvConvexityDefects(hand,hull,storage2 );

		int max = 0;
		vector<int> distance(defects->total);

		for (int i = 0; i < defects->total; i++)
		{
			CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
			p.x = d->depth_point->x; 
			p.y = d->depth_point->y; 
			CvConvexityDefect* d1=(CvConvexityDefect*)cvGetSeqElem(defects,i+1); 
			p1.x = d1->end->x; 
			p1.y = d1->end->y; 
			distance[i] = (int)sqrt(pow((double)(p.x - p1.x),2) + pow((double)(p.y - p1.y), 2));
			max = std::max(max, distance[i]);
		}
		cvZero(conts);
		for (int i = 0; i < defects->total; i++)
		{
			if (distance[i] > minimumDistanceDepthPointToEndPointRatio * max)                    
			{
				CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
				CvPoint p;
				p.x = d->depth_point->x; 
				p.y = d->depth_point->y; 
				cvSeqPush( fingers,&p);	
			//	cvCircle(conts,p,5,CV_RGB(0,0,255),-1,CV_AA,0); 
				p.x = d->end->x; 
				p.y = d->end->y; 
				//cvCircle(conts,p,5,CV_RGB(255,0,0),-1,CV_AA,0); 
			}
		}
	
		
		for(int i = 0; i < hull->total; i++ ) 
		{ 
			pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i ); 
			cvLine( conts, pt0, pt, CV_RGB( 128, 128, 128 ),2,8,0); 
			pt0 = pt; 
		}

		/*for(int i=0;i<defects->total;i++) 
		{ 
			CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
				p.x = d->depth_point->x; 
				p.y = d->depth_point->y; 
				cvCircle(conts,p,5,CV_RGB(0,0,255),-1,CV_AA,0); 
				p.x = d->end->x; 
				p.y = d->end->y; 
				cvCircle(conts,p,5,CV_RGB(255,0,0),-1,CV_AA,0);
		}*/

	}

	cvShowImage("Features",conts);        

}


void hand_detection::mark_features2(IplImage* skin_mask)
{
	IplImage * skin = cvCreateImage( cvGetSize(skin_mask), IPL_DEPTH_8U, 1 );
	cvCopy(skin_mask,skin);
	handstorage = cvCreateMemStorage(0);
	fingerstorage = cvCreateMemStorage(0);
	int conNum = cvFindContours(skin, handstorage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0)); //find contour
	
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

		if (largest_contour)
	{
		cvZero(conts);
		cvDrawContours(conts, largest_contour, CV_RGB(255,255,255), CV_RGB(255,255,255),1,2);

		CvMemStorage* storage2 = cvCreateMemStorage(0);
		CvMemStorage* storage3 = cvCreateMemStorage(0);
		hand = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),handstorage); 
		fingers = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),fingerstorage); 

		largest_contour = cvApproxPoly( largest_contour, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
		for(int i = 0; i < largest_contour->total; i++ )
		{ 
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, largest_contour, i );
			pt0.x = p->x;
			pt0.y = p->y;
			cvSeqPush( hand, &pt0 );
		}

		hull = cvConvexHull2( hand, 0, CV_CLOCKWISE, 0 );

		pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 ); 
		defects= cvConvexityDefects(hand,hull,storage2 );

		int max = 0;
		vector<int> distance(defects->total);

		for (int i = 0; i < defects->total; i++)
		{
			CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
			p.x = d->depth_point->x; 
			p.y = d->depth_point->y; 
			CvConvexityDefect* d1=(CvConvexityDefect*)cvGetSeqElem(defects,i+1); 
			p1.x = d1->end->x; 
			p1.y = d1->end->y; 
			distance[i] = (int)sqrt(pow((double)(p.x - p1.x),2) + pow((double)(p.y - p1.y), 2));
			max = std::max(max, distance[i]);
		}
		cvZero(conts);
		for (int i = 0; i < defects->total; i++)
		{
			if (distance[i] > minimumDistanceDepthPointToEndPointRatio * max)                    
			{
				CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
				CvPoint p;
				p.x = d->depth_point->x; 
				p.y = d->depth_point->y; 
				cvSeqPush( fingers,&p);	
				cvCircle(conts,p,5,CV_RGB(0,0,255),-1,CV_AA,0); 
				p.x = d->end->x; 
				p.y = d->end->y; 
				cvCircle(conts,p,5,CV_RGB(255,0,0),-1,CV_AA,0); 
				p.x = d->start->x; 
				p.y = d->start->y; 
				cvCircle(conts,p,5,CV_RGB(0,255,0),-1,CV_AA,0); 
			}
		}
	
		for(int i = 0; i < hull->total; i++ ) 
		{ 
			pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i ); 
			cvLine( conts, pt0, pt, CV_RGB( 128, 128, 128 ),2,8,0); 
			pt0 = pt; 
		}

	/*	for(int i=0;i<defects->total;i++) 
		{ 
			CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
				p.x = d->depth_point->x; 
				p.y = d->depth_point->y; 
				cvCircle(conts,p,5,CV_RGB(0,0,255),-1,CV_AA,0); 
				p.x = d->end->x; 
				p.y = d->end->y; 
				cvCircle(conts,p,5,CV_RGB(255,0,0),-1,CV_AA,0);
		}*/

	}

		cvShowImage("Features",conts);        
}