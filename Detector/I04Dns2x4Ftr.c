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
#include "I04Dns2x4Ftr.h"

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

void bbf_I04Dns2x4Ftr_init( struct bbf_I04Dns2x4Ftr* ptrA )
{
	bbf_Feature_init( &ptrA->baseE );
	ptrA->baseE.typeE = ( uint32 )bbf_FT_I04_DNS_2X4_FTR;
	ptrA->baseE.vpActivityE = bbf_I04Dns2x4Ftr_activity;
	bbs_UInt32Arr_init( &ptrA->dataArrE );
	bbs_Int16Arr_init( &ptrA->tableE );
	ptrA->activityFactorE = 0;
}

/* ------------------------------------------------------------------------- */

void bbf_I04Dns2x4Ftr_exit( struct bbf_I04Dns2x4Ftr* ptrA )
{
	bbf_Feature_exit( &ptrA->baseE );
	bbs_UInt32Arr_exit( &ptrA->dataArrE );
	bbs_Int16Arr_exit( &ptrA->tableE );
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
	
uint32 bbf_I04Dns2x4Ftr_memSize( const struct bbf_I04Dns2x4Ftr* ptrA )
{
	uint32 memSizeL = bbs_SIZEOF16( uint32 ) +
					  bbs_SIZEOF16( uint32 ); /* version */

	memSizeL += bbf_Feature_memSize( &ptrA->baseE );
	memSizeL += bbs_UInt32Arr_memSize( &ptrA->dataArrE );
	memSizeL += bbs_Int16Arr_memSize( &ptrA->tableE );
	memSizeL += bbs_SIZEOF16( ptrA->activityFactorE );

	return memSizeL; 
}

/* ------------------------------------------------------------------------- */

uint32 bbf_I04Dns2x4Ftr_memRead( struct bbf_I04Dns2x4Ftr* ptrA, 
							     const uint16* memPtrA )
{
	uint32 memSizeL, versionL;
	
	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memReadVersion32( &versionL, bbf_I04_DNS_2X4_FTR_VERSION, memPtrA );
	memPtrA += bbf_Feature_memRead( &ptrA->baseE, memPtrA );
	memPtrA += bbs_UInt32Arr_memRead( &ptrA->dataArrE, memPtrA );
	memPtrA += bbs_Int16Arr_memRead( &ptrA->tableE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->activityFactorE, memPtrA );
	if( memSizeL != bbf_I04Dns2x4Ftr_memSize( ptrA ) )
	{
		printf( "uint32 bbf_I04Dns2x4Ftr_memRead( struct bem_ScanGradientMove* ptrA, const uint16* memPtrA ):\n"
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

int32 bbf_I04Dns2x4Ftr_activity( const struct bbf_Feature* ptrA, const uint32* patchA )
{
	const struct bbf_I04Dns2x4Ftr* ptrL = ( struct bbf_I04Dns2x4Ftr* )ptrA;

	uint32 wL = ptrL->baseE.patchWidthE - 1;
    uint32 hL = ptrL->baseE.patchHeightE - 3;

	const uint32* dataPtrL = ptrL->dataArrE.arrPtrE;
	const int16*  tableL   = ptrL->tableE.arrPtrE;

	uint32 blocksL = ptrL->baseE.patchHeightE >> 2;
	uint32 iL;

	uint32 borderMaskL = ( ( uint32 )1 << hL ) - 1;

	int32 sumL = 0;

	uint32 sL[ 8 ];
	uint32 mL[ 4 ];

	for( iL = 0; iL < wL; iL++ )
	{
		uint32 vL = 0;

		uint32 s1L = patchA[ iL     ];
		uint32 s2L = patchA[ iL + 1 ];

		/* comparison of pixels with patchHeightE - 3 features */
		sL[ 0 ] = ( ( s1L      ) ^ dataPtrL[ 0 ] ) & borderMaskL;
		sL[ 1 ] = ( ( s1L >> 1 ) ^ dataPtrL[ 1 ] ) & borderMaskL;
		sL[ 2 ] = ( ( s1L >> 2 ) ^ dataPtrL[ 2 ] ) & borderMaskL;
		sL[ 3 ] = ( ( s1L >> 3 ) ^ dataPtrL[ 3 ] ) & borderMaskL;

		sL[ 4 ] = ( ( s2L      ) ^ dataPtrL[ 4 ] ) & borderMaskL;
		sL[ 5 ] = ( ( s2L >> 1 ) ^ dataPtrL[ 5 ] ) & borderMaskL;
		sL[ 6 ] = ( ( s2L >> 2 ) ^ dataPtrL[ 6 ] ) & borderMaskL;
		sL[ 7 ] = ( ( s2L >> 3 ) ^ dataPtrL[ 7 ] ) & borderMaskL;

		/* parallel bit counting of patchHeightE - 3 features */
		mL[ 0 ] = ( ( sL[ 0 ] & 0x11111111 ) + ( sL[ 1 ] & 0x11111111 ) + 
					( sL[ 2 ] & 0x11111111 ) + ( sL[ 3 ] & 0x11111111 ) +
				    ( sL[ 4 ] & 0x11111111 ) + ( sL[ 5 ] & 0x11111111 ) + 
					( sL[ 6 ] & 0x11111111 ) + ( sL[ 7 ] & 0x11111111 ) );

		mL[ 1 ] = ( ( sL[ 0 ] & 0x22222222 ) + ( sL[ 1 ] & 0x22222222 ) + 
					( sL[ 2 ] & 0x22222222 ) + ( sL[ 3 ] & 0x22222222 ) +
				    ( sL[ 4 ] & 0x22222222 ) + ( sL[ 5 ] & 0x22222222 ) + 
					( sL[ 6 ] & 0x22222222 ) + ( sL[ 7 ] & 0x22222222 ) ) >> 1;

		mL[ 2 ] = ( ( sL[ 0 ] & 0x44444444 ) + ( sL[ 1 ] & 0x44444444 ) + 
					( sL[ 2 ] & 0x44444444 ) + ( sL[ 3 ] & 0x44444444 ) +
				    ( sL[ 4 ] & 0x44444444 ) + ( sL[ 5 ] & 0x44444444 ) + 
					( sL[ 6 ] & 0x44444444 ) + ( sL[ 7 ] & 0x44444444 ) ) >> 2;

		mL[ 3 ] = ( ( sL[ 0 ] & 0x88888888 ) + ( sL[ 1 ] & 0x88888888 ) + 
					( sL[ 2 ] & 0x88888888 ) + ( sL[ 3 ] & 0x88888888 ) +
				    ( sL[ 4 ] & 0x88888888 ) + ( sL[ 5 ] & 0x88888888 ) + 
					( sL[ 6 ] & 0x88888888 ) + ( sL[ 7 ] & 0x88888888 ) ) >> 3;

		/* parallel comparison with thresholds and packing of results into bit array of size patchHeightE - 3 */
		vL |= ( ( mL[ 0 ] + dataPtrL[  8 ] ) & 0x88888888 ) >> 3;
		vL |= ( ( mL[ 1 ] + dataPtrL[  9 ] ) & 0x88888888 ) >> 2;
		vL |= ( ( mL[ 2 ] + dataPtrL[ 10 ] ) & 0x88888888 ) >> 1;
		vL |= ( ( mL[ 3 ] + dataPtrL[ 11 ] ) & 0x88888888 );

		vL = ( ~vL ) & 0x1FFFFFFF;

		/* parallel processing of weights (4 weights at a time) */
		if( hL == 29 )
		{
			sumL += tableL[         ( vL       ) & 0x0F   ];
			sumL += tableL[  16 + ( ( vL >>  4 ) & 0x0F ) ];
			sumL += tableL[  32 + ( ( vL >>  8 ) & 0x0F ) ];
			sumL += tableL[  48 + ( ( vL >> 12 ) & 0x0F ) ];
			sumL += tableL[  64 + ( ( vL >> 16 ) & 0x0F ) ];
			sumL += tableL[  80 + ( ( vL >> 20 ) & 0x0F ) ];
			sumL += tableL[  96 + ( ( vL >> 24 ) & 0x0F ) ];
			sumL += tableL[ 112 + ( ( vL >> 28 ) & 0x0F ) ];
			tableL += 128;
		}
		else
		{
			uint32 jL;
			for( jL = 0; jL < blocksL; jL++ )
			{
				sumL += tableL[ vL & 0x0F ];
				vL >>= 4;
				tableL += 16;
			}
		}

		dataPtrL += 12;
	}

	return sumL * ( ptrL->activityFactorE >> 8 ) + ( ( sumL * ( int32 )( ptrL->activityFactorE & 0x0FF ) ) >> 8 );
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */

