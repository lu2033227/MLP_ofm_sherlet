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

#ifndef bpi_DCR_EM_H
#define bpi_DCR_EM_H

/* ---- includes ----------------------------------------------------------- */

#include "UInt32Image.h"
#include "IdCluster2D.h"
#include "Scanner.h"

/* ---- related objects  --------------------------------------------------- */

/* ---- typedefs ----------------------------------------------------------- */

/** integer data formats */
typedef signed short s16;
typedef unsigned short u16;
typedef signed int    s32;
typedef unsigned int  u32;

/** signed 16.16 fixed point format */
typedef s32 s16p16;

/** signed 8.24 fixed point format */
typedef s32 s8p24;

/** node data structure */
typedef struct
{
	s16p16  x; /* x-coordinate */
	s16p16  y; /* y-coordinate */
	s32    id; /* node id */
} bpi_Node;

/** rectangle data structure */
typedef struct
{
	s16p16  xMin; /* x min coordinate */
	s16p16  yMin; /* y min coordinate */
	s16p16  xMax; /* x max coordinate */
	s16p16  yMax; /* y max coordinate */
} bpi_Rect;

/* ---- constants ---------------------------------------------------------- */

/** maximum size of dcr cluster */
#define bpi_DCR_MAX_CLUSTER_SIZE 60

/* ---- object definition -------------------------------------------------- */

/** data carrier */
struct bpi_DCR
{
	/* ---- temporary data ------------------------------------------------- */

	/* ---- private data --------------------------------------------------- */

	/* ---- public data ---------------------------------------------------- */

	/** maximum allowed image width */
	uint32 maxImageWidthE;

	/** maximum allowed image height */
	uint32 maxImageHeightE;

	/** pointer to original image data */
	void* imageDataPtrE;

	/** width of original image */
	uint32 imageWidthE;

	/** height of original image */
	uint32 imageHeightE;

	/** offset refering to main and sdk clusters */
	struct bts_Int16Vec2D offsE;

	/** main cluster */
	struct bts_IdCluster2D mainClusterE;

	/** confidence value ( 8.24 ) */
	int32 confidenceE;

	/** approval flag */
	flag approvedE;

	/** (image) id value */
	int32 idE;

	/** region of interest */
	struct bts_Int16Rect roiRectE;
};

/* ---- associated objects ------------------------------------------------- */

/* ---- external functions ------------------------------------------------- */

/* ---- \ghd{ constructor/destructor } ------------------------------------- */

/** initializes data carrier */
void bpi_DCR_init( struct bpi_DCR* ptrA );

/** destroys data carrier */
void bpi_DCR_exit( struct bpi_DCR* ptrA );

/* ---- \ghd{ operators } -------------------------------------------------- */

/* ---- \ghd{ query functions } -------------------------------------------- */

/* ---- \ghd{ modify functions } ------------------------------------------- */

/** create a data carrier */
void bpi_DCR_create( struct bpi_DCR* ptrA, 
					 uint32 imageWidthA,
					 uint32 imageHeightA);

/* ---- \ghd{ memory I/O } ------------------------------------------------- */

/* ---- \ghd{ exec functions } --------------------------------------------- */

/** references external byte gray image through memory block referenced by bufferPtrA to be used as input image */
void bpi_DCR_assignGrayByteImage( struct bpi_DCR* ptrA,
								  const void* bufferPtrA,
								  uint32 widthA,
								  uint32 heightA );

/** assigns external byte gray image as input image and region of interest.
  *
  * bufferPtrA:  pointer to memory block of imput image
  * pRectA:		 rectangle describing region of interest
  */
void bpi_DCR_assignGrayByteImageROI( struct bpi_DCR* ptrA,
									 const void* bufferPtrA, 
									 uint32 widthA, 
									 uint32 heightA,
									 const struct bts_Int16Rect* pRectA );

/** returns confidence 8.24 fixed format */
int32 bpi_DCR_confidence( const struct bpi_DCR* ptrA );

/** extracts facial rectangle */
int32 bpi_DCR_getRect( const struct bpi_DCR* hdcrA,
						   bpi_Rect* pRectA );

/** returns number of available landmark nodes */
uint32 bpi_DCR_nodeCount( const struct bpi_DCR* hdcrA );

/** extracts information about indexed node */
int32 bpi_DCR_getNode( const struct bpi_DCR* hdcrA,
						   u32 indexA,
						   bpi_Node* pNodeA );

/** returns approval flag (0=false; 1=true)*/
u32 bpi_DCR_approved( const struct bpi_DCR* hdcrA );

#endif /* bpi_DCR_EM_H */
