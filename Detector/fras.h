//
//  fras.h
//  AuthenVerifySDK
//
//  Created by Chenxu.Zhao on 15-3-19.
//  Copyright (c) 2015年 timedge. All rights reserved.
//


#include "FaceDetector.h"
#include "FaceAlignment.h"
#include "FaceGLRDet.h"

#ifndef JYVivoUI_fras_h
#define JYVivoUI_fras_h

const int CODE_LEN = 16;
const int KEY_LEN = 32;
const int LANDMARKCOUNT = 16;//关键点个数
const unsigned char key = 0x69;

const int key_index[CODE_LEN] = {
    5, 3, 9, 1, 31, 29, 18, 12,
    7, 2, 26, 19, 11, 14, 21, 23
};


struct FaceLandMarksOffsets
{
    float   left;
    float	top;
    float	right;
    float	bottom;
    float   landmarks[LANDMARKCOUNT * 2];
};//人脸框及关键点

struct DetectionResultOffsets
{
    float   left;
    float	top;
    float	right;
    float	bottom;
};//人脸框


struct FaceEngineOffsets
{
    void*    detector;
    ShapeRegressor*	 regressor;
    CFaceGLRDet*    lbpHandle;
};//引擎handle


#ifdef __cplusplus
extern "C" {
#endif
    
    char codePath[255] = "";
    char licPath[255] = "";

	//初始化引擎，加载三个模型文件
    void* fras_initialize(const char * mouthModelData, const char * detectModelData, const char * lbpModelData, int width, int height);
	//人脸检测，输入图片及长，宽，输出人脸框信息及是否检测到人脸，其中>=1为检测到人脸
    int fras_detect(void* handle, unsigned char * image, int width, int height, DetectionResultOffsets& dt);
	//根据人脸框关键点定位，输出关键点信息
    void fras_landmarksByRect(void* handle, unsigned char * image, int width, int height, float left, float top, float right, float bottom, FaceLandMarksOffsets& fm);
	//直接关键点定位，输出关键点信息
    int fras_landmarks(void* handle, unsigned char * image, int width, int height,
               FaceLandMarksOffsets& fm);
	//检测张闭嘴，输入图片及长，宽，输出嘴巴检测状态，大于0.6为张嘴，反之为闭嘴
    float fras_check_mouth_open(void* handle, unsigned char * image, int width, int height);
    float fras_check_mouth_close(void* handle, unsigned char * image, int width, int height);
	//根据人脸框检测张闭嘴，输入图片及长，宽，输出嘴巴检测状态，大于0.6为张嘴，反之为闭嘴
    float fras_check_mouth_open_by_rect(void* handle, unsigned char * image, int width, int height, float left, float top, float right, float bottom);
    float fras_check_mouth_close_by_rect(void* handle, unsigned char * image, int width, int height, float left, float top, float right, float bottom);
    
    //图像质量检测接口函数，参数数据对应原始图像，参数1为灰度图像字符串指针，updated by 2015.05.07
    float detectQuality(unsigned char* imageData, int imageWidth, int imageHeight, DetectionResultOffsets& dt/*, const char* path*/);
    
	//检测眼睛状态，0为闭眼，1为睁眼
    int fras_check_eyes_open(void* handle, unsigned char * image, int width, int height);
	//销毁引擎
    void fras_destroy(void* handle);
    
    
    
#ifdef __cplusplus
}
#endif

#endif
