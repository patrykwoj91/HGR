#pragma once
class hand_detection
{
public:
	CvSeq* contours;
	CvSeq* result;
	CvSeq* hull;
	CvSeq* filtered_hull;
	CvSeq* defects;
	CvSeq* largest_contour;
	//CvMemStorage *storage;
	//Rect bounding_rect;
	IplImage * conts;
	double largestArea;
	IplImage *skin;
	CvSeq* filtered_contour;

	//////////////////////////////

	    
    
    float radius;
    CvPoint2D32f mincirclecenter;
    CvPoint mincirclecenter2;
          
    CvSeq* hand;
	CvSeq* fingers;

    //int hullcount;
    //int palmcount; 
    CvPoint pt0,pt,p,p1,armcenter,center;
    CvBox2D palmcenter,contourcenter; 
     int minimumDistanceDepthPointToEndPointRatio;// adjust this to find best distance to detect finger
    int palmsize[5];
    int palmsizecount; 
    bool palmcountfull; 
      
    CvPoint palmposition[5];
    int palmpositioncount;  
    bool palmpositionfull; 

	CvMemStorage* handstorage;
	CvMemStorage* filtcontstorage;
    CvMemStorage* defectstorage;    
    CvMemStorage* fingerstorage; 
	CvSeq* ptseq;

	

	int thresh;
	int max_thresh;
	int cont_max_thresh;
	int cont_thresh;
	//////////////////////////////
	void detect_hand(IplImage* skin_image, IplImage* skin_mask);
	void setup(IplImage * rawImage);
	void mark_features();
	void mark_features2(IplImage* skin_mask);

	hand_detection(void);
	~hand_detection(void);
};

