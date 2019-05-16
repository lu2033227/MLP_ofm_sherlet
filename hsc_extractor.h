#ifndef HSC_EXTRACTOR_H
#define HSC_EXTRACTOR_H

#include "hsc_extractor/HSC.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include "hsc_extractor/headers.h"
#include "mlp_classify.h"

class hscExtractor
{
private: 
	HSC hsc;
	int nLevels;
	int nOrients;
	int nPartitions;
	int blockWidth;
	int feature_length;
	MLPNetwork hsc_network;
public:
	hscExtractor(const char* modelPath);
	~hscExtractor();

	float predict(cv::Mat image, int width, int height);
	float* getHSCFeature(cv::Mat image, int width, int height);
	float* getFLTFeature(cv::Mat image, int width, int height);
	float* getCombinedFeature(cv::Mat image, int width, int height);

};

#endif
