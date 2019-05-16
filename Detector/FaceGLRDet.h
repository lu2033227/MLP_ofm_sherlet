// FaceGLRDet.h: interface for the CFaceGLRDet class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_FaceGLRDet_H)
//#define AFX_FaceGLRDet_H
//
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
//
//#define IMAGE_WIDTH 24
//#define IMAGE_HEIGHT 12
//
//#define SAFE_RELEASE(x) if(x) {delete []x; x = NULL;}
//
//#include <vector>
//#include "landmarker25/bytestream.h"
//using namespace std;
//
////#include <windows.h>
//
//class CFaceGLRDet
//{
//public:
//	CFaceGLRDet();
//	virtual ~CFaceGLRDet();
//	bool	LoadModelFromFile(char* modelPath);
//
//private:
//	struct Model
//	{
//		int m_nMaxLayNum;	// MaxLayerNum
//		int maxT;		    // MaxFeatureNum
//
//		int *cascade;
//		float *finalthreshold;
//		int (*finaltype)[6];
//		float (*lookuptable)[256];
//	};
//
//
//
//private:
//	inline float  GetVariance(int scalenum, int x, int y, int winm, int winn);
//	inline int     GetFilterValueWithImage(int **ppzlImageData, int FeaType, int RECTWidth,int RECTHeight,int Posx,int Posy);
//	bool		   LoadOneModelFromFileBinary(Model *&m_pModel, ibstream& in);
//	void		   SAFE_RELEASE_MODEL(Model* pModel);
//	int			   GetFeatureValue(int **itgimage,int type,int Mm,int Nn, int X, int Y);
//	void		   GetItgImage(int **pItgImage, unsigned char **pImage);
//
//private:
//	Model**		m_pArrModel;
//	int **		m_ppItgImg;
//	int			m_nModel;
//
//
//public:
//	inline int GetCodeValue(unsigned char *tmatrix);
//	float IsClose(unsigned char **ppImage);
//};
//
//#endif // !defined(AFX_FaceGLRDet_H__D4238705_9222_402D_93D5_BD7224970D52__INCLUDED_)

// FaceGLRDet.h: interface for the CFaceGLRDet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FaceGLRDet_H)
#define AFX_FaceGLRDet_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMAGE_WIDTH 30
#define IMAGE_HEIGHT 20

#define SAFE_RELEASE(x) if(x) {delete []x; x = NULL;}

#include <vector>

using namespace std;

class CFaceGLRDet
{
public:
	CFaceGLRDet();
	virtual ~CFaceGLRDet();
	bool	LoadModelFromFile(const char* modelPath);

private:
	struct Model
	{
		int m_nMaxLayNum;	// MaxLayerNum
		int maxT;		    // MaxFeatureNum

		int *cascade;
		double *finalthreshold;
		int (*finaltype)[6];
		double (*lookuptable)[256];
	};



private:
	inline double  GetVariance(int scalenum, int x, int y, int winm, int winn);
	inline int     GetFilterValueWithImage(int **ppzlImageData, int FeaType, int RECTWidth,int RECTHeight,int Posx,int Posy);
	bool		   LoadOneModelFromFileBinary(Model *&m_pModel, istream& in);
	void		   SAFE_RELEASE_MODEL(Model* pModel);
	int			   GetFeatureValue(int **itgimage,int type,int Mm,int Nn, int X, int Y);
	void		   GetItgImage(int **pItgImage, unsigned char **pImage);

private:
	Model**		m_pArrModel;
	int **		m_ppItgImg;
	int			m_nModel;


public:
	inline int GetCodeValue(unsigned char *tmatrix);
	double IsClose(unsigned char **ppImage);
};

#endif // !defined(AFX_FaceGLRDet_H__D4238705_9222_402D_93D5_BD7224970D52__INCLUDED_)

