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
#include "UInt32Arr.h"

#include <stdlib.h>
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

void bbs_UInt32Arr_init( struct bbs_UInt32Arr* ptrA )
{
	ptrA->arrPtrE = NULL;
	ptrA->sizeE = 0;
	ptrA->allocatedSizeE = 0;
}

/* ------------------------------------------------------------------------- */

void bbs_UInt32Arr_exit( struct bbs_UInt32Arr* ptrA )
{
	free( ptrA->arrPtrE );
	ptrA->arrPtrE = NULL;
	ptrA->sizeE = 0;
	ptrA->allocatedSizeE = 0;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bbs_UInt32Arr_copy( struct bbs_UInt32Arr* ptrA, 
						 const struct bbs_UInt32Arr* srcPtrA )
{
	bbs_UInt32Arr_size( ptrA, srcPtrA->sizeE );
	bbs_memcpy32( ptrA->arrPtrE, srcPtrA->arrPtrE, srcPtrA->sizeE * bbs_SIZEOF32( uint32 ) ); 
}

/* ------------------------------------------------------------------------- */

flag bbs_UInt32Arr_equal( const struct bbs_UInt32Arr* ptrA, 
						  const struct bbs_UInt32Arr* srcPtrA )
{
	uint32 iL;
	const uint32* ptr1L = ptrA->arrPtrE;
	const uint32* ptr2L = srcPtrA->arrPtrE;
	if( ptrA->sizeE != srcPtrA->sizeE ) return FALSE;
	for( iL = ptrA->sizeE; iL > 0; iL-- )
	{
		if( *ptr1L++ != *ptr2L++ ) return FALSE;
	}
	return TRUE;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint32 bbs_UInt32Arr_heapSize( const struct bbs_UInt32Arr* ptrA, 
							   uint32 sizeA )
{
	return sizeA * bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bbs_UInt32Arr_create( struct bbs_UInt32Arr* ptrA, 
						  uint32 sizeA )
{
	if( ptrA->sizeE == sizeA ) 
		return;

	if( ptrA->arrPtrE != 0 )
	{
		bbs_UInt32Arr_size( ptrA, sizeA );
	}
	else
	{
		ptrA->arrPtrE = malloc( sizeA * sizeof( uint32 ) );
		ptrA->allocatedSizeE = sizeA;
		ptrA->sizeE = sizeA;
	}
}

/* ------------------------------------------------------------------------- */

void bbs_UInt32Arr_size( struct bbs_UInt32Arr* ptrA, 
						 uint32 sizeA )
{
	if( ptrA->allocatedSizeE < sizeA )
	{
		printf( "void bbs_UInt32Arr_size( struct bbs_UInt32Arr*, uint32 ):\n"
				   "Unsufficient allocated memory (allocatedSizeE = '%i')");
		return;
	}
	ptrA->sizeE = sizeA;
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bbs_UInt32Arr_memSize( const struct bbs_UInt32Arr* ptrA )
{
	return bbs_SIZEOF16( uint32 ) + bbs_SIZEOF16( ptrA->sizeE ) + 
										ptrA->sizeE * bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */
	
uint32 bbs_UInt32Arr_memRead( struct bbs_UInt32Arr* ptrA, 
							 const uint16* memPtrA )
{
	uint32 memSizeL, sizeL;
	
	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memRead32( &sizeL, memPtrA );
	bbs_UInt32Arr_create( ptrA, sizeL );
	memPtrA += bbs_memRead32Arr( ptrA->arrPtrE, ptrA->sizeE, memPtrA );

	if( memSizeL != bbs_UInt32Arr_memSize( ptrA ) )
	{
		printf( "uint32 bbs_UInt32Arr_memRead( const struct bbs_UInt32Arr*, const uint16* ):\n"
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

void bbs_UInt32Arr_fill( struct bbs_UInt32Arr* ptrA, 
						 uint32 valA )
{
	uint32 iL;
	for( iL = 0; iL < ptrA->sizeE; iL++ )
	{
		ptrA->arrPtrE[ iL ] = valA;
	}
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */


