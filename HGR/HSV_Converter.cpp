#include "StdAfx.h"
#include "HSV_Converter.h"


HSV_Converter::HSV_Converter()
{
}


HSV_Converter::~HSV_Converter(void)
{
}

void HSV_Converter::Convert(Mat frame)
{
	frame.copyTo(this->frame); // kopiuj klatke
	cvtColor(this->frame,HSV_frame,COLOR_BGR2HSV); //konwersja przestrzeni barwnej
	split(HSV_frame, HSV_split); // rozdziel na kanaly 
}