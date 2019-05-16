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
#include "FaceFinderRef.h"
#include "BPI_Functions.h"

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

void bpi_FaceFinderRef_init( struct bpi_FaceFinderRef* ptrA )
{
	bbs_UInt16Arr_init( &ptrA->objBufE );
	ptrA->faceFinderPtrE = NULL;

	ptrA->facesE = 0;
	ptrA->faceIndexE = 0;
}

/* ------------------------------------------------------------------------- */

void bpi_FaceFinderRef_exit( struct bpi_FaceFinderRef* ptrA )
{
	if( ptrA->faceFinderPtrE != NULL ) 
		bpi_BFFaceFinder_exit( ptrA->faceFinderPtrE );
	bbs_UInt16Arr_exit( &ptrA->objBufE );

	ptrA->facesE = 0;
	ptrA->faceIndexE = 0;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

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
	
uint32 bpi_FaceFinderRef_memSize( const struct bpi_FaceFinderRef* ptrA )
{
	uint32 memSizeL = 0;
	memSizeL += bbs_SIZEOF16( uint32 ); /* mem size */
	memSizeL += bbs_SIZEOF16( flag ); /* object presence flag */
	if( ptrA->faceFinderPtrE != NULL ) 
		memSizeL += bpi_BFFaceFinder_memSize( ptrA->faceFinderPtrE );
	memSizeL += bbs_SIZEOF16( uint16 ); /* csa */
	return memSizeL; 
}

/* ------------------------------------------------------------------------- */

uint32 bpi_FaceFinderRef_memRead( struct bpi_FaceFinderRef* ptrA, 
								  uint32 maxImageWidthA,
								  uint32 maxImageHeightA,
								  const uint16* memPtrA )
{
	uint32 memSizeL;
	flag objPresentL;

	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memRead32( &objPresentL, memPtrA );

	/* check object & allocate data buffer */
	{
		const uint16* memPtrL = memPtrA;
		uint32 dataSizeL = 0;

		if( objPresentL )
		{
			uint32 typeL = bbs_memPeek32( memPtrL + 4 );
			dataSizeL += bbs_SIZEOF16( struct bpi_BFFaceFinder );
			memPtrL += bbs_memPeek32( memPtrL );
		}

		bbs_UInt16Arr_create( &ptrA->objBufE, dataSizeL );
	}

	/* load object */
	{
		uint16* dataPtrL = ptrA->objBufE.arrPtrE;

		if( objPresentL )
		{
			uint32 typeL = bbs_memPeek32( memPtrA + 4 );
			ptrA->faceFinderPtrE = ( struct bpi_BFFaceFinder* )dataPtrL;
			bpi_BFFaceFinder_init( ptrA->faceFinderPtrE );
			bpi_BFFaceFinder_setParams( ptrA->faceFinderPtrE, maxImageWidthA, maxImageHeightA );
			memPtrA += bpi_BFFaceFinder_memRead( ptrA->faceFinderPtrE, memPtrA );
		}
		else
		{
			ptrA->faceFinderPtrE = NULL;
		}
	}

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

void bpi_FaceFinderRef_setRange( struct bpi_FaceFinderRef* ptrA, 
								 uint32 minEyeDistanceA,
								 uint32 maxEyeDistanceA )
{
	if( ptrA->faceFinderPtrE == NULL )
	{
		printf( ":\nNo face finder object was loaded" );
		return;
 	}
	bpi_BFFaceFinder_setRange( ptrA->faceFinderPtrE, minEyeDistanceA, maxEyeDistanceA );
}

/* ------------------------------------------------------------------------- */

int32 bpi_FaceFinderRef_process( struct bpi_FaceFinderRef* ptrA, 
								 struct bpi_DCR* dcrPtrA )
{
	int32 confL;

	if( ptrA->faceFinderPtrE == NULL )
	{
		printf( "\nNo face finder object was loaded" );
		return -1;
 	}

	confL = bpi_BFFaceFinder_processDcr( ptrA->faceFinderPtrE, dcrPtrA );

	dcrPtrA->confidenceE = confL;
	dcrPtrA->approvedE = confL > ( ( int32 )1 << 23 );

	ptrA->faceIndexE = 0;
	ptrA->facesE = 0;

	bts_IdCluster2D_copy( &dcrPtrA->mainClusterE, &dcrPtrA->mainClusterE );

	return 0;
}

/* ------------------------------------------------------------------------- */

int32 bpi_FaceFinderRef_putDcr( struct bpi_FaceFinderRef* ptrA, 
								struct bpi_DCR* dcrPtrA )
{
	if( ptrA->faceFinderPtrE == NULL )
	{
		printf( "\nNo face finder object was loaded" );
		return -1;
 	}

	ptrA->facesE =  bpi_BFFaceFinder_putDcr( ptrA->faceFinderPtrE, dcrPtrA );
	ptrA->faceIndexE = 0;

	return 0;
}

uint32 bpi_FaceFinder_faces( const struct bpi_FaceFinderRef* hFaceFinderA )
{
	if( hFaceFinderA == NULL )				
		return -1;

	return hFaceFinderA->facesE - hFaceFinderA->faceIndexE;
}

/* ------------------------------------------------------------------------- */

void bpi_FaceFinderRef_getDcr( struct bpi_FaceFinderRef* ptrA, 
							   uint32 indexA,
							   struct bpi_DCR* dcrPtrA )
{
	if( ptrA->faceFinderPtrE == NULL )
	{
		printf( "\nNo face finder object was loaded" );
		return;
 	}

	if( ptrA->faceIndexE < ptrA->facesE )
	{
		bpi_BFFaceFinder_getDcr( ptrA->faceFinderPtrE, indexA, dcrPtrA );
		dcrPtrA->approvedE = TRUE;
		ptrA->faceIndexE++;
	}
	else
	{
		bpi_BFFaceFinder_getDcr( ptrA->faceFinderPtrE, 0, dcrPtrA );
		dcrPtrA->approvedE = FALSE;
	}
}


/* ------------------------------------------------------------------------- */

/* ========================================================================= */

