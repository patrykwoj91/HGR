#include "StdAfx.h"
#include "background_subtraction.h"


background_subtraction::background_subtraction(void)
{
model = 0;
class1=1;
filename1[1000];
cc=0;
nframesToLearnBG = 100;

for (int i = 0 ; i<NCHANNELS ; i++)
	ch[i] = true;

model = cvCreateBGCodeBookModel();
//Set color thresholds to default values
model->modMin[0] = 3;
model->modMin[1] = model->modMin[2] = 3;
model->modMax[0] = 10;
model->modMax[1] = model->modMax[2] = 10;
model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;
}

background_subtraction::~background_subtraction(void)
{
}

void background_subtraction::subtract_background(IplImage *rawImage, int nframes)
{
	cvCvtColor( rawImage, yuvImage, CV_BGR2YCrCb );//YUV For codebook method
            
	//This is where we build our background model
            if( nframes-1 < nframesToLearnBG  )
                cvBGCodeBookUpdate( model, yuvImage );

            if( nframes-1 == nframesToLearnBG  )
                cvBGCodeBookClearStale( model, model->t/2 );

            //Find the foreground if any
            if( nframes-1 >= nframesToLearnBG  )
            {
                // Find foreground by codebook method
                cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook );

                // This part just to visualize bounding boxes and centers if desired
                cvCopy(ImaskCodeBook,ImaskCodeBookCC);
                cvSegmentFGMask( ImaskCodeBookCC );
            }
}

int background_subtraction::setup(IplImage *rawImage)
{
	// CODEBOOK METHOD ALLOCATION
    yuvImage = cvCloneImage(rawImage);
    ImaskCodeBook = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
    ImaskCodeBookCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
    cvSet(ImaskCodeBook,cvScalar(255));
	return 1;
}