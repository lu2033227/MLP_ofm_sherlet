#include "mlp_classify.h"

#include <algorithm>

MLPNetwork::MLPNetwork(const char* modelPath) :
		layer_num(0) {
	if ( modelPath ) {
		readFromModel(modelPath);
	}
}

MLPNetwork::~MLPNetwork() {
	freeModel();
}

void MLPNetwork::readFromModel(const char* modelPath) //初始化网络模型
{
	freeModel();

	ifstream in(modelPath, ios::binary);
	in.read((char*)&layer_num, INT_SIZE);

	for (int i = 0; i < layer_num; i++)
	{
		int rows = 0;
		int cols = 0;
		in.read((char*)&rows, INT_SIZE);
		in.read((char*)&cols, INT_SIZE);

		float *tempData = new float[rows * cols];
		
		in.read((char*)tempData, FLOAT_SIZE * rows * cols);
		cv::Mat weight = cv::Mat(rows, cols, CV_32FC1, tempData).clone();

		in.read((char*)tempData, FLOAT_SIZE * cols);
		cv::Mat bias = cv::Mat(1, cols, CV_32FC1, tempData).clone();

		delete[] tempData;

		layers.push_back(weight);
		layers.push_back(bias);
	}

	in.close();
}

float MLPNetwork::predict(float* feature, int featureLength)  //根据特征得到分类结果
{
	cv::Mat featureMat = cv::Mat(1, featureLength, CV_32FC1, feature).clone();

	for (int i = 0; i < layer_num; i++)
	{
		cv::Mat weight = layers[i * 2];
		cv::Mat bias = layers[i * 2 + 1];

		featureMat = featureMat * weight + bias;
		if (i != layer_num - 1)
			reLU(featureMat);
	}

	float x1 = featureMat.at<float>(0, 0);
	float x2 = featureMat.at<float>(0, 1);
	float sum = exp(x1) + exp(x2);
	float score = exp(x2) / sum;
	return score;
}

void MLPNetwork::freeModel()
{
	layer_num = 0;
	layers.clear();
}

void MLPNetwork::reLU(cv::Mat &input)
{
	std::replace_if(input.begin<float>(), input.end<float>(), [](float element) {
		return element < 0;
	}, 0);
}