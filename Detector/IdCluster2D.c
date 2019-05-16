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

#include "Math.h"
#include "IdCluster2D.h"

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

void bts_IdCluster2D_init( struct bts_IdCluster2D* ptrA )
{
	bts_Cluster2D_init( &ptrA->clusterE );
	bbs_Int16Arr_init( &ptrA->idArrE );
}

/* ------------------------------------------------------------------------- */

void bts_IdCluster2D_exit( struct bts_IdCluster2D* ptrA )
{
	bts_Cluster2D_exit( &ptrA->clusterE );
	bbs_Int16Arr_exit( &ptrA->idArrE );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bts_IdCluster2D_copy( struct bts_IdCluster2D* ptrA, 
						   const struct bts_IdCluster2D* srcPtrA )
{
	bts_Cluster2D_copy( &ptrA->clusterE, &srcPtrA->clusterE );
	bbs_Int16Arr_copy( &ptrA->idArrE, &srcPtrA->idArrE );
}

/* ------------------------------------------------------------------------- */

flag bts_IdCluster2D_equal( const struct bts_IdCluster2D* ptrA, 
							const struct bts_IdCluster2D* srcPtrA )
{
	if( !bts_Cluster2D_equal( &ptrA->clusterE, &srcPtrA->clusterE ) ) 
		return FALSE;
	if( !bbs_Int16Arr_equal( &ptrA->idArrE, &srcPtrA->idArrE ) ) 
		return FALSE;
	return TRUE;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

struct bts_Flt16Vec2D bts_IdCluster2D_center( const struct bts_IdCluster2D* ptrA )
{
	return bts_Cluster2D_center( &ptrA->clusterE );
}

/* ------------------------------------------------------------------------- */

struct bts_Int16Rect bts_IdCluster2D_boundingBox( const struct bts_IdCluster2D* ptrA )
{
	return bts_Cluster2D_boundingBox( &ptrA->clusterE );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bts_IdCluster2D_create( struct bts_IdCluster2D* ptrA, 
							 uint32 sizeA )
{
	bts_Cluster2D_create( &ptrA->clusterE, sizeA );
	bbs_Int16Arr_create( &ptrA->idArrE, sizeA );
}

/* ------------------------------------------------------------------------- */
	
void bts_IdCluster2D_size( struct bts_IdCluster2D* ptrA, 
						   uint32 sizeA )
{
	bts_Cluster2D_size( &ptrA->clusterE, sizeA );
	bbs_Int16Arr_size( &ptrA->idArrE, sizeA );
}

/* ------------------------------------------------------------------------- */
	
void bts_IdCluster2D_transform( struct bts_IdCluster2D* ptrA, 
								struct bts_Flt16Alt2D altA )
{
	bts_Cluster2D_transform( &ptrA->clusterE, altA );
}

/* ------------------------------------------------------------------------- */
	
void bts_IdCluster2D_copyTransform( struct bts_IdCluster2D* ptrA, 
									const struct bts_IdCluster2D* srcPtrA, 
									struct bts_Flt16Alt2D altA, 
									uint32 dstBbpA )
{
	bts_Cluster2D_copyTransform( &ptrA->clusterE, &srcPtrA->clusterE, altA, dstBbpA );
	bbs_Int16Arr_copy( &ptrA->idArrE, &srcPtrA->idArrE );
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bts_IdCluster2D_memSize( const struct bts_IdCluster2D *ptrA )
{
	return  bbs_SIZEOF16( uint32 )
		  + bbs_SIZEOF16( uint32 ) /* version */
		  +	bts_Cluster2D_memSize( &ptrA->clusterE )
		  + bbs_Int16Arr_memSize( &ptrA->idArrE );
}

/* ------------------------------------------------------------------------- */
	
uint32 bts_IdCluster2D_memRead( struct bts_IdCluster2D* ptrA, 
							    const uint16* memPtrA )
{
	uint32 memSizeL;
	uint32 versionL;

	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bts_ID_CLUSTER2D_VERSION, memPtrA );
	memPtrA += bts_Cluster2D_memRead( &ptrA->clusterE, memPtrA ); 
	memPtrA += bbs_Int16Arr_memRead( &ptrA->idArrE, memPtrA );
	if( memSizeL != bts_IdCluster2D_memSize( ptrA ) )
	{
		printf( "uint32 bts_IdCluster2D_memRead( const struct bts_IdCluster2D* ptrA, const void* memPtrA ):\n"
                   "size mismatch" ); 
		return 0;
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

void bts_IdCluster2D_convertToEqivalentClusters( const struct bts_IdCluster2D* inCluster1PtrA,
												 const struct bts_IdCluster2D* inCluster2PtrA,
												 struct bts_Cluster2D* outCluster1PtrA,
												 struct bts_Cluster2D* outCluster2PtrA )
{
	uint32 iL, jL;
	uint32 countL = 0;

	uint32 size1L = inCluster1PtrA->clusterE.sizeE;
	uint32 size2L = inCluster2PtrA->clusterE.sizeE;

	const int16* idArr1L = inCluster1PtrA->idArrE.arrPtrE;
	const int16* idArr2L = inCluster2PtrA->idArrE.arrPtrE;

	const struct bts_Int16Vec2D* srcVecArr1E = inCluster1PtrA->clusterE.vecArrE;
	const struct bts_Int16Vec2D* srcVecArr2E = inCluster2PtrA->clusterE.vecArrE;

	struct bts_Int16Vec2D* dstVecArr1E = outCluster1PtrA->vecArrE;
	struct bts_Int16Vec2D* dstVecArr2E = outCluster2PtrA->vecArrE;

	uint32 maxOutSizeL = bbs_min( outCluster1PtrA->allocatedSizeE, outCluster2PtrA->allocatedSizeE );
	bts_Cluster2D_size( outCluster1PtrA, maxOutSizeL );
	bts_Cluster2D_size( outCluster2PtrA, maxOutSizeL );

	for( iL = 0; iL < size1L; iL++ )
	{
		int32 idL = idArr1L[ iL ];
		if( idL >= 0 )
		{
			for( jL = 0; jL < size2L; jL++ )
			{
				if( idL == idArr2L[ jL ] ) break;
			}

			if( jL < size2L )
			{
				if( countL == maxOutSizeL )
				{
					printf( "void bts_IdCluster2D_convertToEqivalentClusters( .... ):\n"
						       "Destination clusters are insufficiently allocated" );
					return;
				}

				dstVecArr1E[ countL ] = srcVecArr1E[ iL ];
				dstVecArr2E[ countL ] = srcVecArr2E[ jL ];
				countL++;
			}
		}
	}

	bts_Cluster2D_size( outCluster1PtrA, countL );
	bts_Cluster2D_size( outCluster2PtrA, countL );

	outCluster1PtrA->bbpE = inCluster1PtrA->clusterE.bbpE;
	outCluster2PtrA->bbpE = inCluster2PtrA->clusterE.bbpE;
}

/* ------------------------------------------------------------------------- */

struct bts_Flt16Alt2D bts_IdCluster2D_alt( const struct bts_IdCluster2D* srcPtrA,
										   struct bts_IdCluster2D* dstPtrA,
										   enum bts_AltType altTypeA,
										   struct bts_Cluster2D* tmpPtr1A,  /* temporary cluster 1 */
										   struct bts_Cluster2D* tmpPtr2A ) /* temporary cluster 2 */
{
	bts_IdCluster2D_convertToEqivalentClusters( srcPtrA, dstPtrA, tmpPtr1A, tmpPtr2A );
	return bts_Cluster2D_alt( tmpPtr1A, tmpPtr2A, altTypeA );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */


