//#include "stdafx.h"
#include "FaceTrack.h"
#include "Common.h"


int     g_nFrameIdx     = 0;
int     g_nKeyFrameIdx  = 0;

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

CFaceTrack::CFaceTrack()
{
	tbInit         = FALSE;
	tnFrameIndex   = 0;

	tpPrevImage    = NULL;
	tpCurrImage    = NULL;
	tpWarpImageGrey= NULL;
	tpMaskImage    = NULL;
	tpMeshIndexMap = NULL;

	tpPrevFeatures = NULL;
	tpCurrFeatures = NULL;
	tpBaryCoords   = NULL;
	tpFeaTriMesh   = NULL;
	tpnPreSelFeaIdx= NULL;

	tpFaceDetection    = NULL;
	tpFaceRectList     = NULL;

	tpAAMRlt       = NULL;
	tpAAMRltTmp    = NULL;

	tnMdlIdx       = 1;
	tbNeedOuterInit= true;

}

CFaceTrack::~CFaceTrack()
{
	/*if(adFacedetector != NULL){
		detector_destroy(adFacedetector);
	}*/
	Destroy();
}

void CFaceTrack::Destroy()
{
	if (!tbInit)
		return;

	SAFE_ARRAY_DELETE(m_ObjModel.pVertex);
	SAFE_ARRAY_DELETE(m_ObjModel.pMesh);


	SAFE_ARRAY_DELETE(tpPrevImage);
	SAFE_ARRAY_DELETE(tpWarpImageGrey);
	SAFE_ARRAY_DELETE(tpMaskImage);
	SAFE_ARRAY_DELETE(tpMeshIndexMap);

	SAFE_ARRAY_DELETE(tpnPreSelFeaIdx);

	SAFE_ARRAY_DELETE( m_CandideMdl.tpMSVertex  );
	SAFE_ARRAY_DELETE( m_CandideMdl.tpMSTriMesh );

	SAFE_ARRAY_DELETE( m_CandideMdl.tpiEach_SUVertexNum );
	SAFE_ARRAY_DELETE( m_CandideMdl.tpiEach_SUVertexIdx );
	SAFE_ARRAY_DELETE( m_CandideMdl.tpEach_SUVertexVal  );

	SAFE_ARRAY_DELETE( m_CandideMdl.tpiEach_AUVertexNum );
	SAFE_ARRAY_DELETE( m_CandideMdl.tpiEach_AUVertexIdx );
	SAFE_ARRAY_DELETE( m_CandideMdl.tpEach_AUVertexVal  );

	int     i, j;
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
	{
		for ( j = 0; j < LUTS_2D3DAAM_NUM; j++ )
		{
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpiComTo2DIdx    );
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpiComTo3DIdx    );
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpi2DNeedIntplIdx);
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpi2DIntplIdx    );
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpfl2DIntplCoef  );
			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpi3DToComIdx    );

			SAFE_ARRAY_DELETE( m_LUTsOf2D3DAAM[i][j].tpiMotionConstIdx);
		}
	}
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpSUCoef );            
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpAUCoef ); 

	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tppflMatSD3D[0]  );
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tppflMatSD3D     );

	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpflMatSD3D_T    );   
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpflKeyPoint3D   );
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpfl2DPtsErr     );
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpflHessian3D    );
	SAFE_ARRAY_DELETE( m_VarsOf2D3DAAM.tpflSDTMulErr3D  );


//	if (tpFaceDetection) tpFaceDetection->Release();
	//tpFaceDetection = NULL;

	for ( i = 0; i < AAM_MODEL_NUM; i++ )
		m_AAMIter[i].FreeModelData();

	SAFE_ARRAY_DELETE( tpAAMRlt    );
	SAFE_ARRAY_DELETE( tpAAMRltTmp );

	SAFE_ARRAY_DELETE(tpPrevFeatures);
	SAFE_ARRAY_DELETE(tpBaryCoords);
	SAFE_ARRAY_DELETE(tpFeaTriMesh);

	tbInit = false;
}

bool CFaceTrack::Init(const char* modelPath, const char* detectorPath, const int iWidth, const int iHeight)
{
	if (tbInit) return false;
	int i, j;
	tnFrameIndex   = 0;
	tnWidth        = iWidth;
	tnHeight       = iHeight;


	i = 0;

	void *pv = LoadDataFromResource_FD(modelPath, 119766);
	m_AAMIter[i++].LoadModelData(pv);
	//pv = LoadDataFromResource_FD(modelPath, 2697390);
	pv = LoadDataFromResource_FD(modelPath, 1848366);
	m_AAMIter[i++].LoadModelData(pv);
	//pv = LoadDataFromResource_FD(modelPath, 5245446);
	pv = LoadDataFromResource_FD(modelPath, 3557254);
	m_AAMIter[i++].LoadModelData(pv);

	adFacedetector = detector_create(iWidth, iHeight, 3, detectorPath);

/*	tpFaceDetection = new CFaceDetector;
	if(tpFaceDetection == NULL)
	{
		SAFE_ARRAY_DELETE(m_ObjModel.pVertex);
		SAFE_ARRAY_DELETE(m_ObjModel.pMesh);

		SAFE_ARRAY_DELETE(tpPrevImage);
		SAFE_ARRAY_DELETE(tpWarpImageGrey);
		SAFE_ARRAY_DELETE(tpMaskImage);
		SAFE_ARRAY_DELETE(tpMeshIndexMap);

		SAFE_ARRAY_DELETE(tpnPreSelFeaIdx);

		SAFE_ARRAY_DELETE( tpAAMRlt    );
		SAFE_ARRAY_DELETE( tpAAMRltTmp );

		SAFE_ARRAY_DELETE(tpPrevFeatures);
		SAFE_ARRAY_DELETE(tpBaryCoords);
		SAFE_ARRAY_DELETE(tpFeaTriMesh);

		tbInit = false;
		return FALSE;
	}
	tpFaceDetection->Init(modelPath);
	int minsize = tnWidth<tnHeight?tnWidth:tnHeight;
	float fScale = (1.3f);                                 
	if (minsize >= 1920)         fScale = (6.0f);        
	else if (minsize >= 1024)    fScale = (4.5f);         
	else if (minsize >= 640)     fScale = (3.0f);       
	else if (minsize >= 320)     fScale = (2.0f);   

	tpFaceDetection->SetImageInfo(tnWidth, tnHeight, fScale);  */ 
	LoadCandide3Model(modelPath, m_CandideMdl);

	m_ObjModel.nVertex  = m_CandideMdl.m_iMSVertexNum;
	m_ObjModel.pVertex  = new(std::nothrow) CVertex3D[m_ObjModel.nVertex];

	int naNotUsedMeshIdx[] = {  186, 187, 37, 20,190, 191, 188, 189, 21, 9, 4, 2, 1, 0, 3, 5, 10, 197, 196, 199, 198, 11, 22, 195, 194};  

	int nNotUsedMeshCnt = sizeof( naNotUsedMeshIdx ) / sizeof( int );
	m_ObjModel.nMesh    = m_CandideMdl.m_iMSTriMeshNun - nNotUsedMeshCnt;
	m_ObjModel.pMesh    = new(std::nothrow) CTriMesh[m_ObjModel.nMesh];

	if ( !m_ObjModel.pVertex || !m_ObjModel.pMesh )
	{
		SAFE_ARRAY_DELETE(m_ObjModel.pVertex);
		SAFE_ARRAY_DELETE(m_ObjModel.pMesh);

		SAFE_ARRAY_DELETE(tpPrevImage);
		SAFE_ARRAY_DELETE(tpWarpImageGrey);
		SAFE_ARRAY_DELETE(tpMaskImage);
		SAFE_ARRAY_DELETE(tpMeshIndexMap);

		SAFE_ARRAY_DELETE(tpnPreSelFeaIdx);

		SAFE_ARRAY_DELETE( tpAAMRlt    );
		SAFE_ARRAY_DELETE( tpAAMRltTmp );

		SAFE_ARRAY_DELETE(tpPrevFeatures);
		SAFE_ARRAY_DELETE(tpBaryCoords);
		SAFE_ARRAY_DELETE(tpFeaTriMesh);

		tbInit = false;
		return FALSE;
	}

	memset(m_ObjModel.pVertex, 0, sizeof(CVertex3D) * m_ObjModel.nVertex);
	memset(m_ObjModel.pMesh, 0, sizeof(CTriMesh) * m_ObjModel.nMesh);
	int     k = 0;
	bool    bNotUsedMesh;
	for ( i = 0; i < m_CandideMdl.m_iMSTriMeshNun; i++ )
	{
		bNotUsedMesh = false;
		for ( j = 0; j < nNotUsedMeshCnt; j++ )
		{
			if ( i == naNotUsedMeshIdx[j] )
			{
				bNotUsedMesh = true;
				break;
			}
		}
		if ( bNotUsedMesh ) 
		{
			continue;
		}

		m_ObjModel.pMesh[k].pt1 = m_CandideMdl.tpMSTriMesh[i].xi;
		m_ObjModel.pMesh[k].pt2 = m_CandideMdl.tpMSTriMesh[i].xj;
		m_ObjModel.pMesh[k].pt3 = m_CandideMdl.tpMSTriMesh[i].xk;
		k++;
	}

	tpPrevImage    = new(std::nothrow) BYTE[tnWidth*tnHeight];
	tpWarpImageGrey= new(std::nothrow) BYTE[tnWidth*tnHeight];
	tpMaskImage    = new(std::nothrow) BYTE[tnWidth*tnHeight];
	tpMeshIndexMap = new(std::nothrow) short int[tnWidth*tnHeight];

	if (!tpPrevImage || !tpWarpImageGrey|| !tpMaskImage || !tpMeshIndexMap )
	{
		SAFE_ARRAY_DELETE(m_ObjModel.pVertex);
		SAFE_ARRAY_DELETE(m_ObjModel.pMesh);

		SAFE_ARRAY_DELETE(tpPrevImage);
		SAFE_ARRAY_DELETE(tpWarpImageGrey);
		SAFE_ARRAY_DELETE(tpMaskImage);
		SAFE_ARRAY_DELETE(tpMeshIndexMap);

		SAFE_ARRAY_DELETE(tpnPreSelFeaIdx);

		SAFE_ARRAY_DELETE( tpAAMRlt    );
		SAFE_ARRAY_DELETE( tpAAMRltTmp );

		SAFE_ARRAY_DELETE(tpPrevFeatures);
		SAFE_ARRAY_DELETE(tpBaryCoords);
		SAFE_ARRAY_DELETE(tpFeaTriMesh);

		tbInit = false;
		return FALSE;
	}

	memset(tpPrevImage, 0, sizeof(BYTE) * tnWidth * tnHeight);
	memset(tpWarpImageGrey, 0, sizeof(BYTE) * tnWidth * tnHeight);
	memset(tpMaskImage, 0, sizeof(BYTE) * tnWidth * tnHeight);
	memset(tpMeshIndexMap, 0, sizeof(short int) * tnWidth * tnHeight);

	tpPrevFeatures = new(std::nothrow) IMAGEFEATURE[NO_TRACKFEATURE*2];
	tpBaryCoords   = new(std::nothrow) VERTEX3D[NO_TRACKFEATURE];
	tpFeaTriMesh   = new(std::nothrow) TrianglesStruct[NO_TRACKFEATURE];
	// eye          left eyebrow                 right eyebrow            
	int     nPreSelFeaIdx[] = { 0, 4, 8, 12,   16, 18, 20, 21, 23, 25,    26, 28, 30, 31, 33, 35,   
		39, 40, 43, 44,   48, 50, 51, 52, 54, 55, 56, 57, 58, 59 };   
	//nose                       mouth

	tnPreSelFeaNum     = sizeof(nPreSelFeaIdx) / sizeof(int);
	tpnPreSelFeaIdx    = new int[tnPreSelFeaNum];

	tpCurrFeatures = tpPrevFeatures + NO_TRACKFEATURE;
	memset(tpPrevFeatures, 0, sizeof(IMAGEFEATURE) * NO_TRACKFEATURE * 2);
	memset(tpBaryCoords, 0, sizeof(VERTEX3D) * NO_TRACKFEATURE);
	memset(tpFeaTriMesh, 0, sizeof(TrianglesStruct) * NO_TRACKFEATURE);

	memcpy( tpnPreSelFeaIdx, nPreSelFeaIdx, sizeof(nPreSelFeaIdx) );

	if (!tfeaTracker.Init(tnWidth, tnHeight, NO_TRACKFEATURE, 5))
	{
		SAFE_ARRAY_DELETE(m_ObjModel.pVertex);
		SAFE_ARRAY_DELETE(m_ObjModel.pMesh);

		SAFE_ARRAY_DELETE(tpPrevImage);
		SAFE_ARRAY_DELETE(tpWarpImageGrey);
		SAFE_ARRAY_DELETE(tpMaskImage);
		SAFE_ARRAY_DELETE(tpMeshIndexMap);

		SAFE_ARRAY_DELETE(tpnPreSelFeaIdx);

		SAFE_ARRAY_DELETE( tpAAMRlt    );
		SAFE_ARRAY_DELETE( tpAAMRltTmp );

		SAFE_ARRAY_DELETE(tpPrevFeatures);
		SAFE_ARRAY_DELETE(tpBaryCoords);
		SAFE_ARRAY_DELETE(tpFeaTriMesh);

		tbInit = false;
		return FALSE;
	}
	tfeaTracker.tpContext     = &m_Context;

	tpAAMRlt       = new(std::nothrow) AAMResult[AAM_MODEL_NUM];
	tpAAMRltTmp    = new(std::nothrow) AAMResult[FRONTAL_INIT_NUM];


	int     iMax2DShapeNum = 0;
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
	{
		iMax2DShapeNum = max( iMax2DShapeNum, m_AAMIter[i].aammodel.shapeAxiNum + 4 );
	}

	// --------------------- 3D model and 2D+3D Fitting releated begin: ---------------------------//
	Fill_AllLUTsof2D3DAAM();


	m_VarsOf2D3DAAM.tpSUCoef   = new float[m_CandideMdl.m_iSUNum];            
	m_VarsOf2D3DAAM.tpAUCoef   = new float[m_CandideMdl.m_iAUNum]; 
	memset( m_VarsOf2D3DAAM.tpSUCoef, 0, sizeof(float)*m_CandideMdl.m_iSUNum );
	memset( m_VarsOf2D3DAAM.tpAUCoef, 0, sizeof(float)*m_CandideMdl.m_iAUNum );
	m_VarsOf2D3DAAM.tfFocalLen = FOCAL_LENGTH / 320 * tnWidth;

	int i3DParaNum = 6 + m_CandideMdl.m_iSUNum + m_CandideMdl.m_iAUNum;
	int iMaxCommonVertexNum = 0;
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
	{
		iMaxCommonVertexNum = max( iMaxCommonVertexNum, m_LUTsOf2D3DAAM[i][0].m_iComVertexNum );
	}


	m_VarsOf2D3DAAM.tppflMatSD3D       = new float*[ (iMaxCommonVertexNum * 2) ];
	m_VarsOf2D3DAAM.tppflMatSD3D[0]    = new float[ (iMaxCommonVertexNum * 2) * (iMax2DShapeNum + i3DParaNum) ];

	m_VarsOf2D3DAAM.tpflMatSD3D_T      = new float[ (iMaxCommonVertexNum * 2) * (iMax2DShapeNum + i3DParaNum) ];
	m_VarsOf2D3DAAM.tpflKeyPoint3D     = new float[ m_CandideMdl.m_iMSVertexNum * 3 ];
	m_VarsOf2D3DAAM.tpfl2DPtsErr       = new float[ (iMaxCommonVertexNum * 2) ];
	m_VarsOf2D3DAAM.tpflHessian3D      = new float[ (iMax2DShapeNum + i3DParaNum) * (iMax2DShapeNum + i3DParaNum) ];
	m_VarsOf2D3DAAM.tpflSDTMulErr3D    = new float[ (iMax2DShapeNum + i3DParaNum) ];

	// --------------------- 3D model and 2D+3D Fitting releated end! ---------------------------//


	int iMaxLandmarkNum = 0;
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
		iMaxLandmarkNum = max( iMaxLandmarkNum,  m_AAMIter[i].aammodel.vertexnum );


	// ----------------- read reconstruct err threshold from RecErrThr.txt ///
	float   flRecErrThr = 19.5f;

	//////////////////////////////////////////////////////////
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
	{
		m_AAMIter[i].SetParameters( tnWidth, tnHeight, FACEAAM_MAXITERATECOUNT, 0.1 );
		m_AAMIter[i].tpPrevFeatures    = tpPrevFeatures   ;
		m_AAMIter[i].tpCurrFeatures    = tpCurrFeatures   ;
		m_AAMIter[i].tpBaryCoords      = tpBaryCoords     ;

		m_AAMIter[i].tpFeaTriMesh      = tpFeaTriMesh     ;
		m_AAMIter[i].tpnTrackedFeatures= &tnTrackedFeatures   ;

		m_AAMIter[i].tpContext         = &m_Context    ;
		m_AAMIter[i].tdbRecErrThr      = flRecErrThr;


		// 3D model
		m_AAMIter[i].tpCandideMdl      = &m_CandideMdl;

		for ( j = 0; j < LUTS_2D3DAAM_NUM; j++ )    
			m_AAMIter[i].tpLUTsOf2D3DAAM[j] = NULL;
		m_AAMIter[i].tpLUTsOf2D3DAAM[0]    = &m_LUTsOf2D3DAAM[i][0];
		m_AAMIter[i].tnCurLUTsOf2D3DAAMIdx = 0;

		m_AAMIter[i].tpVarsOf2D3DAAM       = &m_VarsOf2D3DAAM;

		m_AAMIter[i].tpLUTsofCompGlbAAM    = NULL;
		m_AAMIter[i].tpLUTsofBEBCompGlbAAM = NULL;

		tpAAMRlt[i].CreateObject(m_AAMIter[i].aammodel.shapeAxiNum + 4, m_AAMIter[i].aammodel.vertexnum,
			m_CandideMdl.m_iMSVertexNum, m_CandideMdl.m_iSUNum, m_CandideMdl.m_iAUNum);

	}

	for ( i = 0; i < FRONTAL_INIT_NUM; i++ )
	{
		tpAAMRltTmp[i].CreateObject(m_AAMIter[FRONTAL_MDL_IDX].aammodel.shapeAxiNum + 4, m_AAMIter[FRONTAL_MDL_IDX].aammodel.vertexnum,
			m_CandideMdl.m_iMSVertexNum, m_CandideMdl.m_iSUNum, m_CandideMdl.m_iAUNum);
	}


	tbInit = true;

	tnLostFrameCnt = 0;

	return true;
}

bool CFaceTrack::LoadCandide3Model(const char* modelPath, CandideMdl &candideMdl)
{

	char* pchModelData = (char*)LoadDataFromResource_FD(modelPath, 100016);
	if(pchModelData == NULL) return false;
	LoadCandide3ModelFromBuf((void*)pchModelData, candideMdl);
	return true;
	// --------------------------------- load model from file indirectly -----------------------//

	float   flScale = ZOOM_3D_MODEL_PARA;  
	float*  pData;
	pData = (float*)(candideMdl.tpMSVertex);
	for (int i = 0; i < candideMdl.m_iMSVertexNum * 3; i++ )
		pData[i] *= flScale;

	pData = (float*)(candideMdl.tpEach_SUVertexVal);
	for (int i = 0; i < candideMdl.m_iTotal_SUVertexNum * 3; i++ )
		pData[i] *= flScale;

	pData = (float*)(candideMdl.tpEach_AUVertexVal);
	for (int i = 0; i < candideMdl.m_iTotal_AUVertexNum * 3; i++ )
		pData[i] *= flScale;
	return true;
}

bool CFaceTrack::LoadCandide3ModelFromBuf(void *p, CandideMdl &candideMdl)
{
	if (p == NULL)
	{
		return false;
	}

	char*   pModelData = (char*)p;
	char    temp[MAX_PATH];
	int     nRead;
	size_t  nSize;

	nSize = 0;
	nRead = sscanf(pModelData , "%[^\n]", temp); 
	nSize += nRead * strlen(temp) + 1;

	int     i, j;
	AAMPoint3D*     pVertexVal;
	while( strstr(temp, "# END OF FILE") == NULL )
	{
		if( strstr(temp, "# VERTEX LIST:") != NULL )
		{
			nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
			nSize += nRead * strlen(temp) + 1;

			sscanf(temp, "%d", &candideMdl.m_iMSVertexNum);
			candideMdl.tpMSVertex = new AAMPoint3D[candideMdl.m_iMSVertexNum];

			for ( i = 0; i < candideMdl.m_iMSVertexNum; i++ )
			{
				pVertexVal = candideMdl.tpMSVertex + i;

				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;
				sscanf(temp, "%f %f %f", &pVertexVal->x, &pVertexVal->y, &pVertexVal->z);
			}
		}

		else if( strstr(temp, "# FACE LIST:") != NULL )
		{
			nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
			nSize += nRead * strlen(temp) + 1;
			sscanf(temp, "%d", &candideMdl.m_iMSTriMeshNun);
			candideMdl.tpMSTriMesh = new TrianglesStruct[candideMdl.m_iMSTriMeshNun];
			TrianglesStruct* pMSTriMesh   ;

			for ( i = 0; i < candideMdl.m_iMSTriMeshNun; i++ )
			{
				pMSTriMesh = candideMdl.tpMSTriMesh + i;

				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;
				sscanf(temp, "%d %d %d", &pMSTriMesh->xi, &pMSTriMesh->xj, &pMSTriMesh->xk);
			}
		}

		else if( strstr(temp, "# ANIMATION UNITS LIST:") != NULL )
		{
			nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
			nSize += nRead * strlen(temp) + 1;
			sscanf(temp, "%d", &candideMdl.m_iAUNum);
			candideMdl.tpiEach_AUVertexNum = new int[candideMdl.m_iAUNum];

			int iTotalVertexNum = 0;
			std::vector<int> vecEachVertexIdx;
			std::vector<AAMPoint3D> vecEachVertexVal;

			for ( i = 0; i < candideMdl.m_iAUNum; i++ )
			{
				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;

				while( strstr(temp, "# END OF FILE") == NULL && (strstr(temp, "# ") == NULL) ) 
				{
					nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
					nSize += nRead * strlen(temp) + 1;
				}

				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;
				sscanf(temp, "%d", &candideMdl.tpiEach_AUVertexNum[i]);
				for ( j = 0; j < candideMdl.tpiEach_AUVertexNum[i]; j++ )
				{
					int iIdx;
					AAMPoint3D  vertexVal;

					nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
					nSize += nRead * strlen(temp) + 1;
					sscanf(temp, "%d %f %f %f", &iIdx, &vertexVal.x, &vertexVal.y, &vertexVal.z);
					vecEachVertexIdx.push_back(iIdx);
					vecEachVertexVal.push_back(vertexVal);
					iTotalVertexNum++;
				}
			}
			candideMdl.m_iTotal_AUVertexNum = iTotalVertexNum; 
			candideMdl.tpiEach_AUVertexIdx = new int[iTotalVertexNum];
			candideMdl.tpEach_AUVertexVal = new AAMPoint3D[iTotalVertexNum];

			for( i = 0; i < iTotalVertexNum; i++ )
			{
				candideMdl.tpiEach_AUVertexIdx[i] = vecEachVertexIdx[i];
				candideMdl.tpEach_AUVertexVal[i] = vecEachVertexVal[i];
			}
		}
		else if( strstr(temp, "# SHAPE UNITS LIST:") != NULL )
		{
			nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
			nSize += nRead * strlen(temp) + 1;
			sscanf(temp, "%d", &candideMdl.m_iSUNum);
			candideMdl.tpiEach_SUVertexNum = new int[candideMdl.m_iSUNum];

			int iTotalVertexNum = 0;
			std::vector<int> vecEachVertexIdx;
			std::vector<AAMPoint3D> vecEachVertexVal;

			for ( i = 0; i < candideMdl.m_iSUNum; i++ )
			{
				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;

				while( strstr(temp, "# END OF FILE") == NULL && (strstr(temp, "# ") == NULL) ) 
				{
					nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
					nSize += nRead * strlen(temp) + 1;
				}

				nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
				nSize += nRead * strlen(temp) + 1;
				sscanf(temp, "%d", &candideMdl.tpiEach_SUVertexNum[i]);
				for ( j = 0; j < candideMdl.tpiEach_SUVertexNum[i]; j++ )
				{
					int iIdx;
					AAMPoint3D  vertexVal;
					nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
					nSize += nRead * strlen(temp) + 1;
					sscanf(temp, "%d %f %f %f", &iIdx, &vertexVal.x, &vertexVal.y, &vertexVal.z);
					vecEachVertexIdx.push_back(iIdx);
					vecEachVertexVal.push_back(vertexVal);
					iTotalVertexNum++;
				}
			}
			candideMdl.m_iTotal_SUVertexNum = iTotalVertexNum; 
			candideMdl.tpiEach_SUVertexIdx = new int[iTotalVertexNum];
			candideMdl.tpEach_SUVertexVal = new AAMPoint3D[iTotalVertexNum];

			for( i = 0; i < iTotalVertexNum; i++ )
			{
				candideMdl.tpiEach_SUVertexIdx[i] = vecEachVertexIdx[i];
				candideMdl.tpEach_SUVertexVal[i] = vecEachVertexVal[i];
			}
		}

		nRead = sscanf(pModelData + nSize, "%[^\n]", temp); 
		nSize += nRead * strlen(temp) + 1;

	}

	return true;
}

bool CFaceTrack::Fill_AllLUTsof2D3DAAM()
{
	int j;
	memset( m_LUTsOf2D3DAAM, 0, sizeof(m_LUTsOf2D3DAAM) );

	/////////////////////////////////////  frontal view  ////////////////////////////////////////////////

	// 2 AAM model LUTsOf2D3DAAM  -- frontal view,   
	{
		//LUT
		int     iComTo2DIdx[] = {
			0, 2, 4, 6, 8, 10, 12, 14,          //0-7: left and right eye
			25, 23, 21, 35, 33, 31,             // 8-13: left and right eyebrow
			37, 40, 41, 42, 46,                 // 14-18: nose
			48, 50, 51, 52, 54, 55, 57, 58,     // 19-26: outer lip contour    
			60, 61, 62, 63, 64, 65, 66, 67,     // 27-33: inner lip contour
			68, 70, 73, 75, 77, 78, 81, 83, 86  // 34-42: profile
		};

		int     iComTo3DIdx[] = {
			56, 54, 53, 55, 23, 21, 20, 22,     //0-7: left and right eye
			50, 51, 48, 17, 18, 15,             // 8-13: left and right eyebrow
			93, 112, 6, 111, 92,                // 14-18: nose
			64, 66, 7, 33, 31, 85, 8, 86,       // 19-26: outer lip contour    
			89, 82, 87, 81, 88, 83, 40, 84,     // 27-34: inner lip contour
			62, 61, 63, 65, 10, 32, 30, 28, 29  // 35-43: profile
		};


		int     i2DNeedIntplIdx[] = {                   // in common index
			15, 16, 17, 24, 26, 36, 38, 40, 42
		};

		int     i2DIntplIdx[] = {                       // in common index
			41, 42, 43, 56, 59, 71, 76, 79, 84
		};

		float   fl2DIntplCoef[][2] = {                   // in common index
			{0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
			, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
		};

		int     iMotionConstIdx[] = {
			35, 36, 42,43
		};

		j = FRONTAL_MDL_IDX;
		AllocateAndFill_OneLUTsof2D3DAAM(
			&m_LUTsOf2D3DAAM[j][0],
			sizeof(iComTo2DIdx) / sizeof(int), 
			sizeof(i2DNeedIntplIdx) / sizeof(int), 
			iComTo2DIdx, 
			iComTo3DIdx,
			i2DNeedIntplIdx,
			i2DIntplIdx,
			fl2DIntplCoef,
			sizeof(iMotionConstIdx) / sizeof(int),
			iMotionConstIdx
			);
	}

	/////////////////////////////////////  half left view  ////////////////////////////////////////////////

	// 1 AAM model LUTsOf2D3DAAM  -- half left view, initial
	{
		//LUT
		int     iComTo2DIdx[] = {
			0, 2, 4, 6, 8, 10, 12, 14,          //0-7: left and right eye
			25, 23, 21, 35, 33, 31,             // 8-13: left and right eyebrow
			37, 40, 41, 42, 46,                 // 14-18: nose
			48, 50, 51, 52, 54, 55, 57, 58,     // 19-26: outer lip contour    
			60, 61, 62, 63, 64, 65, 66, 67,     // 27-33: inner lip contour
			68, 70, 73, 75, 77, 78, 81, 83, 86  // 34-42: profile
		};

		int     iComTo3DIdx[] = {
			56, 54, 53, 55, 23, 21, 20, 22,     //0-7: left and right eye
			50, 51, 48, 17, 18, 15,             // 8-13: left and right eyebrow
			93, 112, 6, 111, 92,                // 14-18: nose
			64, 66, 7, 33, 31, 85, 8, 86,       // 19-26: outer lip contour    
			89, 82, 87, 81, 88, 83, 40, 84,     // 27-34: inner lip contour
			62, 61, 63, 65, 10, 32, 30, 117, 118  // 35-43: profile
		};


		int     i2DNeedIntplIdx[] = {                   // in common index
			15, 16, 17, 24, 26, 36, 38, 40, 42
		};

		int     i2DIntplIdx[] = {                       // in common index
			41, 42, 43, 56, 59, 71, 76, 79, 84
		};

		float   fl2DIntplCoef[][2] = {                   // in common index
			{0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
			, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
		};

		int     iMotionConstIdx[] = {
			35, 36, 42,43
		};

		j = FRONTAL_MDL_IDX - 1;
		AllocateAndFill_OneLUTsof2D3DAAM(
			&m_LUTsOf2D3DAAM[j][0],
			sizeof(iComTo2DIdx) / sizeof(int), 
			sizeof(i2DNeedIntplIdx) / sizeof(int), 
			iComTo2DIdx, 
			iComTo3DIdx,
			i2DNeedIntplIdx,
			i2DIntplIdx,
			fl2DIntplCoef,
			sizeof(iMotionConstIdx) / sizeof(int),
			iMotionConstIdx
			);
	}


	/////////////////////////////////////  half right view  ////////////////////////////////////////////////

	{
		int     iComTo2DIdx[] = {
			0, 2, 4, 6, 8, 10, 12, 14,          //0-7: left and right eye
			25, 23, 21, 35, 33, 31,             // 8-13: left and right eyebrow
			37, 40, 41, 42, 46,                 // 14-18: nose
			48, 50, 51, 52, 54, 55, 57, 58,     // 19-26: outer lip contour    
			60, 61, 62, 63, 64, 65, 66, 67,     // 27-33: inner lip contour
			68, 70, 73, 75, 77, 78, 81, 83, 86  // 34-42: profile
		};

		int     iComTo3DIdx[] = {
			56, 54, 53, 55, 23, 21, 20, 22,     //0-7: left and right eye
			50, 51, 48, 17, 18, 15,             // 8-13: left and right eyebrow
			93, 112, 6, 111, 92,                // 14-18: nose
			64, 66, 7, 33, 31, 85, 8, 86,       // 19-26: outer lip contour    
			89, 82, 87, 81, 88, 83, 40, 84,     // 27-34: inner lip contour
			114, 113, 63, 65, 10, 32, 30, 28, 29  // 35-43: profile
		};


		int     i2DNeedIntplIdx[] = {                   // in common index
			15, 16, 17, 24, 26, 36, 38, 40, 42
		};

		int     i2DIntplIdx[] = {                       // in common index
			41, 42, 43, 56, 59, 71, 76, 79, 84
		};

		float   fl2DIntplCoef[][2] = {                   // in common index
			{0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
			, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, 0.5f}
		};

		int     iMotionConstIdx[] = {
			35, 36, 42,43
		};

		j = FRONTAL_MDL_IDX + 1;
		AllocateAndFill_OneLUTsof2D3DAAM(
			&m_LUTsOf2D3DAAM[j][0],
			sizeof(iComTo2DIdx) / sizeof(int), 
			sizeof(i2DNeedIntplIdx) / sizeof(int), 
			iComTo2DIdx, 
			iComTo3DIdx,
			i2DNeedIntplIdx,
			i2DIntplIdx,
			fl2DIntplCoef,
			sizeof(iMotionConstIdx) / sizeof(int),
			iMotionConstIdx
			);
	}

	return true;
}

bool CFaceTrack::AllocateAndFill_OneLUTsof2D3DAAM(
	LUTsOf2D3DAAM*  pLUTsOf2D3DAAM,
	const int iComVertexNum, 
	const int i2DNeedIntplNum, 
	const int iComTo2DIdx[], 
	const int iComTo3DIdx[],
	const int i2DNeedIntplIdx[],
	const int i2DIntplIdx[],
	const float fl2DIntplCoef[][2],
	const int iMotionConstNum, 
	const int iMotionConstIdx[]
)
{
	int     i;

	pLUTsOf2D3DAAM->m_iComVertexNum     = iComVertexNum;   // actural number
	pLUTsOf2D3DAAM->m_i2DNeedIntplNum   = i2DNeedIntplNum;

	pLUTsOf2D3DAAM->tpiComTo2DIdx = new int[pLUTsOf2D3DAAM->m_iComVertexNum];
	memcpy( pLUTsOf2D3DAAM->tpiComTo2DIdx, iComTo2DIdx, sizeof(int) * pLUTsOf2D3DAAM->m_iComVertexNum );

	pLUTsOf2D3DAAM->tpiComTo3DIdx = new int[pLUTsOf2D3DAAM->m_iComVertexNum];
	memcpy( pLUTsOf2D3DAAM->tpiComTo3DIdx, iComTo3DIdx, sizeof(int) * pLUTsOf2D3DAAM->m_iComVertexNum );

	pLUTsOf2D3DAAM->tpi2DNeedIntplIdx = new int[pLUTsOf2D3DAAM->m_i2DNeedIntplNum];
	memcpy( pLUTsOf2D3DAAM->tpi2DNeedIntplIdx, i2DNeedIntplIdx, sizeof(int) * pLUTsOf2D3DAAM->m_i2DNeedIntplNum );

	pLUTsOf2D3DAAM->tpi2DIntplIdx = new int[pLUTsOf2D3DAAM->m_i2DNeedIntplNum];
	memcpy( pLUTsOf2D3DAAM->tpi2DIntplIdx, i2DIntplIdx, sizeof(int) * pLUTsOf2D3DAAM->m_i2DNeedIntplNum );

	pLUTsOf2D3DAAM->tpfl2DIntplCoef = new float[pLUTsOf2D3DAAM->m_i2DNeedIntplNum][2];
	memcpy( pLUTsOf2D3DAAM->tpfl2DIntplCoef, fl2DIntplCoef, sizeof(float) * pLUTsOf2D3DAAM->m_i2DNeedIntplNum * 2 );


	pLUTsOf2D3DAAM->tpi3DToComIdx = new int[m_CandideMdl.m_iMSVertexNum];
	for( i = 0; i < m_CandideMdl.m_iMSVertexNum; i++ )
		pLUTsOf2D3DAAM->tpi3DToComIdx[i] = -1;
	for( i = 0; i < pLUTsOf2D3DAAM->m_iComVertexNum; i++ )
		pLUTsOf2D3DAAM->tpi3DToComIdx[ iComTo3DIdx[i] ] = i;

	pLUTsOf2D3DAAM->m_iMotionConstNum   = iMotionConstNum;
	pLUTsOf2D3DAAM->tpiMotionConstIdx  = new int[pLUTsOf2D3DAAM->m_iMotionConstNum];
	memcpy( pLUTsOf2D3DAAM->tpiMotionConstIdx, iMotionConstIdx, sizeof(int) * pLUTsOf2D3DAAM->m_iMotionConstNum );

	return true;
}


void CFaceTrack::ResetTracker() 
{ 
	int i;
	for ( i = 0; i < AAM_MODEL_NUM; i++ )
	{
		m_AAMIter[i].ResetOnlineTempl();
	}

	g_nFrameIdx     = 0;    
	tnFrameIndex   = 0;  // reset to zero

	tbNeedOuterInit = true; 
	tnLostFrameCnt  = 0;

}

AAMResult* CFaceTrack::TrackImage(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape )
{
	if ( pGrayImage == NULL )
	{
		return FALSE;
	}

	bool    bSuccess = false;

	memcpy(tpCurrImage, pGrayImage, tnWidth * tnHeight);
	bSuccess = DetectShape(tpCurrImage, pFaceRect, pflShape);
	if ( bSuccess )
		return &tpAAMRlt[tnMdlIdx];

	return NULL;
}

bool CFaceTrack::DetectShape(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape)
{
	if ( !pFaceRect && !pflShape )
	{
		pFaceRect = FaceDetection( pGrayImage );		
	}

	if ( pFaceRect || pflShape )    
	{
		InitParaComp objInitAAMComp(1.0f, 0.0f, 0.0f);
		int k = FRONTAL_INIT_NUM;
		for (int i = 0; i < k; i++ )
		{
			objInitAAMComp.SetParameter(1.2f + 0.2f*(i - k/2), 0.0f, -1.0f, 0); 
			m_AAMIter[tnMdlIdx].RunAAM(pGrayImage, pFaceRect, NULL, NULL, &tpAAMRltTmp[i], false, &objInitAAMComp); //true
		}

		int nBestIdx = -1;
		double   dbReconErrTmp;
		for (int i = 0; i < FRONTAL_INIT_NUM; i++ )//
		{
			if (tpAAMRltTmp[i].tbAlignSuccessful)
			{
				if (nBestIdx == -1)     //first successful alignment
				{
					nBestIdx = i;
					dbReconErrTmp = tpAAMRltTmp[i].tdbReconErrFinal;
				}
				else if (dbReconErrTmp > tpAAMRltTmp[i].tdbReconErrFinal)
				{
					nBestIdx = i;
					dbReconErrTmp = tpAAMRltTmp[i].tdbReconErrFinal;
				}
			}
		}	

		if (nBestIdx != -1) 		
			tpAAMRlt[tnMdlIdx].Copy( &tpAAMRltTmp[nBestIdx] );

		if ( tpAAMRlt[tnMdlIdx].tbAlignSuccessful )		
			return true;		
		else		
			return false;		
	}
	else				
		return false;

}


AAMResult* CFaceTrack::Track(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape )
{
	if ( pGrayImage == NULL )
	{
		return FALSE;
	}

	bool    bSuccess = false;
	tpCurrImage = pGrayImage;
	bSuccess = Track_2D(tpCurrImage, pFaceRect, pflShape );
	if ( bSuccess )
		return &tpAAMRlt[tnMdlIdx];

	return NULL;

}

bool CFaceTrack::Track_2D(BYTE *pGrayImage, RECT *pFaceRect, float* pflShape )
{	
	tnFrameIndex++;
	g_nFrameIdx = tnFrameIndex;

	if ( tbNeedOuterInit )
	{

		if ( !pFaceRect && !pflShape )
		{
			pFaceRect = FaceDetection( pGrayImage );		
		}

		if ( pFaceRect || pflShape )   
		{
			tnMdlIdx = FRONTAL_MDL_IDX;
			m_AAMIter[tnMdlIdx].RunAAM(pGrayImage, pFaceRect, pflShape, NULL, &tpAAMRlt[tnMdlIdx], false);

			if ( tpAAMRlt[tnMdlIdx].tbAlignSuccessful )
			{
				goto OnSelFeature;      
			}
			else 
			{
				tbNeedOuterInit = true;
				return false;
			}
		}
		else
		{		
			tbNeedOuterInit = true;
			return false;
		}
	}
	else
	{
		W_Point ptShiftZero(0,0);
		/*bRetTrack = */tfeaTracker.TrackFeatures(
			tpPrevImage,
			pGrayImage,
			tpPrevFeatures,
			tpCurrFeatures,
			false,
			ptShiftZero);

		// stat. the number of tracked features
		tnTrackedFeatures  = 0;
		int j;
		for ( j = 0; j < NO_TRACKFEATURE; j++ )
		{
			if ( tpCurrFeatures[j].w > 0 )  
			{
				tnTrackedFeatures++;
			}
			else
			{
				tpCurrFeatures[j].w = 0;
			}
		}

		bool bSuccess = false;
		if ( tnTrackedFeatures > 0 )
		{
			bSuccess = m_AAMIter[tnMdlIdx].PreAlignByFeatureTrack( pGrayImage, &tpAAMRlt[tnMdlIdx] );
		}

		if ( !bSuccess )    
		{
			tnLostFrameCnt++;
			if ( tnLostFrameCnt > ALLOWED_LOST_FRAME_CNT )
			{
				tbNeedOuterInit    = true;     
				tnLostFrameCnt     = 0;              
			}
			return false;
		}

		double dRatio = CalcuCurViewRatio();
		int nTmpMdlIdx = tnMdlIdx;
		GetNewModelIdx( tnMdlIdx, dRatio, nTmpMdlIdx ); 

		if ( nTmpMdlIdx != tnMdlIdx )
		{

			tpAAMRlt[nTmpMdlIdx].Copy( &tpAAMRlt[tnMdlIdx] );

			m_AAMIter[nTmpMdlIdx].PreAlignByFeatureTrack( pGrayImage, &tpAAMRlt[nTmpMdlIdx] );

			tnMdlIdx = nTmpMdlIdx;
			m_AAMIter[tnMdlIdx].RunAAM(pGrayImage, NULL, NULL, NULL, &tpAAMRlt[tnMdlIdx], false);

		}
		else
		{
			m_AAMIter[tnMdlIdx].RunAAM(pGrayImage, NULL, NULL, NULL, &tpAAMRlt[tnMdlIdx], true);
		}


		//model transition
		if ( tpAAMRlt[tnMdlIdx].tbAlignSuccessful )
			//if ( 0 )
		{
			tbNeedOuterInit = false;

			double dRatio = CalcuCurViewRatio();
			int nTmpMdlIdx = tnMdlIdx;
			GetNewModelIdx( tnMdlIdx, dRatio, nTmpMdlIdx ); 

			if ( nTmpMdlIdx != tnMdlIdx )
			{
				tpAAMRlt[nTmpMdlIdx].Copy( &tpAAMRlt[tnMdlIdx] ); 

				m_AAMIter[nTmpMdlIdx].CalcuParaByShape( tpAAMRlt[tnMdlIdx].tpfShape, tpAAMRlt[nTmpMdlIdx].tpfParameters );
				m_AAMIter[nTmpMdlIdx].RunAAM(pGrayImage, NULL, NULL, NULL, &tpAAMRlt[nTmpMdlIdx], false);

				if ( tpAAMRlt[nTmpMdlIdx].tbAlignSuccessful )
				{
					tnMdlIdx = nTmpMdlIdx;
				}
			}
		}

		// prepare for feature tracking
		if ( tpAAMRlt[tnMdlIdx].tbAlignSuccessful )   
		{      
			goto OnSelFeature;      
		}
		else 
		{
			tnLostFrameCnt++;
			if ( tnLostFrameCnt > ALLOWED_LOST_FRAME_CNT )
			{
				tbNeedOuterInit    = true;     
				tnLostFrameCnt     = 0;               
			}
			return false;
		}
	}


OnSelFeature:
	tbNeedOuterInit    = false;
	tnLostFrameCnt     = 0;

	W_Rect rcFace; 
	Get2DAAMModelMask( tpMaskImage, rcFace, true );

	if ( m_Context.tbPreSelectFeature )
	{		
		memset( tpPrevFeatures, 0, sizeof(IMAGEFEATURE) * NO_TRACKFEATURE );
		int i;
		for( i = 0; i < tnPreSelFeaNum; i++ )
		{
			tpPrevFeatures[i].x = tpAAMRlt[tnMdlIdx].tpfShape[tpnPreSelFeaIdx[i]*2 + 0];
			tpPrevFeatures[i].y = tpAAMRlt[tnMdlIdx].tpfShape[tpnPreSelFeaIdx[i]*2 + 1];
			tpPrevFeatures[i].w = 1;
		}		
	}

	if (!tfeaTracker.SelectFeatures(pGrayImage, tpPrevFeatures,
		rcFace, 9, 5, 20, 1, tpMaskImage))
	{
		return NULL;
	}

	CopyImage(pGrayImage, tpPrevImage, tnWidth, tnHeight);

	GetFeatureBaryCoords( tpPrevFeatures );

	return true;
}

float CFaceTrack::CalcuCurViewRatio( )
{
	float* pflShape = tpAAMRlt[tnMdlIdx].tpfShape;
	float min_dist_left = 1000000;
	for (int i=70; i<74; i++)
	{
		float dist = MinDistance(pflShape[2*i],pflShape[2*i+1],pflShape[2*i+2],pflShape[2*i+3],pflShape[2*48],pflShape[2*48+1]);

		if(dist < min_dist_left)
			min_dist_left = dist;
	}

	float min_dist_right = 1000000;
	for (int i=81; i<85; i++)
	{
		float dist = MinDistance(pflShape[2*i],pflShape[2*i+1],pflShape[2*i+2],pflShape[2*i+3],pflShape[2*54],pflShape[2*54+1]);

		if(dist < min_dist_right)
			min_dist_right = dist;	
	}

	float dRatio = min_dist_left/(min_dist_left + min_dist_right);

	return  dRatio;	

}

float CFaceTrack::MinDistance(float x1,float y1, float x2,float y2, float x3, float y3)
{
	//(v2-v1).(v2-v1)
	float px = x2 - x1;
	float py = y2 - y1;
	float dot1 = px * px + py * py;

	//(v2-v1).(v3-v1)
	float dot2 =  (x3 - x1) * px + (y3 - y1) * py;
	float u =  dot2/dot1;

	if (u > 1) 
	{
		u = 1;
	}
	if (u < 0) 
	{
		u = 0;
	}

	//the closest point
	float x = x1 + u * px;
	float y = y1 + u * py;
	float dx = x - x3;
	float dy = y - y3;      
	float dist = sqrt(dx*dx + dy*dy);

	return dist;
}

bool CFaceTrack::GetNewModelIdx( const int& nOldMdlIdx, double dRatio, int& nNewMdlIdx )
{
	if ( nOldMdlIdx == FRONTAL_MDL_IDX )
	{
		if ( dRatio < 0.35 ) 
			nNewMdlIdx = FRONTAL_MDL_IDX - 1;
		else if ( dRatio > 0.65 )
			nNewMdlIdx = FRONTAL_MDL_IDX + 1;
	}
	else if ( nOldMdlIdx == FRONTAL_MDL_IDX - 1 )
	{
		if ( dRatio > 0.40 ) 
			nNewMdlIdx = FRONTAL_MDL_IDX;
	}
	if ( nOldMdlIdx == FRONTAL_MDL_IDX + 1 )
	{
		if ( dRatio < 0.60 )
			nNewMdlIdx = FRONTAL_MDL_IDX;
	}

	return true;
}

void CFaceTrack::Get2DAAMModelMask( BYTE *pMaskImage,W_Rect &rcFace,bool bSetMeshIndexMap)
{
	if (pMaskImage == NULL)
	{
		return;
	}

	CAAM*    pCurAAMIter = GetCurAAMIter();
	TrianglesStruct* pTriangles = pCurAAMIter->aammodel.triangles;
	float* pflShape = tpAAMRlt[tnMdlIdx].tpfShape;

	rcFace.X = int( tpAAMRlt[tnMdlIdx].trcFaceRect.left );
	rcFace.Y = int( tpAAMRlt[tnMdlIdx].trcFaceRect.top );
	rcFace.Width = int( tpAAMRlt[tnMdlIdx].trcFaceRect.right - tpAAMRlt[tnMdlIdx].trcFaceRect.left );
	rcFace.Height = int( tpAAMRlt[tnMdlIdx].trcFaceRect.bottom - tpAAMRlt[tnMdlIdx].trcFaceRect.top );


	CVertex2D pImageCoord2D[3];
	memset(pMaskImage, 0, sizeof(BYTE) * tnWidth * tnHeight);

	for (int j = 0; j < pCurAAMIter->aammodel.triangleNum; j++)
	{

		pImageCoord2D[0].x = pflShape[pTriangles[j].xi + 0];
		pImageCoord2D[0].y = pflShape[pTriangles[j].xi + 1];
		pImageCoord2D[1].x = pflShape[pTriangles[j].xj + 0];
		pImageCoord2D[1].y = pflShape[pTriangles[j].xj + 1];
		pImageCoord2D[2].x = pflShape[pTriangles[j].xk + 0];
		pImageCoord2D[2].y = pflShape[pTriangles[j].xk + 1];


		// The max, middle, min Y-value of target points
		int y = 0, ymax = 0, ymid = 0, ymin = 0; 

		// index of target points with max, middle, min Y-value
		int imax = 0, imid = 0, imin = 0;    

		// l1: from point ymin->ymax;  l2: from point ymin->ymid;
		// l3: from point ymid->ymax.
		CVertex3D l1, l2, l3;           
		memset(&l1, 0, sizeof(l1));
		memset(&l2, 0, sizeof(l2));                                        
		memset(&l3, 0, sizeof(l3));

		CVertex2D tpt[3];
		memset(&tpt[0], 0, sizeof(CVertex2D) * 3);

		bool bTrue = FALSE;

		for (int k = 0; k < 3; k++)
		{
			tpt[k].x = pImageCoord2D[k].x + 0.5f;
			tpt[k].y = pImageCoord2D[k].y + 0.5f;

			if (tpt[k].x > tnWidth || tpt[k].x < 0
				|| tpt[k].y > tnHeight || tpt[k].y < 0)
			{
				bTrue = TRUE;
			} 
		}
		if (bTrue)
		{
			continue;
		}

		//float    tx1, ty1, tx2, ty2, tx3, ty3;    // target coordinate
		if (tpt[0].y >= tpt[1].y)        // 0 >=1
		{
			if (tpt[1].y >= tpt[2].y)    // 0 >= 1 >= 2
			{
				ymax = (int)tpt[0].y;
				imax = 0;
				ymid = (int)tpt[1].y;
				imid = 1;
				ymin = (int)tpt[2].y;
				imin = 2;
			}
			else if (tpt[0].y >= tpt[2].y)    // 0 >= 2 > 1
			{
				ymax = (int)tpt[0].y;
				imax = 0;
				ymid = (int)tpt[2].y;
				imid = 2;
				ymin = (int)tpt[1].y;
				imin = 1;
			}
			else    // 2 > 0 >= 1
			{
				ymax = (int)tpt[2].y;
				imax = 2;
				ymid = (int)tpt[0].y;
				imid = 0;
				ymin = (int)tpt[1].y;
				imin = 1;
			}
		}
		else                            // 1 > 0
		{
			if (tpt[0].y >= tpt[2].y)    // 1 > 0 >= 2
			{
				ymax = (int)tpt[1].y;
				imax = 1;
				ymid = (int)tpt[0].y;
				imid = 0;
				ymin = (int)tpt[2].y;
				imin = 2;
			}
			else if (tpt[1].y >= tpt[2].y)    // 1 >= 2 > 0
			{
				ymax = (int)tpt[1].y;
				imax = 1;
				ymid = (int)tpt[2].y;
				imid = 2;
				ymin = (int)tpt[0].y;
				imin = 0;
			}
			else    // 2 > 1 > 0
			{
				ymax = (int)tpt[2].y;
				imax = 2;
				ymid = (int)tpt[1].y;
				imid = 1;
				ymin = (int)tpt[0].y;
				imin = 0;
			}
		}
		// the triangle is approximately degenerated to a line
		if ((ymin == ymid) && (ymid == ymax))
		{
			continue;
		}

		Line(tpt[imin].x, tpt[imin].y, tpt[imax].x, tpt[imax].y,
			l1.x, l1.y, l1.z);    // ymin->ymax line

		Line(tpt[imin].x, tpt[imin].y, tpt[imid].x, tpt[imid].y,
			l2.x, l2.y, l2.z);    // ymin->ymid line

		Line(tpt[imid].x, tpt[imid].y, tpt[imax].x, tpt[imax].y,
			l3.x, l3.y, l3.z);    // ymid->ymax line

		int x = 0, x1 = 0, x2 = 0 , xtmp = 0;    // rx, ry
		float lb1 = 0, lc1 = 0, lb2 = 0, lc2 = 0;    // modified line parameters

		if (ymin != ymid)        // l2 is not horizontal
		{
			// calculate modified line parameter: from ax+by+c=0 to x+lb*y+lc=0    // a !=0
			float invl1x = 1.0f / l1.x;
			float invl2x = 1.0f / l2.x;
			float lb1y, lb2y;

			lb1 = l1.y * invl1x;
			lc1 = l1.z * invl1x;
			lb2 = l2.y * invl2x; 
			lc2 = l2.z * invl2x;
			lb1y = lb1 * ymin;
			lb2y = lb2 * ymin;
			for (y = ymin + 1; y <= ymid; y++)
			{
				lb1y += lb1;
				lb2y += lb2;
				x1 = (int)(-lb1y - lc1);        // x1<=x2
				x2 = (int)(-lb2y - lc2);
				if (x1 < 0 || x2 < 0 || x1 >= tnWidth || x2 >= tnWidth)
				{
					continue;
				}
				if (x1 > x2)        
				{
					xtmp = x1; x1 = x2; x2 = xtmp;
				}

				int nPos = y * tnWidth + x1;
				for (x = x1 + 1; x <= x2; x++)
				{
					nPos++;
					pMaskImage[nPos] = 255;

					if (bSetMeshIndexMap)
					{
						pMaskImage[nPos] = j + 1;
					}
				}
			}
		}

		if (ymid != ymax)        // l3 is not horizontal
		{
			// calculate modified line parameter: from ax+by+c=0 to x+lb*y+lc=0    // a !=0
			float invl1x = 1.0f / l1.x; 
			float invl3x = 1.0f / l3.x;
			float lb1y, lb2y;

			lb1 = l1.y * invl1x;
			lc1 = l1.z * invl1x;
			lb2 = l3.y * invl3x;
			lc2 = l3.z * invl3x;
			lb1y = lb1 * ymid;
			lb2y = lb2 * ymid;
			for (y = ymid + 1; y <= ymax; y++)
			{
				lb1y += lb1;
				lb2y += lb2;
				x1 = (int)(-lb1y - lc1);        // x1<=x2
				x2 = (int)(-lb2y - lc2);
				if (x1 < 0 || x2 < 0 || x1 >= tnWidth || x2 >= tnWidth)
				{
					continue;
				}
				if (x1 > x2)        
				{
					xtmp = x1; x1 = x2; x2 = xtmp;
				}

				int nPos = y * tnWidth + x1;
				for (x = x1 + 1; x <= x2; x++)
				{
					nPos++;
					pMaskImage[nPos] = 255;
					if (bSetMeshIndexMap)
					{
						pMaskImage[nPos] = j + 1;
					}
				}
			}
		}
	}
}

void CFaceTrack::GetFeatureBaryCoords( IMAGEFEATURE* pImgFeatures )
{
	int i;
	CAAM*    pCurAAMIter = GetCurAAMIter();
	AAMPoint        aamPoint;

	for ( i = 0; i < NO_TRACKFEATURE; i++ )
	{
		if ( pImgFeatures[i].w <= 0)
			continue;

		int nMeshIdx = tpMaskImage[ int(pImgFeatures[i].y) * tnWidth + int(pImgFeatures[i].x) ] - 1;

		tpFeaTriMesh[i] = pCurAAMIter->aammodel.triangles[nMeshIdx];

		aamPoint.x = pImgFeatures[i].x;
		aamPoint.y = pImgFeatures[i].y;

		pCurAAMIter->GetRelativePos( tpAAMRlt[tnMdlIdx].tpfShape, pCurAAMIter->aammodel.triangles[nMeshIdx], aamPoint, 
			tpBaryCoords[i].x, tpBaryCoords[i].y, tpBaryCoords[i].z ); 
	}
}

RECT * CFaceTrack::FaceDetection( BYTE *pGrayImage )
{

	int faceCount = detector_process(adFacedetector, pGrayImage, tnWidth, tnHeight);

	if(faceCount>0)
	{
		TPointF pt[2];
		detector_get_face(adFacedetector, pt, 0);
		float dist = sqrt((pt[0].x - pt[1].x)*(pt[0].x - pt[1].x) + (pt[0].y - pt[1].y)*(pt[0].y - pt[1].y));
		trcFaceBak.left = pt[0].x - dist/2;
		trcFaceBak.right = pt[1].x + dist/2;
		trcFaceBak.top = pt[0].y - dist/2;
		trcFaceBak.bottom = pt[0].y + dist*2-dist/2;

		return &trcFaceBak;
	}
	else
	{
		return NULL;
	}

/*	trcFaceBak.left = -1;

	tpFaceDetection->SetImageData( pGrayImage );
	tpFaceRectList = tpFaceDetection->GetFaceRegion();

	if( tpFaceRectList->FaceNum > 0 )
	{
		trcFaceBak = tpFaceRectList->facerect[0];
		for (int i = 1; i < tpFaceRectList->FaceNum; i++)
		{
			if (trcFaceBak.right - trcFaceBak.left < 
				tpFaceRectList->facerect[i].right - tpFaceRectList->facerect[i].left)
			{
				trcFaceBak = tpFaceRectList->facerect[i];
			}
		}
		return &trcFaceBak;
	}
	else
	{
		return NULL;
	}*/
}


void * CFaceTrack::getDetector()
{

	return adFacedetector;
}
