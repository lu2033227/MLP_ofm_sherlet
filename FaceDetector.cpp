//#include "stdafx.h"

#include "FaceDetector.h"


CObjectDetector::CObjectDetector(void)
{  
	m_uGrayImg = NULL;
	SetSrcImgSizeEx();
}

CObjectDetector::~CObjectDetector(void)
{  
	if(m_uGrayImg !=NULL)
	{
		delete []m_uGrayImg;
		m_uGrayImg = NULL;
	}
}

int CObjectDetector::is_equalEx( const FastRect * r1, const FastRect* r2)
{	
	int distance = (int)(r1->width*0.2);	

	return r2->x <= r1->x + distance &&
		r2->x >= r1->x - distance &&
		r2->y <= r1->y + distance &&
		r2->y >= r1->y - distance &&
		r2->width <= (int)( r1->width * 1.3 ) &&
		(int)( r2->width * 1.3 ) >= r1->width;
}


int CObjectDetector::RectClusterEx()
{
	std::vector<FastRect> tempNode[1000];
	int nClassSize=0;
	bool bIsClass=false;
	for (unsigned i_re=0;i_re<fastfacerect.size();i_re++)
	{
		bIsClass=false;
		for (int i_class=0;i_class<nClassSize;i_class++)
		{
			for (unsigned i_node=0;i_node<tempNode[i_class].size();i_node++)
			{
				if(is_equalEx(&fastfacerect.at(i_re),&tempNode[i_class].at(i_node)))
				{
					tempNode[i_class].push_back(fastfacerect.at(i_re));
					fastfacerect.at(i_re).nClassIndex=i_class;
					bIsClass=true;
					break;
				}
			}
			if(bIsClass)
				break;
		}
		if(!bIsClass)
		{
			tempNode[nClassSize].push_back(fastfacerect.at(i_re));
			fastfacerect.at(i_re).nClassIndex=nClassSize;
			nClassSize++;			
		}
	}
	return nClassSize; 
}

void CObjectDetector::InitFaceXStructEx(void* p)
{   
	if(LoadFaceStruct(p))
	{
		bIsLoad = true;
	}else{
		bIsLoad = false;
	}
}

void CObjectDetector::ClearFaceXStructEx()
{
	if(bIsLoad)
	{
		DeleteFaceStruct();
	}
}

void CObjectDetector::FindFaceEx(int *fIntegral,int *fSquareIntegral,int *Integralskinmask)
{
	ResetEx();

	int nModelLevel = (3);

	float factor = (2.0f);
	do{ 

		float const ystep = MAX(2, factor );
		float const xstep = (3);

		int nSample_w = ArrayModelWidth[nModelLevel];
		int nSample_h = ArrayModelHeight[nModelLevel];

		int stopHeight = (int)((HaarImg_H -nSample_h - ystep - (10)*2) / ystep);
		int stopWidth  = (int)((HaarImg_W -nSample_w - xstep - (10)*2) / xstep);  

		int nImagebyteNum = nSample_h*nSample_w;
		maskthreshold = (int)(nImagebyteNum*0.2*255); 
		float nImagebyteNum_1=1.0F/(nImagebyteNum);
		//////////////////////////////////////////////////////////////////////////
		int	*pox2			= TwoRect[nModelLevel].pCoordinateX;
		int *poy2			= TwoRect[nModelLevel].pCoordinateY;
		float *ar2			= TwoRect[nModelLevel].pblockSize;


		int *pox3			= ThreeRect[nModelLevel].pCoordinateX;
		int *poy3			= ThreeRect[nModelLevel].pCoordinateY;
		float *ar3			= ThreeRect[nModelLevel].pblockSize;



		int *pox4			= FourRect[nModelLevel].pCoordinateX;
		int *poy4			= FourRect[nModelLevel].pCoordinateY;
		float *ar4			= FourRect[nModelLevel].pblockSize;


		//////////////////////////////////////////////////////////////////////////
		int startx,starty,endx,endy,startx_1;

		for (int yInt = 1; yInt < stopHeight; yInt++)
		{
			starty = (int)(yInt*ystep)+(10);

			for (int xInt = 1; xInt < stopWidth; xInt++)
			{
				startx = (int)(xInt*xstep)+(10);
				startx_1 = startx-1;
				int flag = 1;

				endx = startx+nSample_w-1;
				endy = starty+nSample_h-1;

				int t1 = tempw_y[starty-1];
				int t2 = tempw_y[endy];
				IntegralRect[0]	  = fIntegral[t1+startx_1];
				SqIntegralRect[0] = fSquareIntegral[t1+startx_1];
				IntegralRect[1]   = fIntegral[t2+startx-1];
				SqIntegralRect[1] = fSquareIntegral[t2+startx_1];
				IntegralRect[2]   = fIntegral[t1+endx];
				SqIntegralRect[2] = fSquareIntegral[t1+endx];
				IntegralRect[3]   = fIntegral[t2+endx];
				SqIntegralRect[3] = fSquareIntegral[t2+endx];

				if (Integralskinmask != NULL) 
				{
					int *mask =Integralskinmask;
					int MaskIntegralRect[4];
					MaskIntegralRect[0]=mask[t1+startx_1];
					MaskIntegralRect[1]=mask[t2+startx_1];
					MaskIntegralRect[2]=mask[t1+endx];
					MaskIntegralRect[3]=mask[t2+endx];
					int maskblocksize=MaskIntegralRect[3]+MaskIntegralRect[0]-MaskIntegralRect[2]-MaskIntegralRect[1];
					if(maskblocksize<maskthreshold)
						continue ;
				}

				fMean = (float)(IntegralRect[3]+IntegralRect[0]-IntegralRect[1]-IntegralRect[2]);
				fVar = sqrt((SqIntegralRect[3]+SqIntegralRect[0]-SqIntegralRect[1]-SqIntegralRect[2])*float(nImagebyteNum) - fMean*fMean)*nImagebyteNum_1;

				float fVvar=1/fVar;

				int i_select,i_stage;

				//分配临时样本记录空间，以存放单个特征对每个样本所计算的结果
				float TempStageResult_Pos;
				float TempResult_Pos;


				int	*pos_x2			= pox2;
				int *pos_y2			= poy2;
				float *area2		= ar2;
				int *sign2			= TwoRect_Sign;
				float *alpha2		= TwoRect_Alpha;
				float *threshold2	= TwoRect_Threshold;

				int *pos_x3			= pox3;
				int *pos_y3			= poy3;
				float *area3		= ar3;
				int *sign3			= ThreeRect_Sign;
				float *alpha3		= ThreeRect_Alpha;
				float *threshold3	= ThreeRect_Threshold;


				int *pos_x4			= pox4;
				int *pos_y4			= poy4;
				float *area4		= ar4;
				int *sign4			= FourRect_Sign;
				float *alpha4		= FourRect_Alpha;
				float *threshold4	= FourRect_Threshold;


				//对于每个feature计算；注意：循环嵌套顺序优化过，结果没有区别
				for (i_stage=0; i_stage< tnStageNum; i_stage++)
				{	
					TempStageResult_Pos=0;

					//  2个特征块
					int nTwoFeatureNum = TwoRect_WeakClassifierOfStage[i_stage];
					//			if(nTwoFeatureNum>0)
					{

						for (i_select=0;i_select<nTwoFeatureNum;i_select++)
						{
							if(starty+pos_y2[0]<0)
								Integral[0] = 0;
							else
								Integral[0]=fIntegral[ tempw_y[(starty+pos_y2[0])]+startx+pos_x2[0]];
							if(starty+pos_y2[1]<0)
								Integral[1] = 0;
							else
								Integral[1]=fIntegral[ tempw_y[(starty+pos_y2[1])]+startx+pos_x2[1]];
							if(starty+pos_y2[2]<0)
								Integral[2] = 0;
							else
								Integral[2]=fIntegral[ tempw_y[(starty+pos_y2[2])]+startx+pos_x2[2]];

							if(starty+pos_y2[3]<0)
								Integral[3] = 0;
							else
								Integral[3]=fIntegral[ tempw_y[(starty+pos_y2[3])]+startx+pos_x2[3]];
							if(starty+pos_y2[4]<0)
								Integral[4] = 0;
							else
								Integral[4]=fIntegral[ tempw_y[(starty+pos_y2[4])]+startx+pos_x2[4]];
							if(starty+pos_y2[5]<0)
								Integral[5] = 0;
							else
								Integral[5]=fIntegral[ tempw_y[(starty+pos_y2[5])]+startx+pos_x2[5]];


							int block_r=Integral[5]+Integral[2]-Integral[3]-Integral[4];
							int block_l=Integral[3]+Integral[0]-Integral[1]-Integral[2];

							float  aa =(block_r-block_l)*(*area2);
							TempResult_Pos=aa*fVvar;
							if( !( (0 == sign2[i_select])? ( TempResult_Pos <= threshold2[i_select]):( TempResult_Pos > threshold2[i_select]) ) )
							{
								TempStageResult_Pos += alpha2[i_select];
							}
							pos_x2 += 6;
							pos_y2 += 6;
							area2++;
						}
						alpha2 += nTwoFeatureNum;
						sign2  += nTwoFeatureNum;
						threshold2 += nTwoFeatureNum;
					}


					//  3个特征块
					int nThreeFeatureNum=ThreeRect_WeakClassifierOfStage[i_stage];
					//			if(nThreeFeatureNum>0)
					{
						for (i_select=0;i_select< nThreeFeatureNum;i_select++)
						{
							if(starty+pos_y3[0]<0)
								Integral[0] = 0;
							else
								Integral[0]=fIntegral[ tempw_y[(starty+pos_y3[0])]+startx+pos_x3[0]];
							if(starty+pos_y3[1]<0)
								Integral[1] = 0;
							else
								Integral[1]=fIntegral[ tempw_y[(starty+pos_y3[1])]+startx+pos_x3[1]];
							if(starty+pos_y3[2]<0)
								Integral[2] = 0;
							else
								Integral[2]=fIntegral[ tempw_y[(starty+pos_y3[2])]+startx+pos_x3[2]];
							if(starty+pos_y3[3]<0)
								Integral[3] = 0;
							else
								Integral[3]=fIntegral[ tempw_y[(starty+pos_y3[3])]+startx+pos_x3[3]];

							if(starty+pos_y3[4]<0)
								Integral[4] = 0;
							else
								Integral[4]=fIntegral[ tempw_y[(starty+pos_y3[4])]+startx+pos_x3[4]];

							if(starty+pos_y3[5]<0)
								Integral[5] = 0;
							else
								Integral[5]=fIntegral[ tempw_y[(starty+pos_y3[5])]+startx+pos_x3[5]];

							if(starty+pos_y3[6]<0)
								Integral[6] = 0;
							else
								Integral[6]=fIntegral[ tempw_y[(starty+pos_y3[6])]+startx+pos_x3[6]];

							if(starty+pos_y3[7]<0)
								Integral[7] = 0;
							else
								Integral[7]=fIntegral[ tempw_y[(starty+pos_y3[7])]+startx+pos_x3[7]];


							int block_r=Integral[7]+Integral[4]-Integral[5]-Integral[6];
							int block_l=Integral[3]+Integral[0]-Integral[1]-Integral[2];
							int block_m=Integral[5]+Integral[2]-Integral[3]-Integral[4];
							float  aa =(block_m*2-block_r-block_l)*(*area3);

							TempResult_Pos=aa*fVvar;
							if( !( (0 == sign3[i_select])? ( TempResult_Pos <= threshold3[i_select]):( TempResult_Pos > threshold3[i_select]) ) )
							{
								TempStageResult_Pos += alpha3[i_select];
							}
							pos_x3 += 8;
							pos_y3 += 8;
							area3++;
						}
						alpha3 += nThreeFeatureNum;
						sign3  += nThreeFeatureNum;
						threshold3 += nThreeFeatureNum;
					}
					////  4个特征块
					int nFourFeatureNum=FourRect_WeakClassifierOfStage[i_stage];
					{
						for (i_select=0;i_select<nFourFeatureNum;i_select++)
						{
							if(starty+pos_y4[0]<0)
								Integral[0] = 0;
							else
								Integral[0]=fIntegral[ tempw_y[(starty+pos_y4[0])]+startx+pos_x4[0]];
							if(starty+pos_y4[1]<0)
								Integral[1] = 0;
							else
								Integral[1]=fIntegral[ tempw_y[(starty+pos_y4[1])]+startx+pos_x4[1]];
							if(starty+pos_y4[2]<0)
								Integral[2] = 0;
							else
								Integral[2]=fIntegral[ tempw_y[(starty+pos_y4[2])]+startx+pos_x4[2]];
							if(starty+pos_y4[3]<0)
								Integral[3] = 0;
							else
								Integral[3]=fIntegral[ tempw_y[(starty+pos_y4[3])]+startx+pos_x4[3]];
							if(starty+pos_y4[4]<0)
								Integral[4] = 0;
							else
								Integral[4]=fIntegral[ tempw_y[(starty+pos_y4[4])]+startx+pos_x4[4]];
							if(starty+pos_y4[5]<0)
								Integral[5] = 0;
							else
								Integral[5]=fIntegral[ tempw_y[(starty+pos_y4[5])]+startx+pos_x4[5]];
							if(starty+pos_y4[6]<0)
								Integral[6] = 0;
							else
								Integral[6]=fIntegral[ tempw_y[(starty+pos_y4[6])]+startx+pos_x4[6]];
							if(starty+pos_y4[7]<0)
								Integral[7] = 0;
							else
								Integral[7]=fIntegral[ tempw_y[(starty+pos_y4[7])]+startx+pos_x4[7]];
							if(starty+pos_y4[8]<0)
								Integral[8] = 0;
							else
								Integral[8]=fIntegral[ tempw_y[(starty+pos_y4[8])]+startx+pos_x4[8]];



							int block_l_u=Integral[4]+Integral[0]-Integral[3]-Integral[1];
							int block_l_d=Integral[5]+Integral[1]-Integral[2]-Integral[4];
							int block_r_u=Integral[7]+Integral[3]-Integral[4]-Integral[6];
							int block_r_d=Integral[8]+Integral[4]-Integral[5]-Integral[7];				

							float  aa =(block_r_u+block_l_d-block_l_u-block_r_d)*(*area4);
							TempResult_Pos=aa*fVvar;

							if( !( (0 == sign4[i_select])? ( TempResult_Pos <= threshold4[i_select]):( TempResult_Pos > threshold4[i_select]) ) )
							{
								TempStageResult_Pos+=alpha4[i_select];
							}
							pos_x4 += 9;
							pos_y4 += 9;
							area4++;
						}
						alpha4 += nFourFeatureNum;
						sign4  += nFourFeatureNum;
						threshold4 += nFourFeatureNum;
					}

					if(TempStageResult_Pos > pStageThreashold[i_stage])
					{
						flag = 0;
						break;
					}

				}

				if(flag == 1)
				{
					FastRect r;
					r.x = startx;
					r.y = starty;
					r.width  = nSample_w;
					r.height = nSample_h;
					fastfacerect.push_back(r);
				}
			}
		}

		nModelLevel++;

		factor =factor * (1.25f);

	} while(ArrayModelWidth[nModelLevel]<(HaarImg_W-(10))&&ArrayModelHeight[nModelLevel]<(HaarImg_H-(10)));


	//=============聚类==========
	int mm=RectClusterEx();
	if(mm>0)
	{
		//=============统计个数======
		int *sumfacenum;
		sumfacenum=new int[mm];
		memset(sumfacenum,0,mm*sizeof(int));
		FastRect *temprect;
		temprect=new FastRect [mm];
		memset(temprect,0,mm*sizeof(FastRect));

		for (unsigned i=0;i<fastfacerect.size();i++)
		{
			sumfacenum[fastfacerect.at(i).nClassIndex]++;
			temprect[fastfacerect.at(i).nClassIndex].x+=fastfacerect.at(i).x;
			temprect[fastfacerect.at(i).nClassIndex].y+=fastfacerect.at(i).y;
			temprect[fastfacerect.at(i).nClassIndex].width+=fastfacerect.at(i).width;
			temprect[fastfacerect.at(i).nClassIndex].height+=fastfacerect.at(i).height;
		}


		//=============计算平均宽度================
		for (int i=0;i<mm;i++)
		{
			int nn=sumfacenum[i];
			if(nn>6)
			{
				RECT re;
				re.left	=	(LONG)((temprect[i].x*2 + nn)/(2*nn)*m_scale);
				re.top	=	(LONG)((temprect[i].y*2 + nn)/(2*nn)*m_scale);
				re.right =	(LONG)(re.left+(temprect[i].width*2 + nn)/(2*nn)*m_scale);
				re.bottom=	(LONG)(re.top +(temprect[i].height*2 + nn)/(2*nn)*m_scale);
				facerect.push_back(re);
			}
		}

		delete []sumfacenum;
		delete []temprect;

	}
}

void CObjectDetector::SetSrcImgSizeEx(int img_w/* =320 */, int img_h/* =240 */,float downsamplescale/* =2::5f */)
{


	m_inputimg_w = img_w;
	m_inputimg_h = img_h;
	HaarImg_W = int(m_inputimg_w/downsamplescale);
	HaarImg_H = int(m_inputimg_h/downsamplescale);
	m_scale   = downsamplescale;
	scale     = int(downsamplescale);

	if (m_uGrayImg != NULL)
	{
		delete []m_uGrayImg;
		m_uGrayImg = NULL;
	}
	int size = HaarImg_H*HaarImg_W;
	m_uGrayImg = new unsigned char [HaarImg_H*sizeof(int)+size*(sizeof(unsigned char)*(1+1+3)+sizeof(int)*3)];
	m_Skinmask = m_uGrayImg+size;
	m_ColorImg = m_Skinmask+size;
	tfIntegral = (int*)(m_ColorImg+size*3);
	tfSquareIntegral = tfIntegral+size;
	tfColorIntegral = tfSquareIntegral+size;
	tempw_y = tfColorIntegral+size;
	for(int i=0;i<HaarImg_H;i++)
	{
		tempw_y[i] = HaarImg_W*i;
	}
	ResetEx();
	return;

}

void CObjectDetector::FaceDetectEx(unsigned char *uGrayInputImg,unsigned char *uColorInputImg)
{
	if(!bIsLoad)
		return;

	int left  = 0;
	for (int i=0;i<HaarImg_H;i++)
	{

		for (int j=0;j<HaarImg_W;j++)
		{
			m_uGrayImg[left+j]   =uGrayInputImg[(int(m_scale*i))*m_inputimg_w+int(m_scale*j)];				
		}
		left  += HaarImg_W;
	}
	CalculateIntegral(m_uGrayImg,HaarImg_W,HaarImg_H,tfIntegral);
	CalculateSquareIntegral(m_uGrayImg,HaarImg_W,HaarImg_H,tfSquareIntegral);

	FindFaceEx(tfIntegral,tfSquareIntegral,0);

}

void CObjectDetector::ResetEx()
{
	facerect.clear();
	fastfacerect.clear();
}

bool CObjectDetector::LoadFaceStruct(void *pModelData)
{
	int offset = 0;
	memcpy(&tnStageNum, (char *)pModelData+offset,sizeof(int));	
	offset += sizeof(int);
	memcpy(&tnTwoRectNum,(char *)pModelData+offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&tnThreeRectNum,(char *)pModelData+offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&tnFourRectNum,(char *)pModelData+offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&ArrayModelWidth[0],(char *)pModelData+offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&ArrayModelHeight[0],(char *)pModelData+offset,sizeof(int));
	offset += sizeof(int);



	memcpy(pStageThreashold,(char *)pModelData+offset,sizeof(float)*tnStageNum);
	offset += sizeof(float)*tnStageNum;
	memcpy(TwoRect_WeakClassifierOfStage,(char *)pModelData+offset,sizeof(int)*tnStageNum);
	offset += sizeof(int)*tnStageNum;
	memcpy(ThreeRect_WeakClassifierOfStage,(char *)pModelData+offset,sizeof(float)*tnStageNum);
	offset += sizeof(int)*tnStageNum;
	memcpy(FourRect_WeakClassifierOfStage,(char *)pModelData+offset,sizeof(int)*tnStageNum);
	offset += sizeof(int)*tnStageNum;



	TwoRect_Alpha		= new float[tnTwoRectNum]; //t2a   
	TwoRect_Threshold	= new float[tnTwoRectNum]; //t2t
	TwoRect_Sign		= new   int[tnTwoRectNum]; //t2s

	ThreeRect_Alpha		= new float[tnThreeRectNum]; //t3a   
	ThreeRect_Threshold	= new float[tnThreeRectNum]; //t3t
	ThreeRect_Sign		= new   int[tnThreeRectNum]; //t3s

	FourRect_Alpha		= new float[tnFourRectNum]; //t4a   
	FourRect_Threshold	= new float[tnFourRectNum]; //t4t
	FourRect_Sign		= new   int[tnFourRectNum]; //t4s


	memcpy(TwoRect_Alpha,(char *)pModelData+offset,sizeof(float)*tnTwoRectNum);
	offset += sizeof(float)*tnTwoRectNum;
	memcpy(TwoRect_Threshold,(char *)pModelData+offset,sizeof(float)*tnTwoRectNum);
	offset += sizeof(float)*tnTwoRectNum;
	memcpy(TwoRect_Sign,(char *)pModelData+offset,sizeof(int)*tnTwoRectNum);
	offset += sizeof(int)*tnTwoRectNum;

	memcpy(ThreeRect_Alpha,(char *)pModelData+offset,sizeof(float)*tnThreeRectNum);
	offset += sizeof(float)*tnThreeRectNum;
	memcpy(ThreeRect_Threshold,(char *)pModelData+offset,sizeof(float)*tnThreeRectNum);
	offset += sizeof(float)*tnThreeRectNum;
	memcpy(ThreeRect_Sign,(char *)pModelData+offset,sizeof(int)*tnThreeRectNum);
	offset += sizeof(int)*tnThreeRectNum;

	memcpy(FourRect_Alpha,(char *)pModelData+offset,sizeof(float)*tnFourRectNum);
	offset += sizeof(float)*tnFourRectNum;
	memcpy(FourRect_Threshold,(char *)pModelData+offset,sizeof(float)*tnFourRectNum);
	offset += sizeof(float)*tnFourRectNum;
	memcpy(FourRect_Sign,(char *)pModelData+offset,sizeof(int)*tnFourRectNum);
	offset += sizeof(int)*tnFourRectNum;


	TwoRect[0].pCoordinateX = new int[6*tnTwoRectNum]; //t2x
	TwoRect[0].pCoordinateY = new int[6*tnTwoRectNum]; //t2y

	ThreeRect[0].pCoordinateX = new int[8*tnThreeRectNum]; //t3x
	ThreeRect[0].pCoordinateY = new int[8*tnThreeRectNum]; //t3y

	FourRect[0].pCoordinateX = new int[9*tnFourRectNum];   //t4x
	FourRect[0].pCoordinateY = new int[9*tnFourRectNum];   //t4y

	memcpy(TwoRect[0].pCoordinateX,(char *)pModelData+offset,6*tnTwoRectNum*sizeof(int));
	offset += 6*tnTwoRectNum*sizeof(int);
	memcpy(TwoRect[0].pCoordinateY,(char *)pModelData+offset,6*tnTwoRectNum*sizeof(int));
	offset += 6*tnTwoRectNum*sizeof(int);

	memcpy(ThreeRect[0].pCoordinateX,(char *)pModelData+offset,8*tnThreeRectNum*sizeof(int));
	offset += 8*tnThreeRectNum*sizeof(int);
	memcpy(ThreeRect[0].pCoordinateY,(char *)pModelData+offset,8*tnThreeRectNum*sizeof(int));
	offset += 8*tnThreeRectNum*sizeof(int);

	memcpy(FourRect[0].pCoordinateX,(char *)pModelData+offset,9*tnFourRectNum*sizeof(int));
	offset += 9*tnFourRectNum*sizeof(int);
	memcpy(FourRect[0].pCoordinateY,(char *)pModelData+offset,9*tnFourRectNum*sizeof(int));
	//offset += 9*tnFourRectNum*sizeof(int);




	//=====================初始化第一采样层的面积参数===========================

	{

		TwoRect[0].pblockSize = new float[tnTwoRectNum];   //t2b
		ThreeRect[0].pblockSize = new float[tnThreeRectNum];   //t3b
		FourRect[0].pblockSize = new float[tnFourRectNum];     //t4b


		int		*t2x = TwoRect[0].pCoordinateX;
		int		*t2y = TwoRect[0].pCoordinateY;
		float	*t2b = TwoRect[0].pblockSize;

		int		*t3x = ThreeRect[0].pCoordinateX;
		int		*t3y = ThreeRect[0].pCoordinateY;
		float	*t3b = ThreeRect[0].pblockSize;

		int		*t4x = FourRect[0].pCoordinateX;
		int		*t4y = FourRect[0].pCoordinateY;
		float	*t4b = FourRect[0].pblockSize;



		for (int i=0; i<tnStageNum; i++) 
		{ 
			//two rectangle
			int t2 = TwoRect_WeakClassifierOfStage[0]; 
			for (int k=0; k<t2; k++) 
			{
				*(t2b++) = 1/(float)((*(t2x + 3)-*(t2x))*(*(t2y+3)-*(t2y)));
				//*(t2b++) = 1/(float)((*(t2x + 5)-*(t2x+2))*(*(t2y+5)-*(t2y+2)));
				t2x += 6;
				t2y += 6;
			}

			//////////////////////////////////////////////////////////////////////////
			//three rectangle
			int t3 = ThreeRect_WeakClassifierOfStage[i];		 
			for (int k=0; k<t3; k++) 
			{  
				*(t3b++) = 1/(float)((*(t3x+7)-*(t3x+4))*(*(t3y+7)-*(t3y+4)));				
				t3x += 8;
				t3y += 8;
			}


			//////////////////////////////////////////////////////////////////////////
			//four rectangle
			int t4 = FourRect_WeakClassifierOfStage[i];	
			for (int k=0; k<t4; k++) 
			{
				*(t4b++) =1/(float) ((*(t4x+4)-*(t4x))*(*(t4y+4)-*(t4y)));

				t4x += 9;
				t4y += 9;
			}

		} // end for tnStageNum 

	}


	for (int j=1; j<FACEDETECT_MAXZOOMLEVEL; j++) 
	{  

		float ratio = pow((1.25f),j);
		ArrayModelHeight[j] = int(ratio*ArrayModelHeight[0]);
		ArrayModelWidth[j]	= int(ratio*ArrayModelWidth[0]);


		TwoRect[j].pCoordinateX = new int[6*tnTwoRectNum]; //t2x
		TwoRect[j].pCoordinateY = new int[6*tnTwoRectNum]; //t2y
		TwoRect[j].pblockSize = new float[tnTwoRectNum];   //t2b


		ThreeRect[j].pCoordinateX = new int[8*tnThreeRectNum]; //t3x
		ThreeRect[j].pCoordinateY = new int[8*tnThreeRectNum]; //t3y
		ThreeRect[j].pblockSize = new float[tnThreeRectNum];   //t3b


		FourRect[j].pCoordinateX = new int[9*tnFourRectNum];   //t4x
		FourRect[j].pCoordinateY = new int[9*tnFourRectNum];   //t4y
		FourRect[j].pblockSize = new float[tnFourRectNum];     //t4b


		int		*t2x = TwoRect[j].pCoordinateX;
		int		*t2y = TwoRect[j].pCoordinateY;
		float	*t2b = TwoRect[j].pblockSize;

		int		*t3x = ThreeRect[j].pCoordinateX;
		int		*t3y = ThreeRect[j].pCoordinateY;
		float	*t3b = ThreeRect[j].pblockSize;

		int		*t4x = FourRect[j].pCoordinateX;
		int		*t4y = FourRect[j].pCoordinateY;
		float	*t4b = FourRect[j].pblockSize;

		int *temppointer_x2 = TwoRect[j].pCoordinateX;
		int *temppointer_y2 = TwoRect[j].pCoordinateY;
		int *temppointer_x3 = ThreeRect[j].pCoordinateX;
		int *temppointer_y3 = ThreeRect[j].pCoordinateY;
		int *temppointer_x4 = FourRect[j].pCoordinateX;
		int *temppointer_y4 = FourRect[j].pCoordinateY;

		int *t2x_0 = TwoRect[0].pCoordinateX;
		int *t2y_0 = TwoRect[0].pCoordinateY;
		int *t3x_0 = ThreeRect[0].pCoordinateX;
		int *t3y_0 = ThreeRect[0].pCoordinateY;
		int *t4x_0 = FourRect[0].pCoordinateX;
		int *t4y_0 = FourRect[0].pCoordinateY;

		for (int i=0; i<tnStageNum; i++) 
		{ 
			//two rectangle
			int t2 = TwoRect_WeakClassifierOfStage[i]; 

			for (int k=0; k<t2; k++) 
			{
				int len2x = 0;
				int len2y = 0;
				t2x[0] = int(*(t2x_0)*ratio);
				t2y[0] = int(*(t2y_0)*ratio);
				t2x[1] = int(*(t2x_0+1)*ratio);
				t2y[1] = int(*(t2y_0+1)*ratio);
				t2x[2] = int(*(t2x_0+2)*ratio);
				t2y[2] = int(*(t2y_0+2)*ratio);

				if (t2x[0] == t2x[1]) //横
				{
					len2x = t2x[2]-t2x[0];

					t2x[3] = t2x[2];
					t2x[4] = t2x[2] + len2x;
					t2x[5] = t2x[4];

					t2y[3] = t2y[1];
					t2y[4] = t2y[0];
					t2y[5] = t2y[1];

				}
				else  //竖
				{
					len2y = t2y[2]-t2y[0];

					t2x[3] = t2x[1];
					t2x[4] = t2x[0];
					t2x[5] = t2x[1];

					t2y[3] = t2y[2];
					t2y[4] = t2y[2] + len2y;
					t2y[5] = t2y[4];

				}

				t2x += 6;
				t2y += 6;
				t2x_0 += 6;
				t2y_0 += 6;
			}

			for (int k=0; k<t2; k++) 
			{
				*(t2b++) = 1/(float)((*(temppointer_x2 + 3)-*(temppointer_x2))*(*(temppointer_y2+3)-*(temppointer_y2)));

				temppointer_x2 += 6;
				temppointer_y2 += 6;
			}

			//////////////////////////////////////////////////////////////////////////
			//three rectangle
			int t3 = ThreeRect_WeakClassifierOfStage[i];
			for (int k=0; k<t3; k++) 
			{  

				int len3x = 0;
				int len3y = 0;
				t3x[0] = int(*(t3x_0)*ratio);
				t3y[0] = int(*(t3y_0)*ratio);
				t3x[1] = int(*(t3x_0+1)*ratio);
				t3y[1] = int(*(t3y_0+1)*ratio);
				t3x[2] = int(*(t3x_0+2)*ratio);
				t3y[2] = int(*(t3y_0+2)*ratio);

				if (t3x[0] == t3x[1]) //横
				{
					len3x = t3x[2]-t3x[0];

					t3x[3] = t3x[2];
					t3x[4] = t3x[2] + len3x;
					t3x[5] = t3x[4];
					t3x[6] = t3x[4] + len3x;
					t3x[7] = t3x[6];

					t3y[3] = t3y[1];
					t3y[4] = t3y[0];
					t3y[5] = t3y[1];
					t3y[6] = t3y[0];
					t3y[7] = t3y[1];

				}
				else  //竖
				{
					len3y = t3y[2]-t3y[0];

					t3x[3] = t3x[1];
					t3x[4] = t3x[0];
					t3x[5] = t3x[1];
					t3x[6] = t3x[0];
					t3x[7] = t3x[1];

					t3y[3] = t3y[2];
					t3y[4] = t3y[2] + len3y;
					t3y[5] = t3y[4];
					t3y[6] = t3y[4] + len3y;
					t3y[7] = t3y[6];

				}

				t3x += 8;
				t3y += 8;
				t3x_0 += 8;
				t3y_0 += 8;
			}

			for (int k=0; k<t3; k++) 
			{  
				*(t3b++) = 1/(float)((*(temppointer_x3+7)-*(temppointer_x3+4))*(*(temppointer_y3+7)-*(temppointer_y3+4)));				

				temppointer_x3 += 8;
				temppointer_y3 += 8;
			}


			//////////////////////////////////////////////////////////////////////////
			//four rectangle
			int t4 = FourRect_WeakClassifierOfStage[i];
			for (int k=0; k<t4; k++) 
			{

				int len4x = 0;
				int len4y = 0;
				t4x[0] = int(*(t4x_0)*ratio);
				t4y[0] = int(*(t4y_0)*ratio);
				t4x[1] = int(*(t4x_0+1)*ratio);
				t4y[1] = int(*(t4y_0+1)*ratio);

				t4x[3] = int(*(t4x_0+3)*ratio);
				t4y[3] = int(*(t4y_0+3)*ratio);


				len4x = t4x[3]-t4x[0];
				len4y = t4y[1]-t4y[0];

				t4x[2] = t4x[0];
				t4x[4] = t4x[3];
				t4x[5] = t4x[3];
				t4x[6] = t4x[3] + len4x;
				t4x[7] = t4x[6];
				t4x[8] = t4x[6];

				t4y[2] = t4y[1] + len4y;
				t4y[4] = t4y[1];
				t4y[5] = t4y[2];
				t4y[6] = t4y[0];
				t4y[7] = t4y[1];
				t4y[8] = t4y[2];

				t4x += 9;
				t4y += 9;
				t4x_0 += 9;
				t4y_0 += 9;
			}
			for (int k=0; k<t4; k++) 
			{
				*(t4b++) =1/(float) ((*(temppointer_x4+4)-*(temppointer_x4))*(*(temppointer_y4+4)-*(temppointer_y4)));

				temppointer_x4 += 9;
				temppointer_y4 += 9;
			}

		} // end for tnStageNum

	}//end for MAXZOOMLEVEL

	return 1;
}


void CObjectDetector::DeleteFaceStruct()
{
	for (int j=0; j<FACEDETECT_MAXZOOMLEVEL; j++) 
	{
		delete []TwoRect[j].pCoordinateX; //t2x
		delete []TwoRect[j].pCoordinateY; //t2y
		delete []TwoRect[j].pblockSize;   //t2b


		delete []ThreeRect[j].pCoordinateX; //t3x
		delete []ThreeRect[j].pCoordinateY; //t3y
		delete []ThreeRect[j].pblockSize;   //t3b


		delete []FourRect[j].pCoordinateX;   //t4x
		delete []FourRect[j].pCoordinateY;   //t4y
		delete []FourRect[j].pblockSize;     //t4b
	}

	delete []TwoRect_Sign;
	delete []TwoRect_Alpha;
	delete []TwoRect_Threshold;

	delete []ThreeRect_Sign;
	delete []ThreeRect_Alpha;
	delete []ThreeRect_Threshold;

	delete []FourRect_Sign;
	delete []FourRect_Alpha;  
	delete []FourRect_Threshold;

}


//////////////////////////////////////////////////////////////////////////
CFaceDetector::CFaceDetector()
{
	tnImgWidth  = 320;
	tnImgHeight = 240;
	m_uGrayImg  = new unsigned char [tnImgWidth*tnImgHeight];
	detector.SetSrcImgSizeEx(tnImgWidth,tnImgHeight,2.0f);

}
CFaceDetector::~CFaceDetector()
{
	if(m_uGrayImg != NULL)
	{
		delete []m_uGrayImg;
		m_uGrayImg = NULL;
	}
}

void CFaceDetector::SetImageInfo(int width,int height,float downsamplescale)
{
	tnImgWidth  = width;
	tnImgHeight = height;

	if(m_uGrayImg != NULL)
	{
		delete []m_uGrayImg;
		m_uGrayImg = NULL;
	}

	m_uGrayImg = new unsigned char [tnImgWidth*tnImgHeight];
	m_uGrayImg2 = m_uGrayImg;
	detector.SetSrcImgSizeEx(tnImgWidth,tnImgHeight,downsamplescale);
	return ;
}

void CFaceDetector::SetImageData(unsigned char *inputImg,bool isGray)
{	
	if(inputImg == NULL)
	{
		bHasImg = false;
		return ;
	}
	if(isGray)
	{
		m_uGrayImg2 = inputImg;
	}
	else
	{
		RGBA2GRAY(inputImg,tnImgWidth,tnImgHeight,m_uGrayImg);
		m_uGrayImg2 = m_uGrayImg;
	}
	bHasImg = true;
	return;
}

FaceRectList *CFaceDetector::GetFaceRegion()
{
	if(bHasImg)
	{
		detector.FaceDetectEx(m_uGrayImg2);
		face.FaceNum = (int)detector.facerect.size();
		for (int i=0;i<face.FaceNum;i++)
		{
			face.facerect[i] = detector.facerect.at(i);
		}
		bHasImg = false;
		return &face;
	}
	else
		return NULL;
}

bool CFaceDetector::Init(const char* modelPath)
{                             
	//void *pv = LoadDataFromResource_FD("FaceAam",4608);
	void *pv = LoadDataFromResource_FD(modelPath, 4608);
	if(pv == NULL)
	{
		return false;
	}
	detector.InitFaceXStructEx(pv);
	return true;
}

void CFaceDetector::Release()
{
	detector.ClearFaceXStructEx();
	delete this;
}