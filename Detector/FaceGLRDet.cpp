// FaceGLRDet.cpp: implementation of the CFaceGLRDet class.
//
//////////////////////////////////////////////////////////////////////

//#include "FaceGLRDet.h"
//#include <fstream>
//#include <iostream>
//#include <math.h>
//#include <android/log.h>
//
//#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
////////////////////////////////////////////////////////////////////////
//// Construction/Destruction
////////////////////////////////////////////////////////////////////////
//
//CFaceGLRDet::CFaceGLRDet()
//{
//	m_pArrModel = NULL;
//	m_ppItgImg = NULL;
//
//	m_ppItgImg=new int*[IMAGE_WIDTH+1];
//	int i;
//	for(i=0;i<IMAGE_WIDTH+1;i++)
//	{
//		m_ppItgImg[i]=new int[IMAGE_HEIGHT+1];
//	}
//}
//
//CFaceGLRDet::~CFaceGLRDet()
//{
//	int i;
//	for(i = 0; i < m_nModel; i++)
//	  SAFE_RELEASE_MODEL(m_pArrModel[i]);
//	delete [] m_pArrModel;
//	m_pArrModel = NULL;
//
//	for(i=0; i<IMAGE_WIDTH+1; i++)
//	   delete m_ppItgImg[i];
//
//	delete m_ppItgImg;
//}
//
//bool CFaceGLRDet::LoadModelFromFile(char* modelBuffer)
//{
//	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "aaa %s", "in");
//	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "bbb %s", modelBuffer);
//	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "ccc %s", "xxxx");
//
//	//m_nModel = 1;
//	this->m_nModel = 1;
//	this->m_pArrModel = new Model*[m_nModel];
//	this->m_pArrModel[0] = new Model;
//	ibstream in(modelBuffer);
//	bool bRet = LoadOneModelFromFileBinary(m_pArrModel[0], in);
//	if(!bRet)
//		return bRet;
//
//	return true;
//}
//
//
//bool CFaceGLRDet::LoadOneModelFromFileBinary(Model*& m_pModel, ibstream& in)
//{
//	int m_nMaxLayerNum, m_nMaxFilterNum;
// 	int nLayerNum = 0;
//	int count = 0;
//	int nByte = 1;
//
//	m_nMaxLayerNum = 2;
//	m_nMaxFilterNum = 3000;
//	m_pModel->m_nMaxLayNum = m_nMaxLayerNum;
//	m_pModel->maxT         = m_nMaxFilterNum;
//
//	in.read((char*)&m_nMaxLayerNum, sizeof(int));
//
//    if(NULL==(m_pModel->cascade=new int[m_nMaxLayerNum+1]))
//		return false;
//	if(NULL==(m_pModel->finalthreshold=new float[m_nMaxLayerNum+1]))
//		return false;
//    if(NULL==(m_pModel->finaltype=new int[m_nMaxFilterNum][6]))
//		return false;
//	if(NULL==(m_pModel->lookuptable=new float[m_nMaxFilterNum][256]))
//		return false;
//
//
//    int cascadetemp=0;
//	m_pModel->cascade[0]=0;
//	m_pModel->finalthreshold[0]=0.0;
//	m_nMaxFilterNum=0;
//
//
////	int count=0;
//	for(nLayerNum=1;nLayerNum<m_nMaxLayerNum+1;nLayerNum++)
//	{
//		in.read((char*)&cascadetemp, sizeof(int));
//		in.read((char*)&(m_pModel->finalthreshold[nLayerNum]), sizeof(float));
//
//		 m_pModel->cascade[nLayerNum]=m_pModel->cascade[nLayerNum-1]+cascadetemp;
//		 for(int j=0;j<cascadetemp;j++)
//		 {
//			 in.read((char *)(m_pModel->finaltype[count]), 6*sizeof(int));
//			 in.read((char *)(m_pModel->lookuptable[count]), 256*sizeof(float));
//			 count++;
//		 }
//	}
//
//	m_pModel->m_nMaxLayNum = nLayerNum - 1;
//	m_pModel->maxT = count;
//
//	m_nMaxFilterNum = m_pModel->cascade[nLayerNum-1];
//
//
//    return true;
//}
//
//void CFaceGLRDet::SAFE_RELEASE_MODEL(Model* pModel)
//{
//	if(pModel != NULL)
//	{
//		SAFE_RELEASE(pModel->finalthreshold);
//		SAFE_RELEASE(pModel->cascade);
//
//		delete []pModel->lookuptable;
//		delete []pModel->finaltype;
//		pModel->finaltype    = NULL;
//
//		delete pModel;
//		pModel = NULL;
//	}
//}
//
//inline int CFaceGLRDet::GetFilterValueWithImage(int **ppImageData, int FeaType, int RECTWidth,int RECTHeight,int Posx,int Posy)
//{
//	return GetFeatureValue(ppImageData,
//							FeaType,
//							RECTWidth,
//							RECTHeight,
//							Posx,
//							Posy);
//}
//const int LBP81Compare[8]={
//	0,7,6,1,5,2,3,4
//};
//const int LBP41Compare[4]={0,3,1,2};
//int CFaceGLRDet::GetFeatureValue(int **itgimage,int CodeType,int Mm,int Nn, int X, int Y)
//{
//	unsigned char CodeMatrix[8];
//	float CenterValue=itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*2][Y+Nn*2]
//		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+Mm*2][Y+Nn];
//	int ncout=0;
//	if(CodeType==0)
//	{
//		for(int m=0;m<3;m++)
//			for(int n=0;n<3;n++)
//			{
//				if((m==1)&&(n==1))
//					continue;
//				float CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
//				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//				ncout++;
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//	else if(CodeType==1)
//	{
//		CenterValue = (itgimage[X][Y]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X][Y+Nn*4])/16;
//		for(int m=0;m<4;m++)
//			for(int n=0;n<4;n++)
//			{
//				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
//				{
//					float CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//						-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//	else if(CodeType==2)
//	{
//		CenterValue = (itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm][Y+Nn*4])/9;
//		for(int m=0;m<5;m++)
//			for(int n=0;n<5;n++)
//			{
//				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
//				{
//					float CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//						-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//
//	}
//	//x gradient
//	else if(CodeType==3)
//	{
//		CenterValue=(itgimage[X+2*Mm-1][Y+Nn]+itgimage[X+2*Mm][Y+Nn*2]
//		-itgimage[X+2*Mm-1][Y+Nn*2]-itgimage[X+2*Mm][Y+Nn])-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm+1][Y+Nn*2]
//		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+Mm+1][Y+Nn]);
//		for(int m=0;m<3;m++)
//			for(int n=0;n<3;n++)
//			{
//				if((m==1)&&(n==1))
//					continue;
//				float CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
//				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//				ncout++;
//			}
//		return GetCodeValue(CodeMatrix);
//
//	}
//	else if(CodeType==4)
//	{
//		CenterValue = ((itgimage[X+Mm*4-1][Y]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X+Mm*4-1][Y+Nn*4])
//			-(itgimage[X][Y]+itgimage[X+1][Y+Nn*4]-itgimage[X+1][Y]-itgimage[X][Y+Nn*4]))/16;
//		for(int m=0;m<4;m++)
//			for(int n=0;n<4;n++)
//			{
//				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
//				{
//					float CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//	else if(CodeType==5)
//	{
//		CenterValue = ((itgimage[X+Mm*4-1][Y+Nn]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm*4-1][Y+Nn*4])
//			-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm+1][Y+Nn*4]-itgimage[X+Mm+1][Y+Nn]-itgimage[X+Mm][Y+Nn*4]))/9;
//		for(int m=0;m<5;m++)
//			for(int n=0;n<5;n++)
//			{
//				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
//				{
//					float CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//	//y gradient
//	else if(CodeType==6)
//	{
//		CenterValue=(itgimage[X+Mm][Y+Nn*2-1]+itgimage[X+2*Mm][Y+Nn*2]
//		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+2*Mm][Y+2*Nn-1])-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*2][Y+Nn+1]
//		-itgimage[X+Mm][Y+Nn+1]-itgimage[X+2*Mm][Y+Nn]);
//		for(int m=0;m<3;m++)
//			for(int n=0;n<3;n++)
//			{
//				if((m==1)&&(n==1))
//					continue;
//				float CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
//					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
//				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//				ncout++;
//			}
//		return GetCodeValue(CodeMatrix);
//
//	}
//	else if(CodeType==7)
//	{
//		CenterValue = ((itgimage[X][Y+Nn*4-1]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X][Y+Nn*4-1])
//			-(itgimage[X][Y]+itgimage[X+Mm*4][Y+1]-itgimage[X+Mm*4][Y]-itgimage[X][Y+1]))/16;
//		for(int m=0;m<4;m++)
//			for(int n=0;n<4;n++)
//			{
//				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
//				{
//					float CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
//					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//	else if(CodeType==8)
//	{
//		CenterValue = ((itgimage[X+Mm][Y+Nn*4-1]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn*4-1]-itgimage[X+Mm][Y+Nn*4])
//			-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*4][Y+Nn+1]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm][Y+Nn+1]))/9;
//		for(int m=0;m<5;m++)
//			for(int n=0;n<5;n++)
//			{
//				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
//				{
//					float CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
//					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
//					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
//					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
//					ncout++;
//				}
//			}
//		return GetCodeValue(CodeMatrix);
//	}
//
//}
//
//void CFaceGLRDet::GetItgImage(int **pItgImage, unsigned char **pImage)
//{
//	int x,y;
//	for (x=0; x<IMAGE_WIDTH+1; x++)
//		for (y=0; y<IMAGE_HEIGHT+1; y++)
//		{
//			pItgImage[x][y]=0;
//		}
//
//
//	int* s = new int[IMAGE_HEIGHT];
//
//	for (x=0; x<IMAGE_WIDTH; x++)
//	{
//		for ( y=0; y<IMAGE_HEIGHT; y++)
//		{
//			if (y!=0)
//			{
//				s[y]=s[y-1]+pImage[x][y];
//			}
//			else
//			{
//				s[y]=pImage[x][y];
//			}
//
//			pItgImage[x+1][y+1]=pItgImage[x][y+1]+s[y];
//		}
//	}
//
//	delete []s;
//	s = NULL;
//}
//
//
//float CFaceGLRDet::IsClose(unsigned char **ppImage)
//{
//	int layer, t, tTh ;
//	float hTh, hvalue;
//
//	GetItgImage(m_ppItgImg,ppImage);
//	int i;
//
//	for( i =0 ; i< m_nModel; i++)
//	{
//		if(m_pArrModel[i]==NULL)
//		{
//			continue;
//		}
//
//		layer=1;
//		hvalue=0;
//		do
//		{
//			t=m_pArrModel[i]->cascade[layer-1];
//			tTh=m_pArrModel[i]->cascade[layer];
//			hTh=m_pArrModel[i]->finalthreshold[layer];
//			while ((t<tTh))
//			{
//				hvalue+=(m_pArrModel[i]->lookuptable[t][
//					  GetFilterValueWithImage(m_ppItgImg,
//											  m_pArrModel[i]->finaltype[t][1],
//											  m_pArrModel[i]->finaltype[t][2],
//											  m_pArrModel[i]->finaltype[t][3],
//											  m_pArrModel[i]->finaltype[t][4],
//											  m_pArrModel[i]->finaltype[t][5])]);
//				t++;
//			}
//			layer++;
//
//		}while((hvalue >= m_pArrModel[i]->finalthreshold[layer-1]) && (layer <= m_pArrModel[i]->m_nMaxLayNum));
//	}
//	return hvalue;
//}
//
//
//inline int CFaceGLRDet::GetCodeValue(unsigned char *tmatrix)
//{
////		if(CodeType==0)
///*		{
//			int CodeIndex=0;
//			int MultiValue=1;
//			for(int i=0;i<8;i++)
//			{
//				CodeIndex+=tmatrix[i]*MultiValue;
//				MultiValue=MultiValue*2;
//			}
//			return CodeIndex;
//		}
//		else
//		{
//			int CodeIndex=0;
//			int MultiValue=1;
//			for(int i=0;i<4;i++)
//			{
//				CodeIndex+=tmatrix[i]*MultiValue;
//				MultiValue=MultiValue*2;
//			}
//			return CodeIndex;
//		}*/
////	if(CodeType==0)
//	{
//		tmatrix[7]=tmatrix[7]<<7;
//		tmatrix[6]=tmatrix[6]<<6;
//		tmatrix[5]=tmatrix[5]<<5;
//		tmatrix[4]=tmatrix[4]<<4;
//		tmatrix[3]=tmatrix[3]<<3;
//		tmatrix[2]=tmatrix[2]<<2;
//		tmatrix[1]=tmatrix[1]<<1;
//		return (tmatrix[0]|tmatrix[1]|tmatrix[2]|tmatrix[3]|tmatrix[4]|tmatrix[5]|tmatrix[6]|tmatrix[7]);
//	}
///*	else
//	{
//		tmatrix[3]=tmatrix[3]<<3;
//		tmatrix[2]=tmatrix[2]<<2;
//		tmatrix[1]=tmatrix[1]<<1;
//		return (tmatrix[0]|tmatrix[1]|tmatrix[2]|tmatrix[3]);
//	}*/
//}

// FaceGLRDet.cpp: implementation of the CFaceGLRDet class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceGLRDet.h"
#include <fstream>
#include <iostream>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceGLRDet::CFaceGLRDet()
{
	m_pArrModel = NULL;
	m_ppItgImg = NULL;

	m_ppItgImg=new int*[IMAGE_WIDTH+1];
	int i;
	for(i=0;i<IMAGE_WIDTH+1;i++)
	{
		m_ppItgImg[i]=new int[IMAGE_HEIGHT+1];
	}
}

CFaceGLRDet::~CFaceGLRDet()
{
	int i;
	for(i = 0; i < m_nModel; i++)
	  SAFE_RELEASE_MODEL(m_pArrModel[i]);
	delete [] m_pArrModel;
	m_pArrModel = NULL;

	for(i=0; i<IMAGE_WIDTH+1; i++)
	   delete m_ppItgImg[i];

	delete m_ppItgImg;
}


bool CFaceGLRDet::LoadModelFromFile(const char* modelBufferPath)
{
	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "aaa %s", "in");
	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "bbb %s", modelBuffer);
	//__android_log_print(ANDROID_LOG_DEBUG, "fras_init", "ccc %s", "xxxx");

	//m_nModel = 1;
	this->m_nModel = 1;
	this->m_pArrModel = new Model*[m_nModel];
	this->m_pArrModel[0] = new Model;
    
	ifstream in(modelBufferPath);
	bool bRet = LoadOneModelFromFileBinary(m_pArrModel[0], in);

    if(!bRet)
		return bRet;

	return true;
}


bool CFaceGLRDet::LoadOneModelFromFileBinary(Model*& m_pModel, istream& in)
{
	int m_nMaxLayerNum, m_nMaxFilterNum;
 	int nLayerNum = 0;
	int count = 0;
	int nByte = 1;

	m_nMaxLayerNum = 2;
	m_nMaxFilterNum = 3000;
	m_pModel->m_nMaxLayNum = m_nMaxLayerNum;
	m_pModel->maxT         = m_nMaxFilterNum;

	in.read((char*)&m_nMaxLayerNum, sizeof(int));

    if(NULL==(m_pModel->cascade=new int[m_nMaxLayerNum+1]))
		return false;
	if(NULL==(m_pModel->finalthreshold=new double[m_nMaxLayerNum+1]))
		return false;
    if(NULL==(m_pModel->finaltype=new int[m_nMaxFilterNum][6]))
		return false;
	if(NULL==(m_pModel->lookuptable=new double[m_nMaxFilterNum][256]))
		return false;


    int cascadetemp=0;
	m_pModel->cascade[0]=0;
	m_pModel->finalthreshold[0]=0.0;
	m_nMaxFilterNum=0;


//	int count=0;
	for(nLayerNum=1;nLayerNum<m_nMaxLayerNum+1;nLayerNum++)
	{
		in.read((char*)&cascadetemp, sizeof(int));
		in.read((char*)&(m_pModel->finalthreshold[nLayerNum]), sizeof(double));

		 m_pModel->cascade[nLayerNum]=m_pModel->cascade[nLayerNum-1]+cascadetemp;
		 for(int j=0;j<cascadetemp;j++)
		 {
			 in.read((char *)(m_pModel->finaltype[count]), 6*sizeof(int));
			 in.read((char *)(m_pModel->lookuptable[count]), 256*sizeof(double));
			 count++;
		 }
	}

	m_pModel->m_nMaxLayNum = nLayerNum - 1;
	m_pModel->maxT = count;

	m_nMaxFilterNum = m_pModel->cascade[nLayerNum-1];


    return true;
}

void CFaceGLRDet::SAFE_RELEASE_MODEL(Model* pModel)
{
	if(pModel != NULL)
	{
		SAFE_RELEASE(pModel->finalthreshold);
		SAFE_RELEASE(pModel->cascade);

		delete []pModel->lookuptable;
		delete []pModel->finaltype;
		pModel->finaltype    = NULL;

		delete pModel;
		pModel = NULL;
	}
}

inline int CFaceGLRDet::GetFilterValueWithImage(int **ppImageData, int FeaType, int RECTWidth,int RECTHeight,int Posx,int Posy)
{
	return GetFeatureValue(ppImageData,
							FeaType,
							RECTWidth,
							RECTHeight,
							Posx,
							Posy);
}
const int LBP81Compare[8]={
	0,7,6,1,5,2,3,4
};
const int LBP41Compare[4]={0,3,1,2};
int CFaceGLRDet::GetFeatureValue(int **itgimage,int CodeType,int Mm,int Nn, int X, int Y)
{
	unsigned char CodeMatrix[8];
	double CenterValue=itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*2][Y+Nn*2]
		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+Mm*2][Y+Nn];
	int ncout=0;
	if(CodeType==0)
	{
		for(int m=0;m<3;m++)
			for(int n=0;n<3;n++)
			{
				if((m==1)&&(n==1))
					continue;
				double CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
				ncout++;
			}
		return GetCodeValue(CodeMatrix);
	}
	else if(CodeType==1)
	{
		CenterValue = (itgimage[X][Y]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X][Y+Nn*4])/16;
		for(int m=0;m<4;m++)
			for(int n=0;n<4;n++)
			{
				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
				{
					double CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
						-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);
	}
	else if(CodeType==2)
	{
		CenterValue = (itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm][Y+Nn*4])/9;
		for(int m=0;m<5;m++)
			for(int n=0;n<5;n++)
			{
				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
				{
					double CurrentValue=itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
						-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n];
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);

	}
	//x gradient
	else if(CodeType==3)
	{
		CenterValue=(itgimage[X+2*Mm-1][Y+Nn]+itgimage[X+2*Mm][Y+Nn*2]
		-itgimage[X+2*Mm-1][Y+Nn*2]-itgimage[X+2*Mm][Y+Nn])-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm+1][Y+Nn*2]
		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+Mm+1][Y+Nn]);
		for(int m=0;m<3;m++)
			for(int n=0;n<3;n++)
			{
				if((m==1)&&(n==1))
					continue;
				double CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
				ncout++;
			}
		return GetCodeValue(CodeMatrix);

	}
	else if(CodeType==4)
	{
		CenterValue = ((itgimage[X+Mm*4-1][Y]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X+Mm*4-1][Y+Nn*4])
			-(itgimage[X][Y]+itgimage[X+1][Y+Nn*4]-itgimage[X+1][Y]-itgimage[X][Y+Nn*4]))/16;
		for(int m=0;m<4;m++)
			for(int n=0;n<4;n++)
			{
				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
				{
					double CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);
	}
	else if(CodeType==5)
	{
		CenterValue = ((itgimage[X+Mm*4-1][Y+Nn]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm*4-1][Y+Nn*4])
			-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm+1][Y+Nn*4]-itgimage[X+Mm+1][Y+Nn]-itgimage[X+Mm][Y+Nn*4]))/9;
		for(int m=0;m<5;m++)
			for(int n=0;n<5;n++)
			{
				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
				{
					double CurrentValue=(itgimage[X+Mm*(m+1)-1][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*(m+1)-1][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*n])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*m+1][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*m+1][Y+Nn*n]);
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);
	}
	//y gradient
	else if(CodeType==6)
	{
		CenterValue=(itgimage[X+Mm][Y+Nn*2-1]+itgimage[X+2*Mm][Y+Nn*2]
		-itgimage[X+Mm][Y+Nn*2]-itgimage[X+2*Mm][Y+2*Nn-1])-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*2][Y+Nn+1]
		-itgimage[X+Mm][Y+Nn+1]-itgimage[X+2*Mm][Y+Nn]);
		for(int m=0;m<3;m++)
			for(int n=0;n<3;n++)
			{
				if((m==1)&&(n==1))
					continue;
				double CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
				CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
				ncout++;
			}
		return GetCodeValue(CodeMatrix);

	}
	else if(CodeType==7)
	{
		CenterValue = ((itgimage[X][Y+Nn*4-1]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y]-itgimage[X][Y+Nn*4-1])
			-(itgimage[X][Y]+itgimage[X+Mm*4][Y+1]-itgimage[X+Mm*4][Y]-itgimage[X][Y+1]))/16;
		for(int m=0;m<4;m++)
			for(int n=0;n<4;n++)
			{
				if((m==0&&n==1)||(m==0&&n==2)||(m==1&&n==0)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==3)||(m==3&&n==1)||(m==3&&n==2))
				{
					double CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);
	}
	else if(CodeType==8)
	{
		CenterValue = ((itgimage[X+Mm][Y+Nn*4-1]+itgimage[X+Mm*4][Y+Nn*4]-itgimage[X+Mm*4][Y+Nn*4-1]-itgimage[X+Mm][Y+Nn*4])
			-(itgimage[X+Mm][Y+Nn]+itgimage[X+Mm*4][Y+Nn+1]-itgimage[X+Mm*4][Y+Nn]-itgimage[X+Mm][Y+Nn+1]))/9;
		for(int m=0;m<5;m++)
			for(int n=0;n<5;n++)
			{
				if((m==0&&n==2)||(m==1&&n==1)||(m==1&&n==3)||(m==2&&n==0)||(m==2&&n==4)||(m==3&&n==1)||(m==3&&n==3)||(m==4&&n==2))
				{
					double CurrentValue=(itgimage[X+Mm*m][Y+Nn*(n+1)-1]+itgimage[X+Mm*(m+1)][Y+Nn*(n+1)]
					-itgimage[X+Mm*m][Y+Nn*(n+1)]-itgimage[X+Mm*(m+1)][Y+Nn*(n+1)-1])-(itgimage[X+Mm*m][Y+Nn*n]+itgimage[X+Mm*(m+1)][Y+Nn*n+1]
					-itgimage[X+Mm*m][Y+Nn*n+1]-itgimage[X+Mm*(m+1)][Y+Nn*n]);
					CodeMatrix[LBP81Compare[ncout]]=(CurrentValue>CenterValue)?1:0;
					ncout++;
				}
			}
		return GetCodeValue(CodeMatrix);
	}
    return 0;

}

void CFaceGLRDet::GetItgImage(int **pItgImage, unsigned char **pImage)
{
	int x,y;
	for (x=0; x<IMAGE_WIDTH+1; x++)
		for (y=0; y<IMAGE_HEIGHT+1; y++)
		{
			pItgImage[x][y]=0;
		}


	int* s = new int[IMAGE_HEIGHT];

	for (x=0; x<IMAGE_WIDTH; x++)
	{
		for ( y=0; y<IMAGE_HEIGHT; y++)
		{
			if (y!=0)
			{
				s[y]=s[y-1]+pImage[x][y];
			}
			else
			{
				s[y]=pImage[x][y];
			}

			pItgImage[x+1][y+1]=pItgImage[x][y+1]+s[y];
		}
	}

	delete []s;
	s = NULL;
}


double CFaceGLRDet::IsClose(unsigned char **ppImage)
{
	int layer, t, tTh ;
	double hTh, hvalue;

	GetItgImage(m_ppItgImg,ppImage);
	int i;

	for( i =0 ; i< m_nModel; i++)
	{
		if(m_pArrModel[i]==NULL)
		{
			continue;
		}

		layer=1;
		hvalue=0;
		do
		{
			t=m_pArrModel[i]->cascade[layer-1];
			tTh=m_pArrModel[i]->cascade[layer];
			hTh=m_pArrModel[i]->finalthreshold[layer];
			while ((t<tTh))
			{
				hvalue+=(m_pArrModel[i]->lookuptable[t][
					  GetFilterValueWithImage(m_ppItgImg,
											  m_pArrModel[i]->finaltype[t][1],
											  m_pArrModel[i]->finaltype[t][2],
											  m_pArrModel[i]->finaltype[t][3],
											  m_pArrModel[i]->finaltype[t][4],
											  m_pArrModel[i]->finaltype[t][5])]);
				t++;
			}
			layer++;

		}while((hvalue >= m_pArrModel[i]->finalthreshold[layer-1]) && (layer <= m_pArrModel[i]->m_nMaxLayNum));
	}
	return hvalue;
}


inline int CFaceGLRDet::GetCodeValue(unsigned char *tmatrix)
{
//		if(CodeType==0)
/*		{
			int CodeIndex=0;
			int MultiValue=1;
			for(int i=0;i<8;i++)
			{
				CodeIndex+=tmatrix[i]*MultiValue;
				MultiValue=MultiValue*2;
			}
			return CodeIndex;
		}
		else
		{
			int CodeIndex=0;
			int MultiValue=1;
			for(int i=0;i<4;i++)
			{
				CodeIndex+=tmatrix[i]*MultiValue;
				MultiValue=MultiValue*2;
			}
			return CodeIndex;
		}*/
//	if(CodeType==0)
	{
		tmatrix[7]=tmatrix[7]<<7;
		tmatrix[6]=tmatrix[6]<<6;
		tmatrix[5]=tmatrix[5]<<5;
		tmatrix[4]=tmatrix[4]<<4;
		tmatrix[3]=tmatrix[3]<<3;
		tmatrix[2]=tmatrix[2]<<2;
		tmatrix[1]=tmatrix[1]<<1;
		return (tmatrix[0]|tmatrix[1]|tmatrix[2]|tmatrix[3]|tmatrix[4]|tmatrix[5]|tmatrix[6]|tmatrix[7]);
	}
/*	else
	{
		tmatrix[3]=tmatrix[3]<<3;
		tmatrix[2]=tmatrix[2]<<2;
		tmatrix[1]=tmatrix[1]<<1;
		return (tmatrix[0]|tmatrix[1]|tmatrix[2]|tmatrix[3]);
	}*/
}

