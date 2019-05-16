//#include "stdafx.h"
#include "imagemodule.h"
#include <math.h>
#include <vector>

#pragma warning(disable:4996)
#define AlighWidth(ImageWidth)   ((((ImageWidth*8)+31)/32)*4)

/* BGR -> Gray */
#define fix(x,n)      (int)((x)*(1 << (n)) + 0.5)
#define descale(x,n)       (((x) + (1 << ((n)-1))) >> (n))
#define shift  10
#define cscGr  fix(0.212671,shift)
#define cscGg  fix(0.715160,shift)
#define cscGb  fix(0.072169,shift)
#define fast_cast_8u(t)  icvSaturate8ui[(t+256)<1024?(t+256):1024]

const unsigned char icvSaturate8ui[] = 
{
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255
};

float table[]=
{
	0,0.090909f,0.166667f,0.230769f,0.285714f,0.333333f,0.375000f,0.411765f,0.444444f,0.473684f,0.500000f,0.523810f,0.545455f,0.565217f,0.583333f,0.600000f,0.615385f,0.629630f,0.642857f,0.655172f,0.666667f,
	0.677419f,0.687500f,0.696970f,0.705882f,0.714286f,0.722222f,0.729730f,0.736842f,0.743590f,0.750000f,0.756098f,0.761905f,0.767442f,0.772727f,0.777778f,0.782609f,0.787234f,0.791667f,0.795918f,0.800000f,
	0.803922f,0.807692f,0.811321f,0.814815f,0.818182f,0.821429f,0.824561f,0.827586f,0.830508f,0.833333f,0.836066f,0.838710f,0.841270f,0.843750f,0.846154f,0.848485f,0.850746f,0.852941f,0.855072f,0.857143f,
	0.859155f,0.861111f,0.863014f,0.864865f,0.866667f,0.868421f,0.870130f,0.871795f,0.873418f,0.875000f,0.876543f,0.878049f,0.879518f,0.880952f,0.882353f,0.883721f,0.885057f,0.886364f,0.887640f,0.888889f,
	0.890110f,0.891304f,0.892473f,0.893617f,0.894737f,0.895833f,0.896907f,0.897959f,0.898990f,0.900000f,0.900990f,0.901961f,0.902913f,0.903846f,0.904762f,0.905660f,0.906542f,0.907407f,0.908257f,0.909091f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f,0.93f
};

bool  HistogramEqualization(unsigned char *input,int width,int height, RECT rcROI)
{
	if ( input == NULL )
		return false;
	int col=width;
	int i;
	if(input==NULL)
		return false;

	float iGray[256];
	int gray[256];
	int sum=0;

	memset( gray, 0, sizeof(int) * 256 );
	memset( iGray, 0, sizeof(float) * 256 );
	for(i=rcROI.top;i<rcROI.bottom;i++)
	{
		for(int j=rcROI.left;j<rcROI.right;j++)
		{
			gray[input[i*col+j]]++;
			sum++;
		}
	}
	if ( sum == 0 )
		return true;

	iGray[0]=(float)gray[0];
	for(i=1;i<256;i++)
	{
		iGray[i] = iGray[i-1] + gray[i];         // accumulated histogram
	}

	for(i=0;i<256;i++)
	{
		gray[i] = int( 255 * iGray[i] / sum + 0.5f);
	}
	for(i=rcROI.top;i<rcROI.bottom;i++)
	{
		for(int j=rcROI.left;j<rcROI.right;j++)
		{
			input[i*col+j]=(unsigned char)gray[input[i*col+j]];
		}
	}
	return true;
}

void RGB2GRAY(unsigned char *pSrcImg, long nSrcWidth, long nSrcHeight,unsigned char *pDstImg)
{
	BYTE *buffer = new BYTE[nSrcWidth*nSrcHeight];
	BYTE *pDstImg2 = buffer;
	long Pnum=nSrcWidth*nSrcHeight;
	int b,g,r,t;
	for(long i=0;i<Pnum;i++,pDstImg2++)
	{
		b=*pSrcImg*cscGb;
		pSrcImg++;
		g=*pSrcImg*cscGg;
		pSrcImg++;
		r=*pSrcImg*cscGr;
		pSrcImg++;
		t=b+g+r;
		t = descale(t,shift);
		*pDstImg2=fast_cast_8u(t);
	}
	for(int i=0;i<nSrcHeight;i++)
	{
		memcpy(pDstImg+(nSrcHeight-1-i)*nSrcWidth,buffer+i*nSrcWidth,nSrcWidth);
	}
	delete []buffer;
}

void RGBA2GRAY(unsigned char *scr,long width,long height,unsigned char *dst)
{
	int size = height*width;
	for(int i=0;i<size;i++)
	{
		int b,g,r,t;
		r=*(scr++)*cscGb;
		g=*(scr++)*cscGg;
		b=*(scr)*cscGr;			 	 
		t=b+g+r;
		t = descale(t,shift);
		*(dst++)=fast_cast_8u(t);
		scr+=2;
	}
}

//	SigmoidalPositive��һ��
bool SigmoidalPositive(float *img, int img_w, int img_h, unsigned char *mask)
{

	if(img == 0)
		return false;

	float *output    = img;

	float img_sum     = 0.0f;
	float img_average = 0.0f;

	int img_totalsize      = img_h*img_w;	
	int unmask_totalsize   = 0;

	if(mask == 0)
	{		
		for (int i=0; i<img_totalsize; i++)
		{	
			img_sum += *(output++);	
		}
		img_average = img_sum/img_totalsize;
	}
	else
	{

		for (int i=0; i<img_totalsize; i++,output++)
		{	
			if(0 != mask[i])
			{			
				img_sum += *(output);	
				unmask_totalsize++;			
			}

		}
		img_average = img_sum/unmask_totalsize;
	}

	output   = img;

	if ( img_average <= 0.f )
	{
		memset( output, 0, sizeof(float) * img_totalsize );
	}
	else
	{
		for (int i=0;i<img_totalsize;i++)
		{  
			(*output++) = (*output)/(((*output)+img_average));
		}
	}

	return true;
}

// Erode a mask image use struct [1 1 1; 1 1 1; 1 1 1]
// allocate memory for pOutMask out of the function
void	ErodeMask(unsigned char *pInMask,int width,int height,unsigned char *pOutMask)
{
	int i, j, iOffset1, iOffset2, iOffset3;
	bool bValid;

	for(j=1; j<height-1; j++)
	{
		for(i=1; i<width-1; i++)
		{
			iOffset2 = j*width + i;
			iOffset1 = iOffset2 - width;
			iOffset3 = iOffset2 + width;
			bValid = ( pInMask[iOffset1 - 1] && pInMask[iOffset1] && pInMask[iOffset1 + 1] \
				&& pInMask[iOffset2 - 1] && pInMask[iOffset2] && pInMask[iOffset2 + 1] \
				&& pInMask[iOffset3 - 1] && pInMask[iOffset3] && pInMask[iOffset3 + 1] );

			pOutMask[iOffset2] = (bValid? 255 : 0); 
		}
	}
}

// pOutMask =  pInMask1 - pInMask2;
// allocate memory for pOutMask out of the function
// usually use it to calculate edge of pInMask1
void	MaskSub(BYTE *pInMask1, int width, int height, BYTE *pInMask2, BYTE *pOutMask)
{
	int i, iCount;
	iCount = width * height;
	for(i=0; i<iCount; i++)
	{
		pOutMask[i] =  pInMask1[i] - pInMask2[i];
	}
}

//calculate gradient
void	CalcuGradient(float* pfInImage, int iWidth, int iHeight, BYTE* pbyMask, float* pfDx, float* pfDy)
{
	int i, j, iOffset1, iOffset2, iOffset3;

	for(j=1; j<iHeight-1; j++)
	{
		for(i=1; i<iWidth-1; i++)
		{
			iOffset2 = j*iWidth + i;

			if (pbyMask[iOffset2]) 
			{
				iOffset1 = iOffset2 - iWidth;
				iOffset3 = iOffset2 + iWidth;

				pfDx[iOffset2] = 0.25f * (pfInImage[iOffset1 + 1] - pfInImage[iOffset1 - 1] \
					+ 2*(pfInImage[iOffset2 + 1] - pfInImage[iOffset2 - 1]) \
					+ pfInImage[iOffset3 + 1] - pfInImage[iOffset3 - 1] );

				pfDy[iOffset2] = 0.25f * (-pfInImage[iOffset1 - 1] - 2*pfInImage[iOffset1] - pfInImage[iOffset1 + 1] \
					+ pfInImage[iOffset3 - 1] + 2*pfInImage[iOffset3] + pfInImage[iOffset3 + 1]);
			}

		}
	}
}

void	GaussSmooth(float* pfInImage, int iWidth, int iHeight, BYTE* pbyMask1, BYTE* pbyMask1_Edge)
{
	int i, j, iOffset;
	float* pfImageBak = new float[iWidth * iHeight]; 
	float* pfImageWork = new float[iWidth * iHeight];

	memcpy(pfImageBak, pfInImage, iWidth * iHeight * sizeof(float));
	memcpy(pfImageWork, pfInImage, iWidth * iHeight * sizeof(float));

	//two pass smooth
	//pass 1:
	for(j=0; j<iHeight; j++)
	{
		for(i=1; i<iWidth-1; i++)
		{
			iOffset = j * iWidth + i;
			pfImageWork[iOffset] = pfImageBak[iOffset - 1] + 2*pfImageBak[iOffset] \
				+ pfImageBak[iOffset + 1];
		}
	}
	//pass 2:
	for(j=1; j<iHeight-1; j++)
	{
		for(i=1; i<iWidth-1; i++)
		{
			iOffset = j * iWidth + i;
			pfInImage[iOffset] = (pfImageWork[iOffset - iWidth] + 2*pfImageWork[iOffset] \
				+ pfImageWork[iOffset + iWidth]) / 16;
		}
	}

	//if have not mask input, return
	if (pbyMask1 == NULL && pbyMask1_Edge == NULL) 
	{
		delete [] pfImageBak;
		delete [] pfImageWork;
		return ;
	}

	//else, postprocess
	float   fSum;
	int     iWeight;
	for(j=1; j<iHeight-1; j++)
	{
		for(i=1; i<iWidth-1; i++)
		{
			iOffset = j * iWidth + i;
			if (pbyMask1_Edge[iOffset]) 
			{
				fSum = 4 * pfImageBak[iOffset];
				iWeight = 4;
				if (pbyMask1[iOffset - iWidth - 1]) {
					fSum += pfImageBak[iOffset - iWidth - 1];
					iWeight += 1;
				}
				if (pbyMask1[iOffset - iWidth]) {
					fSum += 2*pfImageBak[iOffset - iWidth];
					iWeight += 2;
				}
				if (pbyMask1[iOffset - iWidth + 1]) {
					fSum += pfImageBak[iOffset - iWidth + 1];
					iWeight += 1;
				}

				if (pbyMask1[iOffset - 1]) {
					fSum += 2*pfImageBak[iOffset - 1];
					iWeight += 2;
				}
				if (pbyMask1[iOffset + 1]) {
					fSum += 2*pfImageBak[iOffset + 1];
					iWeight += 2;
				}

				if (pbyMask1[iOffset + iWidth - 1]) {
					fSum += pfImageBak[iOffset + iWidth - 1];
					iWeight += 1;
				}
				if (pbyMask1[iOffset + iWidth]) {
					fSum += 2*pfImageBak[iOffset + iWidth];
					iWeight += 2;
				}
				if (pbyMask1[iOffset + iWidth + 1]) {
					fSum += pfImageBak[iOffset + iWidth + 1];
					iWeight += 1;
				}

				pfInImage[iOffset] = fSum / iWeight;

			}
		}
	}

	delete [] pfImageBak;
	delete [] pfImageWork;

}

void	CalcuGradient(unsigned char * pucInImage, int iWidth, int iHeight, int* pnGradX, int* pnGradY)
{
	int i, j, iOffset1, iOffset2, iOffset3;

	for(j=1; j<iHeight-1; j++)
	{
		for(i=1; i<iWidth-1; i++)
		{
			iOffset2 = j*iWidth + i;

			iOffset1 = iOffset2 - iWidth;
			iOffset3 = iOffset2 + iWidth;

			//Sobel filter
			pnGradX[iOffset2] = (pucInImage[iOffset1 + 1] - pucInImage[iOffset1 - 1] \
				+ 2*(pucInImage[iOffset2 + 1] - pucInImage[iOffset2 - 1]) \
				+ pucInImage[iOffset3 + 1] - pucInImage[iOffset3 - 1] );

			pnGradY[iOffset2] = (-pucInImage[iOffset1 - 1] - 2*pucInImage[iOffset1] - pucInImage[iOffset1 + 1] \
				+ pucInImage[iOffset3 - 1] + 2*pucInImage[iOffset3] + pucInImage[iOffset3 + 1]);

		}
	}
}

void CalcuBoundRect(float* pflShape, int nPtsNum,  RECT& boundRect)
{
	float   fMinX = 1.e+10;
	float   fMaxX = 0.0f;
	float   fMinY = 1.e+10;
	float   fMaxY = 0.0f;

	int i;
	for(i = 0; i < nPtsNum * 2; i += 2)
	{
		if ( fMinX > pflShape[i] ) fMinX = pflShape[i];
		if ( fMaxX < pflShape[i] ) fMaxX = pflShape[i];
		if ( fMinY > pflShape[i+1] ) fMinY = pflShape[i+1];
		if ( fMaxY < pflShape[i+1] ) fMaxY = pflShape[i+1];
	}
	boundRect.left   = int(fMinX + 0.5f);
	boundRect.right  = int(fMaxX + 0.5f);
	boundRect.top    = int(fMinY + 0.5f);
	boundRect.bottom = int(fMaxY + 0.5f);
}

//use plane fit method to calculate gradient of edge
void CalcuBorderGradient(float* pInImage, int iWidth, int iHeight, BYTE* pbyMask, 
							BYTE* pbyMask_Border, float* pDx, float* pDy)
{
	int     i, j, iOffset;
	int     a1, a2, a3, a4, a5, a6, A1, A2, A3;
	float   b1, b2, b3, B1, B2;
	int     m, n, winStartX, winEndX, winStartY, winEndY, x, y;


	for(j=0; j<iHeight; j++)
	{
		for(i=0; i<iWidth; i++)
		{
			iOffset = j * iWidth + i;
			if (pbyMask_Border[iOffset]) 
			{
				a1 = 0; a2 = 0; a3 = 0;
				a4 = 0; a5 = 0; a6 = 0; 

				b1 = 0; b2 = 0; b3 = 0;

				winStartX = max(0, i-1);
				winStartY = max(0, j-1);
				winEndX   = min(iWidth-1, i+1);
				winEndY   = min(iHeight-1, j+1);

				for(n=winStartY; n<=winEndY; n++)
				{
					for(m=winStartX; m<=winEndX; m++)
					{
						if (pbyMask[n * iWidth + m]) 
						{
							x = m - i;
							y = n - j;

							a1 += 1     ;    //Sigma 1
							a2 += x     ;    //Sigma x
							a3 += y     ;    //Sigma y
							a4 += (x*x) ;    //Sigma x*x
							a5 += (x*y) ;    //Sigma x*y
							a6 += (y*y) ;    //Sigma y*y

							b1 += pInImage[n * iWidth + m];            //Sigma f
							b2 += (pInImage[n * iWidth + m] * x);      //Sigma f * x
							b3 += (pInImage[n * iWidth + m] * y);      //Sigma f * y

						}

					}
				}
				A1 = a2 * a2 - a1 * a4;
				A2 = a2 * a3 - a1 * a5;
				A3 = a3 * a3 - a1 * a6;
				B1 = a2 * b1 - a1 * b2;
				B2 = a3 * b1 - a1 * b3;

				pDx[iOffset] = (A2 * B2 - A3 * B1) / (A2 * A2 - A1 * A3);
				pDy[iOffset] = (A2 * B1 - A1 * B2) / (A2 * A2 - A1 * A3);

				pDx[iOffset] *= 2; 
				pDy[iOffset] *= 2;
			}
		}
	}
}

void CalcuBorderGradient(BYTE* pInImage, int iWidth, int iHeight, BYTE* pbyMask, 
							BYTE* pbyMask_Border, int* pDx, int* pDy)
{
	int     i, j, iOffset;
	int     a1, a2, a3, a4, a5, a6, A1, A2, A3;
	float   b1, b2, b3, B1, B2;
	int     m, n, winStartX, winEndX, winStartY, winEndY, x, y;
	float   Dx, Dy;

	for(j=0; j<iHeight; j++)
	{
		for(i=0; i<iWidth; i++)
		{
			iOffset = j * iWidth + i;
			if (pbyMask_Border[iOffset]) 
			{
				a1 = 0; a2 = 0; a3 = 0;
				a4 = 0; a5 = 0; a6 = 0; 

				b1 = 0; b2 = 0; b3 = 0;

				winStartX = max(0, i-1);
				winStartY = max(0, j-1);
				winEndX   = min(iWidth-1, i+1);
				winEndY   = min(iHeight-1, j+1);

				for(n=winStartY; n<=winEndY; n++)
				{
					for(m=winStartX; m<=winEndX; m++)
					{
						if (pbyMask[n * iWidth + m]) 
						{
							x = m - i;
							y = n - j;

							a1 += 1     ;    //Sigma 1
							a2 += x     ;    //Sigma x
							a3 += y     ;    //Sigma y
							a4 += (x*x) ;    //Sigma x*x
							a5 += (x*y) ;    //Sigma x*y
							a6 += (y*y) ;    //Sigma y*y

							b1 += pInImage[n * iWidth + m];            //Sigma f
							b2 += (pInImage[n * iWidth + m] * x);      //Sigma f * x
							b3 += (pInImage[n * iWidth + m] * y);      //Sigma f * y

						}

					}
				}

				A1 = a2 * a2 - a1 * a4;
				A2 = a2 * a3 - a1 * a5;
				A3 = a3 * a3 - a1 * a6;
				B1 = a2 * b1 - a1 * b2;
				B2 = a3 * b1 - a1 * b3;

				Dx = (A2 * B2 - A3 * B1) / (A2 * A2 - A1 * A3);
				Dy = (A2 * B1 - A1 * B2) / (A2 * A2 - A1 * A3);

				pDx[iOffset] = int( Dx * 8);     //��sobel������ݶȣ�Kernelδ����һ�������
				pDy[iOffset] = int( Dy * 8);
			}
		}
	}
}

HRESULT CalculateIntegral(const unsigned char *uInputImg,int nImgWidth,int nImgHeight,int *OutputImg)//�ڴ�����������
{
	int i,j;
	int *nTempImg;
	if(uInputImg==NULL||OutputImg==NULL)
		return ERROR_INVALID_BLOCK;
	nTempImg=new int [nImgHeight*nImgWidth];
	if(nTempImg==NULL)
		return ERROR_NO_SYSTEM_RESOURCES;

	for (i=0;i<nImgWidth;i++)
	{
		nTempImg[i]=uInputImg[i];
	}	
	for (i=0;i<nImgWidth;i++)
		for (j=1;j<nImgHeight;j++)		
		{
			nTempImg[j*nImgWidth+i]=nTempImg[(j-1)*nImgWidth+i]+uInputImg[j*nImgWidth+i];
		}

		for (i=0;i<nImgHeight;i++)
		{
			OutputImg[i*nImgWidth]=nTempImg[i*nImgWidth];
		}	
		for (i=0;i<nImgHeight;i++)
			for (j=1;j<nImgWidth;j++)
			{
				OutputImg[i*nImgWidth+j]=OutputImg[i*nImgWidth+j-1]+nTempImg[i*nImgWidth+j];
			}

			delete [] nTempImg;
			nTempImg=NULL;
			return S_OK;
}




HRESULT CalculateSquareIntegral(const unsigned char *uInputImg,int nImgWidth,int nImgHeight,int *OutputImg)//�ڴ�����������
{
	int i,j;
	int *nTempImg;
	if(uInputImg==NULL||OutputImg==NULL)
		return ERROR_INVALID_BLOCK;
	nTempImg=new int [nImgHeight*nImgWidth];
	if(nTempImg==NULL)
		return ERROR_NO_SYSTEM_RESOURCES;

	for (i=0;i<nImgWidth;i++)
	{
		nTempImg[i]=uInputImg[i]*uInputImg[i];
	}	

	for (i=0;i<nImgWidth;i++)
		for (j=1;j<nImgHeight;j++)		
		{
			nTempImg[j*nImgWidth+i]=nTempImg[(j-1)*nImgWidth+i]+uInputImg[j*nImgWidth+i]*uInputImg[j*nImgWidth+i];
		}

		for (i=0;i<nImgHeight;i++)
		{
			OutputImg[i*nImgWidth]=nTempImg[i*nImgWidth];
		}	
		for (i=0;i<nImgHeight;i++)
			for (j=1;j<nImgWidth;j++)
			{
				OutputImg[i*nImgWidth+j]=OutputImg[i*nImgWidth+j-1]+nTempImg[i*nImgWidth+j];
			}

			delete [] nTempImg;
			nTempImg=NULL;
			return S_OK;
}
