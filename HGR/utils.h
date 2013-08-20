#ifndef UTILS_H
#define UTILS_H

#include "stdafx.h"

using namespace std;


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

#endif