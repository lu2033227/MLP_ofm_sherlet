//******************************************************************************
// HSC - Histogram of Shearlet Coefficients.
// Copyright (C) 2010-2011 by William Robson Schwartz.
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/lgpl.html>
//******************************************************************************

#ifndef HSC_H
#define HSC_H
#include "misc.h"
#include "maths.h"


class HSC {
int nLevels,nOrients,imageWidth,nPartitions, nPartitionsInWidth,blockWidth,maskSize;
bool useCells;
string LapFilter;	// filter used for Laplacian decomposition
string normType;	// normalization type (global, local, local+global, none)
Vector<float> *g;
Vector<float> *h;
vector<Matrix<float> *> shearMatrix;
vector<vector<Matrix<float> *> > shearMaps;	// keep computed features for the current image //shearMaps[scale][orient][map]


	// maths for speed-up (for normalization)
	Maths mat;
	
	// sum a region of an matrix
	float SumRegion(Matrix<float> *m, int x0, int y0, int x1, int y1);

	// extract features withour selection
	void ShearletHistogram(Position *pos, float *feat);

	// extract Shearlet Based Quality Feature 
//	void ExtractQualityFeatures(Position *pos, float *feat);

	// reset some structures after extraction features from an image
	void Clear_shearMaps();

public:
	FILE *fid;
	HSC(); 
	// Set parameters and initilize Laplacian and Shearlet Matrices
//	void SetParameter(string name, int value);

	// add new image for this extraction method
	void ShearletTransform(IplImage *img);

	// get number of features per block
	int GetNFeatures();

	void ShearBlockAverage(Vector<float> *shearBlockAveFeatures, int blockW, int blockH, int stepX, int stepY);

	// function to extract feature vectors
	void ShearletHistogram(Vector<float> *shearHistogramFeatures,int blockW, int blockH, int stepX, int stepY);

	// function to extract feature vectors considering image as a whole (not dividing in blocks)
	void ConcatenateFeatures(Vector<float> *shearFeatures,Vector<float> *shearBlockAveFeatures,Vector<float> *shearHistogramFeatures);
	float BlockAverage(Matrix<float> *mat,int x,int y,int w,int h);




};

#endif