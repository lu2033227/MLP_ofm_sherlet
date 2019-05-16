#ifndef LIVENESS_DETECTOR_H
#define LIVENESS_DETECTOR_H

typedef void* LivenessDetector;

LivenessDetector CreateLivenessDetector(const char* modelPath, float T1, float T2, float T3, float T4);

float LivenessScore(LivenessDetector handle, unsigned char** inner_images, unsigned char** outer_images, int width, int height, int nFrames,
	float &score1, float &score2, float &score3, float &score4);

void FreeLivenessDetector(LivenessDetector handle);


#endif