#pragma once
class hand_detection
{
public:
	CvSeq* contours;
	CvSeq* result;
	CvMemStorage *storage;
	Rect bounding_rect;

	void detect_hand(IplImage* skin_mask, IplImage* rawImage);

	hand_detection(void);
	~hand_detection(void);
};

