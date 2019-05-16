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
#include "Feature.h"

#include "Sequence.h"
#include "I04Dns2x2Ftr.h"
#include "I04Tld2x4Ftr.h"
#include "I04Dns2x4Ftr.h"
#include "L01Tld2x4Ftr.h"
#include "L01Dns2x4Ftr.h"
#include "L04Dns2x4Ftr.h"
#include "L04Dns3x3Ftr.h"
#include "L06Dns3x3Ftr.h"
#include "L06Dns4x4Ftr.h"
#include "L06DnsNx4x4Ftr.h"
#include "L01Tld1x1Ftr.h"
#include "L04Tld2x4Ftr.h"
#include "L04Dns2x2Ftr.h"

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

void bbf_Feature_init( struct bbf_Feature* ptrA )
{
	ptrA->typeE = 0;
	ptrA->patchWidthE = 0;
	ptrA->patchHeightE = 0;
	ptrA->vpActivityE = NULL;
}

/* ------------------------------------------------------------------------- */

void bbf_Feature_exit( struct bbf_Feature* ptrA )
{
	ptrA->typeE = 0;
	ptrA->patchWidthE = 0;
	ptrA->patchHeightE = 0;
	ptrA->vpActivityE = NULL;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bbf_Feature_copy( struct bbs_Context* cpA,
					   struct bbf_Feature* ptrA, 
					   const struct bbf_Feature* srcPtrA )
{
	ptrA->typeE			= srcPtrA->typeE;
	ptrA->patchWidthE	= srcPtrA->patchWidthE;
	ptrA->patchHeightE	= srcPtrA->patchHeightE;
	ptrA->vpActivityE	= srcPtrA->vpActivityE;
}

/* ------------------------------------------------------------------------- */

flag bbf_Feature_equal( struct bbs_Context* cpA,
					    const struct bbf_Feature* ptrA, 
						const struct bbf_Feature* srcPtrA )
{

	if( ptrA->typeE			!= srcPtrA->typeE ) return FALSE;
	if( ptrA->patchWidthE	!= srcPtrA->patchWidthE ) return FALSE;
	if( ptrA->patchHeightE	!= srcPtrA->patchHeightE ) return FALSE;
	if( ptrA->vpActivityE	!= srcPtrA->vpActivityE ) return FALSE;
	return TRUE;
}

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
	
uint32 bbf_Feature_memSize( const struct bbf_Feature* ptrA )
{
	uint32 memSizeL = 0;
	memSizeL += bbs_SIZEOF16( ptrA->typeE );
	memSizeL += bbs_SIZEOF16( ptrA->patchWidthE );
	memSizeL += bbs_SIZEOF16( ptrA->patchHeightE );
	return memSizeL; 
}

/* ------------------------------------------------------------------------- */

uint32 bbf_Feature_memRead( struct bbf_Feature* ptrA, 
							const uint16* memPtrA )
{
	memPtrA += bbs_memRead32( &ptrA->typeE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->patchWidthE, memPtrA );
	memPtrA += bbs_memRead32( &ptrA->patchHeightE, memPtrA );
	return bbf_Feature_memSize(  ptrA );
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ exec functions } --------------------------------------------- */
/*                                                                           */
/* ========================================================================= */
	
/* ------------------------------------------------------------------------- */

void bbf_featureInit( struct bbf_Feature* ptrA,
					  enum bbf_FeatureType typeA )
{
	switch( typeA )
	{
		case bbf_FT_SEQUENCE:		    bbf_Sequence_init( ( struct bbf_Sequence* )ptrA ); return;
		case bbf_FT_I04_DNS_2X2_FTR:    bbf_I04Dns2x2Ftr_init( ( struct bbf_I04Dns2x2Ftr* )ptrA ); return;
		case bbf_FT_I04_TLD_2X4_FTR:    bbf_I04Tld2x4Ftr_init( ( struct bbf_I04Tld2x4Ftr* )ptrA ); return;
		case bbf_FT_I04_DNS_2X4_FTR:    bbf_I04Dns2x4Ftr_init( ( struct bbf_I04Dns2x4Ftr* )ptrA ); return;
		case bbf_FT_L01_TLD_2X4_FTR:    bbf_L01Tld2x4Ftr_init( ( struct bbf_L01Tld2x4Ftr* )ptrA ); return;
		case bbf_FT_L01_DNS_2X4_FTR:    bbf_L01Dns2x4Ftr_init( ( struct bbf_L01Dns2x4Ftr* )ptrA ); return;
		case bbf_FT_L04_DNS_2X4_FTR:    bbf_L04Dns2x4Ftr_init( ( struct bbf_L04Dns2x4Ftr* )ptrA ); return;
		case bbf_FT_L04_DNS_3X3_FTR:    bbf_L04Dns3x3Ftr_init( ( struct bbf_L04Dns3x3Ftr* )ptrA ); return;
		case bbf_FT_L06_DNS_3X3_FTR:    bbf_L06Dns3x3Ftr_init( ( struct bbf_L06Dns3x3Ftr* )ptrA ); return;
		case bbf_FT_L06_DNS_4X4_FTR:    bbf_L06Dns4x4Ftr_init( ( struct bbf_L06Dns4x4Ftr* )ptrA ); return;
		case bbf_FT_L06_DNS_NX4X4_FTR:  bbf_L06DnsNx4x4Ftr_init( ( struct bbf_L06DnsNx4x4Ftr* )ptrA ); return;
		case bbf_FT_L01_TLD_1X1_FTR:    bbf_L01Tld1x1Ftr_init( ( struct bbf_L01Tld1x1Ftr* )ptrA ); return;
		case bbf_FT_L04_TLD_2X4_FTR:    bbf_L04Tld2x4Ftr_init( ( struct bbf_L04Tld2x4Ftr* )ptrA ); return;
		case bbf_FT_L04_DNS_2X2_FTR:    bbf_L04Dns2x2Ftr_init( ( struct bbf_L04Dns2x2Ftr* )ptrA ); return;
			
		default: 
			printf( "bbf_featureInit: invalid type" );
	}
}

/* ------------------------------------------------------------------------- */

void bbf_featureExit( struct bbf_Feature* ptrA )
{
	switch( ptrA->typeE )
	{
		case bbf_FT_SEQUENCE:		   
			bbf_Sequence_exit( ( struct bbf_Sequence* )ptrA ); 
			return;
		case bbf_FT_I04_DNS_2X2_FTR:   
			bbf_I04Dns2x2Ftr_exit( ( struct bbf_I04Dns2x2Ftr* )ptrA ); 
			return;
		case bbf_FT_I04_TLD_2X4_FTR:   
			bbf_I04Tld2x4Ftr_exit( ( struct bbf_I04Tld2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_I04_DNS_2X4_FTR:   
			bbf_I04Dns2x4Ftr_exit( ( struct bbf_I04Dns2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L01_TLD_2X4_FTR:   
			bbf_L01Tld2x4Ftr_exit( ( struct bbf_L01Tld2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L01_DNS_2X4_FTR:   
			bbf_L01Dns2x4Ftr_exit( ( struct bbf_L01Dns2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L04_DNS_2X4_FTR:   
			bbf_L04Dns2x4Ftr_exit( ( struct bbf_L04Dns2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L04_DNS_3X3_FTR:   
			bbf_L04Dns3x3Ftr_exit( ( struct bbf_L04Dns3x3Ftr* )ptrA ); 
			return;
		case bbf_FT_L06_DNS_3X3_FTR:   
			bbf_L06Dns3x3Ftr_exit( ( struct bbf_L06Dns3x3Ftr* )ptrA ); 
			return;
		case bbf_FT_L06_DNS_4X4_FTR:   
			bbf_L06Dns4x4Ftr_exit( ( struct bbf_L06Dns4x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L06_DNS_NX4X4_FTR: 
			bbf_L06DnsNx4x4Ftr_exit( ( struct bbf_L06DnsNx4x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L01_TLD_1X1_FTR:   
			bbf_L01Tld1x1Ftr_exit( ( struct bbf_L01Tld1x1Ftr* )ptrA ); 
			return;
		case bbf_FT_L04_TLD_2X4_FTR:   
			bbf_L04Tld2x4Ftr_exit( ( struct bbf_L04Tld2x4Ftr* )ptrA ); 
			return;
		case bbf_FT_L04_DNS_2X2_FTR:   
			bbf_L04Dns2x2Ftr_exit( ( struct bbf_L04Dns2x2Ftr* )ptrA ); 
			return;
		default: 
			printf( "bbf_featureExit: invalid type" );
	}
}

/* ------------------------------------------------------------------------- */

uint32 bbf_featureMemSize( const struct bbf_Feature* ptrA )
{
	switch( ptrA->typeE )
	{
		case bbf_FT_SEQUENCE:		   return bbf_Sequence_memSize( ( struct bbf_Sequence* )ptrA );
		case bbf_FT_I04_DNS_2X2_FTR:   return bbf_I04Dns2x2Ftr_memSize( ( struct bbf_I04Dns2x2Ftr* )ptrA );
		case bbf_FT_I04_TLD_2X4_FTR:   return bbf_I04Tld2x4Ftr_memSize( ( struct bbf_I04Tld2x4Ftr* )ptrA );
		case bbf_FT_I04_DNS_2X4_FTR:   return bbf_I04Dns2x4Ftr_memSize( ( struct bbf_I04Dns2x4Ftr* )ptrA );
		case bbf_FT_L01_TLD_2X4_FTR:   return bbf_L01Tld2x4Ftr_memSize( ( struct bbf_L01Tld2x4Ftr* )ptrA );
		case bbf_FT_L01_DNS_2X4_FTR:   return bbf_L01Dns2x4Ftr_memSize( ( struct bbf_L01Dns2x4Ftr* )ptrA );
		case bbf_FT_L04_DNS_2X4_FTR:   return bbf_L04Dns2x4Ftr_memSize( ( struct bbf_L04Dns2x4Ftr* )ptrA );
		case bbf_FT_L04_DNS_3X3_FTR:   return bbf_L04Dns3x3Ftr_memSize( ( struct bbf_L04Dns3x3Ftr* )ptrA );
		case bbf_FT_L06_DNS_3X3_FTR:   return bbf_L06Dns3x3Ftr_memSize( ( struct bbf_L06Dns3x3Ftr* )ptrA );
		case bbf_FT_L06_DNS_4X4_FTR:   return bbf_L06Dns4x4Ftr_memSize( ( struct bbf_L06Dns4x4Ftr* )ptrA );
		case bbf_FT_L06_DNS_NX4X4_FTR: return bbf_L06DnsNx4x4Ftr_memSize( ( struct bbf_L06DnsNx4x4Ftr* )ptrA );
		case bbf_FT_L01_TLD_1X1_FTR:   return bbf_L01Tld1x1Ftr_memSize( ( struct bbf_L01Tld1x1Ftr* )ptrA );
		case bbf_FT_L04_TLD_2X4_FTR:   return bbf_L04Tld2x4Ftr_memSize( ( struct bbf_L04Tld2x4Ftr* )ptrA );
		case bbf_FT_L04_DNS_2X2_FTR:   return bbf_L04Dns2x2Ftr_memSize( ( struct bbf_L04Dns2x2Ftr* )ptrA );
		default: 
			printf( "bbf_featureExit: invalid type" );
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

uint32 bbf_featureMemRead( struct bbf_Feature* ptrA, 
						   const uint16* memPtrA )
{
	switch( ptrA->typeE )
	{
		case bbf_FT_SEQUENCE:		   return bbf_Sequence_memRead( ( struct bbf_Sequence* )ptrA, memPtrA );
		case bbf_FT_I04_DNS_2X2_FTR:   return bbf_I04Dns2x2Ftr_memRead( ( struct bbf_I04Dns2x2Ftr* )ptrA, memPtrA );
		case bbf_FT_I04_TLD_2X4_FTR:   return bbf_I04Tld2x4Ftr_memRead( ( struct bbf_I04Tld2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_I04_DNS_2X4_FTR:   return bbf_I04Dns2x4Ftr_memRead( ( struct bbf_I04Dns2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L01_TLD_2X4_FTR:   return bbf_L01Tld2x4Ftr_memRead( ( struct bbf_L01Tld2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L01_DNS_2X4_FTR:   return bbf_L01Dns2x4Ftr_memRead( ( struct bbf_L01Dns2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L04_DNS_2X4_FTR:   return bbf_L04Dns2x4Ftr_memRead( ( struct bbf_L04Dns2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L04_DNS_3X3_FTR:   return bbf_L04Dns3x3Ftr_memRead( ( struct bbf_L04Dns3x3Ftr* )ptrA, memPtrA );
		case bbf_FT_L06_DNS_3X3_FTR:   return bbf_L06Dns3x3Ftr_memRead( ( struct bbf_L06Dns3x3Ftr* )ptrA, memPtrA );
		case bbf_FT_L06_DNS_4X4_FTR:   return bbf_L06Dns4x4Ftr_memRead( ( struct bbf_L06Dns4x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L06_DNS_NX4X4_FTR: return bbf_L06DnsNx4x4Ftr_memRead( ( struct bbf_L06DnsNx4x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L01_TLD_1X1_FTR:   return bbf_L01Tld1x1Ftr_memRead( ( struct bbf_L01Tld1x1Ftr* )ptrA, memPtrA );
		case bbf_FT_L04_TLD_2X4_FTR:   return bbf_L04Tld2x4Ftr_memRead( ( struct bbf_L04Tld2x4Ftr* )ptrA, memPtrA );
		case bbf_FT_L04_DNS_2X2_FTR:   return bbf_L04Dns2x2Ftr_memRead( ( struct bbf_L04Dns2x2Ftr* )ptrA, memPtrA );
		default: 
			printf( "bbf_featureMemRead: invalid type" );
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

uint32 bbf_featureSizeOf16( enum bbf_FeatureType typeA )
{
	switch( typeA )
	{
		case bbf_FT_SEQUENCE:		   return bbs_SIZEOF16( struct bbf_Sequence );
		case bbf_FT_I04_DNS_2X2_FTR:   return bbs_SIZEOF16( struct bbf_I04Dns2x2Ftr );
		case bbf_FT_I04_TLD_2X4_FTR:   return bbs_SIZEOF16( struct bbf_I04Tld2x4Ftr );
		case bbf_FT_I04_DNS_2X4_FTR:   return bbs_SIZEOF16( struct bbf_I04Dns2x4Ftr );
		case bbf_FT_L01_TLD_2X4_FTR:   return bbs_SIZEOF16( struct bbf_L01Tld2x4Ftr );
		case bbf_FT_L01_DNS_2X4_FTR:   return bbs_SIZEOF16( struct bbf_L01Dns2x4Ftr );
		case bbf_FT_L04_DNS_2X4_FTR:   return bbs_SIZEOF16( struct bbf_L04Dns2x4Ftr );
		case bbf_FT_L04_DNS_3X3_FTR:   return bbs_SIZEOF16( struct bbf_L04Dns3x3Ftr );
		case bbf_FT_L06_DNS_3X3_FTR:   return bbs_SIZEOF16( struct bbf_L06Dns3x3Ftr );
		case bbf_FT_L06_DNS_4X4_FTR:   return bbs_SIZEOF16( struct bbf_L06Dns4x4Ftr );
		case bbf_FT_L06_DNS_NX4X4_FTR: return bbs_SIZEOF16( struct bbf_L06DnsNx4x4Ftr );
		case bbf_FT_L01_TLD_1X1_FTR:   return bbs_SIZEOF16( struct bbf_L01Tld1x1Ftr );
		case bbf_FT_L04_TLD_2X4_FTR:   return bbs_SIZEOF16( struct bbf_L04Tld2x4Ftr );
		case bbf_FT_L04_DNS_2X2_FTR:   return bbs_SIZEOF16( struct bbf_L04Dns2x2Ftr );
		default: 
			printf( "bbf_featureSizeOf16: invalid type" );
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */

