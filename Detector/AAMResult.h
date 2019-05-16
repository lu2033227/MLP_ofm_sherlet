#pragma once
#include "InterfaceStruct.h"
#include "Common.h"

class AAMResult
{
public:
	AAMResult(void);
	~AAMResult(void);

	void    CreateObject(int nParaNum, int nLandmarkNum);
	void    CreateObject(int nParaNum, int nLandmarkNum, int i3DVtxNum, int iSUNum, int iAUNum);
	void    DestroyObject();
	void    Copy( const AAMResult*  pRlt);


	bool    tbAlignSuccessful  ;       
	int     tnIteCount         ;       
	double	tdbScore           ;       
	double  tdbReconErrInit    ;       // reconstruction err of face image
	double  tdbReconErrFinal   ;       // reconstruction err of face image

	int     tnParaNum          ;       
	float*  tpfParameters      ;       

	int     tnLandmarkNum      ;       
	float*  tpfShape           ;       //size: nLandmarkNum * 2
	float*  tpfShapeNoRST      ;       //size: nLandmarkNum * 2

	//3D model releated results
	int     m_i3DVtxNum         ;
	float*  tpf3DProj2DShape   ;
	float   m_RotAngleXYZ[3]    ;       //rotation angle around x, y, z axis (degree, different from which is in tagVarsOf2D3DAAM)
	float   m_TranslXYZ[3]      ;       //translation vector
	int     m_iSUNum            ;       // 3D Shape Units number
	int     m_iAUNum            ;       // 3D Animation Units number
	float*  tpSUCoef           ; 
	float*  tpAUCoef           ;   
	float   tfFocalLength      ;
	float   tfCMOSPixelSize    ;



	RECT    trcFaceRect        ;       //tpfShape
	int     tnOffsetX          ;       //ROI.left
	int     tnOffsetY          ;       //ROI.right

	float   tflMainDir         ;       // main direction of motion according to feature matching, unit: rad
	float   tflMainDirMag      ;       // magnitude of main direction

};

class  InitParaComp 
{   
public:
	InitParaComp();
	InitParaComp(float ScaleComp, float OffsetXComp, float OffsetYComp, float InitP1 = 0.0f, float InitP2 = 0.0f);
	void SetParameter(float ScaleComp, float OffsetXComp, float OffsetYComp, float InitP1 = 0.0f, float InitP2 = 0.0f); 

	float fScaleComp;
	float fOffsetXComp;
	float fOffsetYComp;
	float fInitP1;
	float fInitP2;  //reserved

} ;

class CTrackContext
{
public:
	CTrackContext();
	~CTrackContext();


public:

	bool    tbDispFeatureScore         ;
	bool    tbSaveTrackFeature         ;
	bool    tbDispTrackFeature         ;
	bool    tbDispInlierFeature        ;
	bool    tbSavePoseTrackerRlt       ;
	bool    tbDispPreAlignByFeaTrack   ;
	bool    tbDispPreAlignBy3DProjPts  ;
	bool    tbSaveAAMInitShape         ;
	bool    tbDispAAMIterate           ;
	bool    tbSaveRTWarpedImg          ;
	bool    tbDispReconstructErrShape  ;
	bool    tbShow2DShape              ;
	bool    tbShow3DShape              ;

	bool    tbPreSelectFeature         ;


	// used by q.wang
	bool tbEnableDebug;
	bool tbDrawTrackFeature;
	bool tbUseNewRansac;
	bool tbUseNewAlgorithm;
	bool tbEnableVirtualKeyFrame;
	bool tbKeyframeMatchOnly;
	bool tbSmoothImage;
	bool tbUseNewWarpFunc;

	// RunPara
	bool	tbTrackSinglePerson		;
};

#pragma pack(1)


typedef struct _tagEyePosition		//define EyePosition struct
{             
	float   fLeft_eye_x  ;        //left eye x coordinate
	float   fLeft_eye_y  ;        //left eye y coordinate
	float   fRight_eye_x ;        //right eye x coordinate
	float   fRight_eye_y ;        //right eye y coordinate
}
EyePosition, *PEyePosition;


typedef struct _tagAAMPoint			//define AAMPoint struct
{             
	float  x;					//x
	float  y;					//y
}
AAMPoint;

typedef struct _tagAAMPoint3D
{
	float        x;
	float        y;
	float        z;
}AAMPoint3D;

typedef struct tagRECTF {
	float fX;
	float fY;                   
	float fWidth;
	float fHeight;
} RECTF;

typedef struct _AAMAppearance
{
	int		index;   
	int		imgx;   
	int		imgy;    //ditto                        //means when use gradient feature

	float	alpha; 
	float	beta;  //ditto
	float	gamma; //ditto

	int		vi;   
	int		vj;   //ditto
	int		vk;   //ditto
}AAMAppearance;


typedef struct _AAMModelStruct
{
	int		vertexnum;
	int		pixelnum;
	int		shapeAxiNum;
	int     appearanceAxiNum;

	int		sampleImgW;
	int		sampleImgH;

	float   xoffset;
	float   yoffset;
	int     zootparameter;


	float  *ShapeEigenvalue;
	int		ShapeEigenvalue_num;

	float  *mean_appearance;
	int		appearance_len;     //appearance_len = pixelnum * featureNum
	int     featureNum;       

	int		MeanShape_len;
	float  *MeanShape;

	AAMAppearance  *aamAppearance;      // its size equal to pixelnum

	//////////////////////////////////////////////////////////////////////////
	int		pseudoInv_r;
	int		pseudoInv_c;

	float *pseudoInv;           

	int		ShapeComp_p_r;      //aammodel.shapeAxiNum
	int		ShapeComp_p_c;      //vertexnum * 2 + 1
	float **ShapeComp_p;        //axes respect to p ,orthonormalized   

	int		ShapeComp_q_r;
	int		ShapeComp_q_c;
	float **ShapeComp_q;        //axes respect to q ;orthonormalized   

	int     triangleNum;            //number of triangles
	int    *triangle_vertex;        
	int    (*triangle_index)[MaxAdjacentTrianglesNum];     
	TrianglesStruct* triangles;     //dimension: triangleNum

	float  *dpq0;                   

	unsigned char *mask;            // sampleImgW * sampleImgH
	int   *pAlpha;
	int   *pBeta;
	int   *pGamma;

	float **appearancePCA   ;

	int         m_iHessianNum;      // number of hessian matrix
	float***    tpppHessian ;      //m_iHessianNum hessian matrix


	//#if defined(USE_3D_CONSTRAINT) || defined(USE_ICOV_TRACKING)
	float**     tppHessianPo;          // full project out hessian matrix 
	//#endif

	//#ifdef USE_3D_CONSTRAINT          // using 3D model(or simplied vertex position) constraint as part of object function
	float*          tpInvTriangleArea ;    // inverse of twice area of each triangle, length:triangleNum 

	//#endif


	//#ifdef USE_ICOV_TRACKING
	// for tracking
	float       **JWp;              //Jacobian of the Warp, (әW/әp)    dim: (ShapeComp_r * 2) X pixelnum 
	//#endif

}AAMModelStruct,*PAAMModelStruct;

typedef struct _FakeAAMModelStruct
{
	int		vertexnum;
	int		pixelnum;
	int		shapeAxiNum;
	int     appearanceAxiNum;

	int		sampleImgW;
	int		sampleImgH;

	float   xoffset;
	float   yoffset;
	int     zootparameter;


	int   p_ShapeEigenvalue;
	int		ShapeEigenvalue_num;

	int   p_mean_appearance;
	int		appearance_len;     //appearance_len = pixelnum * featureNum
	int     featureNum;        

	int		MeanShape_len;
	int   p_MeanShape;

	int   p_aamAppearance;      // its size equal to pixelnum

	//////////////////////////////////////////////////////////////////////////
	int		pseudoInv_r;
	int		pseudoInv_c;


	int   p_tpMatPseudoInv;

	int		ShapeComp_p_r;      //aammodel.shapeAxiNum
	int		ShapeComp_p_c;      //vertexnum * 2 + 1
	int   p_ShapeComp_p;        //axes respect to p ,orthonormalized  

	int		ShapeComp_q_r;
	int		ShapeComp_q_c;
	int   p_ShapeComp_q;        //axes respect to q ;orthonormalized

	int     triangleNum;            //number of triangles
	int   p_triangle_vertex;        //dimension: vertexnum   
	int    p_triangle_index;     //dimension: vertexnum * MaxAdjacentTrianglesNum
	int    p_triangles;     //dimension: triangleNum

	int    p_dpq0;                   //Inverse(H)*Transpose(▽A0 * (aW/ap))*A0, dimension: pseudoInv_r

	int    p_mask;            // sampleImgW * sampleImgH
	int    p_pAlpha;
	int    p_pBeta;
	int    p_pGamma;

	int    p_appearancePCA   ;

	int         m_iHessianNum;      // number of hessian matrix
	int    p_tpppHessian ;      //m_iHessianNum hessian matrix


	//#if defined(USE_3D_CONSTRAINT) || defined(USE_ICOV_TRACKING)
	int    p_tppHessianPo;          // full project out hessian matrix 
	//#endif

	//#ifdef USE_3D_CONSTRAINT          // using 3D model(or simplied vertex position) constraint as part of object function
	int          p_tpInvTriangleArea ;    // inverse of twice area of each triangle, length:triangleNum 

	//#endif


	//#ifdef USE_ICOV_TRACKING
	// for tracking
	int    p_JWp;              //Jacobian of the Warp,   dim: (ShapeComp_r * 2) X pixelnum 
	//#endif

}FakeAAMModelStruct,*PFakeAAMModelStruct;


typedef struct tagRotTrigVal 
{
	float  Sx, Cx;
	float  Sy, Cy;
	float  Sz, Cz;
} RotTrigVal;

typedef struct tagCandideMdl
{
	// 3D Mean shape
	int             m_iMSVertexNum  ;       // 3D Mean Shape Vertex Number
	AAMPoint3D*     tpMSVertex     ;       // length:m_iMSVertexNum
	int             m_iMSTriMeshNun ;
	TrianglesStruct* tpMSTriMesh   ;       // length:m_iMSTriMeshNun

	// 3D Shape Units
	int             m_iSUNum        ;
	int*            tpiEach_SUVertexNum ;  // length:m_iSUNum
	int             m_iTotal_SUVertexNum ;
	int*            tpiEach_SUVertexIdx ;  // each vertex index in 3D mean shape, length:m_iTotal_SUVertexNum
	AAMPoint3D*     tpEach_SUVertexVal  ;  // length:m_iTotal_SUVertexNum

	// 3D Animation Units
	int             m_iAUNum        ;
	int*            tpiEach_AUVertexNum ;  // length:m_iAUNum
	int             m_iTotal_AUVertexNum ;
	int*            tpiEach_AUVertexIdx ;  // each vertex index in 3D mean shape, length:m_iTotal_AUVertexNum
	AAMPoint3D*     tpEach_AUVertexVal  ;  // length:m_iTotal_AUVertexNum
}CandideMdl;

typedef struct tagLUTsOf2D3DAAM
{

	int*        tpiComTo2DIdx      ;
	int*        tpiComTo3DIdx      ;
	int*        tpi2DNeedIntplIdx  ;
	int*        tpi2DIntplIdx      ;
	float     (*tpfl2DIntplCoef)[2];

	int         m_iComVertexNum     ;   // actural number
	int         m_i2DNeedIntplNum   ;

	int*        tpi3DToComIdx      ;   // length: tpCandideMdl->m_iMSVertexNum 

	// AAM motion constraint on some profile landmarks w.r.t motion of projection of 3D model
	int*        tpiMotionConstIdx  ;
	int         m_iMotionConstNum   ;
}LUTsOf2D3DAAM;

typedef struct tagLUTsOfCompGlbAAM     // constraint between Component-AAM and global AAM
{
	int*        tpiComToCompAAM    ;
	int*        tpiComToGlbAAM     ;
	int         m_iComVertexNum     ;   // actural number
}LUTsOfCompGlbAAM;


typedef struct tagVarsOf2D3DAAM         // variables of 3D part for 2D+3D AAM Fitting
{
	
	RotTrigVal  m_Trig              ;   
	float       m_RotAngleXYZ[3]    ;   //rotation angle around x, y, z axis (rad)
	float       m_TranslXYZ[3]      ;   //translation vector
	float       tfFocalLen         ;
	float*      tpSUCoef           ;            
	float*      tpAUCoef           ; 

	float       m_InitRotAngleXYZ[3]    ;   //initial rotation angle around x, y, z axis (rad)
	float       m_InitTranslXYZ[3]      ;   //initial translation vector

	//pre-allocated memory
	float**     tppflMatSD3D       ;   // 3D Steepest descent images
	float*      tpflMatSD3D_T      ;   // Transpose of tppflMatSD3D
	float*      tpflKeyPoint3D     ;   // synthesized 3d shape (no rigid transform)
	float*      tpfl2DPtsErr       ;
	float*      tpflHessian3D      ;
	float*      tpflSDTMulErr3D    ;
}VarsOf2D3DAAM;
#pragma pack()