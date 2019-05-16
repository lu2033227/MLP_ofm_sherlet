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

#ifndef bbs_FUNCTIONS_EM_H
#define bbs_FUNCTIONS_EM_H

/**
 * This files contains gerenral purpose functions.
 */

/* ---- includes ----------------------------------------------------------- */
#include "Basic.h"
#include "Memory.h"

/* ---- related objects  --------------------------------------------------- */

/* ---- typedefs ----------------------------------------------------------- */

/* ---- constants ---------------------------------------------------------- */

/* ---- external functions ------------------------------------------------- */

/* Memory I/O Functions */

/* Memory I/O Operations use the little endian data alignment 
   regardless of platform */
   
/** reads a 32 bit word from memory; returns bbs_SIZEOF16( uint32 ) */
uint32 bbs_memRead32( void* ptrA, 
					  const uint16* memPtrA );

/** reads a 32 bit word from memory and returns it */
uint32 bbs_memPeek32( const uint16* memPtrA );
   
/** reads a 16 bit word from memory; returns bbs_SIZEOF16( uint16 ) */
uint32 bbs_memRead16( void* ptrA, 
					  const uint16* memPtrA );
   
/** reads a 32 bit word array from memory; sizeA specifies number of words in array; returns bbs_SIZEOF16( uint32 ) * sizeA */
uint32 bbs_memRead32Arr( void* ptrA, 
						 uint32 sizeA, const uint16* memPtrA );
   
/** reads a 16 bit word array from memory; sizeA specifies number of words in array; returns bbs_SIZEOF16( uint16 ) * sizeA */
uint32 bbs_memRead16Arr( void* ptrA, 
						 uint32 sizeA, const uint16* memPtrA );

/* Spcial I/O Functions */
   
/** reads a 32 bit version number word from memory.
 *  Compares the number to refVersionA.
 *  If refVersionA is lower an error condition is created.
 *  returns bbs_SIZEOF16( uint32 ) 
 */
uint32 bbs_memReadVersion32( uint32* versionPtrA, 
							uint32 refVersionA,
							const uint16* memPtrA );

/** return 16bit check sum of memory area (applies platform specific byte swapps) */
uint16 bbs_memCheckSum16( const uint16* memPtrA, uint32 sizeA );
						
#endif /* bbs_FUNCTIONS_EM_H */
