

//#include "stdafx.h"
#include "TrackFeature.h"
#include "Common.h"
#include "ImageModule.h"

#pragma warning(disable:4996)

extern int g_nFrameIdx      ;
extern int g_nKeyFrameIdx   ;

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


void store_match_addrnxn(int *pDx, int *pDy, const int nSearchRange)
{
	if (pDx == NULL || pDy == NULL)
	{
		return;
	}

	pDx[0] = 0; 
	pDy[0] = 0; 

	int k = 1;
	for (int m = 1; m <= nSearchRange; m++)
	{
		for (int i = -m; i < m; i++)        // up line 1 2
		{
			pDx[k] = i; pDy[k] = -m; 
			k++;
		}
		for (int i = -m; i < m; i++)        // right line 3 4
		{
			pDx[k] = m; pDy[k] = i; 
			k++;
		}
		for (int i = -m; i < m; i++)    // bottom line 5 6
		{
			pDx[k] = -i; pDy[k] = m; 
			k++;
		}
		for (int i = -m; i < m; i++)        // left line   7 8
		{
			pDx[k] = -m; pDy[k] = -i; 
			k++;
		}
	}
}

CTrackFeature::CTrackFeature()
{
    tbInit = FALSE;
    tpContext  = NULL;
    tbIncFrame = true;

    tpPrevPmdImage = NULL;
    tpCurrPmdImage = NULL;
    tpPrevFeatures = NULL;
    tpNewFeatures = NULL;
    tpPrevGradX = NULL;
    tpPrevGradY = NULL;
    tpFeatureMap = NULL;
    tpFeatureScore = NULL;
    tpFeatureMask = NULL;
    tpImgGxx = NULL;
    tpImgGyy = NULL;
    tpImgGxy = NULL;

    tpHistEqImage = NULL;
    
    tpBufForSelectGoodFeature = NULL;

    tbEnableRejectFeatureTrick = FALSE;    
   
}

CTrackFeature::~CTrackFeature()
{
    Destroy();
}

/////////////////////////////// public functions ///////////////////////////

bool CTrackFeature::Init(
        const int nWidth,
        const int nHeight,
        const int nMaxFeatures,
        const int nWinSize)
{
    if (tbInit)
    {
        Destroy();
    }

    tpContext = NULL;

    tnWidth = nWidth;
    tnHeight = nHeight;
    tnMaxFeatures = nMaxFeatures;
    tnWinSize = nWinSize;

    //Review: Strange, maybe replace with log*/log2;
    tnPmdLevels = 0;
    while (((nWidth>>tnPmdLevels) >= nWinSize) && ((nHeight>>tnPmdLevels) >= nWinSize))
    {
        tnPmdLevels++;
    }

    if (tnPmdLevels == 0)
    {
        return FALSE;
    }

    //////////////////////////////////////
    // Allocate memory Py
    tnPmdSizeTotal = 0;
    for (int i = 0; i < tnPmdLevels; i++)
    {
        tnPmdSizeTotal += (nWidth * nHeight) >> (2 * i);
    }

    tpPrevPmdImage = new(std::nothrow) BYTE*[tnPmdLevels*2];
    if (tpPrevPmdImage == NULL)
    {
        tbInit = FALSE;
        return FALSE;
    }
    memset(tpPrevPmdImage, 0, sizeof(BYTE*) * tnPmdLevels * 2);

    tpCurrPmdImage = tpPrevPmdImage + tnPmdLevels;

    tpPrevPmdImage[0] = new(std::nothrow) BYTE[tnPmdSizeTotal*2];
    if (tpPrevPmdImage[0] == NULL)
    {
        SAFE_ARRAY_DELETE(tpPrevPmdImage[0]);
        SAFE_ARRAY_DELETE(tpPrevFeatures);
        SAFE_ARRAY_DELETE(tpNewFeatures);
        SAFE_ARRAY_DELETE(tpPrevGradX);
        SAFE_ARRAY_DELETE(tpPrevGradY);
        SAFE_ARRAY_DELETE(tpFeatureMap);
        SAFE_ARRAY_DELETE(tpBufForSelectGoodFeature);
        SAFE_ARRAY_DELETE(tpImgGxx);
        SAFE_ARRAY_DELETE(tpImgGyy);
        SAFE_ARRAY_DELETE(tpImgGxy);
        SAFE_ARRAY_DELETE(tpFeatureScore);
        SAFE_ARRAY_DELETE(tpFeatureMask);
        SAFE_ARRAY_DELETE(tpPrevPmdImage);
        SAFE_ARRAY_DELETE(tpHistEqImage);
        tbInit = FALSE;
        return FALSE;
    }
    memset(tpPrevPmdImage[0], 0, sizeof(BYTE) * tnPmdSizeTotal * 2);

    tpImgGxx = new(std::nothrow) int[(tnWidth+1) * (tnHeight+1)];
    tpImgGyy = new(std::nothrow) int[(tnWidth+1) * (tnHeight+1)];
    tpImgGxy = new(std::nothrow) int[(tnWidth+1) * (tnHeight+1)];


    if (tpImgGxx == NULL || tpImgGxy == NULL || tpImgGyy == NULL)
    {
        SAFE_ARRAY_DELETE(tpPrevPmdImage[0]);
        SAFE_ARRAY_DELETE(tpPrevFeatures);
        SAFE_ARRAY_DELETE(tpNewFeatures);
        SAFE_ARRAY_DELETE(tpPrevGradX);
        SAFE_ARRAY_DELETE(tpPrevGradY);
        SAFE_ARRAY_DELETE(tpFeatureMap);
        SAFE_ARRAY_DELETE(tpBufForSelectGoodFeature);
        SAFE_ARRAY_DELETE(tpImgGxx);
        SAFE_ARRAY_DELETE(tpImgGyy);
        SAFE_ARRAY_DELETE(tpImgGxy);
        SAFE_ARRAY_DELETE(tpFeatureScore);
        SAFE_ARRAY_DELETE(tpFeatureMask);
        SAFE_ARRAY_DELETE(tpPrevPmdImage);
        SAFE_ARRAY_DELETE(tpHistEqImage);
        tbInit = FALSE;
        return FALSE;
    }
    memset(tpImgGxx, 0, sizeof(int) * (tnWidth + 1) * (tnHeight + 1));
    memset(tpImgGyy, 0, sizeof(int) * (tnWidth + 1) * (tnHeight + 1));
    memset(tpImgGxy, 0, sizeof(int) * (tnWidth + 1) * (tnHeight + 1));

    m_InteGxx.SetSumImage(tpImgGxx, nWidth, nHeight);
    m_InteGyy.SetSumImage(tpImgGyy, nWidth, nHeight);
    m_InteGxy.SetSumImage(tpImgGxy, nWidth, nHeight);
    tpCurrPmdImage[0] = tpPrevPmdImage[0] + tnPmdSizeTotal;

    for (int i = 1; i < tnPmdLevels; i++)
    {
        tpPrevPmdImage[i] = tpPrevPmdImage[i-1] + ((nWidth * nHeight) >> (2*i-2));
        tpCurrPmdImage[i] = tpCurrPmdImage[i-1] + ((nWidth * nHeight) >> (2*i-2));
    }
    //////////////////////////////////////

    tpFeatureScore = new(std::nothrow) int[nWidth * nHeight];
    tpFeatureMask = new(std::nothrow) bool[nWidth * nHeight];

    tpPrevFeatures = new(std::nothrow) IMAGEFEATURE[nMaxFeatures];
    tpNewFeatures = new(std::nothrow) IMAGEFEATURE[nWidth * nHeight];
    
    tpPrevGradX = new(std::nothrow) short[nWidth * nHeight];
    tpPrevGradY = new(std::nothrow) short[nWidth * nHeight];
    tpFeatureMap = new(std::nothrow) bool[nWidth * nHeight];

    tpBufForSelectGoodFeature =
        new(std::nothrow) int[(nWidth + 1) * (nHeight + 1) * 3];

    tpHistEqImage  = new(std::nothrow) BYTE[nWidth * nHeight];


    if (!tpPrevFeatures || !tpNewFeatures
        || !tpPrevGradX || !tpPrevGradY 
        || !tpFeatureMap || !tpBufForSelectGoodFeature || !tpFeatureScore || !tpHistEqImage)
    {
        SAFE_ARRAY_DELETE(tpPrevPmdImage[0]);
        SAFE_ARRAY_DELETE(tpPrevFeatures);
        SAFE_ARRAY_DELETE(tpNewFeatures);
        SAFE_ARRAY_DELETE(tpPrevGradX);
        SAFE_ARRAY_DELETE(tpPrevGradY);
        SAFE_ARRAY_DELETE(tpFeatureMap);
        SAFE_ARRAY_DELETE(tpBufForSelectGoodFeature);
        SAFE_ARRAY_DELETE(tpImgGxx);
        SAFE_ARRAY_DELETE(tpImgGyy);
        SAFE_ARRAY_DELETE(tpImgGxy);
        SAFE_ARRAY_DELETE(tpFeatureScore);
        SAFE_ARRAY_DELETE(tpFeatureMask);
        SAFE_ARRAY_DELETE(tpPrevPmdImage);
        SAFE_ARRAY_DELETE(tpHistEqImage);
        tbInit = FALSE;
        return FALSE;
    }
    memset(tpFeatureScore, 0, sizeof(int) * nWidth * nHeight);
    memset(tpFeatureMask, 0, sizeof(bool) * nWidth * nHeight);
    memset(tpPrevFeatures, 0, sizeof(IMAGEFEATURE) * nMaxFeatures);
    memset(tpNewFeatures, 0, sizeof(IMAGEFEATURE) * nWidth * nHeight);
    memset(tpPrevGradX, 0, sizeof(short) * nWidth * nHeight);
    memset(tpPrevGradY, 0, sizeof(short) * nWidth * nHeight);
    memset(tpFeatureMap, 0, sizeof(bool) * nWidth * nHeight);
    memset(tpBufForSelectGoodFeature, 0, sizeof(int) * (nWidth + 1) * (nHeight + 1) * 3);
    memset(tpHistEqImage, 0, sizeof(BYTE) * nWidth * nHeight);

    tbInit = TRUE;
    return TRUE;

//OnError:
    SAFE_ARRAY_DELETE(tpPrevPmdImage[0]);
    SAFE_ARRAY_DELETE(tpPrevFeatures);
    SAFE_ARRAY_DELETE(tpNewFeatures);
    SAFE_ARRAY_DELETE(tpPrevGradX);
    SAFE_ARRAY_DELETE(tpPrevGradY);
    SAFE_ARRAY_DELETE(tpFeatureMap);
    SAFE_ARRAY_DELETE(tpBufForSelectGoodFeature);
    SAFE_ARRAY_DELETE(tpImgGxx);
    SAFE_ARRAY_DELETE(tpImgGyy);
    SAFE_ARRAY_DELETE(tpImgGxy);
    SAFE_ARRAY_DELETE(tpFeatureScore);
    SAFE_ARRAY_DELETE(tpFeatureMask);
    SAFE_ARRAY_DELETE(tpPrevPmdImage);
    SAFE_ARRAY_DELETE(tpHistEqImage);
//DirectRet:
    tbInit = FALSE;
    return FALSE;
}


bool CTrackFeature::SelectFeatures(
        const BYTE *pImage,
        IMAGEFEATURE *pFeatures,
        const W_Rect& rcFace,
        const int nWinDetect,
        const int nFeatureSeparation,
        const int nMinVal,
        const int nSpeedUp,
        const BYTE *pMaskImage)
{
    //_ASSERT(pImage != NULL && pFeatures != NULL && pMaskImage != NULL);

    if (pImage == NULL || pFeatures == NULL || pMaskImage == NULL)
    {
        return FALSE;
    }

    if ( tpContext->tbPreSelectFeature )    // add some preSelect feature begin
    {
        memcpy( tpPrevFeatures, pFeatures, sizeof(IMAGEFEATURE) * tnMaxFeatures );
    }
    else
    {
        memset(tpPrevFeatures, 0, sizeof(IMAGEFEATURE) * tnMaxFeatures);
    }

    int nHalfWinDet = nWinDetect;

    //Review: enlarge face rect with a half windows size of feature selection
    W_Rect rcTemp;
    int nTemp = rcFace.X - nHalfWinDet;
    rcTemp.X = nTemp < 0 ? 0 : nTemp;

    //nTemp = rcFace.Y - nHalfWinDet;
    rcTemp.Y = rcFace.Y - nHalfWinDet < 0 ? 0 : rcFace.Y - nHalfWinDet;

    nTemp = rcFace.Width + rcTemp.X+ nWinDetect*2;
    rcTemp.Width = 
        nTemp > tnWidth - 1 ? tnWidth - 1 - rcTemp.X : nTemp - rcTemp.X;

    nTemp = rcFace.Height + rcTemp.Y + nWinDetect*2;
    rcTemp.Height = 
        nTemp > tnHeight - 1 ? tnHeight - 1 - rcTemp.Y : nTemp - rcTemp.Y;

   
    RECT rc;
    rc.top  = rcTemp.Y;
    rc.left = rcTemp.X;
    rc.bottom = rcTemp.Y + rcTemp.Height;
    rc.right  = rcTemp.X + rcTemp.Width;
    
    memcpy( tpHistEqImage, pImage, sizeof(BYTE) * tnWidth * tnHeight );
    HistogramEqualization( tpHistEqImage, tnWidth, tnHeight, rc );

    //Compute image x/y Gradient 
    CenterGradient3X1(tpHistEqImage, tpPrevGradX, tpPrevGradY,
        tnWidth, tnHeight, &rc);

    if (4 > SelectGoodFeature_NSort(
                tpPrevGradX, 
                tpPrevGradY,
                pMaskImage,
                tnWidth,
                tnHeight,
                tpPrevFeatures,
                tnMaxFeatures,
                tpFeatureMap,
                rcTemp,
                FALSE,
                nWinDetect,
                nFeatureSeparation,
                nMinVal,
                nSpeedUp))
    {
        return FALSE;
    }

    memcpy(pFeatures, tpPrevFeatures, sizeof(IMAGEFEATURE) * tnMaxFeatures);
    return TRUE;
}


bool CTrackFeature::TrackFeatures(
        const BYTE *pPrevImage,
        const BYTE *pCurrImage,
        const IMAGEFEATURE *pPrevFeatures,
        IMAGEFEATURE *pCurrFeatures,
        const bool bFloatFeature,
        const W_Point& ptShift)
{
    if (pPrevImage == NULL || pCurrImage == NULL)
    {
        return FALSE;
    }

    bool BuildPyramidSucceed = FALSE;

    //Review: remove redundant of BuildGrayImagePyramid
    BuildPyramidSucceed = BuildGrayImagePyramid(
        pPrevImage, tpPrevPmdImage, tnWidth, tnHeight, tnPmdLevels);

    if (!BuildPyramidSucceed)
    {
        return FALSE;
    }


    BuildPyramidSucceed = BuildGrayImagePyramid(
        pCurrImage, tpCurrPmdImage, tnWidth, tnHeight, tnPmdLevels);

    if (!BuildPyramidSucceed)
    {
        return FALSE;
    }

    FeatureBlockMatchingIllum_Old(
            tpPrevPmdImage,
            tpCurrPmdImage,
            tnWidth,
            tnHeight,
            tnPmdLevels, 
            pPrevFeatures,
            pCurrFeatures,
            tnMaxFeatures,
            tnWinSize,
            !bFloatFeature,
            ptShift);
    return TRUE;
}



///////////// protected functions /////////////////////

void CTrackFeature::Destroy()
{
    if (tpPrevPmdImage)
    {
        delete tpPrevPmdImage[0];
        delete tpPrevPmdImage;
        tpPrevPmdImage = tpCurrPmdImage = NULL;
    }

    SAFE_ARRAY_DELETE(tpPrevFeatures);
    SAFE_ARRAY_DELETE(tpNewFeatures);
    SAFE_ARRAY_DELETE(tpPrevGradX);
    SAFE_ARRAY_DELETE(tpPrevGradY);
    SAFE_ARRAY_DELETE(tpFeatureMap);

    SAFE_ARRAY_DELETE(tpBufForSelectGoodFeature);
    SAFE_ARRAY_DELETE(tpImgGxx);
    SAFE_ARRAY_DELETE(tpImgGyy);
    SAFE_ARRAY_DELETE(tpImgGxy);
    
    SAFE_ARRAY_DELETE(tpFeatureScore);
    SAFE_ARRAY_DELETE(tpFeatureMask);
    SAFE_ARRAY_DELETE(tpHistEqImage);

    tbInit = FALSE;
}




//-----------------------------------------------------------------------------------------
// Sort features by .w field values (in decreasing order)
int  _CompareFeaturesW(const void *a, const void *b)
{
    if (((IMAGEFEATURE*)a)->w > ((IMAGEFEATURE*)b)->w)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

int CTrackFeature::SelectGoodFeature_NSort(
        const short *pGradX,
        const short *pGradY,
        const BYTE *pMaskImage,
        const int nWidth,
        const int nHeight,
        IMAGEFEATURE *pFeatures,
        const int nMaxFeatures,
        bool *pFeatureMap,
        const W_Rect& rcFace,
        const bool bAppendFeatures,
        const int nWinSize,
        const int nFeatureSeparation,
        const int nMinVal,
        const int nSpeedUp)
{
    if (pGradX == NULL || pGradY == NULL ||
        pMaskImage == NULL || pFeatures == NULL)
    {
        return (-1);
    }

    // Calculate integral image of gxx, gxy, gyy
    W_Rect rc = rcFace;
    int nSizeShift = (int)(log((double)nWinSize * nWinSize) / log(2.0));

    m_InteGxx.CalcSumImage(pGradX, NULL,   tnWidth, tnHeight, &rc);
    m_InteGyy.CalcSumImage(pGradY, NULL,   tnWidth, tnHeight, &rc);
    m_InteGxy.CalcSumImage(pGradX, pGradY, tnWidth, tnHeight, &rc);


    // Select feature through computing corner score
    int* pFeatureScore = tpFeatureScore;
    memset(pFeatureScore, 0, sizeof(int) * nWidth * nHeight);
    memset(pFeatureMap, 0, sizeof(bool) * nWidth * nHeight);

    int nBorder  = nWinSize >> 1;
    int nMinVal2 = nMinVal >> 1;
    int nNewFeatureCount = 0;
    const BYTE* pMaskPtr = pMaskImage + nBorder * nWidth;

    int i,j;

    for (j = nBorder; j < nHeight - nBorder; j += nSpeedUp, pMaskPtr += nWidth)
    {
        for (i = nBorder; i < nWidth - nBorder; i += nSpeedUp)
        {
            if (pMaskPtr[i] != 0)
            {
                int gxx = m_InteGxx.Area(i-nBorder, j-nBorder,
                                    i+nBorder, j+nBorder) >> nSizeShift;

                if (gxx >= nMinVal2)
                {
                    int gyy = m_InteGyy.Area(i-nBorder, j-nBorder,
                                        i+nBorder, j+nBorder) >> nSizeShift;

                    if (gyy >= nMinVal2)
                    {
                        int gxy = m_InteGxy.Area(i-nBorder, j-nBorder,
                                        i+nBorder, j+nBorder) >> nSizeShift;
                        int val = HarrisScore(gxx, gxy, gyy);

                        if (val >= nMinVal)
                        {
                            int nID = j * nWidth + i;
                            pFeatureMap[nID]  = true;
                            pFeatureScore[nID] = val;                                                     
                            nNewFeatureCount++;
                        }
                    }
                }
            }
        }
    }

    if ( tpContext->tbPreSelectFeature )    // add some preSelect feature begin
    {
        int k;
        for ( k = 0; k < tnMaxFeatures; k++ )
        {
            if ( pFeatures[k].w > 0.5f )
            {
                i = int( pFeatures[k].x );
                j = int( pFeatures[k].y );

                if ( i >= 0 && i < nWidth && j >= 0 && j < nHeight )
                {
                    int nID = j * nWidth + i;
                    if ( pMaskImage[nID] != 0 )
                    {
                        pFeatureMap[nID]  = true;
                        pFeatureScore[nID] = 1 << 30;                                                     
                        nNewFeatureCount++;
                    }
                }
            }
            else
            {
                break;   
            }
        }
        memset(tpPrevFeatures, 0, sizeof(IMAGEFEATURE) * k);
    }

    bool bDone = nNewFeatureCount > nMaxFeatures ? false : true ; 
    int nHalfFeatureSeparation = 1;
    int nFinalFeatures = 0;
 //   bool bSmallFace = false;

    while(!bDone)
    {
        nFinalFeatures = 0;
        int nWidth_nFeatureSeparation_1 = nWidth - nFeatureSeparation - 1;
        for (j = nFeatureSeparation + 1; j < nHeight - nFeatureSeparation - 1; j++)
        {
            int htmp = j * nWidth;
            bool *ppFeatureMap = pFeatureMap+htmp+nFeatureSeparation + 1;
            for (i = nFeatureSeparation + 1; i < nWidth_nFeatureSeparation_1; i++,ppFeatureMap++)
            {
               // if (!pFeatureMap[htmp + i])
                if (!(*ppFeatureMap))
                {
                    continue;
                }

                int s =  pFeatureScore[htmp + i];
                bool bLocalMin = true;

                for (int m = j - nHalfFeatureSeparation; m <= j + nHalfFeatureSeparation; m++)
                {
                    int htmp2 = m * nWidth;
                    for (int n = i - nHalfFeatureSeparation; n <= i + nHalfFeatureSeparation; n++)
                    {    
                        if (pFeatureScore[htmp2 + n] > s)
                        {
                            bLocalMin = false;
                            goto SAVE_FEATURE;
                        }
                    }
                }

SAVE_FEATURE:
                if (bLocalMin)
                {
                    for (int m = j - nHalfFeatureSeparation; m <= j + nHalfFeatureSeparation; m++)
                    {
                        int htmp2 = m * nWidth;
                        for (int n = i - nHalfFeatureSeparation; n <= i + nHalfFeatureSeparation; n++)
                        {    
                            pFeatureMap[htmp2 + n] = false;  
                        }
                    }
                    pFeatureMap[htmp + i] = true;    
                    nFinalFeatures++;                            
                }
                else
                {
                    pFeatureMap[htmp + i] = false;                    
                }
            }
        }

        if (nFinalFeatures <= nMaxFeatures)
        {
            bDone = true;
        }
        else
        {
            nHalfFeatureSeparation += 1;            
        }
    }

    nFinalFeatures = 0;
    for (j = nFeatureSeparation + 1; j < nHeight - nFeatureSeparation-1; j++)
    {
        for (i = nFeatureSeparation + 1; i < nWidth - nFeatureSeparation-1; i++)
        {
            if (pFeatureMap[j*nWidth+i] )
            {
                pFeatures[nFinalFeatures].x = (float)i;
                pFeatures[nFinalFeatures].y = (float)j;
                pFeatures[nFinalFeatures].w = (float)pFeatureScore[j*nWidth+i];
                nFinalFeatures++;
                if (nFinalFeatures >= nMaxFeatures - 1)
                {
                    return nFinalFeatures;
                }
            }
        }
    }

    //If we regard the input is small face, we will add some uniform feature on the image.
    if (nFinalFeatures < 60)
    {
        memset(pFeatureMap, 0 , sizeof(bool) * nWidth * nHeight);
        AddUniformFeature(pFeatureMap, pMaskImage, pFeatureScore,
                rcFace, nFinalFeatures);     

        for (j = nFeatureSeparation+1; j < nHeight - nFeatureSeparation-1; j++)
        {
            for (i = nFeatureSeparation+1; i < nWidth - nFeatureSeparation-1; i++)
            {
                if (pFeatureMap[j*nWidth+i] )
                {
                    pFeatures[nFinalFeatures].x = (float)i;
                    pFeatures[nFinalFeatures].y = (float)j;
                    pFeatures[nFinalFeatures].w = 
                        (float)pFeatureScore[j * nWidth + i];

                    nFinalFeatures++;
                    if (nFinalFeatures >= nMaxFeatures - 1)
                    {
                        return nFinalFeatures;
                    }
                }
            }
        }
    }

//CLEAN_UP:
    return nFinalFeatures;
}

///Consider more carefully
int CTrackFeature::AddUniformFeature(
        bool* pFeatureMap,
        const BYTE* pMaskImage,
        const int* pFeatureScore,
        const W_Rect& rcFace,
        const int nCurrFeatureCount)
{
    const int nFeatureSeparation = 5;
    const int nSampleCount = 
        (int)min(sqrt((float)(tnMaxFeatures - nCurrFeatureCount)) + 1, 8);

    if (nSampleCount < 2)
    {
        return 0;
    }
    
    int nNewSampleCount = 0;
    
    bool* pFeatureMask = tpFeatureMask;
    memset(pFeatureMask, 0 , sizeof(bool) * tnWidth * tnHeight);

    const int nRow = rcFace.Width / nSampleCount;
    const int nCol = rcFace.Height / nSampleCount;

    for (int j = 0; j < nSampleCount; j++)
    {
        for (int i = 0; i < nSampleCount; i++)
        {
            int nMaxScore = 0;
            int nMaxID[2]={0,0};

            const int y = rcFace.Y + j * nCol;
            const int x = rcFace.X + i * nRow;

            bool bHasFeature = false;

            for (int m = 0; m < nCol; m++)
            {
                for (int n = 0; n < nRow; n++)
                {
                    int CurrentFeature = (y + m) * tnWidth + x + n;

                    if (pFeatureScore[CurrentFeature] > nMaxScore
                        && !pFeatureMask[CurrentFeature])
                    {
                        nMaxID[0] = x + n;
                        nMaxID[1] = y + m;
                        nMaxScore = pFeatureScore[CurrentFeature];
                        bHasFeature = true;
                    }
                }
            }

            if (!pFeatureMask[ nMaxID[1] * tnWidth + nMaxID[0] ] &&
                 pMaskImage[ nMaxID[1] * tnWidth + nMaxID[0] ] &&
                 bHasFeature)
            {
                for (int m = nMaxID[1] - nFeatureSeparation; m <= nMaxID[1] + nFeatureSeparation; m++)
                {
                    for (int n = nMaxID[0] - nFeatureSeparation; n <= nMaxID[0] + nFeatureSeparation; n++)
                    {    
                        if (m >= 0 && m <= tnHeight - 1 &&
                            n >= 0 && n <= tnWidth - 1)
                        {
                            pFeatureMask[m * tnWidth + n] = true;
                        }
                    }
                }
                pFeatureMap[nMaxID[1] * tnWidth + nMaxID[0]] = true;
                nNewSampleCount++;
            }
        }
    }
    return 0;
}

bool CTrackFeature::FeatureBlockMatchingIllum_Old(
        BYTE **pPmdImage1,
        BYTE **pPmdImage2,
        const int nWidth,
        const int nHeight,
        const int nPmdLevels,
        const IMAGEFEATURE *pFeatures1,
        IMAGEFEATURE *pFeatures2,
        const int nFeatures,
        const int nWinSize,
        const bool bComputeSSDError,
        const W_Point& ptShift)
{
    if (pPmdImage1 == NULL || pPmdImage2 == NULL || pFeatures1 == NULL || pFeatures2 == NULL)
    {
            return FALSE;
    }

    const int nhWinSize = (nWinSize - 1) / 2;        // half width of window
    const int nWinBuffer = 81;
    const int nWinPixels = nWinSize * nWinSize;

    const int nMaxSearchBlkSize = 128;            // maximum search block: 64*64
    const int nSearchRange = 1;
    const int nSearchBlkSize = (2 * nSearchRange + 1) * (2 * nSearchRange + 1);

    const float fErrorSigma = 1.f / (nWinSize * nWinSize * 5 * 5);

    //double k = 0;
    int pDxTemp, pDyTemp;

    int nError;
    float fError;
    int sx, sy, dx, dy;
    int nSumPmd1 = 0, nSumPmd2 = 0;                    
    BYTE *pPmd1Ptr, *pPmd2Ptr, *pPmd1PtrTemp, *pPmd2PtrTemp;

    int pDx[nMaxSearchBlkSize], pDy[nMaxSearchBlkSize];
    BYTE MatchMask[nWinBuffer];

    memcpy(pFeatures2, pFeatures1, sizeof(IMAGEFEATURE) * nFeatures);     // commented for simulation
    store_match_addrnxn(pDx, pDy, nSearchRange);    


    // int nMinSqrDist = nDx * nDx + nDy * nDy;
    // find flow for each given point    
    for (int i = 0; i < nFeatures; i++)
    //for (int i = 0; i < 0; i++) // for simulation
    {
        if (pFeatures1[i].w <= 0)
        {
            pFeatures2[i].x = -1;
            pFeatures2[i].y = -1;
            pFeatures2[i].w = 0;            
            continue;
        }

        float fMinError = 255.0f;
        bool bFeatureOK = TRUE;
        int nPmdScale = 1 << (nPmdLevels-1);
            
        int nPmdWidth;
        int nPmdHeight;
        int nMinIndex = 0;
        int nMinSumPmd1 = 255, nMinSumPmd2 = 255;
        int nUx, nUy, nVx, nVy;
        int nDx = 0, nDy = 0;
        
        nDx = (int)(ptShift.X / nPmdScale);
        nDy = (int)(ptShift.Y / nPmdScale);

        int nMinSqrDist;
        for (int l = nPmdLevels - 1; l >= 0; l--, nPmdScale /= 2)
        {
            if (l != nPmdLevels - 1)
            {
                nDx *= 2;
                nDy *= 2;
            }
            
            nUx = (int)(pFeatures1[i].x / nPmdScale +0.5f);
            nUy = (int)(pFeatures1[i].y / nPmdScale +0.5f);
            nVx = nUx + nDx;
            nVy = nUy + nDy;
            nPmdWidth = nWidth / nPmdScale;
            nPmdHeight = nHeight / nPmdScale;
            BYTE* pPmd1 = pPmdImage1[l];
            BYTE* pPmd2 = pPmdImage2[l];
            {

                bool bAllinImage = 
                    nUx - nhWinSize - nSearchRange >= 0 && nUx + nhWinSize + nSearchRange < nPmdWidth &&
                    nUy - nhWinSize - nSearchRange >= 0 && nUy + nhWinSize + nSearchRange < nPmdHeight &&
                    nVx - nhWinSize - nSearchRange >= 0 && nVx + nhWinSize + nSearchRange < nPmdHeight &&
                    nVy - nhWinSize - nSearchRange >= 0 && nVy + nhWinSize + nSearchRange < nPmdHeight;

                if ((nVx >= 0) && (nVy >= 0) && (nVx < nPmdWidth) && (nVy < nPmdHeight))
                {
                    // (nVx, nVy) and (nUx, nUy) is in the pyramid image
                    fMinError = 255.0f;
                    nMinIndex = 0;
                    nMinSqrDist = nDx * nDx + nDy * nDy;
                    nMinSumPmd1 = nMinSumPmd2 = 255;

                    ////////////////////////////////////////////
                    for (int j = 0; j < nSearchBlkSize; j++)
                    {    
                        //////////////////////offset
                        pDxTemp = nVx + pDx[j];
                        pDyTemp = nVy + pDy[j];

                        pPmd1PtrTemp = pPmd1Ptr = pPmd1 + (nUy-nhWinSize) * nPmdWidth + nUx;
                        pPmd2PtrTemp = pPmd2Ptr = pPmd2 + (pDyTemp-nhWinSize) * nPmdWidth + pDxTemp;

                        nSumPmd1 = 0;
                        nSumPmd2 = 0;

                        if (bAllinImage)
                        {
                            for (int y = -nhWinSize; y <= nhWinSize;
                                    y++, pPmd1Ptr += nPmdWidth, pPmd2Ptr += nPmdWidth)
                            {
                                for (int x = -nhWinSize; x <= nhWinSize; x++)
                                {
                                    nSumPmd1 += pPmd1Ptr[x];
                                    nSumPmd2 += pPmd2Ptr[x];
                                }
                            }
                            if ((nSumPmd1 == 0) || (nSumPmd2 == 0))
                            {
                                continue;
                            }

                            pPmd1Ptr = pPmd1PtrTemp;
                            pPmd2Ptr = pPmd2PtrTemp;

                            nError = 0;
                            for (int y = -nhWinSize; y <= nhWinSize;
                                    y++, pPmd1Ptr += nPmdWidth, pPmd2Ptr += nPmdWidth)
                            {
                                for (int x=-nhWinSize; x<=nhWinSize; x++)
                                {
                                    //nError += abs(pPmd1Ptr[x] * nSumPmd2 - pPmd2Ptr[x] * nSumPmd1);  //BXU
                                    int tmp = pPmd1Ptr[x] * nSumPmd2 - pPmd2Ptr[x] * nSumPmd1;
                                    nError+= tmp>=0?tmp:-tmp;
                                }
                            }
                        }
                        else
                        {
                            if ((pDxTemp < 0) || (pDyTemp < 0) || (pDxTemp >= nPmdWidth)
                                || (pDyTemp >= nPmdHeight))
                            {
                                continue;
                            }
                            //////////////////////////
                
                            memset(MatchMask, 0, nWinPixels);
                            for (int y = -nhWinSize; y <= nhWinSize;
                                    y++, pPmd1Ptr += nPmdWidth, pPmd2Ptr += nPmdWidth)
                            {
                                sy = nUy + y;
                                if (sy < 0 || sy >= nPmdHeight)                            
                                {
                                    continue;
                                } 

                                dy = pDyTemp + y;
                                if (dy < 0 || dy >= nPmdHeight)
                                {
                                    continue;
                                }

                                for (int x = -nhWinSize; x <= nhWinSize; x++)
                                {
                                    sx = nUx + x;
                                    if (sx < 0 || sx >= nPmdWidth)                            
                                    {
                                        continue;
                                    }

                                    dx = pDxTemp + x ;
                                    if (dx < 0 || dx >= nPmdWidth)                            
                                    {
                                        continue;
                                    }

                                    nSumPmd1 += pPmd1Ptr[x];
                                    nSumPmd2 += pPmd2Ptr[x];

                                    MatchMask[(y + nhWinSize) * nWinSize + x + nhWinSize] = 1;
                                }
                            }
                            if ((nSumPmd1 == 0) || (nSumPmd2 == 0))
                            {
                                continue;
                            }

                            pPmd1Ptr = pPmd1PtrTemp;
                            pPmd2Ptr = pPmd2PtrTemp;

                            nError = 0;
                            const BYTE *pMask = MatchMask;
                            for (int y = -nhWinSize; y <= nhWinSize;
                                    y++, pPmd1Ptr += nPmdWidth, pPmd2Ptr += nPmdWidth)
                            {
                                for (int x = -nhWinSize; x <= nhWinSize; x++)
                                {
                                    if (*pMask++)
                                    {
                                        nError +=
                                            abs(pPmd1Ptr[x] * nSumPmd2 - pPmd2Ptr[x] * nSumPmd1);
                                    }
                                }
                            }
                        }
                        //////////////////////////////////////

                        if (nSumPmd1 == 0 || nSumPmd2 == 0)
                        {
                            continue;
                        }
                        fError = (float)nError / (nSumPmd1 * nSumPmd2);
                        if (fError < fMinError)
                        {
                            fMinError = fError;
                            nMinIndex = j;
                            nMinSqrDist = 
                                (nDx+pDx[j]) * (nDx+pDx[j]) + (nDy+pDy[j]) * (nDy+pDy[j]);
                            nMinSumPmd1 = nSumPmd1;
                            nMinSumPmd2 = nSumPmd2;
                        }
                        else if (fError == fMinError)
                        {
                            int nSqrDist = 
                                (nDx+pDx[j]) * (nDx+pDx[j]) + (nDy+pDy[j]) * (nDy+pDy[j]);

                            if (nSqrDist < nMinSqrDist)
                            {
                                fMinError = fError;
                                nMinIndex = j;
                                nMinSqrDist = nSqrDist;
                                nMinSumPmd1 = nSumPmd1;
                                nMinSumPmd2 = nSumPmd2;
                            }
                        }
                    }

                    nDx += pDx[nMinIndex];
                    nDy += pDy[nMinIndex];                
                }
                else    // (nVx, nVy) or (nUx, nUy) is outside of pyramid image
                {
                    bFeatureOK = FALSE;
                    break;
                }
            }
        }

        if (bFeatureOK)
        {
            pFeatures2[i].x = pFeatures1[i].x + nDx;
            pFeatures2[i].y = pFeatures1[i].y + nDy;

            float illumRatio = 1;
            if (nMinSumPmd1 > 0)
            {
                illumRatio = (float)nMinSumPmd2 / nMinSumPmd1;
            }

            pFeatures2[i].w = illumRatio;

            //review : if (bComputeSSDError) block maybe moved out
            if (bComputeSSDError)
            {
                int cx1 = (int)(pFeatures1[i].x);
                int cy1 = (int)(pFeatures1[i].y);
                int cx2 = (int)(pFeatures2[i].x);
                int cy2 = (int)(pFeatures2[i].y);

                pPmd1Ptr = pPmdImage1[0] + (cy1 - nhWinSize) * nWidth + cx1;
                pPmd2Ptr = pPmdImage2[0] + (cy2 - nhWinSize) * nWidth + cx2;

                float ssd_err = 0;
                int ssd_count = 0;
                for (int y=-nhWinSize; y<=nhWinSize; y++, pPmd1Ptr+=nWidth, pPmd2Ptr+=nWidth)
                {
                    for (int x=-nhWinSize; x<=nhWinSize; x++)
                    {
                        if (x+cx1 >=0 && x+cx1 < nWidth && y+cy1 >=0 && y+cy1 < nHeight &&
                            x+cx2 >=0 && x+cx2 < nWidth && y+cy2 >=0 && y+cy2 < nHeight)
                        {
                            float r = pPmd2Ptr[x] - illumRatio * pPmd1Ptr[x];
                            ssd_err += r*r;
                            ssd_count++;
                        }                    
                    }
                }

                if (ssd_count > 0)
                {
                    pFeatures2[i].w = expf(-sqrt(ssd_err * fErrorSigma));
                }
                else
                {
                    pFeatures2[i].w = 0;
                }
            }
        }
        else
        {
            pFeatures2[i].x = -1;
            pFeatures2[i].y = -1;
            pFeatures2[i].w = 0;
        }
    }    

    return TRUE;
}