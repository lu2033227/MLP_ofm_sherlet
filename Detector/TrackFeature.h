
#pragma once
#include "ImageUtil.h"
#include "Common.h"
#include "AAMResult.h"
#include <string.h>

class CTrackContext;
typedef struct tagMESHCENTERINDEX
{
    float x;
    float y;
    float z;
    int index;
}MESHCENTERINDEX, *PMESHCENTERINDEX;

class CIntegralImageRect
{
public:
    CIntegralImageRect(){ s = NULL; rowptrs = NULL; psumImg = NULL; };
    ~CIntegralImageRect(){
        if (s) delete [] s;
        if (rowptrs) delete [] rowptrs;
    };

    int* psumImg;
    int  tnImageWidth;
    int  tnImageHeight;
    int*  s;                    // size: width + 1
    int** rowptrs;              // size: height + 1

    int Area(int x0, int y0, int x1, int y1) const 
    {
        x1++;
        y1++;

        const int* py0 = rowptrs[y0];
        const int* py1 = rowptrs[y1];
        return (py0[x0] + py1[x1] - py0[x1] - py1[x0]);
    }

    int Area_NoAddition(int x0, int y0, int x1, int y1) const 
    {
        const int* py0 = rowptrs[y0];
        const int* py1 = rowptrs[y1];
        return (py0[x0] + py1[x1] - py0[x1] - py1[x0]);
    }

    void SetSumImage(int* sumImg, const int nWidth, const int nHeight)
    {
        psumImg = sumImg;
        tnImageWidth  = nWidth + 1;
        tnImageHeight = nHeight + 1;

        if (s) delete [] s;
        if (rowptrs) delete [] rowptrs;
        s = new int[nWidth + 1];
        rowptrs = new int*[nHeight + 1];

        memset(sumImg,0, sizeof(int) * tnImageWidth * tnImageHeight);

        int* psum = psumImg;

        for (int y = 0; y < tnImageHeight; y++)
        {
            *psum = 0;
            rowptrs[y] = psum;
            psum += tnImageWidth;
        }
    }

    void CalcSumImage(
        const short* diffImg1,
        const short* diffImg2,
        const int nWidth,
        const int nHeight,
        W_Rect* pRect=NULL)
    {
        memset(s, 0, sizeof(int) * tnImageWidth);

        int nLeft, nTop, nRight, nBottom;

        if (pRect == NULL)
        {
            nLeft   = 0;
            nTop    = 0;
            nRight  = nWidth;
            nBottom = nHeight;  
        }
        else
        {
            nLeft   = pRect->X;
            nTop    = pRect->Y;
            nRight  = pRect->X + pRect->Width;
            nBottom = pRect->Y + pRect->Height;  
        }

        if (diffImg2 != NULL)
        {
            const short* pImg1 = diffImg1; 
            const short* pImg2 = diffImg2; 

            pImg1 += nTop * nWidth;
            pImg2 += nTop * nWidth;

            int* pSum = psumImg + (nTop + 1) * tnImageWidth;
            int* pSum_1 = pSum + 1;

            for (int y = nTop; y < nBottom; y++)
            {
                int* pS = s+nLeft;

                const short* pImgRow1 = pImg1 + nLeft;
                const short* pImgRow2 = pImg2 + nLeft;
                int* pSumTemp  = pSum   + nLeft;
                int* pSumTemp1 = pSum_1 + nLeft;

                for (int x = nLeft; x < nRight; x++)            
                {
                    *pS += (*pImgRow1++) * (*pImgRow2++);                    
                    *pSumTemp1++ = (*pSumTemp++) + *pS++;
                }

                pSum += tnImageWidth;
                pSum_1 = pSum + 1;

                pImg1 += nWidth;        
                pImg2 += nWidth;               
            }

        }
        else
        {
            const short* pImg1 = diffImg1; 
            pImg1 += nTop * nWidth;

            int* pSum = psumImg + (nTop + 1) * tnImageWidth;
            int* pSum_1 = pSum + 1;

            for (int y = nTop; y < nBottom; y++)
            {
                int* pS = s+nLeft;

                const short* pImgRow1 = pImg1 + nLeft;

                int* pSumTemp  = pSum   + nLeft;
                int* pSumTemp1 = pSum_1 + nLeft;

                for (int x = nLeft; x < nRight; x++)            
                {
                    *pS         += (*pImgRow1) * (*pImgRow1++);                    
                    *pSumTemp1++ = (*pSumTemp++) + (*pS++);
                }

                pSum += tnImageWidth;
                pSum_1 = pSum + 1;

                pImg1 += nWidth;        
            }
        }
    }
};


class CTrackFeature
{
public:
    CTrackFeature();
    ~CTrackFeature();


    // Attributes
public:
    CTrackContext  *tpContext  ;   // just a pointer, not allocate memory
    bool            tbIncFrame ;   //

private:
    bool            tbInit;
    bool            tbEnableRejectFeatureTrick;

    int             tnPmdLevels;                  //auto compute
    int             tnPmdSizeTotal;
    int             tnWidth, tnHeight;
    int             tnMaxFeatures;
    int             tnWinSize;

    BYTE          **tpPrevPmdImage;               // previous image pyramid
    BYTE          **tpCurrPmdImage;               // current image pyramid

    IMAGEFEATURE   *tpPrevFeatures;               // image features in previous image
    IMAGEFEATURE   *tpNewFeatures;                // Temporal variable for feature selection

    short          *tpPrevGradX;                  // x-gradient of previous image
    short          *tpPrevGradY;                  // y-gradient of previous image
    int            *tpBufForSelectGoodFeature;
    bool           *tpFeatureMap;                 // Temporal variable for feature selection

    CIntegralImageRect m_InteGxx;
    CIntegralImageRect m_InteGyy;
    CIntegralImageRect m_InteGxy;

    int* tpImgGxx;
    int* tpImgGyy;
    int* tpImgGxy;

    // fyang: temp buffer, can be replaced by local variable
    int*  tpFeatureScore;
    bool* tpFeatureMask;

    // mingcz
    BYTE            *tpHistEqImage     ;   // for save Histogram Equalized image


    // Operations
public:
    bool Init(const int nWidth, const int nHeight, const int nMaxFeatures, const int nWinSize = 7);

    bool SelectFeatures(
        const BYTE *pImage,
        IMAGEFEATURE *pFeatures,
        const W_Rect& rcFace,
        const int nWinDetect = 7,
        const int nFeatureSeparation = 10,
        const int nMinVal = 20,
        const int nSpeedUp = 1,
        const BYTE *pMaskImage = NULL);

    bool TrackFeatures(
        const BYTE *pPrevImage,
        const BYTE *pCurrImage,
        const IMAGEFEATURE *pPrevFeatures,
        IMAGEFEATURE *pCurrFeatures,
        const bool bFloatFeature,
        const W_Point& ptShift);

protected:
    void Destroy();

    int SelectGoodFeature_NSort(
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
        const int nSpeedUp);
   
    int AddUniformFeature(
        bool* pFeatureMap,
        const BYTE* pMaskImage,
        const int* pFeatureScore,
        const W_Rect& rcFace,
        const int nCurrFeatureCount);

    bool FeatureBlockMatchingIllum_Old(
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
        const W_Point& ptShift);

};
