#include "stdafx.h"
#include "skin_detection.h"
#include "background_subtraction.h"
#include "hand_detection.h"

using namespace cv;

//mainApp singelton class
class mainApp
{  
private: 
	CvCapture* capture;
	IplImage* skin_mask;
	IplImage* background_mask;
	IplImage* final_mask;
	
private:
	mainApp() {}
	mainApp(const mainApp &);
	mainApp& operator=(const mainApp&);

	int setup(); // Initial commands for setup processing
	void update(); // Commands to modify the parameters
	void draw(); // Drawing functions:
	void on_trackbar( int, void* );


public:
	IplImage *rawImage;
	int numerKlatki;
	background_subtraction back_subtractor;
	skin_detection skin_detector;
	hand_detection hand_detector;

public:
	static mainApp& getInstance()
	{
		static mainApp instance;
		return instance;
	}
	void run();
};