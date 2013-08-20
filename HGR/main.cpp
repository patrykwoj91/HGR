// HGR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mainApp.h"
using namespace std;
using namespace cv;

int main(int, char**)
{
	mainApp::getInstance().run();
	system("pause");
	return 0;
}

