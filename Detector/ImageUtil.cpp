//#include "Stdafx.h"
#include "ImageUtil.h"
#include <map>
#include <string.h>

// Basic Image/Processing Operations
bool CopyImage(const BYTE *pSrcImage, BYTE *pDesImage, const int nWidth, const int nHeight)
{
	if (pSrcImage == NULL || pDesImage == NULL)
	{
		return FALSE;
	}
	memcpy(pDesImage, pSrcImage, sizeof(BYTE) * nWidth * nHeight);
	return TRUE;
}


//////////////////////// called by TrackFeature ////////////////////////////

//-----------------------------------------------------------------------------------------
// downsampling pSrcImage to pDesImage, new size is |_ nSrcWidth/2 _| & |_ nSrcHeight/2 _|
// filter is   1/16  1/8   1/16
//             1/8   1/4   1/8
//             1/16  1/8   1/16
// This is new 4-neighbors, single channel version
//*
void DownSampling(
				  const BYTE *pSrc,
				  BYTE *pDes,
				  const int nSrcWidth,
				  const int nSrcHeight,
				  const int nChannels)
{
	int nDesWidth = nSrcWidth / 2;
	int nDesHeight = nSrcHeight / 2;
	int i, j, ps;
	int a1,a3,a5,a7;
	int nSrcStride = nSrcWidth * nChannels;
	int nDesStride = nDesWidth * nChannels;

	if (nChannels != 3)
	{
		/*a0 = -nSrcStride-1;*/  a1 = -nSrcStride; // a2 = -nSrcStride+1;  
		a7 = -1;                                a3 =  1;
		/*a6 =  nSrcStride-1;*/  a5 =  nSrcStride; // a4 =  nSrcStride+1;  
		ps = 0;
		pDes[0] = (((int)pSrc[ps] << 2) + (int)pSrc[a3] + (int)pSrc[a5]) / 6;
		for (i = 1; i < nDesWidth; i++)
		{
			ps = i*2;
			pDes[i] = (((int)pSrc[ps]<<2) + (int)pSrc[ps+a3] + (int)pSrc[ps+a5] + (int)pSrc[ps+a7]) /7;
		}

		for (j = 1; j < nDesHeight; j++)
		{
			ps = j*2*nSrcStride;
			pDes[j*nDesStride] = (((int)pSrc[ps]<<2) + (int)pSrc[ps+a1] + (int)pSrc[ps+a3] + (int)pSrc[ps+a5]) /7;
		}

		for (j = 1; j < nDesHeight; j++)
		{
			const BYTE* ps_row   = pSrc + j*2*nSrcStride;
			const BYTE* ps_row_u = pSrc + (j-1)*2*nSrcStride;
			const BYTE* ps_row_d = pSrc + (j+1)*2*nSrcStride;
			int x_off = 2;
			for (i = 1; i < nDesWidth; i++, x_off += 2)
			{
				pDes[j*nDesStride+i] = (((short int)ps_row[x_off]<<3) +
					(((short int)ps_row[x_off-1] + ps_row[x_off+1] + ps_row_u[x_off] + ps_row_d[x_off])<<1) + 6) >> 4;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------
// Image Pyramid/3D functions
bool BuildGrayImagePyramid(
						   const BYTE *pGrayImage,
						   BYTE **pGrayImagePyramid,
						   const int nWidth,
						   const int nHeight,
						   const int nLevel)
{
	int i;
	int nPmdWidth = nWidth;
	int nPmdHeight = nHeight;
	int nPrevWidth, nPrevHeight;

	if (!pGrayImagePyramid[0])    // Pyramid is not allocated yet
	{
		pGrayImagePyramid[0] = 
			new(std::nothrow) BYTE[nPmdWidth * nPmdHeight];

		if (pGrayImagePyramid[0] == NULL)
		{
			return FALSE;
		}
	}
	memcpy(pGrayImagePyramid[0], pGrayImage,
		sizeof(BYTE) * nPmdWidth * nPmdHeight);

	for (i = 1; i < nLevel; i++)
	{
		nPrevWidth = nPmdWidth;
		nPrevHeight = nPmdHeight;
		nPmdWidth /= 2;
		nPmdHeight /= 2;
		if (!pGrayImagePyramid[i])    // Pyramid is not allocated yet
		{
			pGrayImagePyramid[i] =
				new(std::nothrow) BYTE[nPmdWidth * nPmdHeight];

			if (pGrayImagePyramid[i] == NULL)
			{
				return FALSE;
			}
		}
		DownSampling(pGrayImagePyramid[i-1], pGrayImagePyramid[i], nPrevWidth, nPrevHeight, 1);
	}

	return TRUE;
}

int HarrisScore(const int gxx, const int gxy, const int gyy)
{

	int trace = (gxx + gyy) * (gxx + gyy);
	return (gxx * gyy - gxy * gxy - trace / 25);
}

//-----------------------------------------------------------------------------------------
// The filter kernel for horizontal gradient is 1/2 * [-1, (0), 1]
// The filter kernel for verticl gradient is 1/2 * [-1, (0), 1]^T
void CenterGradient3X1(
					   const BYTE *pBits,
					   short *pGradX,
					   short *pGradY,
					   const int nWidth,
					   const int nHeight,
					   const RECT *pRect)
{
	int nLeft, nTop, nRight, nBottom;
	// int nMargin = 1;

	if (pRect)
	{
		nLeft = pRect->left;
		nTop = pRect->top;
		nRight = pRect->right;
		nBottom = pRect->bottom;
	}
	else
	{
		nLeft = 0;
		nTop = 0;
		nRight = nWidth;
		nBottom = nHeight;
	}

	//Get Gradient in horizontal(X) direction
	for (int i = nTop; i < nBottom; i++)
	{
		const BYTE* pRowBits = pBits + i * nWidth;
		short* pRowGradX = pGradX + i*nWidth;

		pRowGradX[nLeft] = pRowGradX[nRight-1] = 0;

		for (int j = nLeft + 1; j < nRight-1; j++) 
		{    
			pRowGradX[j] = ((int)pRowBits[j+1] - pRowBits[j-1]) >> 1;
		}
	}
	//Get Gradient in horizontal(Y) direction

	for (int j = nLeft; j < nRight; j++) 
	{
		pGradY[j + nTop * nWidth] = 0;
		pGradY[j + (nBottom - 1) * nWidth] = 0;
	}

	for (int i = nTop + 1; i < nBottom - 1; i++)
	{
		const BYTE* pRowBits_U = pBits + (i - 1) * nWidth;
		const BYTE* pRowBits_D = pBits + (i + 1) * nWidth;

		short* pRowGradY = pGradY + i*nWidth;

		for (int j = nLeft; j < nRight; j++) 
		{
			pRowGradY[j] = ((int)pRowBits_D[j] - pRowBits_U[j]) >> 1;
		}
	}
}



