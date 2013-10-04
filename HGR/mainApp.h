#include "stdafx.h"
#include "skin_detection.h"

using namespace cv;

//mainApp singelton class
class mainApp
{  
private: 
	VideoCapture videoCapture;
	string win_name; //window name
	double dWidth,dHeight; //width and height of captured video
	skin_detection skin_detector;

private:
	mainApp() {}
	mainApp(const mainApp &);
	mainApp& operator=(const mainApp&);

	int setup(); // Initial commands for setup processing
	void update(); // Commands to modify the parameters
	void draw(); // Drawing functions:

public:
	Mat raw_frame;
	Mat skin_frame;
	Mat final_frame;

public:
	static mainApp& getInstance()
	{
		static mainApp instance;
		return instance;
	}
	void run();
};