#include "StdAfx.h"
#include "skin_detection.h"

skin_detection::skin_detection()
{	
	//zakresy dla masek
	hsv_min = /*cvScalar(0, 0, 0, 0);*/								cvScalar(0,0.2*255.0,0.35*255.0);
	hsv_max = /*cvScalar(50.0, 50, 0, 0);*/							cvScalar(50.0/2.0,0.68*255.0,1.0*255.0);
	nrgb_min = /*cvScalar(0, 0.28, 0.36, 0);*/						cvScalar(0,0.28,0.36);
	nrgb_max = /*cvScalar(1.0, 0.363, 0.465, 0);*/					cvScalar(1.0,0.363,0.465);

	mw[0] = 114.8755;
	mw[1] = 56.5551;
	macierz[0] = 0.1792 * 1000;
	macierz[1] = -0.1985 * 1000;
	macierz[2] = -0.1985 * 1000;
	macierz[3] = 2.8325 * 1000;

	// przygotowujemy macierze
	Cs = cvCreateMat(2, 2, CV_32FC1);
	cvInitMatHeader(Cs, 2, 2, CV_32FC1, macierz);
	CsInv = cvCreateMat(2, 2, CV_32FC1);
	cvInvert(Cs, CsInv);
	ms = cvCreateMat(1, 2, CV_32FC1);
	cvInitMatHeader(ms, 1, 2, CV_32FC1, mw);

	 X = cvCreateMat(1, 2, CV_32FC1);
	 Xms = cvCreateMat(1, 2, CV_32FC1);
	 XmsT = cvCreateMat(2, 1, CV_32FC1);
	 XmsCsInv = cvCreateMat(1, 2, CV_32FC1);
	 XmsCsInvXmsT = cvCreateMat(1, 1, CV_32FC1);
}

skin_detection::~skin_detection(void)
{
}

IplImage* skin_detection::getNRGB(IplImage* rawImage)
{
	IplImage* redchannel = cvCreateImage(cvGetSize(rawImage), 8, 1);
    IplImage* greenchannel = cvCreateImage(cvGetSize(rawImage), 8, 1);
    IplImage* bluechannel = cvCreateImage(cvGetSize(rawImage), 8, 1);
	
	IplImage* redavg = cvCreateImage(cvGetSize(rawImage), 8, 1);
    IplImage* greenavg = cvCreateImage(cvGetSize(rawImage), 8, 1);
    IplImage* blueavg= cvCreateImage(cvGetSize(rawImage), 8, 1);
    
	IplImage* imgavg = cvCreateImage(cvGetSize(rawImage), 8, 3);

	cvSplit(rawImage, bluechannel, greenchannel, redchannel, NULL);

	for(int x=0; x < rawImage->width ;x++)
    {
		for(int y=0;y < rawImage->height ;y++)
        {
			int redValue = cvGetReal2D(redchannel, y, x);
            int greenValue = cvGetReal2D(greenchannel, y, x);
            int blueValue = cvGetReal2D(bluechannel, y, x);

			 double sum = redValue+greenValue+blueValue;

			cvSetReal2D(redavg, y, x, redValue/sum*255);
            cvSetReal2D(greenavg, y, x, greenValue/sum*255);
            cvSetReal2D(blueavg, y, x, blueValue/sum*255);
        }
    }

	cvMerge(blueavg, greenavg, redavg, NULL, imgavg);

	cvReleaseImage(&redchannel);
    cvReleaseImage(&greenchannel);
    cvReleaseImage(&bluechannel);
    cvReleaseImage(&redavg);
    cvReleaseImage(&greenavg);
    cvReleaseImage(&blueavg);
 
    return imgavg;
}

void skin_detection::get_mask(IplImage *rawImage)
{
	cvCvtColor(rawImage,hsvImage,COLOR_RGB2HSV); //konwersja do HSV
	//nrgbImage = getNRGB(rawImage); //Normalize RGB 

	//cvInRangeS (hsvImage, hsv_min, hsv_max, hsvMask); 
	//cvInRangeS (nrgbImage, nrgb_min, nrgb_max, nrgbMask); 


	//combine the masks should be here
	//cvAnd(hsvMask,nrgbMask,maskImage);
}

void skin_detection::setup(IplImage * rawImage)
{
	hsvImage = cvCreateImage(cvGetSize(rawImage),8,3);
	nrgbImage = cvCreateImage(cvGetSize(rawImage),8,3);
	hsvMask = cvCreateImage(cvGetSize(rawImage),8,1);
	nrgbMask = cvCreateImage(cvGetSize(rawImage),8,1);
	maskImage = cvCreateImage(cvGetSize(rawImage), 8, 1);
	// liczymy
	height = hsvImage->height;
	width = hsvImage->width;
	step = hsvImage->widthStep;
	channels = hsvImage->nChannels;
}

void skin_detection::detection(IplImage *obrazHSV, double progLambda, unsigned int progKanaluV) {
	if (this->maskImage) {
		cvReleaseImage(&maskImage);
	}
	maskImage = cvCreateImage(cvSize(obrazHSV->width, obrazHSV->height), 8, 1);

	uchar* data = (uchar *)obrazHSV->imageData;
	int stepMask = maskImage->widthStep;
	uchar* dataMask = (uchar *)maskImage->imageData;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			cvmSet(X, 0, 0, (double)data[i*step+j*channels+0]);
			cvmSet(X, 0, 1, (double)data[i*step+j*channels+1]);
			cvSub(X, ms, Xms);
			cvTranspose(Xms, XmsT);
			cvMatMul(Xms, CsInv, XmsCsInv);
			cvMatMul(XmsCsInv, XmsT, XmsCsInvXmsT);
			// sprawdzamy przynaleznosc
			double lambda = cvmGet(XmsCsInvXmsT, 0, 0);
			dataMask[i*stepMask+j] = ((data[i*step+j*channels+2] >= progKanaluV) && (lambda < progLambda)) ? 255 : 0;
	}
}

IplImage* skin_detection::mask_skin(IplImage *rawImage)
{
	//get_mask(rawImage);
	//detection(hsvImage, 2, 40);

IplImage *YCrCb = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);

int x = 0 , y = 0;
int Cr = 0, Cb = 0,w=0,h=0;

cvCvtColor(rawImage,YCrCb,CV_BGR2YCrCb);
w = YCrCb->width, h = YCrCb->height;
for (y = 0; y < h ; y++)
for (x = 0; x < w ; x++)
{

Cr= (int)((unsigned char*)(YCrCb->imageData + YCrCb->widthStep*(y)))[(x)*3+1];
Cb =(int)((unsigned char*)(YCrCb->imageData + YCrCb->widthStep*(y)))[(x)*3+2];

if ( (Cr>alpha_slider && Cr<beta_slider ) && (Cb>alpha_slider_2 && Cb<beta_slider_2))
((unsigned char*)(maskImage->imageData + maskImage->widthStep*(y)))[(x)] = 255;
else
((unsigned char*)(maskImage->imageData + maskImage->widthStep*(y)))[(x)] = 0;
}


cvWaitKey(10);

	return maskImage;
}

