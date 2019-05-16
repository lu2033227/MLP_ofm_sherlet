#ifndef MLP_CLASSIFY_H
#define MLP_CLASSIFY_H

#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace std;

#define INT_SIZE 4
#define FLOAT_SIZE 4

class MLPNetwork
{
public:
	MLPNetwork(const char* modelPath = nullptr);
	virtual ~MLPNetwork();

	void readFromModel(const char* modelPath); //初始化网络模型
	float predict(float* feature, int featureLength); //根据特征得到分类结果
	void freeModel();

private:
	int layer_num;  //网络层数
	vector<cv::Mat> layers; //用于加载网络模型
	void reLU(cv::Mat &input);
};

#endif
