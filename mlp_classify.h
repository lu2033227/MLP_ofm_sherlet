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

	void readFromModel(const char* modelPath); //��ʼ������ģ��
	float predict(float* feature, int featureLength); //���������õ�������
	void freeModel();

private:
	int layer_num;  //�������
	vector<cv::Mat> layers; //���ڼ�������ģ��
	void reLU(cv::Mat &input);
};

#endif
