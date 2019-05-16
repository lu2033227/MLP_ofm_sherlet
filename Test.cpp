//#include <iostream>
//#include "hsc_extractor.h"
//#include "ofm_extractor.h"
//#include <opencv2/opencv.hpp>
//#include <string>
//#include <strstream>
//#include <fstream>
//#include "AIMG.h"
//#include "imtransform.h"
//
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
////cap from pic
////int main()
////{
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
////	vector<string> videoList;
////	string folder = "D:\\AuthenMetric\\151026";
////	string filelistname = folder + "\\cap_list.txt";
////	getVideoList(videoList, filelistname);
////
////	for (int m = 0; m < videoList.size(); m++){
////
////		cv::Mat frame = cv::imread(videoList[m].c_str());
////
////		TFaceDetectionResult templates2[1];
////		int count = IAimDetect(hAIM, (BYTE*)frame.data,
////			frame.cols, frame.rows, frame.step.p[0],
////			24, 1, templates2);
////
////
////		cout << "faceCount:" << count << endl;
////
////		if (count <= 0) {
////			cout << "no face found:" << videoList[m] << endl;
////			break;
////		}
////
////		int eyeleft = templates2[0].ptLeftEye.x;
////		int eyeright = templates2[0].ptRightEye.x;
////		int eyeWidth = eyeright - eyeleft;
////		float scale = (float)eyeWidth / 80.0;
////		cout << scale << endl;
////		int left = templates2[0].rtFace.left;
////		int top = templates2[0].rtFace.top;
////		int right = templates2[0].rtFace.right;
////		int bottom = templates2[0].rtFace.bottom;
////
////
////		int w = right - left;
////		int h = bottom - top;
////		//float ratio = scale;
////
////		int center_x = (left + right) / 2;
////		int center_y = (top + bottom) / 2;
////
////		right = ((center_x + w * 0.5 * scale) < frame.cols) ? (center_x + w * 0.5 * scale) : frame.cols;
////		left = ((center_x - w * 0.5 * scale) > 0) ? (center_x - w * 0.5 * scale) : 0;
////		top = ((center_y - h * 0.5 * scale) > 0) ? (center_y - h * 0.5 * scale) : 0;
////		bottom = ((center_y + h * 0.5 * scale) < frame.rows) ? (center_y + h * 0.5 * scale) : frame.rows;
////
////		/*right = ((right + w * ratio) < frame.cols) ? right + w * ratio : frame.cols;
////		left = ((left - w * ratio) > 0) ? left - w * ratio : 0;
////		top = ((top - h * ratio) > 0) ? top - h * ratio : 0;
////		bottom = ((bottom + h * ratio) < frame.rows) ? bottom + h * ratio : frame.rows;*/
////
////		cout << "*********************************" << endl;
////		cout << right << "  " << left << "  " << top << "  " << bottom << endl;
////		cout << frame.rows << "   " << frame.cols << endl;
////		cout << "*********************************" << endl;
////
////		cv::Mat faceFrame = frame.clone();
////
////		cv::Mat face = faceFrame(cv::Range(top, bottom), cv::Range(left, right));
////		cv::Mat dst;
////		cv::resize(face, dst, cv::Size(128, 128));
////
////		strstream ss;
////		string s;
////		ss << m;
////		ss >> s;
////		string errorFileName = s + ".bmp";
////
////		cout << "file name:" << errorFileName.c_str() << endl;
////		cv::imwrite(errorFileName.c_str(), dst);
////	}
////
////	getchar();
////	return 0;
////
////
////}
//
//
////cap from video
//int main()
//{
//
//
//	string modelFoldePath = "model";
//	
//	CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	
//	// initialize face engine
//	char wPath[MAX_PATH];
//	char apiPath[MAX_PATH];
//	GetWindowsDirectory(wPath, MAX_PATH);
//	sprintf_s(apiPath, MAX_PATH, "%s\\Help\\%s\\jet9.chi", wPath, AIM_VERSION);
//	HMODULE app = LoadAPI(apiPath);
//	
//	// face detector
//	
//	HAIM hAIM = IAimCreate();
//	cout << "hAIM:" << hAIM << endl;
//	// set params
//	// min eye, max eye, roll, yaw, pitch, conf, quality, sort type
//	TFaceParams params = { 30.0f, 10000.0f, 0, 90, 90, 0.2f, 100, 1, COMPACT_FEATURE };
//	IAimSetParams(hAIM, &params);
//	
//	
//	//zcx
//	/*const char* model = "authen_detect100.bmd";
//	const char* aam = "faceaam.dll";
//	void* detector = detector_create(1000, 1000, 10, model);
//	AAMInit(aam, model, width, height);*/
//	
//	
//	vector<string> videoList;
//	string folder = "D:\\AuthenMetric\\anti_spoofing3.0\\BLBP";
//	string filelistname = folder + "\\video_list.txt";
//	getVideoList(videoList, filelistname);
//	
//	//LivenessDetector livenessDetector = CreateLivenessDetector(modelFoldePath.c_str(), 0.0, 0.0, 0.0, 0.0);
//	hscExtractor *hscextractor = new hscExtractor("");
//	OFMExtractor *ofmextractor = new OFMExtractor("", "");
//
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
//		string temp = videoList[x].substr(index0);
//	
//		int index = temp.find_first_of(".");
//		string suffix_name = temp.substr(index + 1);
//		cout << suffix_name << endl;
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
//	
//		int sampleNum = (totalFrameNumber / 30 - 1) > 1 ? (totalFrameNumber / 30 - 1) : 1;
//	
//		for (int m = 0; m < sampleNum; m++)
//		{
//			int stride = 6;
//			int n_pics = 4;
//			int start = m * stride * (n_pics + 1) + 3;
//	
//			cv::Mat *shearlet = new cv::Mat[2];
//	
//			cv::Mat *faces = new cv::Mat[n_pics + 1];
//	
//			cv::Mat *grounds = new cv::Mat[n_pics + 1];
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
//				/*TFaceDetectionResult templates3[1];
//				if (i == 0){
//					count = IAimDetect(hAIM, (BYTE*)frame.data,
//						frame.cols, frame.rows, frame.step.p[0],
//						24, 1, templates3);
//				}
//				cv::Mat img_transform;
//	
//				img_transform = sim_transform_image_3channels(frame,
//					templates3[0].ptLeftEye.x,
//					templates3[0].ptLeftEye.y,
//					templates3[0].ptRightEye.x,
//					templates3[0].ptRightEye.y);
//	
//				string errorFileName = videoList[x].c_str();
//				strstream ss, ss1, ss2;
//				string s, s1, s2;
//				ss << x;
//				ss >> s;
//				ss1 << m;
//				ss1 >> s1;
//				ss2 << i;
//				ss2 >> s2;
//				errorFileName = errorFileName + "_" + s + "_" + s1 + "_" + s2 + "_face.jpg";
//	
//				cout << "file name:" << errorFileName.c_str() << endl;
//				cv::imwrite(errorFileName.c_str(), img_transform);
//	
//				faces[i] = img_transform.clone();*/
//
//				cv::Mat groundFrame = frame.clone();
//				//cv::Mat dstB;
//				//cv::resize(groundFrame, dstB, cv::Size(256, 256));
//
//				string errorFileName2 = videoList[x].c_str();
//				strstream ssx2, ss12, ss22;
//				string sx2, s12, s22;
//				ssx2 << x;
//				ssx2 >> sx2;
//				ss12 << m;
//				ss12 >> s12;
//				ss22 << i;
//				ss22 >> s22;
//				errorFileName2 = errorFileName2 + "_" + sx2 + "_" + s12 + "_" + s22 + "_ground.jpg";
//				cout << "file name:" << errorFileName2.c_str() << endl;
//				cv::imwrite(errorFileName2.c_str(), groundFrame);
//
//				//grounds[i] = dstB.clone();
//			}
//			/*if (count <= 0) {
//				delete[] shearlet;
//				delete[] faces;
//				delete[] grounds;
//				cout << "drop this group" << endl;
//				break;
//			}*/
//	
//			delete[] grounds;
//			delete[] faces;
//			delete[] shearlet;
//			//delete[] hscFeature;
//			//delete[] fltFeature;
//			//delete[] combinedFeature;
//			//delete[] shearletMirrorFeature;
//		}
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
//	getchar();
//	return 0;
//
//}
//
//
//
////int main()
////{
////	//only shearlet PREDICT
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
////	vector<string> videoList;
////	string folder = "D:\\AuthenMetric\\151026";
////	string filelistname = folder + "\\error_hsc_list.txt";
////	getVideoList(videoList, filelistname);
////
////	hscExtractor *hscextractor = new hscExtractor("mlp_hscslt_ground.model");
////
////	std::ofstream shearlet_train_label(folder + "\\error_hsc_result.txt");
////
////	for (int x = 0; x < videoList.size(); x++)
////	{
////
////		cv::Mat frame = cv::imread(videoList[x]);
////		TFaceDetectionResult templates2[1];
////		int count = IAimDetect(hAIM, (BYTE*)frame.data,
////			frame.cols, frame.rows, frame.step.p[0],
////			24, 1, templates2);
////
////		int eyeleft_x = templates2[0].ptLeftEye.x;
////		int eyeleft_y = templates2[0].ptLeftEye.y;
////		int eyeright_x = templates2[0].ptRightEye.x;
////		int eyeright_y = templates2[0].ptRightEye.y;
////		int eyeWidth = eyeright_x - eyeleft_x;
////		
////		int canshu = 128;
////		
////		float scale = 72.0 / (float)eyeWidth;
////		cout << scale << endl;
////		
////		float cols = frame.cols * scale;
////		float rows = frame.rows * scale;
////		
////		cv::Mat scaledSource;
////		cv::resize(frame.clone(), scaledSource, cv::Size(cols, rows));
////;
////		int left = ((eyeleft_x + eyeright_x) * scale / 2 - canshu / 2) > 0 ? ((eyeleft_x + eyeright_x) * scale / 2 - canshu / 2) : 0;
////		int right = ((eyeleft_x + eyeright_x) * scale / 2 + canshu / 2) < cols ? ((eyeleft_x + eyeright_x) * scale / 2 + canshu / 2) : cols;
////		int top = ((eyeleft_y + eyeright_y) * scale / 2 - canshu / 4) > 0 ? ((eyeleft_y + eyeright_y) * scale / 2 - canshu / 4) : 0;
////		int bottom = ((eyeleft_y + eyeright_y) * scale / 2 + canshu * 3 / 4) < rows ? ((eyeleft_y + eyeright_y) * scale / 2 + canshu * 3 / 4) : rows;
////
////		cv::Mat faceFrame = scaledSource.clone();
////		cv::Mat face = faceFrame(cv::Range(top, bottom), cv::Range(left, right));
////		cv::Mat dst;
////		cv::resize(face, dst, cv::Size(128, 128));
////
////		float hscScore = hscextractor->predict(dst.clone(), 128, 128);
////
////		cout << "hsc score:" << hscScore << endl;
////
////		string errorMessage = videoList[x].c_str();
////		strstream ss;
////		string s;
////		ss << hscScore;
////		ss >> s;
////		errorMessage = errorMessage + "_" + s + ".bmp";
////
////		cv::imwrite(errorMessage, dst.clone());
////		
////
////		
////
////		/*string errorMessage = videoList[x].c_str();
////		strstream ss;
////		string s;
////		ss << hscScore;
////		errorMessage = errorMessage + "_" + s + ".bmp";*/
////
////		//shearlet_train_label.write(errorMessage.c_str(), 1024);
////
////	}
////
////
////	getchar();
////	return 0;
////}
//
//
////int main(){
////	//ofm face test
////	string modelFoldePath = "model";
////	int top = 0, left = 0, bottom = 0, right = 0;
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
////	vector<string> videoList;
////	string folder = "D:\\AuthenMetric\\anti_spoofing3.0\\trainingdata\\ofm_face_se";
////	string filelistname = folder + "\\test_face_list.txt";
////	getVideoList(videoList, filelistname);
////	
////	//LivenessDetector livenessDetector = CreateLivenessDetector(modelFoldePath.c_str(), 0.0, 0.0, 0.0, 0.0);
////	OFMExtractor *ofmextractor = new OFMExtractor("", "");
////	
////	std::ofstream ofm_face_train_data(folder + "\\test_data_face.dat", ios::binary);
////	std::ofstream ofm_face_train_label(folder + "\\test_label_face.dat", ios::binary);
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
////		//int xxx = 3;
////		//xxx = (totalFrameNumber / 30 < 3) ? totalFrameNumber / 30 : 4;
////	
////		int sampleNum = (totalFrameNumber / 30 - 1) > 1 ? (totalFrameNumber / 30 - 1) : 1;
////	
////	
////		for (int m = 0; m < sampleNum; m++)
////		{
////			int stride = 6;
////			int n_pics = 4;
////			int start = m * stride * (n_pics + 1) + 3;
////			
////			
////	
////			cv::Mat *faces = new cv::Mat[n_pics + 1];
////	
////			cv::Mat frame;
////	
////			cap.set(CV_CAP_PROP_POS_FRAMES, start);
////			cap >> frame;
////
////			for (int i = 0; i < n_pics + 1; i++)
////			{
////				for (int k = 0; k < stride; k++)
////				{
////					cap >> frame;
////				}
////	
////				//cv::imwrite("f111.bmp", frame.clone());
////	
////				
////				if (i == 0){
////					TFaceDetectionResult templates2[1];
////					count = IAimDetect(hAIM, (BYTE*)frame.data,
////						frame.cols, frame.rows, frame.step.p[0],
////						24, 1, templates2);
////
////
////					cout << "faceCount:" << count << endl;
////
////					if (count <= 0) {
////						cout << "no face found:" << videoList[x] << endl;
////						break;
////					}
////
////					left = templates2[0].rtFace.left;
////					top = templates2[0].rtFace.top;
////					right = templates2[0].rtFace.right;
////					bottom = templates2[0].rtFace.bottom;
////
////					int w = right - left;
////					int h = bottom - top;
////
////
////					right = (right < frame.cols) ? right : frame.cols;
////					left = (left > 0) ? left : 0;
////					top = (top > 0) ? top / 4 : 0;
////					bottom = (bottom < frame.rows) ? bottom : frame.rows;
////
////					/*right = ((right + w / 4) < frame.cols) ? right + w / 4 : frame.cols;
////					left = ((left - w / 4) > 0) ? left - w / 4 : 0;
////					top = ((top - h / 4) > 0) ? top - h / 4 : 0;
////					bottom = ((bottom + h / 4) < frame.rows) ? bottom + h / 4 : frame.rows;*/
////
////					cout << "*********************************" << endl;
////					cout << right << "  " << left << "  " << top << "  " << bottom << endl;
////					cout << frame.rows << "   " << frame.cols << endl;
////					cout << "*********************************" << endl;
////				}
////
////				cv::Mat groundFrame = frame.clone();
////				cv::Mat faceFrame = frame.clone();
////	
////				cv::Mat face = faceFrame(cv::Range(top, bottom), cv::Range(left, right));
////				cv::Mat dst;
////				cv::resize(face, dst, cv::Size(256, 256));
////
////				cv::imwrite("ofm_face.bmp", dst.clone());
////	
////				cv::Mat dstB;
////				cv::resize(groundFrame, dstB, cv::Size(256, 256));
////
////				string errorFileName = videoList[x].c_str();
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
////				cv::imwrite(errorFileName, dst.clone());
////	
////				faces[i] = dst.clone();
////	
////				//images[i + 1] = frame.clone();
////			}
////			if (count <= 0) {
////				delete[] faces;
////				cout << "drop this group" << endl;
////				break;
////			}
////			float* faceFeature = ofmextractor->getOFMFeature(faces, 256, 256, n_pics + 1);
////	
////			ofm_face_train_data.write((char*)faceFeature, sizeof(float)* 1024);
////			ofm_face_train_data.flush();
////			ofm_face_train_label.write((char*)&label, sizeof(float));
////			ofm_face_train_label.flush();
////	
////			delete[] faces;
////			delete[] faceFeature;
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
////	ofm_face_train_data.close();
////	ofm_face_train_label.close();
////
////	getchar();
////	return 0;
////}
//
////
////int main()
////{
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
////
////	hscExtractor *hscextractor = new hscExtractor("mlp_hscslt_ground.model");
////	// set params
////	// min eye, max eye, roll, yaw, pitch, conf, quality, sort type
////	TFaceParams params = { 30.0f, 10000.0f, 0, 90, 90, 0.2f, 100, 1, COMPACT_FEATURE };
////	IAimSetParams(hAIM, &params);
////
////	cv::Mat frame = cv::imread("d.jpg", 1);
////
////	TFaceDetectionResult templates2[1];
////	int count = IAimDetect(hAIM, (BYTE*)frame.data,
////		frame.cols, frame.rows, frame.step.p[0],
////		24, 1, templates2);
////
////
////	cout << "faceCount:" << count << endl;
////	
////	int eyeleft_x = templates2[0].ptLeftEye.x;
////	int eyeleft_y = templates2[0].ptLeftEye.y;
////	int eyeright_x = templates2[0].ptRightEye.x;
////	int eyeright_y = templates2[0].ptRightEye.y;
////	int eyeWidth = eyeright_x - eyeleft_x;
////	
////	int canshu = 128;
////	
////	float scale = 72.0 / (float)eyeWidth;
////	cout << scale << endl;
////	
////	cv::circle(frame, cv::Point(templates2[0].ptLeftEye.x, templates2[0].ptLeftEye.y), 1, cv::Scalar(0, 0, 255), 1, 8);
////	cv::circle(frame, cv::Point(templates2[0].ptRightEye.x, templates2[0].ptRightEye.y), 1, cv::Scalar(0, 0, 255), 1, 8);
////	
////	cv::Mat scaledSource;
////	cv::resize(frame.clone(), scaledSource, cv::Size(frame.cols * scale, frame.rows * scale));
////	
////	
////	int left = (eyeleft_x + eyeright_x) * scale / 2 - canshu/2;
////	int right = (eyeleft_x + eyeright_x) * scale / 2 + canshu/2;
////	int top = (eyeleft_y + eyeright_y) * scale / 2 - canshu/4;
////	int bottom = (eyeleft_y + eyeright_y) * scale / 2 + canshu*3/4;
////	
////	cv::Mat faceFrame = scaledSource.clone();
////	
////	cv::Mat face = faceFrame(cv::Range(top, bottom), cv::Range(left, right));
////	
////	cv::imwrite("face.bmp", face.clone());
////	
////	float* hscFeature = hscextractor->getHSCFeature(face.clone(), 128, 128);
////	
////	std::ofstream hsc_feature("test.dat", ios::binary);
////	hsc_feature.write((char*)hscFeature, sizeof(float)* 512);
////	hsc_feature.flush();
////	hsc_feature.close();
////
////	getchar();
////	return 0;
////
////
////}