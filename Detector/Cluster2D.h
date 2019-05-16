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

#ifndef bts_CLUSTER2D_EM_H
#define bts_CLUSTER2D_EM_H

/* ---- includes ----------------------------------------------------------- */

#include "Int16Vec2D.h"
#include "Flt16Vec2D.h"
#include "Int16Rect.h"
#include "Flt16Alt2D.h"
#include "BTS_Functions.h"

/* ---- related objects  --------------------------------------------------- */

struct bts_RBFMap2D;

/* ---- typedefs ----------------------------------------------------------- */

/* data format version number */
#define bts_CLUSTER2D_VERSION 100

/* ---- constants ---------------------------------------------------------- */

/* ---- object definition -------------------------------------------------- */

/** 2d vector array */
struct bts_Cluster2D 
{

	/* ---- private data --------------------------------------------------- */

	/* ---- public data ---------------------------------------------------- */

	/** number of allocated vectors */
	uint32 allocatedSizeE;

	/** number of vectors */
	uint32 sizeE;

	/** format of vectors (bbpE always > 0) */
	int32 bbpE;

	/** array of int16 vectors */
	struct bts_Int16Vec2D* vecArrE;

};

/* ---- associated objects ------------------------------------------------- */

/* ---- external functions ------------------------------------------------- */

/* ---- \ghd{ constructor/destructor } ------------------------------------- */

/** initializes cluster */
void bts_Cluster2D_init( struct bts_Cluster2D* ptrA );

/** destroys cluster */
void bts_Cluster2D_exit( struct bts_Cluster2D* ptrA );

/* ---- \ghd{ operators } -------------------------------------------------- */

/** copies cluster */
void bts_Cluster2D_copy( struct bts_Cluster2D* ptrA, 
						 const struct bts_Cluster2D* srcPtrA );

/** compares cluster */
flag bts_Cluster2D_equal( const struct bts_Cluster2D* ptrA, 
						  const struct bts_Cluster2D* srcPtrA );

/* ---- \ghd{ query functions } -------------------------------------------- */

/** returns center of gravity */
struct bts_Flt16Vec2D bts_Cluster2D_center( const struct bts_Cluster2D* ptrA );

/** returns check sum (for debugging purpose) */
uint32 bts_Cluster2D_checkSum( struct bbs_Context* cpA,
							   const struct bts_Cluster2D* ptrA );

/** returns bounding box */
struct bts_Int16Rect bts_Cluster2D_boundingBox( const struct bts_Cluster2D* ptrA );

/** returns int32 x-coordinate with given bbp at indexed position */
int32 bts_Cluster2D_int32X( struct bbs_Context* cpA,
						    const struct bts_Cluster2D* ptrA, 
							uint32 indexA, int32 bbpA );

/** returns int32 y-coordinate with given bbp at indexed position */
int32 bts_Cluster2D_int32Y( struct bbs_Context* cpA,
						    const struct bts_Cluster2D* ptrA, 
							uint32 indexA, 
							int32 bbpA );

/* ---- \ghd{ modify functions } ------------------------------------------- */

/** allocates cluster */
void bts_Cluster2D_create( struct bts_Cluster2D* ptrA, 
						   uint32 sizeA );

/** resize cluster (sizeA must be smaller or equal to allocated size)*/
void bts_Cluster2D_size( struct bts_Cluster2D* ptrA, 
						 uint32 sizeA );

/** transforms cluster according to alt (function does not change bbp of cluster) */
void bts_Cluster2D_transform( struct bts_Cluster2D* ptrA, 
							  struct bts_Flt16Alt2D altA );

/** transforms cluster according to alt and set bbp of output cluster */
void bts_Cluster2D_transformBbp( struct bts_Cluster2D* ptrA, 
							     struct bts_Flt16Alt2D altA,
								 uint32 dstBbpA );

/** copies src cluster and simultaneously transforms vectors according to alt using dstBbpA as resulting cluster format */
void bts_Cluster2D_copyTransform( struct bts_Cluster2D* ptrA, 
								  const struct bts_Cluster2D* srcPtrA, 
								  struct bts_Flt16Alt2D altA, 
								  uint32 dstBbpA );

/* ---- \ghd{ memory I/O } ------------------------------------------------- */

/** size in words (16-bit) object needs when written to memory */
uint32 bts_Cluster2D_memSize( const struct bts_Cluster2D* ptrA );

/** reads object from memory; returns number of words (16-bit) read */
uint32 bts_Cluster2D_memRead( struct bts_Cluster2D* ptrA, 
							  const uint16* memPtrA );

/* ---- \ghd{ exec functions } --------------------------------------------- */

/** Computes the best affine linear transformation from *srcPtrA to *dstPtrA.
 *  Constrains of trafo are given by altTypeA
 */
struct bts_Flt16Alt2D bts_Cluster2D_alt( const struct bts_Cluster2D* srcPtrA,
										 const struct bts_Cluster2D* dstPtrA,
										 enum bts_AltType altTypeA );

#endif /* bts_CLUSTER2D_EM_H */

