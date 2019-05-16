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
#include "Int32Arr.h"

#include <stdio.h>
#include <stdlib.h>

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

void bbs_Int32Arr_init( struct bbs_Int32Arr* ptrA )
{
	ptrA->arrPtrE = NULL;
	ptrA->sizeE = 0;
	ptrA->allocatedSizeE = 0;
}

/* ------------------------------------------------------------------------- */

void bbs_Int32Arr_exit( struct bbs_Int32Arr* ptrA )
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

void bbs_Int32Arr_copy( struct bbs_Int32Arr* ptrA, 
						const struct bbs_Int32Arr* srcPtrA )
{
	bbs_memcpy32( ptrA->arrPtrE, srcPtrA->arrPtrE, srcPtrA->sizeE * bbs_SIZEOF32( int32 ) ); 
}

/* ------------------------------------------------------------------------- */

flag bbs_Int32Arr_equal( const struct bbs_Int32Arr* ptrA, 
						 const struct bbs_Int32Arr* srcPtrA )
{
	uint32 iL;
	const int32* ptr1L = ptrA->arrPtrE;
	const int32* ptr2L = srcPtrA->arrPtrE;
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

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bbs_Int32Arr_create( struct bbs_Int32Arr* ptrA, 
						  uint32 sizeA )
{
	if( ptrA->arrPtrE != 0 ) {
		return;
	}
	else
	{
		ptrA->arrPtrE = malloc( sizeA * sizeof( int32 ) );
		ptrA->allocatedSizeE = sizeA;
		ptrA->sizeE = sizeA;
	}
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bbs_Int32Arr_memSize( const struct bbs_Int32Arr* ptrA )
{
	return bbs_SIZEOF16( uint32 ) + bbs_SIZEOF16( ptrA->sizeE ) + 
										ptrA->sizeE * bbs_SIZEOF16( int32 );
}

/* ------------------------------------------------------------------------- */
	
uint32 bbs_Int32Arr_memRead( struct bbs_Int32Arr* ptrA, 
							 const uint16* memPtrA )
{
	uint32 memSizeL, sizeL;

	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memRead32( &sizeL, memPtrA );
	bbs_Int32Arr_create( ptrA, sizeL );
	memPtrA += bbs_memRead32Arr( ptrA->arrPtrE, ptrA->sizeE, memPtrA );

	if( memSizeL != bbs_Int32Arr_memSize( ptrA ) )
	{
		printf( "uint32 bbs_Int32Arr_memRead( const struct bbs_Int32Arr*, const uint16* ):\n"
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

void bbs_Int32Arr_fill( struct bbs_Context* cpA,
					    struct bbs_Int32Arr* ptrA, 
						int32 valA )
{
	uint32 iL;
	for( iL = 0; iL < ptrA->sizeE; iL++ )
	{
		ptrA->arrPtrE[ iL ] = valA;
	}
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */


