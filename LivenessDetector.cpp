#include "LivenessDetector.h"
#include "ofm_extractor.h"
#include "hsc_extractor.h"
#include <string>

struct LivenessEngine
{
	hscExtractor *hscextractor;
	OFMExtractor *ofmextractor;
	float T1;
	float T2;
	float T3;
	float T4;
};


LivenessDetector CreateLivenessDetector(const char* modelPath, float T1, float T2, float T3, float T4)
{
	LivenessEngine* engine = new LivenessEngine;

	string modelPath_s = modelPath;
	engine->hscextractor = new hscExtractor((modelPath_s + "\\mlp_hscslt_ground.model").c_str());
	engine->ofmextractor = new OFMExtractor((modelPath_s + "\\mlp_ofm_face.model").c_str(),
		(modelPath_s + "\\mlp_ofm_c_ground.model").c_str());

	cout << "create Liveness Detector success" << endl;

	return (LivenessDetector)engine;

	//engine->ofmextractor = new OFMExtractor(modelPath);
}

//bool isLive(LivenessDetector handle, unsigned char** inner_images, unsigned char** outer_images, int width, int height, 
//	int ground_width, int ground_height, int nFrames)  old version

float LivenessScore(LivenessDetector handle, unsigned char** inner_images, unsigned char** outer_images, int width, int height, int nFrames,
	float &score1, float &score2, float &score3, float &score4)
{
	if (handle != NULL)
	{
		LivenessEngine* engine = (LivenessEngine*)handle;
		float score = 0.0;
		//float score1 = 0.0;
		//float score2 = 0.0;
		//float score3 = 0.0;
		//float score4 = 0.0;

		
		cv::Mat hsc_frame = cv::Mat(height, width, CV_8UC3, inner_images[0]);
		score1 = engine->hscextractor->predict(hsc_frame, width, height);
		cout << "sherlet score:" << score1 << endl;
		if (score1 < engine->T1)
			return score1;
		

		cv::Mat* ofm_ground_frames = new cv::Mat[nFrames];

		for (int i = 0; i < nFrames; i++)
		{
			ofm_ground_frames[i] = cv::Mat(height, width, CV_8UC3, outer_images[i]);
		}
		score3 = engine->ofmextractor->ground_predict(ofm_ground_frames, width, height, nFrames);
		cout << "ofm_ground:" << score3 << endl;
		delete[] ofm_ground_frames;

		if (score3 < engine->T3)
			return score3;

		cv::Mat* ofm_face_frames = new cv::Mat[nFrames];
		for (int i = 0; i < nFrames; i++)
		{
			ofm_face_frames[i] = cv::Mat(height, width, CV_8UC3, inner_images[i]);
		}
		score4 = engine->ofmextractor->face_predict(ofm_face_frames, width, height, nFrames);
		cout << "ofm_face:" << score4 << endl;
		delete[] ofm_face_frames;

		if (score4 < engine->T4)
			return score4;
			
		score2 = 1;
		score = 0.4 * score1 + 0.3 * score3 + 0.3 * score4;

		return score;
	}
	cout << "engine init failed " << endl;
	return false;
}

void FreeLivenessDetector(LivenessDetector handle)
{
	if (handle != NULL)
	{
		LivenessEngine* engine = (LivenessEngine*)handle;
		delete engine;
	}
	return;
	
}