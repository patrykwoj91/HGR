#pragma once

#include "stdafx.h"

using namespace cv;

class skin_detection
{
public: 
	
	Mat HSV_frame; // klatka po konwersji z BRG do HSV
	Mat mask; //maska stworzona na podstawie bootstrapu i histogramow dla obszarow skin i non skin

private:
	vector<Mat> HSV_split;   //skladowe H S V
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame; //bierzaca po normalizacji;
	MatND skin_Histogram; //histogram na pixeli uznanych za skore
	MatND non_skin_Histogram; //histogram dla pixeli nie uznanych..

	float theta_thresh;
    Scalar hist_bins;
    Scalar low_range;
    Scalar high_range;
    Scalar range_dist;
public:
	Mat detect_skin(Mat raw_frame);
	skin_detection();
	~skin_detection(void);
private:
	Mat toHSV(Mat frame);
	Mat get_bootstrap();
	void calc_hist();
	Mat train();

	MatND calc_rg_hist(const Mat& img, const Mat& mask, const Scalar& bins = Scalar(250, 250), const Scalar& low = Scalar(0, 0), const Scalar& high = Scalar(1, 1)) {
		Scalar channels(1, 2);
		return this->calc_2D_hist(img,mask,channels,bins,low,high);		
	}
	MatND calc_2D_hist(const Mat& img, const Mat& mask, Scalar wchannels, Scalar bins, Scalar low, Scalar high) {
		MatND hist;
		int histSize[] = { bins[0], bins[1] };
		float uranges[] = { low[0], high[0] };
		float vranges[] = { low[1], high[1] };
		const float* ranges[] = { uranges, vranges };
		int channels[] = {wchannels[0], wchannels[1]};
		
		calcHist( &img, 1, channels, mask,
				 hist, 2, histSize, ranges,
				 true, // the histogram is uniform
				 false );
				
		return hist;
	}

	void predict(Mat& output_mask) {
		Mat_<Vec3f> nrgb = NormalizeRGB(this->frame).reshape(3, this->frame.rows*this->frame.cols);
		Mat_<uchar> result_mask(nrgb.size());
		
		for (int i=0; i<nrgb.rows; i++) {
            if (nrgb.at<Vec3f>(i)[1] < low_range[0] || nrgb(i)[1] > high_range[0] ||
                nrgb(i)[2] < low_range[1] || nrgb(i)[2] > high_range[1])
            {
                result_mask(i) = 0;
                continue;
            }
			int gbin = cvRound((nrgb(i)[1] - low_range[0])/range_dist[0] * hist_bins[0]);
			int rbin = cvRound((nrgb(i)[2] - low_range[1])/range_dist[1] * hist_bins[1]);
			//float skin_hist_val = skin_Histogram.at<float>(gbin,rbin);
			/*if (skin_hist_val > 0) {
				float non_skin_hist_val = non_skin_Histogram.at<float>(gbin,rbin);
				if (non_skin_hist_val > 0) {
					if((skin_hist_val / non_skin_hist_val) > theta_thresh)
						result_mask(i) = 255;
					else 
						result_mask(i) = 0;
				} else {
					result_mask(i) = 0;
				}
			} else {
				result_mask(i) = 0;
			}*/
		}
		
		output_mask = result_mask.reshape(1, this->frame.rows);
	}
};

