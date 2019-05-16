#include "hsc_extractor.h"
#include <opencv2/opencv.hpp>

hscExtractor::hscExtractor(const char* modelPath)
{
	if (modelPath) {
		hsc_network.readFromModel(modelPath);
	}
	nLevels = 4;
	nOrients = 8;
	int imageWidth = 256;
	nPartitions = 16;

	feature_length = 512; //HSCÌØÕ÷Î¬Êý

	blockWidth = imageWidth / (int)sqrt((float)nPartitions);
}

hscExtractor::~hscExtractor() 
{
	hsc_network.freeModel();
}

float hscExtractor::predict(cv::Mat image, int width, int height)
{
	//cv::Mat temp = cv::Mat(width, height, CV_8UC3, image);
	Vector<float> *shearHistogramFeatures = new Vector<float>(nLevels*nOrients*nPartitions);
	IplImage img(image);

	hsc.ShearletTransform(&img);
	hsc.ShearletHistogram(shearHistogramFeatures, blockWidth, blockWidth, blockWidth, blockWidth);

	float *hscfeature = new float[feature_length];
	for (int j = 0; j < feature_length; j++)
	{
		hscfeature[j] = shearHistogramFeatures->GetElement(j);
	}

	float score = hsc_network.predict(hscfeature, feature_length);

	delete shearHistogramFeatures;
	delete[] hscfeature;

	return score;
}

float* hscExtractor::getHSCFeature(cv::Mat image, int width, int height)
{
	Vector<float> *shearHistogramFeatures = new Vector<float>(nLevels*nOrients*nPartitions);
	IplImage img(image);

	hsc.ShearletTransform(&img);
	hsc.ShearletHistogram(shearHistogramFeatures, blockWidth, blockWidth, blockWidth, blockWidth);

	float *hscfeature = new float[feature_length];
	for (int j = 0; j < feature_length; j++)
	{
		hscfeature[j] = shearHistogramFeatures->GetElement(j);
	}
	delete shearHistogramFeatures;
	return hscfeature;
}

float* hscExtractor::getFLTFeature(cv::Mat image, int width, int height)
{
	Vector<float> *shearBlockAveFeatures = new Vector<float>(nLevels*nOrients*nPartitions);
	IplImage img(image);

	hsc.ShearletTransform(&img);
	hsc.ShearBlockAverage(shearBlockAveFeatures, blockWidth, blockWidth, blockWidth, blockWidth);

	float *fltfeature = new float[feature_length];
	for (int j = 0; j < feature_length; j++)
	{
		fltfeature[j] = shearBlockAveFeatures->GetElement(j);
	}
	delete shearBlockAveFeatures;
	return fltfeature;
}

float* hscExtractor::getCombinedFeature(cv::Mat image, int width, int height)
{
	float* shearHistogramFeatures = getHSCFeature(image, width, height);
	float* shearBlockAveFeatures = getFLTFeature(image, width, height);

	float *combinedfeature = new float[feature_length * 2];
	for (int j = 0; j < feature_length; j++)
	{
		combinedfeature[j] = shearHistogramFeatures[j];
	}
	for (int j = 0; j < feature_length; j++)
	{
		combinedfeature[j + feature_length] = shearBlockAveFeatures[j];
	}

	delete[] shearBlockAveFeatures;
	delete[] shearHistogramFeatures;

	return combinedfeature;
}