#include <iostream>
#include "hsc_extractor.h"
#include "ofm_extractor.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <strstream>
#include <fstream>
#include "AIMG.h"

using namespace std;

void getVideoList(vector<string>& videolist, string& filename)
{
	videolist.clear();
	ifstream filein(filename);
	string tempStr;
	while (filein >> tempStr)
	{
		videolist.push_back(tempStr);
	}
	filein.close();
}





int main()
{
	//only shearlet PREDICT
	string modelFoldePath = "model";


	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// initialize face engine
	char wPath[MAX_PATH];
	char apiPath[MAX_PATH];
	GetWindowsDirectory(wPath, MAX_PATH);
	sprintf_s(apiPath, MAX_PATH, "%s\\Help\\%s\\jet9.chi", wPath, AIM_VERSION);

	HMODULE app = LoadAPI(apiPath);
	cout << AIM_VERSION << endl;
	// face detector

	HAIM hAIM = IAimCreate();
	cout << "hAIM:" << hAIM << endl;
	cout << "hththhhd"<< endl;
	// set params
	// min eye, max eye, roll, yaw, pitch, conf, quality, sort type
	TFaceParams params = { 30.0f, 10000.0f, 0, 90, 90, 0.2f, 100, 1, COMPACT_FEATURE };
	IAimSetParams(hAIM, &params);

	vector<string> videoList;
	string folder = "C:\\Users\\lulu\\Desktop\\MLP_ofm_sherlet\\MLP_ofm_sherlet\\x64\\Release";
	string filelistname = folder + "\\hsc_super_test_list.txt";
	getVideoList(videoList, filelistname);

	//LivenessDetector livenessDetector = CreateLivenessDetector(modelFoldePath.c_str(), 0.0, 0.0, 0.0, 0.0);
	hscExtractor *hscextractor = new hscExtractor("mlp_hscslt_ground.model");
	OFMExtractor *ofmextractor = new OFMExtractor("", "");

	for (int x = 0; x < videoList.size(); x++)
	{

		float label = 1;

		int count = 0;

		cv::VideoCapture cap(videoList[x]);

		int index0 = videoList[x].find_last_of("\\");
		string temp = videoList[x].substr(index0);

		int index = temp.find_first_of(".");
		string suffix_name = temp.substr(index + 1);
		cout << suffix_name << endl;
		if (strcmp(suffix_name.c_str(), "real.avi") == 0)
		{
			cout << 1 << endl;
			label = 1.0;
		}
		else {
			cout << 0 << endl;
			label = 0.0;
		}


		for (int m = 0; m < 3; m++)
		{
			int stride = 6;
			int n_pics = 4;
			int start = m * stride * (n_pics + 1) + 30;

			cv::Mat *shearlet = new cv::Mat[2];

			cv::Mat frame;

			cap.set(CV_CAP_PROP_POS_FRAMES, start);

			long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
			cout << "整个视频共" << totalFrameNumber << "帧" << endl;


			cap >> frame;


			for (int i = 0; i < n_pics + 1; i++)
			{
				for (int k = 0; k < stride; k++)
				{
					cap >> frame;
				}

				TFaceDetectionResult templates2[1];
				count = IAimDetect(hAIM, (BYTE*)frame.data,
					frame.cols, frame.rows, frame.step.p[0],
					24, 1, templates2);


				cout << "faceCount:" << count << endl;

				if (count <= 0) {
					cout << "no face found:" << videoList[x] << endl;
					break;
				}

				int left = templates2[0].rtFace.left;
				int top = templates2[0].rtFace.top;
				int right = templates2[0].rtFace.right;
				int bottom = templates2[0].rtFace.bottom;


				int w = right - left;
				int h = bottom - top;

				right = ((right + w / 10) < frame.cols) ? right + w / 10 : frame.cols;
				left = ((left - w / 10) > 0) ? left - w / 10 : 0;
				top = ((top - h / 10) > 0) ? top - h / 10 : 0;
				bottom = ((bottom + h / 10) < frame.rows) ? bottom + h / 10 : frame.rows;

				cout << "*********************************" << endl;
				cout << right << "  " << left << "  " << top << "  " << bottom << endl;
				cout << frame.rows << "   " << frame.cols << endl;
				cout << "*********************************" << endl;

				cv::Mat faceFrame = frame.clone();

				cv::Mat face = faceFrame(cv::Range(top, bottom), cv::Range(left, right));
				cv::Mat dst;
				cv::resize(face, dst, cv::Size(256, 256));

				//cv::Mat dstMirror;

				if (i == 0){
					shearlet[0] = dst.clone();
					//cv::flip(dst.clone(), dstMirror, 1);
					//shearlet[1] = dstMirror.clone();
				}

				//images[i + 1] = frame.clone();
			}
			if (count <= 0) {
				delete[] shearlet;
				cout << "drop this group" << endl;
				break;
			}

			//cv::imwrite("source.jpg", shearlet[0]);
			//cv::imwrite("mirror.jpg", shearlet[1]);

			float hscScore = hscextractor->predict(shearlet[0], 256, 256);
			//float* shearletMirrorFeature = hscextractor->getHSCFeature(shearlet[1], 256, 256);

			if ((hscScore <= 0.5 && label == 1) || (hscScore > 0.5 && label == 0)){
				cout << "error score:" << hscScore << endl;

				string errorFileName = videoList[x].c_str();
				strstream ss;
				string s;
				ss << hscScore;
				ss >> s;
				errorFileName = errorFileName + "_" + s + ".jpg";

				cout << "file name:" << errorFileName.c_str() << endl;
				cv::imwrite(errorFileName.c_str(), shearlet[0]);

			}

			//shearlet_train_data.write((char*)shearletMirrorFeature, sizeof(float)* 512);
			//shearlet_train_data.flush();
			//shearlet_train_label.write((char*)&label, sizeof(float));
			//shearlet_train_label.flush();

			delete[] shearlet;
			//delete[] shearletMirrorFeature;
		}

		cap.release();

		if (count <= 0) {
			cout << "drop this video" << endl;
			continue;
		}

	}


	getchar();
	return 0;
}

