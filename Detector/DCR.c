/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ---- includes ----------------------------------------------------------- */

#include "DCR.h"
#include "BBS_Functions.h"

#include <stdio.h>

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ auxiliary functions } ---------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ constructor / destructor } ----------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bpi_DCR_init( struct bpi_DCR* ptrA )
{
	ptrA->maxImageWidthE = 0;
	ptrA->maxImageHeightE = 0;
	ptrA->imageDataPtrE = NULL;
	ptrA->imageWidthE = 0;
	ptrA->imageHeightE = 0;
	bts_Int16Vec2D_init( &ptrA->offsE );
	bts_IdCluster2D_init( &ptrA->mainClusterE );
	ptrA->confidenceE = 0;
	ptrA->approvedE = FALSE;
	ptrA->idE = 0;
	ptrA->roiRectE = bts_Int16Rect_create( 0, 0, 0, 0 );
}

/* ------------------------------------------------------------------------- */

void bpi_DCR_exit( struct bpi_DCR* ptrA )
{
	ptrA->maxImageWidthE = 0;
	ptrA->maxImageHeightE = 0;
	ptrA->imageDataPtrE = NULL;
	ptrA->imageWidthE = 0;
	ptrA->imageHeightE = 0;
	bts_Int16Vec2D_exit( &ptrA->offsE );
	bts_IdCluster2D_exit( &ptrA->mainClusterE );
	ptrA->confidenceE = 0;
	ptrA->approvedE = FALSE;
	ptrA->idE = 0;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bpi_DCR_create( struct bpi_DCR* ptrA,
					 uint32 imageWidthA,
					 uint32 imageHeightA)
{
	bpi_DCR_init(ptrA);

	bts_IdCluster2D_create( &ptrA->mainClusterE, bpi_DCR_MAX_CLUSTER_SIZE );
	bts_IdCluster2D_size( &ptrA->mainClusterE, 0 );

	ptrA->maxImageWidthE = imageWidthA;
	ptrA->maxImageHeightE = imageHeightA;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ exec functions } --------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bpi_DCR_assignGrayByteImage( struct bpi_DCR* ptrA, 
								  const void* bufferPtrA, 
								  uint32 widthA, 
								  uint32 heightA )
{
	if( widthA > ptrA->maxImageWidthE || heightA > ptrA->maxImageHeightE )
	{
		return;
	}

	if( ( widthA & 1 ) != 0 )
	{
		return;
	}

	ptrA->imageDataPtrE = ( void* )bufferPtrA;
	ptrA->imageWidthE = widthA;
	ptrA->imageHeightE = heightA;

	/* reset some data */
	ptrA->roiRectE = bts_Int16Rect_create( 0, 0, widthA, heightA );
	bts_IdCluster2D_size( &ptrA->mainClusterE, 0 );
}

/* ------------------------------------------------------------------------- */

void bpi_DCR_assignGrayByteImageROI( struct bpi_DCR* ptrA, 
									 const void* bufferPtrA, 
									 uint32 widthA, 
									 uint32 heightA,
									 const struct bts_Int16Rect* pRectA )
{
	if( widthA > ptrA->maxImageWidthE || heightA > ptrA->maxImageHeightE )
	{
		printf( "%s:\nSize of assigned image (%ix%i) exceeds maximum size defined at DCR initialization (%ix%i).",
				    "void bpi_DCR_assignGrayByteImageROI( struct bpi_DCR* ptrA, const void* bufferPtrA, uint32 widthA, uint32 heightA )",
					widthA, heightA,
					ptrA->maxImageWidthE, ptrA->maxImageHeightE	);
		return;
	}

	if( ( widthA & 1 ) != 0 )
	{
		printf( "%s:\nWidth of image must be even.\n",
				    "void bpi_DCR_assignGrayByteImageROI( struct bpi_DCR* ptrA, const void* bufferPtrA, uint32 widthA, uint32 heightA )" );
		return;
	}

	if( pRectA->x2E < pRectA->x1E || pRectA->y2E < pRectA->y1E || 
		pRectA->x1E < 0           || pRectA->y1E < 0 ||
		pRectA->x2E > ( int32 )widthA || pRectA->y2E > ( int32 )heightA )
	{
		printf( "%s:\nInvalid ROI rectangle.\n", "void bpi_DCR_assignGrayByteImageROI( struct bpi_DCR* ptrA, const void* bufferPtrA, uint32 widthA, uint32 heightA )" );
		return;
	}

	ptrA->imageDataPtrE = ( void* )bufferPtrA;
	ptrA->imageWidthE = widthA;
	ptrA->imageHeightE = heightA;

	/* reset some data */
	ptrA->roiRectE = *pRectA;
	bts_IdCluster2D_size( &ptrA->mainClusterE, 0 );
}

/* ------------------------------------------------------------------------- */

int32 bpi_DCR_confidence( const struct bpi_DCR* ptrA )
{
	return ptrA->confidenceE;
}

/* ------------------------------------------------------------------------- */

u32 bpi_DCR_nodeCount( const struct bpi_DCR* hdcrA )
{
	if( hdcrA == NULL )					
		return 0;

	return hdcrA->mainClusterE.clusterE.sizeE;
}

/* ------------------------------------------------------------------------- */

int32 bpi_DCR_getNode( const struct bpi_DCR* hdcrA, 
						   u32 indexA, 
						   bpi_Node* nodePtrA )
{
	if( hdcrA == NULL )					
		return -1;

	if( indexA >= hdcrA->mainClusterE.clusterE.sizeE )
		return -2;

	nodePtrA->id = hdcrA->mainClusterE.idArrE.arrPtrE[ indexA ];
	nodePtrA->x  = ( ( s16p16 )hdcrA->mainClusterE.clusterE.vecArrE[ indexA ].xE ) << ( 16 - hdcrA->mainClusterE.clusterE.bbpE );
	nodePtrA->y  = ( ( s16p16 )hdcrA->mainClusterE.clusterE.vecArrE[ indexA ].yE ) << ( 16 - hdcrA->mainClusterE.clusterE.bbpE );

	// adjust according to roi
	if( hdcrA->roiRectE.x1E > 0 ) 
		nodePtrA->x += ( int32 )hdcrA->roiRectE.x1E << 16;
	if( hdcrA->roiRectE.y1E > 0 ) 
		nodePtrA->y += ( int32 )hdcrA->roiRectE.y1E << 16;

	nodePtrA->x += ( int32 )hdcrA->offsE.xE << 16;
	nodePtrA->y += ( int32 )hdcrA->offsE.yE << 16;

	return 0;
}

/* ------------------------------------------------------------------------- */

int32 bpi_DCR_getRect( const struct bpi_DCR* hdcrA, 
						   bpi_Rect* pRectA )
{
	if( hdcrA == NULL )					
		return -1;

	/* find eye nodes */
	{
		const struct bbs_Int16Arr* pIdArrL = &hdcrA->mainClusterE.idArrE;
		int32 lIndexL = -1;
		int32 rIndexL = -1;
		uint32 iL;
		for( iL = 0; iL < pIdArrL->sizeE; iL++ )
		{
			if( pIdArrL->arrPtrE[ iL ] == 0 )
			{
				lIndexL = iL;
			}
			else if( pIdArrL->arrPtrE[ iL ] == 1 )
			{
				rIndexL = iL;
			}
		}

		if( lIndexL == -1 || rIndexL == -1 )
		{
			return -2;
		}

		{
			int32 bbpL = hdcrA->mainClusterE.clusterE.bbpE; 
			int32 lxL = ( hdcrA->mainClusterE.clusterE.vecArrE[ lIndexL ].xE + ( 1 << ( bbpL - 1 ) ) ) >> bbpL;
			int32 lyL = ( hdcrA->mainClusterE.clusterE.vecArrE[ lIndexL ].yE + ( 1 << ( bbpL - 1 ) ) ) >> bbpL;
			int32 rxL = ( hdcrA->mainClusterE.clusterE.vecArrE[ rIndexL ].xE + ( 1 << ( bbpL - 1 ) ) ) >> bbpL;
			int32 ryL = ( hdcrA->mainClusterE.clusterE.vecArrE[ rIndexL ].yE + ( 1 << ( bbpL - 1 ) ) ) >> bbpL;
			int32 doffL = ( rxL - lxL ) >> 1;

			pRectA->xMin = ( lxL - doffL ) << 16;
			pRectA->xMax = ( rxL + doffL ) << 16;
			pRectA->yMin = ( ( ( lyL + ryL + 1 ) >> 1 ) - doffL ) << 16;
			pRectA->yMax = ( pRectA->yMin + ( pRectA->xMax - pRectA->xMin ) );
			if( hdcrA->roiRectE.x1E > 0 ) 
			{	
				pRectA->xMin += ( int32 )hdcrA->roiRectE.x1E << 16;
				pRectA->xMax += ( int32 )hdcrA->roiRectE.x1E << 16;
			}
			if( hdcrA->roiRectE.y1E > 0 ) 
			{
				pRectA->yMin += ( int32 )hdcrA->roiRectE.y1E << 16;
				pRectA->yMax += ( int32 )hdcrA->roiRectE.y1E << 16;
			}

			pRectA->xMin += ( int32 )hdcrA->offsE.xE << 16;
			pRectA->yMin += ( int32 )hdcrA->offsE.yE << 16;
			pRectA->xMax += ( int32 )hdcrA->offsE.xE << 16;
			pRectA->yMax += ( int32 )hdcrA->offsE.yE << 16;

		}
	}

	return 0;
}

/* ------------------------------------------------------------------------- */

u32 bpi_DCR_approved( const struct bpi_DCR* hdcrA )
{
	if( hdcrA == NULL )					
		return 0;

	return ( u32 )hdcrA->approvedE;
}

/* ========================================================================= */
