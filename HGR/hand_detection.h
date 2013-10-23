#pragma once
class hand_detection
{
public:
	CvSeq* contours;
	CvSeq* result;
	CvSeq* hull;
	CvSeq* defects;
	//CvMemStorage *storage;
	Rect bounding_rect;
	IplImage *cont;
	double largestArea;
	IplImage *skin;
	CvSeq* largest_contour;

	//////////////////////////////

	    
    
    float radius;
    CvPoint2D32f mincirclecenter;
    CvPoint mincirclecenter2;
          
    CvSeq* palm;
    //int hullcount;
    //int palmcount; 
    CvPoint pt0,pt,p,armcenter,center;
    CvBox2D palmcenter,contourcenter; 
      
    int palmsize[5];
    int palmsizecount; 
    bool palmcountfull; 
      
    CvPoint palmposition[5];
    int palmpositioncount;  
    bool palmpositionfull; 
    CvSeq* fingerseq;
	CvMemStorage* storage;
    CvMemStorage* defectstorage;
    CvMemStorage* palmstorage;     
    CvMemStorage* fingerstorage; 
	CvSeq* ptseq;

	//////////////////////////////
	void detect_hand(IplImage* skin_mask, IplImage* rawImage);
	void setup(IplImage * rawImage);
	void mark_features();

	void getconvexhull();


	hand_detection(void);
	~hand_detection(void);
};

