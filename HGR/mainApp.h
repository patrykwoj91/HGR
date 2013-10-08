#include "stdafx.h"
#include "skin_detection.h"
#include "background_subtraction.h"

using namespace cv;

//mainApp singelton class
class mainApp
{  
private: 
	CvCapture* capture;
	IplImage* finalImage;
	IplImage* skin_mask;
	IplImage* background_mask;
	
private:
	mainApp() {}
	mainApp(const mainApp &);
	mainApp& operator=(const mainApp&);

	int setup(); // Initial commands for setup processing
	void update(); // Commands to modify the parameters
	void draw(); // Drawing functions:

public:
	IplImage *rawImage;
	int numerKlatki;
	background_subtraction back_subtractor;
	skin_detection skin_detector;

public:
	static mainApp& getInstance()
	{
		static mainApp instance;
		return instance;
	}
	void run();
};