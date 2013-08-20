#ifndef MAINAPP_H
#define MAINAPP_H

#include "stdafx.h"

using namespace cv;

//mainApp singelton class
class mainApp
{  
private: 
	VideoCapture videoCapture;
	string win_name; //window name
	Mat frame;
	double dWidth,dHeight; //width and height of captured video

private:
	mainApp() {}
	mainApp(const mainApp &);
	mainApp& operator=(const mainApp&);

	int setup(); // Initial commands for setup processing
	void update(); // Commands to modify the parameters
	void draw(); // Drawing functions:

public:
	int a;

public:
	static mainApp& getInstance()
	{
		static mainApp instance;
		return instance;
	}
	void run();
};

#endif