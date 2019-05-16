#pragma once
#include "TrackFeature.h"
#include "facedetectora.h"
#include "AAM.h"
#include "FaceDetector.h"

class CFaceTrack
{
public:
	CFaceTrack();
	~CFaceTrack();

	CandideMdl          m_CandideMdl;   // 3D model releated
	CTrackContext       m_Context           ;

	bool Init(const char* modelPath, const char* detectorPath, const int iWidth, const int iHeight);
	void ResetTracker();

	AAMResult* TrackImage(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape);
	bool DetectShape(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape);

	AAMResult* Track( BYTE *pGrayImage, RECT *pFaceRect = NULL, float* pflShape = NULL );
	bool Track_2D(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape);

	void* getDetector();

private:
	bool  tbInit;
	int   tnFrameIndex;
	int   tnLostFrameCnt ;  
	int   tnWidth, tnHeight;
	bool  tbNeedOuterInit;
public:
	CAAM   m_AAMIter[AAM_MODEL_NUM]    ;   
private:
	WIREFRAMEMODEL      m_ObjModel          ;       // object's wireframe model
	POSEPARAM3DOBJECT   m_PrevPoseParam     ;       // previous pose to original model position
	POSEPARAM3DOBJECT   m_CurrPoseParam     ;       // current pose to original model position

	void * adFacedetector;

	CFaceDetector*  tpFaceDetection    ;			//����������
	FaceRectList*       tpFaceRectList     ;
	RECT                trcFaceBak         ;

	BYTE               *tpPrevImage        ;		//������������
	BYTE               *tpCurrImage        ;
	BYTE               *tpWarpImageGrey    ;
	BYTE               *tpMaskImage        ;       // to be replaced with a Byte image
	short int          *tpMeshIndexMap     ;

	IMAGEFEATURE       *tpPrevFeatures ;       // image features in previous image, siez: NO_TRACKFEATURE
	IMAGEFEATURE       *tpCurrFeatures ;       // image features in current image, ditto
	VERTEX3D           *tpBaryCoords   ;       // barycentric coordinates, ditto
	TrianglesStruct    *tpFeaTriMesh   ;       // length:NO_TRACKFEATURE
	int                 tnTrackedFeatures  ;   // number of tracked features
	int                 tnPreSelFeaNum ;
	int                *tpnPreSelFeaIdx;       // pre-select feature index in AAM landmarks

	CTrackFeature       tfeaTracker    ;       //algorithm for 2D-3D

	AAMResult     *tpAAMRltTmp   ;
	AAMResult     *tpAAMRlt      ;
	int                 tnMdlIdx       ;

	LUTsOf2D3DAAM       m_LUTsOf2D3DAAM[AAM_MODEL_NUM][LUTS_2D3DAAM_NUM]    ;
	VarsOf2D3DAAM       m_VarsOf2D3DAAM     ;

	bool LoadCandide3Model(const char* modelPath, CandideMdl &candideMdl);
	bool LoadCandide3ModelFromBuf(void *p, CandideMdl &candideMdl);

	bool Fill_AllLUTsof2D3DAAM();
	bool AllocateAndFill_OneLUTsof2D3DAAM(
		LUTsOf2D3DAAM*  pLUTsOf2D3DAAM,
		const int iComVertexNum, 
		const int i2DNeedIntplNum, 
		const int iComTo2DIdx[], 
		const int iComTo3DIdx[],
		const int i2DNeedIntplIdx[],
		const int i2DIntplIdx[],
		const float fl2DIntplCoef[][2],
		const int iMotionConstNum, 
		const int iMotionConstIdx[]
	);

	CAAM* GetCurAAMIter() { return &m_AAMIter[tnMdlIdx]; }

	float  MinDistance(float x1,float y1, float x2,float y2, float x3, float y3);
	float  CalcuCurViewRatio();
	bool    GetNewModelIdx( const int& nOldMdlIdx, double dRatio, int& nNewMdlIdx );

	void Get2DAAMModelMask(
		BYTE *pMaskImage,
		W_Rect &rcFace,
		bool bSetMeshIndexMap);

	void  GetFeatureBaryCoords( IMAGEFEATURE* pImgFeatures );
	RECT* FaceDetection( BYTE *pGrayImage );
	void  Destroy();
};
