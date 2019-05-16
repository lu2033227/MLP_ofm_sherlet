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
#include "UInt32Image.h"

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

void bim_UInt32Image_init( struct bim_UInt32Image* ptrA )
{
	bbs_UInt32Arr_init( &ptrA->arrE );
	ptrA->widthE = 0;
	ptrA->heightE = 0;
}

/* ------------------------------------------------------------------------- */

void bim_UInt32Image_exit( struct bim_UInt32Image* ptrA )
{
	bbs_UInt32Arr_exit( &ptrA->arrE );
	ptrA->widthE = 0;
	ptrA->heightE = 0;	
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bim_UInt32Image_copy( struct bim_UInt32Image* ptrA, 
						   const struct bim_UInt32Image* srcPtrA )
{
	ptrA->widthE  = srcPtrA->widthE;
	ptrA->heightE = srcPtrA->heightE;
	bbs_UInt32Arr_copy( &ptrA->arrE, &srcPtrA->arrE );
}

/* ------------------------------------------------------------------------- */

flag bim_UInt32Image_equal( const struct bim_UInt32Image* ptrA, 
							const struct bim_UInt32Image* srcPtrA )
{
	if( ptrA->widthE  != srcPtrA->widthE ) 
		return FALSE;
	if( ptrA->heightE != srcPtrA->heightE ) 
		return FALSE;
	return bbs_UInt32Arr_equal( &ptrA->arrE, &srcPtrA->arrE );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint32 bim_UInt32Image_heapSize( const struct bim_UInt32Image* ptrA, 
								 uint32 widthA, 
								 uint32 heightA )
{
	return bbs_UInt32Arr_heapSize( &ptrA->arrE, widthA * heightA );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bim_UInt32Image_create( struct bim_UInt32Image* ptrA, 
						    uint32 widthA, 
							uint32 heightA )
{
	if( ptrA->arrE.arrPtrE != 0 )
	{
		bim_UInt32Image_size( ptrA, widthA, heightA );
	}
	else
	{
		bbs_UInt32Arr_create( &ptrA->arrE, widthA * heightA );
		ptrA->widthE  = widthA;
		ptrA->heightE = heightA;
	}
}

/* ------------------------------------------------------------------------- */

void bim_UInt32Image_assignExternalImage( struct bim_UInt32Image* ptrA, 
										  struct bim_UInt32Image* srcPtrA )
{
	if( ptrA->arrE.arrPtrE != 0 )
	{
		printf( "void bim_UInt32Image_assignExternalImage( ... ): image was already created once" );
		return;
	}

	bim_UInt32Image_create( ptrA, 
					        srcPtrA->widthE, 
						    srcPtrA->heightE );
}

/* ------------------------------------------------------------------------- */
	
void bim_UInt32Image_size( struct bim_UInt32Image* ptrA, 
						   uint32 widthA, 
						   uint32 heightA )
{
	if( ptrA->arrE.allocatedSizeE < widthA * heightA )
	{
		printf( "void bim_UInt32Image_size( struct bim_UInt32Image*, uint32 sizeA ):\n"
				   "Unsufficient allocated memory" );
		return;
	}
	ptrA->widthE  = widthA;
	ptrA->heightE = heightA;
	bbs_UInt32Arr_size( &ptrA->arrE, widthA * heightA );
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint32 bim_UInt32Image_memSize( const struct bim_UInt32Image* ptrA )
{
	return  bbs_SIZEOF16( uint32 )
		  + bbs_SIZEOF16( uint32 ) /* version */
		  + bbs_SIZEOF16( ptrA->widthE ) 
		  + bbs_SIZEOF16( ptrA->heightE )
		  + bbs_UInt32Arr_memSize( &ptrA->arrE ); 
}

/* ------------------------------------------------------------------------- */
	
uint32 bim_UInt32Image_memRead( struct bim_UInt32Image* ptrA, 
							   const uint16* memPtrA )
{
	uint32 memSizeL, versionL;

	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bim_UINT32_IMAGE_VERSION, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->widthE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->heightE, memPtrA );
	bbs_UInt32Arr_memRead( &ptrA->arrE, memPtrA );

	if( memSizeL != bim_UInt32Image_memSize( ptrA ) )
	{
		printf( "uint32 bim_UInt32Image_memRead( const struct bim_UInt32Image* ptrA, const void* memPtrA ):\n"
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

void bim_UInt32Image_setAllPixels( struct bim_UInt32Image* ptrA, 
								   uint32 valueA, 
								   int32 bbpA )
{
	long iL;
	uint32* ptrL = ptrA->arrE.arrPtrE;
	for( iL = ptrA->widthE * ptrA->heightE; iL > 0; iL-- )
	{
		*ptrL++ = valueA;
	}
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */


