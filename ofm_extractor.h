#ifndef OFM_EXTRACTOR_H
#define OFM_EXTRACTOR_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include "mlp_classify.h"

class OFMExtractor
{
private:
	double alpha;
	double ratio;
	int minWidth;
	int nOuterFPIterations;
	int nInnerFPIterations;
	int nSORIterations;
	int feature_length;
	MLPNetwork ofm_face_network;
	MLPNetwork ofm_ground_network;

public:
	OFMExtractor(const char* face_modelPath, const char* ground_modelPath);
	~OFMExtractor();

	float face_predict(cv::Mat *images, int width, int height, int numImage);
	float ground_predict(cv::Mat *images, int width, int height, int numImage);

	float* getOFMFeature(cv::Mat *images, int width, int height, int numImage);
	//float* getGroundOFMFeature(cv::Mat *images, int width, int height, int numImage);

};

#endif
