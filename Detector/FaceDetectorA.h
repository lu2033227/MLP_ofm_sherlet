#pragma once
#include "imagemodule.h"
#include <string>
#include <vector>
#include "Common.h"
#include <math.h>

#define  MAXSTAGE  100

typedef struct _tagFastRect
{
	int x;
	int y;
	int width;
	int height;
	int nClassIndex;
}FastRect,*PFastRect;

typedef struct _tagXRectangleAllStage
{   
	int		*pCoordinateX;					//分类器的坐标
	int		*pCoordinateY;						//分类器的坐标
	float	*pblockSize;						//一个弱分类器的各个小块的面积
}XRectangleAllStage;

class CObjectDetector//:public CHaarDetector
{
public:
	CObjectDetector(void);
	~CObjectDetector(void);

	int inline is_equalEx( const FastRect * r1, const FastRect* r2);
	int	 RectClusterEx();
	void InitFaceXStructEx(void* p);
	void ClearFaceXStructEx();
	void ResetEx();
	void SetSrcImgSizeEx(int img_w=320, int img_h=240,float downsamplescale=2.0f);
	void FaceDetectEx(unsigned char *uGrayInputImg,unsigned char *uColorInputImg=0);
	void FindFaceEx(int *fIntegral,int *fSquareIntegral,int *Integralskinmask=NULL);


public:
	int bIsLoad;
	std::vector <RECT> facerect;
	std::vector<FastRect>  fastfacerect;
	int model_width;
	int model_height;

	unsigned char *m_uGrayImg;
	unsigned char *m_ColorImg;
	unsigned char *m_Skinmask;
	int   *tfIntegral;
	int   *tfSquareIntegral;
	int   *tfColorIntegral;

	int   m_inputimg_w;
	int   m_inputimg_h;
	int   HaarImg_W;
	int   HaarImg_H;
	float m_scale;
	int   scale;



	int   *tempw_y; 
	int maskthreshold;
	int maxModelLevel; 
	int Integral[9];
	int IntegralRect[4];
	int SqIntegralRect[4];

	float fMean;
	float fVar;
	float downSampleLevel;

public:

	bool LoadFaceStruct(void* p);
	void DeleteFaceStruct();


public:
	int   tnStageNum;
	int	  tnTwoRectNum;
	int   tnThreeRectNum;
	int   tnFourRectNum;	
	float pStageThreashold[MAXSTAGE];		
	int   ArrayModelWidth[MAXSTAGE];
	int   ArrayModelHeight[MAXSTAGE];


	float *TwoRect_Alpha;
	float *TwoRect_Threshold;
	int   *TwoRect_Sign;
	int    TwoRect_WeakClassifierOfStage[MAXSTAGE];
	XRectangleAllStage TwoRect[FACEDETECT_MAXZOOMLEVEL];

	float *ThreeRect_Alpha;
	float *ThreeRect_Threshold;
	int   *ThreeRect_Sign;
	int    ThreeRect_WeakClassifierOfStage[MAXSTAGE];
	XRectangleAllStage ThreeRect[FACEDETECT_MAXZOOMLEVEL];

	float *FourRect_Alpha;
	float *FourRect_Threshold;
	int   *FourRect_Sign;
	int    FourRect_WeakClassifierOfStage[MAXSTAGE];
	XRectangleAllStage FourRect[FACEDETECT_MAXZOOMLEVEL];
};



typedef struct _FaceRectList
{
	int FaceNum;        //检测到人脸数
	RECT facerect[20];  //人脸区域
}FaceRectList,*PFaceRectList;


class CFaceDetector
{
public:
	//初始化模型
	bool Init(const char* modelPath);
	//释放模型
	void Release();
	//默认处理的图像是320*240,人脸检测是下采样率为2
	//若不是可在载入模型后修改图像大小
	//width图像宽度,height图像高度,type图像类型
	//downsamplescale，人脸检测时的下采样率
	//IsUseColorInfo = 1,人脸检测时利用肤色信息
	//type = GRAY_IMG 表示输入为灰度图象 type = BGR_IMG  输入为彩色图像格式为B\G\R
	//downsamplescale，图像下采样率，推荐320*240时，取2f或2.5f,176*144时，取1.6f
	void SetImageInfo(int width,int height,float downsamplescale);

	//设置输入的图像（倒立） 
	void SetImageData(unsigned char *uInputImg,bool isGray = true);

	//检测人脸区域
	FaceRectList *GetFaceRegion();

	CFaceDetector(void);
	~CFaceDetector(void);

private:
	int tnImgWidth;
	int tnImgHeight;
	unsigned char *m_uGrayImg;
	unsigned char *m_uGrayImg2;

private:
	CObjectDetector detector;
	int bHasImg;
	FaceRectList face;

};
