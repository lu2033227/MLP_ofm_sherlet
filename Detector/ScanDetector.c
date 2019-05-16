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

#include "BBS_Functions.h"
#include "Math.h"
#include "ScanDetector.h"

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

void bbf_ScanDetector_init( struct bbf_ScanDetector* ptrA )
{
	uint32 iL;

	ptrA->minScaleE = 0;
	ptrA->maxScaleE = 0;
	ptrA->maxImageWidthE = 0;
	ptrA->maxImageHeightE = 0;
	bbf_Scanner_init( &ptrA->scannerE );

	ptrA->patchWidthE = 0;
	ptrA->patchHeightE = 0;
	ptrA->minDefScaleE = 0;
	ptrA->maxDefScaleE = 0;
	ptrA->scaleStepE = 0;
	ptrA->overlapThrE = 0;
	ptrA->borderWidthE = 0;
	ptrA->borderHeightE = 0;
	ptrA->featuresE = 0;
	for( iL = 0; iL < bbf_SCAN_DETECTOR_MAX_FEATURES; iL++ ) 
		bbf_BitParam_init( &ptrA->bitParamArrE[ iL ] );
	for( iL = 0; iL < bbf_SCAN_DETECTOR_MAX_FEATURES; iL++ ) 
		bbf_Sequence_init( &ptrA->featureArrE[ iL ] );
	bts_IdCluster2D_init( &ptrA->refClusterE );
	ptrA->refDistanceE = 10;
}

/* ------------------------------------------------------------------------- */

void bbf_ScanDetector_exit( struct bbf_ScanDetector* ptrA )
{
	uint32 iL;

	ptrA->minScaleE = 0;
	ptrA->maxScaleE = 0;
	ptrA->maxImageWidthE = 0;
	ptrA->maxImageHeightE = 0;
	bbf_Scanner_exit( &ptrA->scannerE );

	ptrA->patchWidthE = 0;
	ptrA->patchHeightE = 0;
	ptrA->minDefScaleE = 0;
	ptrA->maxDefScaleE = 0;
	ptrA->scaleStepE = 0;
	ptrA->overlapThrE = 0;
	ptrA->borderWidthE = 0;
	ptrA->borderHeightE = 0;
	ptrA->featuresE = 0;
	for( iL = 0; iL < bbf_SCAN_DETECTOR_MAX_FEATURES; iL++ ) 
		bbf_BitParam_exit( &ptrA->bitParamArrE[ iL ] );
	for( iL = 0; iL < bbf_SCAN_DETECTOR_MAX_FEATURES; iL++ ) 
		bbf_Sequence_exit( &ptrA->featureArrE[ iL ] );
	bts_IdCluster2D_exit( &ptrA->refClusterE );
	ptrA->refDistanceE = 0;
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
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bbf_ScanDetector_memSize( const struct bbf_ScanDetector* ptrA )
{
	uint32 iL;
	uint32 memSizeL = bbs_SIZEOF16( uint32 ) +
					  bbs_SIZEOF16( uint32 ); /* version */

	memSizeL += bbs_SIZEOF16( ptrA->patchWidthE );
	memSizeL += bbs_SIZEOF16( ptrA->patchHeightE );
	memSizeL += bbs_SIZEOF16( ptrA->minDefScaleE );
	memSizeL += bbs_SIZEOF16( ptrA->maxDefScaleE );
	memSizeL += bbs_SIZEOF16( ptrA->scaleStepE );
	memSizeL += bbs_SIZEOF16( ptrA->overlapThrE );
	memSizeL += bbs_SIZEOF16( ptrA->borderWidthE );
	memSizeL += bbs_SIZEOF16( ptrA->borderHeightE );
	memSizeL += bbs_SIZEOF16( ptrA->featuresE );
	for( iL = 0; iL < ptrA->featuresE; iL++ ) 
		memSizeL += bbf_BitParam_memSize( &ptrA->bitParamArrE[ iL ] );
	for( iL = 0; iL < ptrA->featuresE; iL++ ) 
		memSizeL += bbf_Sequence_memSize( &ptrA->featureArrE[ iL ] );
	memSizeL += bts_IdCluster2D_memSize( &ptrA->refClusterE );
	memSizeL += bbs_SIZEOF16( ptrA->refDistanceE );

	return memSizeL; 
}

/* ------------------------------------------------------------------------- */

uint32 bbf_ScanDetector_memRead( struct bbf_ScanDetector* ptrA, 
							     const uint16* memPtrA )
{
	uint32 iL;
	uint32 memSizeL, versionL;

	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bbf_SCAN_DETECTOR_VERSION, memPtrA );

	memPtrA += bbs_memRead32( &ptrA->patchWidthE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->patchHeightE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->minDefScaleE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->maxDefScaleE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->scaleStepE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->overlapThrE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->borderWidthE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->borderHeightE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->featuresE, memPtrA );
	for( iL = 0; iL < ptrA->featuresE; iL++ ) 
		memPtrA += bbf_BitParam_memRead( &ptrA->bitParamArrE[ iL ], memPtrA );
	for( iL = 0; iL < ptrA->featuresE; iL++ ) 
		memPtrA += bbf_Sequence_memRead( &ptrA->featureArrE[ iL ], memPtrA );
	memPtrA += bts_IdCluster2D_memRead( &ptrA->refClusterE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->refDistanceE, memPtrA );

	ptrA->minScaleE = ptrA->minDefScaleE;
	ptrA->maxScaleE = ptrA->maxDefScaleE;

	/* initialize scanner; be aware of shared memory settings(!) */
	{
		uint32 maxImageSizeL = ptrA->maxImageWidthE * ptrA->maxImageHeightE;

		/* estimate of maximal possible faces in image */
		uint32 maxFacesL = maxImageSizeL / ( 768 << 1 );

		uint32 maxRadiusL = 0;

		if( maxImageSizeL == 0 ) 
		{
			printf( ":\nMaximum image size was not defined (size variables must be set before calling _memRead)" );
			return memSizeL; 
		}

		for( iL = 0; iL < ptrA->featuresE; iL++ ) 
		{
			maxRadiusL = maxRadiusL > ptrA->bitParamArrE[ iL ].outerRadiusE ? maxRadiusL : ptrA->bitParamArrE[ iL ].outerRadiusE;
		}

		if( maxFacesL < 1 ) maxFacesL = 1;

		bbf_Scanner_create( &ptrA->scannerE, 
							ptrA->maxImageWidthE,
							ptrA->maxImageHeightE,
							maxRadiusL, 
							ptrA->patchWidthE,
							ptrA->patchHeightE,
							ptrA->minScaleE,
							ptrA->maxScaleE,
							ptrA->scaleStepE,
							ptrA->borderWidthE,
							ptrA->borderHeightE,
							maxFacesL * 20  /* bufferSizeA */ 
							);
	}

	return memSizeL;
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ exec functions } --------------------------------------------- */
/*                                                                           */
/* ========================================================================= */
	
/* ------------------------------------------------------------------------- */

uint32 bbf_ScanDetector_process( struct bbf_ScanDetector* ptrA,
							     const void* imagePtrA,
								 uint32 imageWidthA,
								 uint32 imageHeightA,
								 const struct bts_Int16Rect* roiPtrA,
								 int32** outArrPtrPtrA )
{
	/* best global values (used when no positives could be found) */
	int32 bestGlobalActL = ( int32 )0x80000000;
	int32 bestGlobalXL = 0;
	int32 bestGlobalYL = 0;
	uint32 bestGlobalScaleL = 0;

	struct bbf_Scanner* scannerPtrL = &ptrA->scannerE;

	scannerPtrL->minScaleE = ptrA->minScaleE;
	scannerPtrL->maxScaleE = ptrA->maxScaleE;

	*outArrPtrPtrA = NULL;

	if( ptrA->featuresE == 0 )
	{
		printf( "bbf_ScanDetector_process: detector has no features" );
		return 0;
	}

	if( imageWidthA > ptrA->maxImageWidthE || imageHeightA > ptrA->maxImageHeightE )
	{
		printf( "bbf_ScanDetector_process: images size exceeds preallocated size" );
		return 0;
	}

	/* resets output positions */
	bbf_Scanner_resetOutPos( scannerPtrL ); 

	/* assign image to scanner - reset scanner */
	bbf_Scanner_assign( scannerPtrL, imagePtrA, imageWidthA, imageHeightA, roiPtrA, &ptrA->bitParamArrE[ 0 ] );

	while( bbf_Scanner_positions( scannerPtrL ) > 0 )
	{
		int32 bestActL = ( int32 )0x80000000;
		uint32 bestIdxL = 0;
		uint32 bestLvlL = 0;
		uint32 iL;

		const struct bbf_Feature* featurePtrL = ( const struct bbf_Feature* )&ptrA->featureArrE[ 0 ];
		const struct bbf_BitParam* paramPtrL = &ptrA->bitParamArrE[ 0 ];
		bbf_Scanner_bitParam( scannerPtrL, paramPtrL );

		/* resets internal positions */
		bbf_Scanner_resetIntPos( scannerPtrL );

		do
		{
			int32 actL = featurePtrL->vpActivityE( featurePtrL, bbf_Scanner_getPatch( scannerPtrL ) );
			if( actL > 0 ) 
			{
				bbf_Scanner_addIntPos( scannerPtrL, bbf_Scanner_scanIndex( scannerPtrL ), actL );
			}
			
			if( actL > bestActL )
			{
				bestActL = actL;
				bestIdxL = bbf_Scanner_scanIndex( scannerPtrL );
			}
		}
		while( bbf_Scanner_next( scannerPtrL ) );

		for( iL = 1; iL < ptrA->featuresE; iL++ )
		{
			const struct bbf_Feature* featurePtrL = ( const struct bbf_Feature* )&ptrA->featureArrE[ iL ];
			const struct bbf_BitParam* paramPtrL = &ptrA->bitParamArrE[ iL ];
			uint32* idxArrL = scannerPtrL->idxArrE.arrPtrE;
			int32* actArrL = scannerPtrL->actArrE.arrPtrE;

			uint32 kL = 0;
			uint32 jL;

			if( scannerPtrL->intCountE == 0 ) break;
			bestActL = ( int32 )0x80000000;
			bbf_Scanner_bitParam( scannerPtrL, paramPtrL );

			for( jL = 0; jL < scannerPtrL->intCountE; jL++ )
			{
				int32 actL;
				bbf_Scanner_goToIndex( scannerPtrL, idxArrL[ jL ] );
				actL = featurePtrL->vpActivityE( featurePtrL, bbf_Scanner_getPatch( scannerPtrL ) );
				if( actL > 0 )
				{
					idxArrL[ kL ] = idxArrL[ jL ];
					actArrL[ kL ] = ( actArrL[ jL ] + actL ) >> 1;
					kL++;
				}

				if( actL > bestActL )
				{
					bestActL = actL;
					bestIdxL = idxArrL[ jL ];
					bestLvlL = iL;
				}
			}

			scannerPtrL->intCountE = kL;
		}

		if( scannerPtrL->intCountE == 0 )
		{
			int32 xL, yL;
			uint32 scaleL;

			/* 8.24 */
			int32 actL = ( bestActL >> 4 ) + ( ( ( int32 )( bestLvlL + 1 - ptrA->featuresE ) << 24 ) / ( int32 )ptrA->featuresE );

			/* 4.28 */
			actL <<= 4;

			bbf_Scanner_idxPos( scannerPtrL, bestIdxL, &xL, &yL, &scaleL );

			if( actL > bestGlobalActL )
			{
            	bestGlobalActL = actL;
				bestGlobalXL = xL;
				bestGlobalYL = yL;
				bestGlobalScaleL = scaleL;
			}
		}
		else
		{
			/* remove overlaps for current scale */
			bbf_Scanner_removeIntOverlaps( scannerPtrL, ptrA->overlapThrE );

			for( iL = 0; iL < scannerPtrL->intCountE; iL++ )
			{
				int32 xL, yL;
				uint32 scaleL;
				uint32* idxArrL = scannerPtrL->idxArrE.arrPtrE;
				int32* actArrL = scannerPtrL->actArrE.arrPtrE;

				int32 actL = actArrL[ iL ];
				bbf_Scanner_idxPos( scannerPtrL, idxArrL[ iL ], &xL, &yL, &scaleL );

				/* add external position */
				bbf_Scanner_addOutPos( scannerPtrL, xL, yL, scaleL, actL ); 
			}

			/* remove overlapping positions */
			bbf_Scanner_removeOutOverlaps( scannerPtrL, ptrA->overlapThrE ); 

		}

		if( !bbf_Scanner_nextScale( scannerPtrL ) ) break;
	}

	*outArrPtrPtrA = scannerPtrL->outArrE.arrPtrE;
	if( scannerPtrL->outCountE == 0 )
	{
		/* no positive activities found: store best negative activity */
		bbf_Scanner_addOutPos( scannerPtrL, bestGlobalXL, bestGlobalYL, bestGlobalScaleL, bestGlobalActL );
		return 0;
	}
	else
	{
		return scannerPtrL->outCountE;
	}
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */

