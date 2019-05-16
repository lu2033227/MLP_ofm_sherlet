//#include <iostream>
//#include "hsc_extractor.h"
//#include "ofm_extractor.h"
//#include "imtransform.h"
//#include <opencv2/opencv.hpp>
//#include <string>
//#include <strstream>
//#include <fstream>
//using namespace std;
//
//void getVideoList(vector<string>& videolist, string& filename)
//{
//	videolist.clear();
//	ifstream filein(filename);
//	string tempStr;
//	while (filein >> tempStr)
//	{
//		videolist.push_back(tempStr);
//	}
//	filein.close();
//}
//
//int main()
//{
//
//
//	vector<string> videoList;
//	string folder = "C:\\Users\\lulu\\Desktop\\MLP_ofm_sherlet\\MLP_ofm_sherlet\\x64\\Release";
//	string filelistname = folder + "\\hsc_super_test_list.txt";
//	cout << filelistname << endl;
//	
//	getVideoList(videoList, filelistname);
//	cout << videoList[0] << endl;
//
//	for (int x = 0; x < videoList.size(); x++)
//	{
//
//		float label = 1;
//
//		int count = 0;
//
//		cv::VideoCapture cap(videoList[x]);
//
//		int index0 = videoList[x].find_last_of("\\");
//		cout << "index0: " << index0 << endl;
//
//		string temp = videoList[x].substr(index0);
//		cout <<"temp:"<< temp << endl;
//
//		int index = temp.find_first_of(".");
//		cout << "index :"<<index << endl;
//
//		string suffix_name = temp.substr(index + 1);
//		cout <<"suffix_name:" <<suffix_name << endl;
//		cout << "suffix_name.c_str():" << suffix_name.c_str() << endl;
//		if (strcmp(suffix_name.c_str(), "real.avi") == 0)
//		{
//			cout << 1 << endl;
//			label = 1.0;
//		}
//		else {
//			cout << 0 << endl;
//			label = 0.0;
//		}
//
//		long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
//		cout << "整个视频共" << totalFrameNumber << "帧" << endl;
//		int start = 3;
//
//		cv::Mat frame;
//
//		cap.set(CV_CAP_PROP_POS_FRAMES, start);
//
//		int j = 0;
//		for (int i = 0; i < totalFrameNumber; i++)
//		{
//			cap >> frame;
//			//if (i == 0) {			
//				string errorFileName = videoList[x].c_str();
//				strstream ss, ss1, ss2;
//				string s, s1, s2;
//				ss << x;
//				ss >> s;
//				ss1 << j;
//				ss1 >> s1;
//				errorFileName = errorFileName + "_" + s1 + ".jpg";
//
//				cout << "file name:" << errorFileName.c_str() << endl;
//				cv::imwrite(errorFileName.c_str(), frame.clone());
//
//				j++;
//				if (j == 100){
//					break;
//				}
//
//			//}
//		}
//
//		/*int sampleNum = (totalFrameNumber / 30 - 1) > 1 ? (totalFrameNumber / 30 - 1) : 1;
//
//		for (int m = 0; m < sampleNum; m++)
//		{
//			int stride = 6;
//			int n_pics = 4;
//			int start = m * stride * (n_pics + 1) + 3;
//
//			cv::Mat frame;
//
//			cap.set(CV_CAP_PROP_POS_FRAMES, start);
//
//			long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
//			cout << "整个视频共" << totalFrameNumber << "帧" << endl;
//
//
//			cap >> frame;
//
//
//			for (int i = 0; i < n_pics + 1; i++)
//			{
//				for (int k = 0; k < stride; k++)
//				{
//					cap >> frame;
//				}
//
//				string errorFileName = videoList[x].c_str();
//				strstream ss, ss1, ss2;
//				string s, s1, s2;
//				ss << x;
//				ss >> s;
//				ss1 << m;
//				ss1 >> s1;
//				errorFileName = errorFileName + "_" + s + "_" + s1 + "_" + s2 + ".jpg";
//				
//				cout << "file name:" << errorFileName.c_str() << endl;
//				cv::imwrite(errorFileName.c_str(), frame.clone());
//			}
//
//
//			
//		}*/
//
//		cap.release();
//
//		if (count <= 0) {
//			cout << "drop this video" << endl;
//			continue;
//		}
//
//	}
//
//
//
//	getchar();
//	return 0;
//}
//
//
//
//
//
////int main()
////{
////
////	//predict
////	string modelFoldePath = "model";
////
////
////	CoInitializeEx(NULL, COINIT_MULTITHREADED);
////
////	// initialize face engine
////	char wPath[MAX_PATH];
////	char apiPath[MAX_PATH];
////	GetWindowsDirectory(wPath, MAX_PATH);
////	sprintf_s(apiPath, MAX_PATH, "%s\\Help\\%s\\jet9.chi", wPath, AIM_VERSION);
////	HMODULE app = LoadAPI(apiPath);
////
////	// face detector
////
////	HAIM hAIM = IAimCreate();
////	cout << "hAIM:" << hAIM << endl;
////	// set params
////	// min eye, max eye, roll, yaw, pitch, conf, quality, sort type
////	TFaceParams params = { 30.0f, 10000.0f, 0, 90, 90, 0.2f, 100, 1, COMPACT_FEATURE };
////	IAimSetParams(hAIM, &params);
////
////
////	//zcx
////	/*const char* model = "authen_detect100.bmd";
////	const char* aam = "faceaam.dll";
////	void* detector = detector_create(1000, 1000, 10, model);
////	AAMInit(aam, model, width, height);*/
////
////
////	vector<string> videoList;
////	string folder = "D:\\AuthenMetric\\anti_spoofing3.0\\trainingdata\\super_zcx";
////	string filelistname = folder + "\\train_list.txt";
////	getVideoList(videoList, filelistname);
////
////	//LivenessDetector livenessDetector = CreateLivenessDetector(modelFoldePath.c_str(), 0.0, 0.0, 0.0, 0.0);
////	hscExtractor *hscextractor = new hscExtractor("");
////	OFMExtractor *ofmextractor = new OFMExtractor("mlp_ofm_face.model", "mlp_ofm_c_ground.model");
////
////
////	//int topFace = 0, leftFace = 0, bottomFace = 0, rightFace = 0;
////
////	for (int x = 0; x < videoList.size(); x++)
////	{
////
////		float label = 1;
////
////		int count = 0;
////
////		cv::VideoCapture cap(videoList[x]);
////
////		int index0 = videoList[x].find_last_of("\\");
////		string temp = videoList[x].substr(index0);
////
////		int index = temp.find_first_of(".");
////		string suffix_name = temp.substr(index + 1);
////		cout << suffix_name << endl;
////		if (strcmp(suffix_name.c_str(), "real.avi") == 0)
////		{
////			cout << 1 << endl;
////			label = 1.0;
////		}
////		else {
////			cout << 0 << endl;
////			label = 0.0;
////		}
////
////		long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
////		cout << "整个视频共" << totalFrameNumber << "帧" << endl;
////
////		int sampleNum = (totalFrameNumber / 30 - 1) > 1 ? (totalFrameNumber / 30 - 1) : 1;
////
////		for (int m = 0; m < sampleNum; m++)
////		{
////			int stride = 6;
////			int n_pics = 4;
////			int start = m * stride * (n_pics + 1) + 3;
////
////
////			cv::Mat *faces = new cv::Mat[n_pics + 1];
////
////			cv::Mat *grounds = new cv::Mat[n_pics + 1];
////
////			cv::Mat frame;
////
////			cap.set(CV_CAP_PROP_POS_FRAMES, start);
////
////			long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
////			cout << "整个视频共" << totalFrameNumber << "帧" << endl;
////
////
////			cap >> frame;
////
////
////			for (int i = 0; i < n_pics + 1; i++)
////			{
////				for (int k = 0; k < stride; k++)
////				{
////					cap >> frame;
////				}
////
////				TFaceDetectionResult templates3[1];
////				if (i == 0){
////					count = IAimDetect(hAIM, (BYTE*)frame.data,
////						frame.cols, frame.rows, frame.step.p[0],
////						24, 1, templates3);
////				}
////				cv::Mat img_transform;
////
////				img_transform = sim_transform_image_3channels(frame,
////					templates3[0].ptLeftEye.x,
////					templates3[0].ptLeftEye.y,
////					templates3[0].ptRightEye.x,
////					templates3[0].ptRightEye.y);
////
////				/*string errorFileName = videoList[x].c_str();
////				strstream ss, ss1, ss2;
////				string s, s1, s2;
////				ss << x;
////				ss >> s;
////				ss1 << m;
////				ss1 >> s1;
////				ss2 << i;
////				ss2 >> s2;
////				errorFileName = errorFileName + "_" + s + "_" + s1 + "_" + s2 + ".bmp";
////
////				cout << "file name:" << errorFileName.c_str() << endl;
////				cv::imwrite(errorFileName.c_str(), img_transform);*/
////
////
////
////
////				//cv::Mat faceOFMFrame = frame.clone();
////				//cv::Mat faceOFM = faceOFMFrame(cv::Range(topFace, bottomFace), cv::Range(leftFace, rightFace));
////				//cv::Mat dstOFM;
////				//cv::resize(faceOFM, dstOFM, cv::Size(256, 256));
////
////				faces[i] = img_transform.clone();
////
////
////
////				cv::Mat groundFrame = frame.clone();
////				cv::Mat dstB;
////				cv::resize(groundFrame, dstB, cv::Size(256, 256));
////				grounds[i] = dstB.clone();
////			}
////			if (count <= 0) {
////				delete[] faces;
////				delete[] grounds;
////				cout << "drop this group" << endl;
////				break;
////			}
////
////			float faceScore = ofmextractor->face_predict(faces, 256, 256, n_pics + 1);
////
////			float groundScore = ofmextractor->ground_predict(grounds, 256, 256, n_pics + 1);
////
////			if (faceScore < 0.8 && label == 1.0 || faceScore >= 0.8 && label == 0.0){
////				string errorFileName = videoList[x].c_str();
////				strstream ss, ss1, ss2;
////				string s, s1, s2;
////				ss << x;
////				ss >> s;
////				ss1 << m;
////				ss1 >> s1;
////				ss2 << faceScore;
////				ss2 >> s2;
////				errorFileName = errorFileName + "_" + s + "_" + s1 + "_" + s2 + ".bmp";
////
////				cout << "file name:" << errorFileName.c_str() << endl;
////				cv::imwrite(errorFileName.c_str(), faces[0]);
////			}
////
////			if (groundScore < 0.8 && label == 1.0 || groundScore >= 0.8 && label == 0.0){
////				string errorFileName = videoList[x].c_str();
////				strstream ss, ss1, ss2;
////				string s, s1, s2;
////				ss << x;
////				ss >> s;
////				ss1 << m;
////				ss1 >> s1;
////				ss2 << groundScore;
////				ss2 >> s2;
////				errorFileName = errorFileName + "_" + s + "_" + s1 + "_" + s2 + ".bmp";
////
////				cout << "file name:" << errorFileName.c_str() << endl;
////				cv::imwrite(errorFileName.c_str(), grounds[0]);
////			}
////
////			delete[] grounds;
////			delete[] faces;
////
////		}
////
////		cap.release();
////
////		if (count <= 0) {
////			cout << "drop this video" << endl;
////			continue;
////		}
////
////	}
////
////
////	getchar();
////	return 0;
////}