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
#include "L01Tld1x1Ftr.h"
#include "BBF_Functions.h"

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

void bbf_L01Tld1x1Ftr_init( struct bbf_L01Tld1x1Ftr* ptrA )
{
	bbf_Feature_init( &ptrA->baseE );
	ptrA->baseE.typeE = ( uint32 )bbf_FT_L01_TLD_1X1_FTR;
	ptrA->baseE.vpActivityE = bbf_L01Tld1x1Ftr_activity;
	bbs_UInt32Arr_init( &ptrA->dataArrE );
	ptrA->activityFactorE = 0;
}

/* ------------------------------------------------------------------------- */

void bbf_L01Tld1x1Ftr_exit( struct bbf_L01Tld1x1Ftr* ptrA )
{
	bbf_Feature_exit( &ptrA->baseE );
	bbs_UInt32Arr_exit( &ptrA->dataArrE );
	ptrA->activityFactorE = 0;
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
	
uint32 bbf_L01Tld1x1Ftr_memSize( const struct bbf_L01Tld1x1Ftr* ptrA )
{
	uint32 memSizeL = bbs_SIZEOF16( uint32 ) +
					  bbs_SIZEOF16( uint32 ); /* version */

	memSizeL += bbf_Feature_memSize( &ptrA->baseE );
	memSizeL += bbs_UInt32Arr_memSize( &ptrA->dataArrE );
	memSizeL += bbs_SIZEOF16( ptrA->activityFactorE );

	return memSizeL; 
}

/* ------------------------------------------------------------------------- */

uint32 bbf_L01Tld1x1Ftr_memRead( struct bbf_L01Tld1x1Ftr* ptrA, 
							     const uint16* memPtrA )
{
	uint32 memSizeL, versionL;
	
	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bbf_L01_TLD_1X1_FTR_VERSION, memPtrA );
	memPtrA += bbf_Feature_memRead( &ptrA->baseE, memPtrA );
	memPtrA += bbs_UInt32Arr_memRead( &ptrA->dataArrE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->activityFactorE, memPtrA );
	if( memSizeL != bbf_L01Tld1x1Ftr_memSize( ptrA ) )
	{
		printf( "uint32 bbf_L01Tld1x1Ftr_memRead( struct bem_ScanGradientMove* ptrA, const uint16* memPtrA ):\n"
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

int32 bbf_L01Tld1x1Ftr_activity( const struct bbf_Feature* ptrA, const uint32* patchA )
{
	const struct bbf_L01Tld1x1Ftr* ptrL = ( struct bbf_L01Tld1x1Ftr* )ptrA;

	int32 iL;

	const uint32* dataPtrL = ptrL->dataArrE.arrPtrE;
	const uint32* patchL = patchA;

	uint32 bsL = 0;

	for( iL = ptrL->baseE.patchWidthE >> 2; iL > 0; iL-- )
	{
		uint32 vL;
		vL = ( patchL[ 0 ] ^ dataPtrL[ 0 ] ) & dataPtrL[ 1 ];
		bsL += bbf_BIT_SUM_32( vL );

		vL = ( patchL[ 1 ] ^ dataPtrL[ 2 ] ) & dataPtrL[ 3 ];
		bsL += bbf_BIT_SUM_32( vL );

		vL = ( patchL[ 2 ] ^ dataPtrL[ 4 ] ) & dataPtrL[ 5 ];
		bsL += bbf_BIT_SUM_32( vL );

		vL = ( patchL[ 3 ] ^ dataPtrL[ 6 ] ) & dataPtrL[ 7 ];
		bsL += bbf_BIT_SUM_32( vL );

		dataPtrL += 8;
		patchL   += 4;
	}

	return bsL * ptrL->activityFactorE;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */

