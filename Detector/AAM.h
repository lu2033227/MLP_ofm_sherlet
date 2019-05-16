#pragma once
#include "Matrix.h"
#include "ImageUtil.h" 
#include "AAMResult.h"    
#include "ImageModule.h"   
#include "MotionHistory.h"     
//��ֹ 'sscanf' ��ȫ����
#pragma warning(disable:4996)

class CTrackContext;
class CAAMRunPara {
public:
	CAAMRunPara();
	~CAAMRunPara();
public:

	bool    tbUseHueOnlineConstr       ;
	bool    tbUse3DPoseConstr          ;
	bool    tbUseProfileMotionConstr   ;
	float   tflOnlineK         ;
	float   tflMIRCoefK        ;
	float   tfl3DConstrK       ;
	bool    tbUseICOVTracking          ;
	bool    tbUseMouthInequRestrict    ;
	bool    tbUse3DConstraint          ;
	bool    tbUseFeatureTrack          ;
	bool    tbUseCompGlbAAMConstraint  ;       // for Left/Right Eye_eyeBrow Component-AAM 
	bool    tbUseBEBCompGlbAAMConstraint  ;    // for Both_Eye_eyeBrow Component-AAM
	float   tflFeaTrackK       ;
	float   tflCompGlbAamK     ;
	float   tflHueOnlineK      ;
	float   tfl3DPoseConstrK   ;
	float   tflProfileMotionK  ;

};


class CAAM
{
	enum enRunMode
	{
		rmONLINE_TRACK  = 0,        //���߸��ٵ�����ģʽ�����ü򵥵ĸ��ٲ���
		rmOFFLINE_TEST  = 1         //����ͼƬ���Ե�����ģʽ����������
	};

	enum enRotationType
	{
		rtNormRotMatrix         = 1,          // �����ת����
		rtFstDeriv_x            = 2,          // �ֱ��x��y��z��ת����һ��ƫ�������ת����
		rtFstDeriv_y            = 3,
		rtFstDeriv_z            = 4
	};


public:               
	bool LoadModelData(void *p);                  

	void FreeModelData();                          
	void SetParameters(int width, int height, int nMaxIteCnt, double dbScoreThr = 0.10);   

	void RunAAM(unsigned char* inputGrayImg, RECT *pFaceRect, float* pflShape, EyePosition *pEyePos, 
		AAMResult* pAAMRlt, bool bTracking = true,
		InitParaComp* initParaComp = NULL, unsigned char* inputHueImg = NULL);            

	void    GetRelativePos(float *S,TrianglesStruct &tri,AAMPoint &point,float &alpha,float &beta,float &gamma); 

	void    FilterFeatureByMainDirection( AAMResult* pAAMRlt );                         
	bool    PreAlignByFeatureTrack(unsigned char* inputGrayImg, AAMResult* pAAMRlt );  
	void    CalcuParaByShape( float* pflShape, float* pflParameters );  

	void    ResetOnlineTempl();  
	void    GetPosePara3DObject( double* pRot, double* pTrans );  
	void    SynthesizeShape_qp(float* pflQ, float* pflP, float* pflShape);  

	void    RotateTransPoint3D(const float pRotMat[], const float pTrans[], const float*  pPnt3DSrc, float*  pPnt3DDst);
	double  CalcuReconstructErr();


private:
	void InitParameters();       
	void DeleteParameters();     
	int  EvaluateShape(const int nCount, double& dbScore, const bool tbHasOnlineTempl); // ����-1,ʧ�ܣ�����1,�ɹ�������0,������

	int     WarpImage();     
             
	void    GetROIGrayImage(BYTE* pInputGrayImg, BYTE* pbyROIGrayImg, RECT &faceRect, RECT &rcROI); 
	bool    UpdateFaceRect(float* shape, RECT& faceRect);   
	void    InitWithFaceRectOrEyePos(RECT *pFaceRect, EyePosition *pEyePos, InitParaComp* initParaComp, RECT& rcFaceRect);

	void    CalcuTemplateOnline();   

	// feature matching related
	void    AddConstraint_FeaTrack();   
	void    CalcuDerivativeSd_dqp();
	void    SynthesizeShape_p(float* pflP, float* pflShape);                            
	void    SynthesizeShape_q(float* pflQ, float* pflShapeIn, float* pflShapeOut);     
	void    CalcuSqDistSum( const float r, double& dbDistSum, int& nValidPtsCnt );    

	// impose mouse Inequation
	void    AddConstraint_MouthInequ( bool bTracking, float& flLipDist );    

	//3D constraint
	void    CalcRotationTrigVal();    
	void    SynPersKeyPoint3D( float*  pKeyPoint3D );
	void    SynPersKeyPoint3D( float*  pKeyPoint3D, float* pSUCoef, float* pAUCoef );
	void    GetRotMatrix(float pRotMat[], int nRotType);
	void    RotatePoint3D(const float pRotMat[], const float*  pPnt3DSrc, float*  pPnt3DDst);
	//3D Network
	void    Calc3DModelProj2DImgCrd(float*  pfl3DProj2DShape);           //old
	void    Calc3DModelProj2DImgCrd(float*  pfl3DProj2DShape,RECT rcROI);  //new
	void    Calcu3DPara_By_2DShape( const bool bTracking, const RECT rcROI, bool bCalcuOnlyPose );   
	void    AddConstraint_3DModel( CMATRIX &matHessian3D, CMATRIX &matSDTMulErr3D, const bool bTracking, const RECT rcROI ); 

	void    CalcuDerivativeNW_qp_FA( const int nCurShapeNum );
	void    RotMat2AngleDegree(const double *pInRotMat, float *pOutRotAngle );
	void    RotRad2Degree( float *pInRotAngle, float *pOutRotAngle ); 


	//////////////////////////////////////////////////////////////////////////
public:
	AAMModelStruct  aammodel;

	double  tdbSigma       ;   //���ڿ��Ƽ���EdgeCorner��calcuABC��ƽ���̶�
	int     tnMaxIteCnt    ;   //max iteration count
	double	tdbScoreThr	;	//�ж��Ƿ�fitting�õķ�����ֵ
	double	tdbRecErrThr	;	//�ж��Ƿ�fitting�õ��ع������ֵ
	int     tnRunMode      ;   //����ģʽ:���߸���(������ͷ����Ƶ�ļ�����) �� ͼƬ(����)����

	CTrackContext  *tpContext      ;   // just a pointer, not allocate memory
	CAAMRunPara     m_AAMRunPara    ;

private:

	int		SampleImgH      ;   //Sample image height(e.i, base appearance image height)
	int		SampleImgW      ;   //Sample image width(e.i, base appearance image width)
	int     InputImgW       ;   //input image width
	int     InputImgH       ;   //input image height
	int     ImgWidth        ;   //Roi width of input image
	int     ImgHeight       ;   //Roi height of input image

	// Index for each feature in total appearance
	int     tnGrayIndex        ;    //�Ҷ��������������������
	int     tnDxDyIndex        ;    //dx,dy�������������������
	int     tnEdgeCnrIndex     ;    //edge+corner�������������������
	int     tnWholeGMagIndex   ;    //Whole gradient magnitude�������������������
	int     tnWholeDstMIndex   ;    //Whole Distance map�������������������

	float  *shape_bak;       //���������б��Ƿ�����

	float *appearanceCrd;    //warped appearance, may be multiply features
	int   *appearanceCrdInt; //integer coordinate of each pixel in input image

	float * appearance_Prj;     //��appearance gray featreͶӰ���Уã��ϲ���Ĳ�

	float *shapeCrd;    //N(W(s0;p),q) 
	int *shapeCrdInt;   //N(W(s0;p),q) 

	float *SpdS0;       //S0+dS0
	float *SpdS;        //S+dS
	float *dpq;
	float *SDErr;


	float coef_q_new[4];
	float *coef_p_new;

	RECTF	    trcfBaseShapeExtent;	//the Base face shape extent
	EyePosition m_eyePosition       ;   //eye position

	int *pAlpha;
	int *pBeta;
	int *pGamma;

	float x, y, x1, x2, x3, y1, y2, y3;

	// feature buffer
	BYTE    *tpbyGrayImg       ;       // ����Ҷ�ͼ�����ROIͼ��,��SetParameter()�ڷ���
	BYTE    *tpbyWarpGrayImg   ;       // warped gray image
	int     *tpnGradData       ;       // ������warp���ͼ���ϼ����dx��dy
	float   *tpflEdgeData      ;       // edge image of whole input image, ,��SetParameter()�ڷ���

	BYTE    *tpbyLUT           ;       // look up table, size: 256
	BYTE    *tpbyMaskSubBorder ;       // aammodel.mask - tpbyMask_Border, but with lentgh aammodel.pixelnum
	BYTE    *tpbyMaskCalcAvg   ;       // mask for calculating average value in sigmoid function
	BYTE    *tpbyMask_Border   ;       // border of mask of mean appearance
	std::vector<int> m_vecInLipIdx;     // point index which lies in inner lip

	CMATRIX tpMatAppearanceCrd;

	int     m_iCurShapeNum;     //progressive increase shape component

	CMATRIX   tpMatHessian2DPre     ;   
	CMATRIX   tpMatHessian2DFnl     ;     
	CMATRIX   tpMatHessian2DTmp     ;    

	CMATRIX   tpMatSDTMulErr2DFnl   ;     
	CMATRIX   tpMatSDTMulErr2DTmp   ;     

	bool    tbHasCalcuDerivSd_dqp  ;       // flag indicating whether caluclate dirivative of Sd_dqp   
	// end


public:
	// 3D model and 2D+3D Fitting releated
	CandideMdl     *tpCandideMdl       ;
	LUTsOf2D3DAAM  *tpLUTsOf2D3DAAM[LUTS_2D3DAAM_NUM]  ;
	int             tnCurLUTsOf2D3DAAMIdx  ;
	VarsOf2D3DAAM  *tpVarsOf2D3DAAM    ;
private:

	int     m_iTemplOnlineLen   ;       // maybe aammodel.pixelnum or appearance_len
	
	CMATRIX  tpMatSDOnLine      ;    
	CMATRIX  tpMatTemplOnline   ;       
	CMATRIX  tpMatApprnOnline   ;   

	bool    tbHasOnlineTempl   ;       // falg, indicate whether has online template or not
	int     m_iTemplAccCnt      ;
	int     m_iTemplAccCntThr   ;

	int     m_iHueOnlineLen         ;     // aammodel.pixelnum
	CMATRIX tpMatHueApprnOnline    ;                                        //ARRAY NEED CHANGE 

	BYTE   *tpbyHueImgOnline       ;
	int     m_iHueTemplAccCnt       ;
	int     m_iHueTemplAccCntThr    ;
	BYTE*   tpbyHueLUT             ;
	bool    tbHueLUTOK             ;

	unsigned char *tpIplValidPixesMask;
                                                                //ARRAY NEED CHANGE 
	CMATRIX  tpMatFstDerB_p         ;  
	CMATRIX  tpMatSndDerB_p         ;    
	CMATRIX  tpMatDerP_dqp          ;   
	CMATRIX  tpMatDerP_Sd           ;   

	float  **tppflDerSd_dqp        ;   // aS+/a(q,p)

public:
	IMAGEFEATURE       *tpPrevFeatures ;       // image features in previous image, just a pointer, no allocated memory
	IMAGEFEATURE       *tpCurrFeatures ;       // image features in current image, just a pointer, no allocated memory
	VERTEX3D           *tpBaryCoords   ;       // barycentric coordinates, ditto
	TrianglesStruct    *tpFeaTriMesh   ;       // length:NO_TRACKFEATURE
	int                *tpnTrackedFeatures;    // number of tracked features
private:
	float   tflFeaTrackR           ; 
	CMATRIX  tpMatFeaTrackSD        ;       
	CMATRIX  tpMatFeaTrackSD_T      ;      
	CMATRIX  tpMatFeaTrackErr       ;      

	int     tnInlierFeatures       ;       // number of inlier features

	// just for Component-AAM
public:
	float               *tflGlobalAAMShape ;       // just a pointer
	LUTsOfCompGlbAAM    *tpLUTsofCompGlbAAM;       // just a pointer, Left/Right Eye_eyeBrow Component-AAM
	LUTsOfCompGlbAAM    *tpLUTsofBEBCompGlbAAM;    // just a pointer, Both_Eye_eyeBrow Component-AAM
private:

	CMotionHistory  m_MotionHistory;

	CMATRIX matAppearancePCA;
	CMATRIX pmatLambda;
	CMATRIX pmatAppearanceTmp;
	CMATRIX pmatRst;
};
