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
	cont = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
	thresh = 100;
	max_thresh = 255;
	cvNamedWindow( "Canny",1);
	createTrackbar( " Canny thresh:", "Canny", &thresh, max_thresh );
}

void hand_detection::detect_hand(IplImage* skin_image)
{
	cvCopy(skin_image,skin);
	storage	= cvCreateMemStorage(0);

	IplImage * grey_scale = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 1 );
	IplImage * canny_out = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 1 );
	IplImage * conty = cvCreateImage( cvGetSize(skin_image), IPL_DEPTH_8U, 3 );
	
	cvCvtColor( skin_image, grey_scale, CV_BGR2GRAY );
	cvSmooth( grey_scale, grey_scale,CV_MEDIAN,3 );

	cvCanny( grey_scale, canny_out, thresh, thresh*2, 3 );
	//cvDilate(canny_out,canny_out);

	cvShowImage("Canny", canny_out);

	int conNum = cvFindContours(canny_out, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0)); //find contour
	/// Draw contours
		for( CvSeq* c=contours; c!=NULL; c=c->h_next )
		{
		cvDrawContours(skin_image, c, CV_RGB(255,0,0), CV_RGB(255,255,255), 100);
		}

	//zamaskowac obraz konturem, a potem dopiero dac canny tutaj odszumic przez treshhold minimalnej wielkosci konturu ;]
	
	/*CvSeq* current_contour = contours;

	largestArea = 0;
	largest_contour = NULL;

	while (current_contour != NULL)
	{
		Rect r = cvBoundingRect(current_contour,0);
	

		if(r.area() > largestArea)
		{
        largestArea = r.area();
        largest_contour = current_contour;
		
		}
    current_contour = current_contour->h_next;
	}

	cvZero(cont);
	cvDrawContours(cont, largest_contour, CV_RGB(255,0,0), CV_RGB(255,255,255), 0, 2, 8); // Draw the largest contour using previously stored index.
	*/
	//mark_features();
}

void hand_detection::mark_features()
{
	storage = cvCreateMemStorage(0);
    defectstorage = cvCreateMemStorage(0);
    palmstorage = cvCreateMemStorage(0);       
    fingerstorage = cvCreateMemStorage(0);

	fingerseq = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),fingerstorage);
    palmsizecount=0;  
    palmcountfull = false; 
    palmpositioncount=0;  
    palmpositionfull = false; 

	CvMemStorage* storage = cvCreateMemStorage(0); 
    CvMemStorage* defectstorage = cvCreateMemStorage(0); 
    CvMemStorage* palmstorage = cvCreateMemStorage(0);   
	palm = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),palmstorage); 

	  if(largest_contour)
        { 
               
                 contourcenter =  cvMinAreaRect2(largest_contour,0); 
                 armcenter.x = cvRound(contourcenter.center.x); 
                 armcenter.y = cvRound(contourcenter.center.y); 
                 getconvexhull();                                                                                               
        } 
}



void  hand_detection::getconvexhull() 
{ 
	
	CvMemStorage* storage1 = cvCreateMemStorage(0);
	CvMemStorage* storage2 = cvCreateMemStorage(0);
	CvMemStorage* storage3 = cvCreateMemStorage(0);
	CvMemStorage* storage4 = cvCreateMemStorage(0);
	//CvMemStorage* storage5 = cvCreateMemStorage(0);

		largest_contour = cvApproxPoly( largest_contour, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
		ptseq = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour),sizeof(CvPoint), storage1 );

		for(int i = 0; i < largest_contour->total; i++ )
		{ 
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, largest_contour, i );
			pt0.x = p->x;
			pt0.y = p->y;
			cvSeqPush( ptseq, &pt0 );
		}

		hull = cvConvexHull2( ptseq, 0, CV_CLOCKWISE, 0 );
		 

		pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 ); 
		defects= cvConvexityDefects(ptseq,hull,storage2 );

                    for(int i = 0; i < hull->total; i++ ) 
                    { 
                            pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i ); 
                            cvLine( cont, pt0, pt, CV_RGB( 128, 128, 128 ),2,8,0); 
                            pt0 = pt; 
                    } 

               
                    for(int i=0;i<defects->total;i++) 
                    { 
                         CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defects,i); 
                         if(d->depth > 10) 
                         { 
                            p.x = d->depth_point->x; 
                            p.y = d->depth_point->y; 
                            cvCircle(cont,p,5,CV_RGB(255,0,255),-1,CV_AA,0); 
							p.x = d->end->x; 
                            p.y = d->end->y; 
							cvCircle(cont,p,5,CV_RGB(255,255,0),-1,CV_AA,0); 
							p.x = d->start->x; 
                            p.y = d->start->y; 
							cvCircle(cont,p,5,CV_RGB(0,255,0),-1,CV_AA,0); 
                         } 
                     }                                           
} 
