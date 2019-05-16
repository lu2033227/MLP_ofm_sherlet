#include "ofm_extractor.h"
#include "ofm_extractor/Image.h"
#include "ofm_extractor/OpticalFlow.h"
#include <iostream>
#include <math.h>


OFMExtractor::OFMExtractor(const char* face_modelPath, const char* ground_modelPath)
{
	alpha = 0.012;
	ratio = 0.75;
	minWidth = 20;
	nOuterFPIterations = 7;
	nInnerFPIterations = 1;
	nSORIterations = 30;
	feature_length = 1024;

	ofm_ground_network.readFromModel(ground_modelPath);
	ofm_face_network.readFromModel(face_modelPath);
}

float OFMExtractor::ground_predict(cv::Mat *images, int width, int height, int numImage)
{
	if (images == NULL)
	{
		return -1;
	}

	DImage *frames = new DImage[numImage];

	for (int i = 0; i < numImage; i++)
	{
		frames[i].imreadfromMat(images[i]);
		frames[i].imresize(32, 32);
	}

	double* magnitudes = new double[feature_length];
	memset(magnitudes, 0, sizeof(double)* feature_length);
	DImage vx, vy, warpI2;

	DImage img1 = frames[0];
	for (int i = 1; i < numImage; i++)
	{
		DImage img2 = frames[i];

		OpticalFlow::Coarse2FineFlow(vx, vy, warpI2, img1, img2, alpha, ratio, minWidth, nOuterFPIterations, nInnerFPIterations, nSORIterations);

		double* xData = vx.pData;
		double* yData = vy.pData;

		for (int i = 0; i < vx.imHeight * vx.imWidth; i++){
			magnitudes[i] += double(sqrt(pow(xData[i], 2) + pow(yData[i], 2)));
		}

		img1.copyData(img2);

	}

	float* magnitudes_f = new float[feature_length];
	memset(magnitudes_f, 0, sizeof(float)* feature_length);
	for (int i = 0; i < feature_length; i++){
		magnitudes[i] /= (numImage - 1);
	}

	for (int i = 0; i < feature_length; i++)
	{
		magnitudes_f[i] = float(magnitudes[i]);
	}

	float score = ofm_ground_network.predict(magnitudes_f, feature_length);

	delete[] magnitudes_f;
	delete[] magnitudes;
	delete[] frames;

	return score;

}


float OFMExtractor::face_predict(cv::Mat *images, int width, int height, int numImage)
{
	if (images == NULL)
	{
		return -1;
	}

	DImage *frames = new DImage[numImage];

	for (int i = 0; i < numImage; i++)
	{
		//cv::Mat image = cv::Mat(width, height, CV_8UC3, images[i]);
		frames[i].imreadfromMat(images[i]);
		frames[i].imresize(32, 32);
	}

	double* magnitudes = new double[feature_length];
	memset(magnitudes, 0, sizeof(double)* feature_length);
	DImage vx, vy, warpI2;

	DImage img1 = frames[0];
	for (int i = 1; i < numImage; i++)
	{
		DImage img2 = frames[i];

		OpticalFlow::Coarse2FineFlow(vx, vy, warpI2, img1, img2, alpha, ratio, minWidth, nOuterFPIterations, nInnerFPIterations, nSORIterations);

		double* xData = vx.pData;
		double* yData = vy.pData;

		for (int i = 0; i < vx.imHeight * vx.imWidth; i++){
			magnitudes[i] += float(sqrt(pow(xData[i], 2) + pow(yData[i], 2)));
		}

		img1.copyData(img2);

	}

	float* magnitudes_f = new float[feature_length];
	memset(magnitudes_f, 0, sizeof(float)* feature_length);
	for (int i = 0; i < feature_length; i++){
		magnitudes[i] /= (numImage - 1);
	}

	for (int i = 0; i < feature_length; i++)
	{
		magnitudes_f[i] = float(magnitudes[i]);
	}


	float score = ofm_face_network.predict(magnitudes_f, feature_length);

	delete[] magnitudes_f;
	delete[] magnitudes;
	delete[] frames;

	return score;

}

float* OFMExtractor::getOFMFeature(cv::Mat *images, int width, int height, int numImage)
{ 
	if (images == NULL)
	{
		return NULL;
	}

	DImage *frames = new DImage[numImage];

	for (int i = 0; i < numImage; i++)
	{
		frames[i].imreadfromMat(images[i]);
		frames[i].imresize(32, 32);
	}

	double* magnitudes = new double[feature_length];
	memset(magnitudes, 0, sizeof(double)* feature_length);
	DImage vx, vy, warpI2;

	DImage img1 = frames[0];
	for (int i = 1; i < numImage; i++)
	{
		DImage img2 = frames[i];

		OpticalFlow::Coarse2FineFlow(vx, vy, warpI2, img1, img2, alpha, ratio, minWidth, nOuterFPIterations, nInnerFPIterations, nSORIterations);

		double* xData = vx.pData;
		double* yData = vy.pData;

		for (int i = 0; i < vx.imHeight * vx.imWidth; i++){
			magnitudes[i] += double(sqrt(pow(xData[i], 2) + pow(yData[i], 2)));
		}

		img1.copyData(img2);

	}

	float* magnitudes_f = new float[feature_length];
	memset(magnitudes_f, 0, sizeof(float)* feature_length);
	for (int i = 0; i < feature_length; i++){
		magnitudes[i] /= (numImage - 1);
	}

	for (int i = 0; i < feature_length; i++)
	{
		magnitudes_f[i] = float(magnitudes[i]);
	}
	delete[] magnitudes;
	delete[] frames;

	return magnitudes_f;
}


OFMExtractor::~OFMExtractor()
{
	ofm_ground_network.freeModel();
	ofm_face_network.freeModel();
}