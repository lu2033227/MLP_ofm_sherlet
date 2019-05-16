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

#include <stdio.h>

/* ---- related objects  --------------------------------------------------- */

/* ---- typedefs ----------------------------------------------------------- */

/* ---- constants ---------------------------------------------------------- */

/* ---- globals   ---------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ external functions } ----------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint16 bbs_swapBytes( uint16 valA )
{
	return ( ( valA >> 8 ) & 0x00FF ) | ( ( valA << 8 ) & 0xFF00 );	
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memRead32( void* ptrA, 
					  const uint16* memPtrA )
{
	uint32 valL = 0;
	
	#ifdef HW_BIG_ENDIAN
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 0  );
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 16 );
	#else
		valL |= ( ( uint32 )*memPtrA++ << 0  );
		valL |= ( ( uint32 )*memPtrA++ << 16 );
	#endif

	*( uint32* )ptrA = valL;

	return bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memPeek32( const uint16* memPtrA )
{
	uint32 valL = 0;

	#ifdef HW_BIG_ENDIAN
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 0  );
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 16 );
	#else
		valL |= ( ( uint32 )*memPtrA++ << 0  );
		valL |= ( ( uint32 )*memPtrA++ << 16 );
	#endif

	return valL;
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memRead16( void* ptrA, 
					  const uint16* memPtrA )
{
	#ifdef HW_BIG_ENDIAN
		*( uint16* )ptrA = bbs_swapBytes( *memPtrA++ );
	#else
		*( uint16* )ptrA = *memPtrA++;
	#endif

	return bbs_SIZEOF16( uint16 );
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memRead32Arr( void* ptrA, 
						 uint32 sizeA, 
						 const uint16* memPtrA )
{
	uint32 iL;
	uint32* dstL = ( uint32* )ptrA;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memRead32( dstL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint32 ); 
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memRead16Arr( void* ptrA, 
						 uint32 sizeA, 
						 const uint16* memPtrA )
{
	uint32 iL;
	uint16* dstL = ( uint16* )ptrA;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memRead16( dstL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint16 ); 
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memReadVersion32( uint32* versionPtrA, 
							 uint32 refVersionA,
							 const uint16* memPtrA )
{
	bbs_memRead32( versionPtrA, memPtrA );
	if( *versionPtrA > refVersionA )
	{
		printf( "uint32 bbs_memReadVersion32( .... ):\n"
			       "Data format is newer than software or corrupt\n" );
	}
	return bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */

uint16 bbs_memCheckSum16( const uint16* memPtrA, uint32 sizeA )
{
	uint32 iL;
	uint16 sumL = 0;
	for( iL = 0; iL < sizeA; iL++ )
	{
		#ifdef HW_BIG_ENDIAN
			sumL += bbs_swapBytes( memPtrA[ iL ] );
		#else
			sumL += memPtrA[ iL ];
		#endif
	}

	return sumL;
}

/* ------------------------------------------------------------------------- */
