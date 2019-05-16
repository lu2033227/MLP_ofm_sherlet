//#include "stdafx.h"
#include "AAMResult.h"
#include <string>

AAMResult::AAMResult(void)
{
	tbAlignSuccessful  = false;  
	tnIteCount         = 0;      
	tdbScore           = 0.0; 
	tdbReconErrInit    = 0;
	tdbReconErrFinal   = 0;
	trcFaceRect.left   = 0;
	trcFaceRect.top    = 0;
	trcFaceRect.right  = 0;
	trcFaceRect.bottom = 0;

	tnParaNum          = 0;      
	tpfParameters      = NULL;   

	tnLandmarkNum      = 0;      
	tpfShape           = NULL;
	tpfShapeNoRST      = NULL; 

	m_i3DVtxNum         = 0;
	tpf3DProj2DShape   = NULL;
	m_iSUNum            = 0;
	m_iAUNum            = 0;
	tpSUCoef           = NULL;
	tpAUCoef           = NULL;

	tflMainDir         = 0.f;       // main direction of motion according to feature matching, unit: rad
	tflMainDirMag      = 0.f;       // magnitude of main direction
}

AAMResult::~AAMResult(void)
{
	DestroyObject();
}

void AAMResult::CreateObject(int nParaNum, int nLandmarkNum)
{
	if ( tpfParameters && (tnParaNum != nParaNum) )
	{
		delete [] tpfParameters;
		tpfParameters = NULL;
	}
	if ( !tpfParameters )
	{
		tnParaNum      = nParaNum;
		tpfParameters  = new float[tnParaNum];
		memset(tpfParameters, 0, sizeof(float) * tnParaNum);
	}


	if ( tpfShape && tpfShapeNoRST && (tnLandmarkNum != nLandmarkNum) )
	{
		delete [] tpfShape;
		tpfShape = NULL;

		delete [] tpfShapeNoRST;
		tpfShapeNoRST = NULL;
	}
	if ( !tpfShape )
	{
		tnLandmarkNum  = nLandmarkNum;
		tpfShape       = new float[tnLandmarkNum * 2];
		tpfShapeNoRST  = new float[tnLandmarkNum * 2];

		memset(tpfShape, 0, sizeof(float) * tnLandmarkNum * 2);
		memset(tpfShapeNoRST, 0, sizeof(float) * tnLandmarkNum * 2);
	}     
}

void AAMResult::CreateObject(int nParaNum, int nLandmarkNum, int i3DVtxNum, int iSUNum, int iAUNum)
{
	if ( tpfParameters && (tnParaNum != nParaNum) )
	{
		delete [] tpfParameters;
		tpfParameters = NULL;
	}
	if ( !tpfParameters )
	{
		tnParaNum      = nParaNum;
		tpfParameters  = new float[tnParaNum];
		memset(tpfParameters, 0, sizeof(float) * tnParaNum);
	}      

	if ( tpfShape && tpfShapeNoRST && (tnLandmarkNum != nLandmarkNum) )
	{
		delete [] tpfShape;
		tpfShape = NULL;

		delete [] tpfShapeNoRST;
		tpfShapeNoRST = NULL;
	}
	if ( !tpfShape )
	{
		tnLandmarkNum  = nLandmarkNum;
		tpfShape       = new float[tnLandmarkNum * 2];
		tpfShapeNoRST  = new float[tnLandmarkNum * 2];

		memset(tpfShape, 0, sizeof(float) * tnLandmarkNum * 2);
		memset(tpfShapeNoRST, 0, sizeof(float) * tnLandmarkNum * 2);
	}    


	if ( tpf3DProj2DShape && (m_i3DVtxNum != i3DVtxNum) )
	{
		delete [] tpf3DProj2DShape;
		tpf3DProj2DShape = NULL;
	}
	if ( !tpf3DProj2DShape )
	{
		m_i3DVtxNum      = i3DVtxNum;
		tpf3DProj2DShape  = new float[m_i3DVtxNum * 2];
		memset(tpf3DProj2DShape, 0, sizeof(float) * m_i3DVtxNum * 2);
	}


	if ( tpSUCoef && (m_iSUNum != iSUNum) )
	{
		delete [] tpSUCoef;
		tpSUCoef = NULL;
	}
	if ( !tpSUCoef )
	{
		m_iSUNum    = iSUNum;
		tpSUCoef   = new float[m_iSUNum];
		memset(tpSUCoef, 0, sizeof(float) * m_iSUNum);
	}  


	if ( tpAUCoef && (m_iAUNum != iAUNum) )
	{
		delete [] tpAUCoef;
		tpAUCoef = NULL;
	}
	if ( !tpAUCoef )
	{
		m_iAUNum    = iAUNum;
		tpAUCoef   = new float[m_iAUNum];
		memset(tpAUCoef, 0, sizeof(float) * m_iAUNum);
	}  
}

// both alignrlt memory should be allocated before 
void AAMResult::Copy( const AAMResult*  pRlt)
{
	if ( tnParaNum == pRlt->tnParaNum && tpfParameters )
		memcpy(tpfParameters, pRlt->tpfParameters, sizeof(float) * tnParaNum);

	if ( tnLandmarkNum == pRlt->tnLandmarkNum && tpfShape )
	{
		memcpy(tpfShape, pRlt->tpfShape, sizeof(float) * tnLandmarkNum * 2);
		memcpy(tpfShapeNoRST, pRlt->tpfShapeNoRST, sizeof(float) * tnLandmarkNum * 2);
	}

	if ( m_i3DVtxNum == pRlt->m_i3DVtxNum && tpf3DProj2DShape )
		memcpy(tpf3DProj2DShape, pRlt->tpf3DProj2DShape, sizeof(float) * m_i3DVtxNum * 2);

	if ( m_iSUNum == pRlt->m_iSUNum && tpSUCoef )
		memcpy(tpSUCoef, pRlt->tpSUCoef, sizeof(float) * m_iSUNum);

	if ( m_iAUNum == pRlt->m_iAUNum && tpAUCoef )
		memcpy(tpAUCoef, pRlt->tpAUCoef, sizeof(float) * m_iAUNum);

	memcpy(m_RotAngleXYZ, pRlt->m_RotAngleXYZ, sizeof(m_RotAngleXYZ));
	memcpy(m_TranslXYZ, pRlt->m_TranslXYZ, sizeof(m_TranslXYZ));

	tbAlignSuccessful  = pRlt->tbAlignSuccessful  ;      
	tnIteCount         = pRlt->tnIteCount         ;   
	tdbScore           = pRlt->tdbScore           ; 
	tdbReconErrInit    = pRlt->tdbReconErrInit    ;
	tdbReconErrFinal   = pRlt->tdbReconErrFinal   ;
	trcFaceRect        = pRlt->trcFaceRect        ;   
	tnOffsetX          = pRlt->tnOffsetX          ;   
	tnOffsetY          = pRlt->tnOffsetY          ;   
	tflMainDir         = pRlt->tflMainDir         ;   
	tflMainDirMag      = pRlt->tflMainDirMag      ;   
}


void AAMResult::DestroyObject()
{
	if (tpfParameters)
	{
		delete [] tpfParameters;
		tpfParameters = NULL;
	}

	if (tpfShape)
	{
		delete [] tpfShape;
		tpfShape = NULL;
	}

	if (tpfShapeNoRST)
	{
		delete [] tpfShapeNoRST;
		tpfShapeNoRST = NULL;
	}

	if ( tpf3DProj2DShape )
	{
		delete [] tpf3DProj2DShape;
		tpf3DProj2DShape = NULL;
	}

	if ( tpSUCoef )
	{
		delete [] tpSUCoef;
		tpSUCoef = NULL;
	}

	if ( tpAUCoef )
	{
		delete [] tpAUCoef;
		tpAUCoef = NULL;
	}    
}

InitParaComp::InitParaComp()
{
	SetParameter(1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
};

InitParaComp::InitParaComp(float ScaleComp, float OffsetXComp, float OffsetYComp, float InitP1, float InitP2)
{
	SetParameter(ScaleComp, OffsetXComp, OffsetYComp, InitP1, InitP2);
};

void InitParaComp::SetParameter(float ScaleComp, float OffsetXComp, float OffsetYComp, float InitP1, float InitP2) 
{
	fScaleComp	= ScaleComp;
	fOffsetXComp = OffsetXComp;
	fOffsetYComp = OffsetYComp;
	fInitP1  = InitP1;
	fInitP2  = InitP2;
}


CTrackContext::CTrackContext()
{
	tbDispFeatureScore         = false;
	tbSaveTrackFeature         = false;
	tbDispTrackFeature         = false;
	tbDispInlierFeature        = false;
	tbSavePoseTrackerRlt       = false;
	tbDispPreAlignByFeaTrack   = false;
	tbDispPreAlignBy3DProjPts  = false;
	tbSaveAAMInitShape         = false;
	tbDispAAMIterate           = false;
	tbDispReconstructErrShape  = false;
	tbShow2DShape              = true ;
	tbShow3DShape              = true;
	tbPreSelectFeature         = true;    // false true
	tbEnableDebug       = false;
	tbDrawTrackFeature  = false;
	tbUseNewRansac      = true;        
	tbUseNewAlgorithm   = true;
	tbEnableVirtualKeyFrame = true;
	tbKeyframeMatchOnly = false;
	tbSmoothImage       = false;
	tbUseNewWarpFunc    = true;
	tbTrackSinglePerson	= true;
};

CTrackContext::~CTrackContext()
{

}
