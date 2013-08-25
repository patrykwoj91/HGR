#pragma once

#include "stdafx.h"

using namespace std;
using namespace cv;


static string toString(double value, int precision){
	stringstream sstr;
	sstr << fixed << setprecision(precision) << value;
	return sstr.str();
}

//--------------------------------------------------
static string toString(int value){
	stringstream sstr;
	sstr << value;
	return sstr.str();
}
//--------------------------------------------------

static Mat NormalizeRGB(Mat RGB)
{
	assert(RGB.type() == CV_8UC3); // ocen czy ten plik to faktycznie rgb
	Mat RGB32f; 
	RGB.convertTo(RGB32f, CV_32FC3); // typ obrazka z 8UC3 do 32bit3channels
	vector<Mat> RGB32f_split; //vektor na channele R G B
	split(RGB32f,RGB32f_split); //rozdziel na oddzielne kanaly

	Mat nRGB;
	Mat RGB32f_sum = RGB32f_split[0] + RGB32f_split[1] +RGB32f_split[2]; // suma potrzebna do normalizacji S = R + G + B

	RGB32f_split[0] = RGB32f_split[0] / RGB32f_sum; //Znormalizowane kanaly: R/S
	RGB32f_split[1] = RGB32f_split[1] / RGB32f_sum;						  // G/S
	RGB32f_split[2] = RGB32f_split[2] / RGB32f_sum;						  // B/S
	merge(RGB32f_split,nRGB); // znormalizowane RGB zlozone kanaly
	return nRGB;

}


