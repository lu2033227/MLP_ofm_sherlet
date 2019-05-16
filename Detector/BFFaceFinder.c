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

#include "BFFaceFinder.h"
#include "BPI_Functions.h"
#include "DCR.h"
#include "BBS_Functions.h"
#include "Math.h"

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

void bpi_BFFaceFinder_init( struct bpi_BFFaceFinder* ptrA )
{
	ptrA->typeE = 0;
	ptrA->detectedFacesE = 0;
	ptrA->availableFacesE = 0;
	ptrA->faceDataBufferE = NULL;
	bbf_ScanDetector_init( &ptrA->detectorE );
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_exit( struct bpi_BFFaceFinder* ptrA )
{
	ptrA->typeE = 0;
	ptrA->detectedFacesE = 0;
	ptrA->availableFacesE = 0;
	ptrA->faceDataBufferE = NULL;
	bbf_ScanDetector_exit( &ptrA->detectorE );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint32 bpi_BFFaceFinder_getMinEyeDistance( const struct bpi_BFFaceFinder* ptrA )
{
	return ( ( ptrA->detectorE.refDistanceE >> 8 ) * ( ptrA->detectorE.minScaleE >> 12 ) ) >> 16;
}

/* ------------------------------------------------------------------------- */

uint32 bpi_BFFaceFinder_getMaxEyeDistance( const struct bpi_BFFaceFinder* ptrA )
{
	return ( ( ptrA->detectorE.refDistanceE >> 8 ) * ( ptrA->detectorE.maxScaleE >> 12 ) ) >> 16;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bpi_BFFaceFinder_setMinEyeDistance( struct bpi_BFFaceFinder* ptrA, 
										 uint32 distA )
{
	ptrA->detectorE.minScaleE = ( ( distA << 16 ) / ( ptrA->detectorE.refDistanceE >> 8 ) ) << 12;
	if( ptrA->detectorE.minScaleE < 0x100000 /* 1.0 */ ) 
		ptrA->detectorE.minScaleE = 0x100000;
}

/* ------------------------------------------------------------------------- */
	
void bpi_BFFaceFinder_setMaxEyeDistance( struct bpi_BFFaceFinder* ptrA, 
										 uint32 distA )
{
	if( distA > 0x0FFFF )
	{
		ptrA->detectorE.maxScaleE = 0; /* unlimited */
	}
	else
	{
		ptrA->detectorE.maxScaleE = ( ( distA << 16 ) / ( ptrA->detectorE.refDistanceE >> 8 ) ) << 12;
	}
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bpi_BFFaceFinder_memSize( const struct bpi_BFFaceFinder *ptrA )
{
	uint32 memSizeL = 0;
	memSizeL += bbs_SIZEOF16( uint32 );
	memSizeL += bbs_SIZEOF16( uint32 ); /* version */
	memSizeL += bbs_SIZEOF16( ptrA->typeE );
	memSizeL += bbf_ScanDetector_memSize( &ptrA->detectorE );
	memSizeL += bbs_SIZEOF16( uint16 ); /* csa */
	return memSizeL;
}

/* ------------------------------------------------------------------------- */
	
uint32 bpi_BFFaceFinder_memRead( struct bpi_BFFaceFinder* ptrA, 
								 const uint16* memPtrA )
{
	uint32 memSizeL, versionL;

	// read face detector
	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bpi_BF_FACE_FINDER_VERSION, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->typeE, memPtrA );

	memPtrA += bbf_ScanDetector_memRead( &ptrA->detectorE, memPtrA );
	memPtrA += bpi_memReadCsa16( memPtrA );

	return memSizeL;
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ exec functions } --------------------------------------------- */
/*                                                                           */
/* ========================================================================= */
	
/* ------------------------------------------------------------------------- */

int32 bpi_BFFaceFinder_process( const struct bpi_BFFaceFinder* ptrA, 
								void* imagePtrA,
								uint32 imageWidthA,
								uint32 imageHeightA,
								const struct bts_Int16Rect* roiPtrA,
								struct bts_Int16Vec2D* offsPtrA,
								struct bts_IdCluster2D* idClusterPtrA )
{
	int32 xL = 0; /* 16.16 */
	int32 yL = 0; /* 16.16 */
	uint32 scaleL = 0;
	int32 actL = 0;
	int32* outArrL;

	struct bts_Flt16Alt2D altL;
	struct bts_Flt16Vec2D centerL;

	struct bpi_BFFaceFinder* ptrL = ( struct bpi_BFFaceFinder* )ptrA;

	/* reset multi face imformation so they are not accidentally used */
	ptrL->detectedFacesE = 0;
	ptrL->availableFacesE = 0;
	ptrL->faceDataBufferE = NULL;

	bbf_ScanDetector_process( ( struct bbf_ScanDetector* )&ptrA->detectorE, imagePtrA, imageWidthA, imageHeightA, roiPtrA, &outArrL );

	xL      = outArrL[ 0 ]; /* 16.16 */
	yL      = outArrL[ 1 ]; /* 16.16 */
	scaleL  = outArrL[ 2 ]; /* 12.20 */
	actL    = outArrL[ 3 ]; /*  4.28 */

	offsPtrA->xE = xL >> 16;
	offsPtrA->yE = yL >> 16;
	xL -= ( ( int32 )offsPtrA->xE << 16 );
	yL -= ( ( int32 )offsPtrA->yE << 16 );

	centerL = bts_Flt16Vec2D_create32( 0, 0, 0 );
	altL = bts_Flt16Alt2D_createScale( scaleL, 20, &centerL );
	altL.vecE = bts_Flt16Vec2D_create32( xL, yL, 16 );

	/* compute cluster */
	{
		uint32 eyeDistL = ( ( ptrA->detectorE.refDistanceE >> 16 ) * scaleL ) >> 20;
		uint32 logEyeDistL = bbs_intLog2( eyeDistL );
		int32 bbpL = 11 - logEyeDistL;
		bbpL = bbpL < 0 ? 0 : bbpL;
		bbpL = bbpL > 6 ? 6 : bbpL;
		bts_IdCluster2D_copyTransform( idClusterPtrA, &ptrA->detectorE.refClusterE, altL, bbpL );
	}


	return ( actL + 0x10000000 ) >> 5; /*output range 0...1 in 8.24*/
}

/* ------------------------------------------------------------------------- */

uint32 bpi_BFFaceFinder_multiProcess( const struct bpi_BFFaceFinder* ptrA, 
									  void* imagePtrA,
									  uint32 imageWidthA,
									  uint32 imageHeightA,
									  const struct bts_Int16Rect* roiPtrA )
{
	struct bpi_BFFaceFinder* ptrL = ( struct bpi_BFFaceFinder* )ptrA;
	ptrL->detectedFacesE = bbf_ScanDetector_process( ( struct bbf_ScanDetector* )&ptrA->detectorE, imagePtrA, imageWidthA, imageHeightA, roiPtrA, &ptrL->faceDataBufferE );
	ptrL->availableFacesE = ptrA->detectedFacesE > 0 ? ptrA->detectedFacesE : 1;

	return ptrL->detectedFacesE;
}

/* ------------------------------------------------------------------------- */

uint32 bpi_BFFaceFinder_getFace( const struct bpi_BFFaceFinder* ptrA, 
								 uint32 indexA,
								 struct bts_Int16Vec2D* offsPtrA,
								 struct bts_IdCluster2D* idClusterPtrA )
{
	int32 xL = 0; /* 16.16 */
	int32 yL = 0; /* 16.16 */
	uint32 scaleL = 0;
	int32 actL = 0;
	struct bts_Flt16Alt2D altL;
	struct bts_Flt16Vec2D centerL;

	if( ptrA->availableFacesE == 0 || ptrA->faceDataBufferE == NULL ) 
	{
		printf( "%s:\nNo faces are availabe. This function was called before the face finder could detect multiple faces in an image", "bpi_BFFaceFinder_getFace" );
		return 0;
	}

	if( indexA >= ptrA->availableFacesE ) 
	{
		printf( "%s:\nface index exceeds number of available faces", "bpi_BFFaceFinder_getFace" );
		return 0;
	}

	xL      = ptrA->faceDataBufferE[ indexA * 4 + 0 ]; /* 16.16 */
	yL      = ptrA->faceDataBufferE[ indexA * 4 + 1 ]; /* 16.16 */
	scaleL  = ptrA->faceDataBufferE[ indexA * 4 + 2 ]; /* 12.20 */
	actL    = ptrA->faceDataBufferE[ indexA * 4 + 3 ]; /*  4.28 */

	offsPtrA->xE = xL >> 16;
	offsPtrA->yE = yL >> 16;

	xL -= ( ( int32 )offsPtrA->xE << 16 );
	yL -= ( ( int32 )offsPtrA->yE << 16 );

	centerL = bts_Flt16Vec2D_create32( 0, 0, 0 );
	altL = bts_Flt16Alt2D_createScale( scaleL, 20, &centerL );
	altL.vecE = bts_Flt16Vec2D_create32( xL, yL, 16 );

	/* compute cluster */
	{
		uint32 eyeDistL = ( ( ptrA->detectorE.refDistanceE >> 16 ) * scaleL ) >> 20;
		uint32 logEyeDistL = bbs_intLog2( eyeDistL );
		int32 bbpL = 11 - logEyeDistL;
		bbpL = bbpL < 0 ? 0 : bbpL;
		bbpL = bbpL > 6 ? 6 : bbpL;
		bts_IdCluster2D_copyTransform( idClusterPtrA, &ptrA->detectorE.refClusterE, altL, bbpL );
	}

	return ( actL + 0x10000000 ) >> 5; /*output range 0...1 in 8.24*/
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_getFaceDCR( const struct bpi_BFFaceFinder* ptrA, 
								  uint32 indexA,
								  struct bpi_DCR* dcrPtrA )
{
	dcrPtrA->confidenceE = bpi_BFFaceFinder_getFace( ptrA, indexA, &dcrPtrA->offsE, &dcrPtrA->mainClusterE );
	dcrPtrA->approvedE = dcrPtrA->confidenceE > ( ( int32 )1 << 23 );
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_setMaxImageSize( struct bpi_BFFaceFinder* ptrA, 
									   uint32 maxImageWidthA,
									   uint32 maxImageHeightA )
{
	ptrA->detectorE.maxImageWidthE = maxImageWidthA;
	ptrA->detectorE.maxImageHeightE = maxImageHeightA;
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_setParams( struct bpi_BFFaceFinder* ptrA, 
								 uint32 maxImageWidthA,
								 uint32 maxImageHeightA )
{
	bpi_BFFaceFinder_setMaxImageSize( ptrA, maxImageWidthA, maxImageHeightA );
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_setRange( struct bpi_BFFaceFinder* ptrA, 
								uint32 minEyeDistanceA,
								uint32 maxEyeDistanceA )
{
	if( ptrA->typeE != 1 ) 
	{
		printf( "%s:\nObject type mismatch", "bpi_BFFaceFinder_setParams" );
		return;
	}
	bpi_BFFaceFinder_setMinEyeDistance( ptrA, minEyeDistanceA );
	bpi_BFFaceFinder_setMaxEyeDistance( ptrA, maxEyeDistanceA );
}

/* ------------------------------------------------------------------------- */

int32 bpi_BFFaceFinder_processDcr( const struct bpi_BFFaceFinder* ptrA, 
						           struct bpi_DCR* dcrPtrA )
{
	if( ptrA->typeE != 1 ) 
	{
		printf( ":\nObject type mismatch" );
		return 0;
	}

	return bpi_BFFaceFinder_process( 
									ptrA, 
									dcrPtrA->imageDataPtrE,
									dcrPtrA->imageWidthE,
									dcrPtrA->imageHeightE,
									&dcrPtrA->roiRectE,
									&dcrPtrA->offsE,
									&dcrPtrA->mainClusterE );
}

/* ------------------------------------------------------------------------- */

int32 bpi_BFFaceFinder_putDcr( const struct bpi_BFFaceFinder* ptrA, 
							   struct bpi_DCR* dcrPtrA )
{
	if( ptrA->typeE != 1 ) 
	{
		printf( "\nObject type mismatch" );
		return 0;
	}

	return bpi_BFFaceFinder_multiProcess(  
										 ptrA, 
										 dcrPtrA->imageDataPtrE,
										 dcrPtrA->imageWidthE,
										 dcrPtrA->imageHeightE,
										 &dcrPtrA->roiRectE );
}

/* ------------------------------------------------------------------------- */

void bpi_BFFaceFinder_getDcr( const struct bpi_BFFaceFinder* ptrA, 
							  uint32 indexA,
							  struct bpi_DCR* dcrPtrA )
{
	if( ptrA->typeE != 1 ) 
	{
		printf( "\nObject type mismatch");
		return;
	}

	bpi_BFFaceFinder_getFaceDCR( ptrA, indexA, dcrPtrA );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
