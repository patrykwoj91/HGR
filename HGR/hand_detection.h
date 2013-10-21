#pragma once
class hand_detection
{
public:
	CvSeq* contours;
	CvSeq* result;
	CvSeq* hull;
	CvSeq* defects;
	CvMemStorage *storage;
	Rect bounding_rect;
	IplImage *cont;
	double largestArea;
	IplImage *skin;
	CvSeq* largest_contour;

	void detect_hand(IplImage* skin_mask, IplImage* rawImage);
	void setup(IplImage * rawImage);
	void find_defects();

	hand_detection(void);
	~hand_detection(void);
};

