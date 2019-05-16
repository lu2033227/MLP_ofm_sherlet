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

#include "headers.h"
#include "misc.h"
#include "HSCmatrices.h"
#include "HSC.h"
#include <strstream>


HSC::HSC() {

	nLevels=4; nOrients=8; nPartitions=16,imageWidth=256; 
	nPartitionsInWidth=(int) sqrt((float) nPartitions); //num of partitions in width
	blockWidth=imageWidth/nPartitionsInWidth; // 4块: 256x256->128x128； 16块:256x256->64x64  
	maskSize=4;
	LapFilter = "Burt"; normType = "local"; useCells=false;
	g = NULL;
	h = NULL;

char str[1024];

	// load filters
	h = LapPyrpfiltersH(LapFilter);
	g = LapPyrpfiltersG(LapFilter);

	sprintf(str, "M_%d_%dx%d", nOrients, maskSize, maskSize); //str defines filter type
	shearMatrix = GetHSCMatrix(str);
	
	// debug parameters
	printf("[HSC] [nLevels: %d] [nOrients: %d] [Filter: %s] [norm: %s] [Cells: %d]\n", 
		nLevels, nOrients, LapFilter.c_str(), normType.c_str(), useCells);
	
	fid=fopen("result.txt","w");
}


void HSC::Clear_shearMaps() {
vector<Matrix<float> *> d;
int s, i;

	for (s = 0; s < (int) shearMaps.size(); s++) {
		d = shearMaps.at(s);
		
		for (i = 0; i < (int) d.size(); i++) {
			delete d.at(i);
		}
	}
	
	shearMaps.clear();
}




int HSC::GetNFeatures() {

	if (useCells == true)
		return 4 * nLevels * nOrients;
	else
		return nLevels * nOrients;
}


float HSC::SumRegion(Matrix<float> *m, int x0, int y0, int x1, int y1) {
float value = 0;
int x, y;
int count=0;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			value += fabs(m->GetElement(x, y));
			count++;
		}
	}
//	printf(" [count=%4d] ",count); 	fprintf(fid," [count=%4d] ",count);
	return value;
}

void HSC::ShearletTransform(IplImage *img) {
vector<Matrix<float> *> d;
Matrix<float> *m, *xlo = NULL;
IplImage *dst;
int i;

	// reset structures first
	Clear_shearMaps();

	dst = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 1 ); //IPL_DEPTH_8U - 无符号8位整型

	// convert to a gray scale image
	cvCvtColor( img, dst, CV_RGB2GRAY );

	// compute features for the entire image
	m = CropImgMatrix(dst, 0, 0, img->width - 1, img->height - 1);
	
	// release image
	cvReleaseImage(&dst);
/*
	m = new Matrix<float>(256, 256);
	for (int y = 0; y <256; y++) {
		for (int x = 0; x <256; x++) {
			//m->GetElement(x, y)
			m->SetValue(x,y,(float) rand());
		}
	}
*/

	// apply shearlet transform
	for (i = 0; i < nLevels; i++) {
		//printf("Shearlet Level %d\n",i);
		Shearletsapply(m, nOrients, &xlo, d, shearMatrix, g, h); //d contains the maps for nOrients at Level i
		// store results to be used later
		shearMaps.push_back(d); //vector<vector<Matrix<float> *> > shearMaps[i][nOrients][matrix *];	

		// clear structures and set for the next level
		delete m;
		d.clear();

		// set downsampled image
		m = xlo;
	}

	// release image matrix
	delete xlo;

	//for (int j = 0; j < nOrients;j++){
	//	for (i = 0; i < nLevels; i++) {
	//		float* data = shearMaps[i][j]->GetData();
	//		int featureLength = shearMaps[i][j]->GetNRows() * shearMaps[i][j]->GetNCols();
	//		unsigned char* dataResult = new unsigned char[featureLength];

	//		float min = 9999999.0, max = -9999999.0;
	//		for (int x = 0; x < featureLength; x++) {
	//			if (data[x] > max) max = data[x];
	//			if (data[x] < min) min = data[x];
	//		}

	//		for (int x = 0; x < featureLength; x++){
	//			dataResult[x] = 255 * (data[x] - min) / (max - min);
	//		}

	//		cv::Mat dataMat = cv::Mat(shearMaps[i][j]->GetNRows(), shearMaps[i][j]->GetNCols(), CV_8UC1, dataResult).clone();
	//		//cout << "rows:" << shearMaps[i][j]->GetNRows() << "cols:" << shearMaps[i][j]->GetNCols() << "data:" << data << endl;
	//		//cv::Mat featureMat = cv::Mat(shearMaps[i][j]->GetNRows(), shearMaps[i][j]->GetNCols(), CV_32FC1, data).clone();
	//		cout << "map convert end" << endl;

	//		strstream ss, ss1;
	//		string s, s1;
	//		ss << j;
	//		ss >> s;
	//		ss1 << i;
	//		ss1 >> s1;
	//		string errorFileName = "c_" + s + "_" + s1 + ".bmp";
	//		cv::imwrite(errorFileName, dataMat);
	//	}
	//}
	


	for (i = 0; i < nLevels; i++) {
		//printf("Level=%-4d nOrient=%-3d nShearOrient=%-3d #row=%-3d #col=%-3d\n", 
//						i,nOrients,shearMaps[i].size(),	shearMaps[i][0]->GetNRows(), shearMaps[i][0]->GetNCols());
		fprintf(fid, "Level=%-4d nOrient=%-3d nShearOrient=%-3d #row=%-3d #col=%-3d\n", 
						i,nOrients,shearMaps[i].size(),	shearMaps[i][0]->GetNRows(), shearMaps[i][0]->GetNCols());

	}
}

void HSC::ShearletHistogram(Position *pos, float *feat) {
int i, s, x0, y0, hh, ww, x1, y1;
vector<Matrix<float> *> d;
Matrix<float> *mtmp;
float value;
float values[4];
int idx = 0;
float valueSum = 0;
float L2;
int nfeatLevel;	// number of features in one level
int xhalf, yhalf;
int nFeatures=0;

	CV_FUNCNAME( "HSC::ExtractFeatures");

	// define number of features ina single level
	if (useCells == true)
		nfeatLevel = 4 * nOrients;
	else
		nfeatLevel = nOrients;

	// set initial coordinates to extract features
	x0 = pos->x0;
	y0 = pos->y0;
	ww = pos->w;
	hh = pos->h;

	// retrieve computed features for each scale
	for (s = 0; s < (int) shearMaps.size(); s++) //shearMaps.size() = #scales
	{

		// retrieve features
		d = shearMaps.at(s); //vector<Matrix<float> *> d;

		// set block location
		x0 = x0 >> s;
		y0 = y0 >> s;
		hh = hh >> s;
		ww = ww >> s;
		x1 = x0 + ww - 1;
		y1 = y0 + hh - 1;

		// compute the histogram for each orientation
		for (i = 0; i < (int) d.size(); i++) { // d.size() = #orientations

			mtmp = d.at(i); // d[i] = Matrix<float> *
			
			if (useCells == false) {
				value = SumRegion(mtmp, x0, y0, x1, y1);
				feat[idx + i] = value;
				valueSum += value;
//				printf("Feature %d [Scale=%d] [orient=%d] %10.3f\n", nFeatures, s, i,feat[idx + i]);
//				fprintf(fid,"Feature %d [Scale=%d] [orient=%d] %10.3f\n", nFeatures, s, i,feat[idx + i]);	
				nFeatures++;	
			}
			else {
				xhalf = x0 + ((x1 - x0 + 1) / 2);	
				yhalf = y0 + ((y1 - y0 + 1) / 2);
				values[0] = SumRegion(mtmp, x0, y0, xhalf - 1, yhalf - 1);
				values[1] = SumRegion(mtmp, xhalf, y0, x1, yhalf - 1);
				values[2] = SumRegion(mtmp, x0, yhalf, xhalf - 1, y1);
				values[3] = SumRegion(mtmp, xhalf, yhalf, x1, y1);
				valueSum += values[0];

				// set them into the feature vector
				feat[idx + i] = values[0];
				feat[idx + i + nOrients] = values[1];
				feat[idx + i + (2 * nOrients)] = values[2];
				feat[idx + i + (3 * nOrients)] = values[3];
			}
		}
		idx += nfeatLevel;
	}


	if (valueSum > 0) {
		// L2
		if (normType.compare("local") == 0 || normType.compare("local+global") == 0) { // normalization for each level
			idx = 0;
			for (i = 0; i < nLevels; i++) {  // local normalization
				L2 = mat.DotProductSSENotMultof4(feat + idx, feat + idx, nfeatLevel);
				L2 = sqrt(L2 + (float) EPS2);

				// compute L2-norm
				mat.DivideVectorsSSE(feat + idx, L2, feat + idx, nfeatLevel);
				idx += nfeatLevel;
			}
		}
		if (normType.compare("global") == 0 || normType.compare("local+global") == 0) {
			L2 = mat.DotProductSSENotMultof4(feat, feat, this->GetNFeatures());
			L2 = sqrt(L2 + (float) EPS2);

			// compute L2-norm
			mat.DivideVectorsSSE(feat, L2, feat, this->GetNFeatures());
		}
		if (normType.compare("none") == 0) {
			;
		}
	}

	//printf("After normalization\n");
	fprintf(fid,"After normalization\n");

	
	idx =0; nFeatures=0;
	for (s = 0; s < (int) shearMaps.size(); s++) //shearMaps.size() = #scale(nLevels)
	{
		for (i = 0; i < (int) shearMaps[s].size(); i++) { // d.size() = #orient
//				printf("Feature %d [Scale=%d] [orient=%d] %-10.3f\n", nFeatures, s, i, feat[idx + i]);
//				fprintf(fid,"Feature %d [Scale=%d] [orient=%d]  %-10.3f\n", nFeatures, s, i, feat[idx + i]);
				nFeatures++;	
		}
		idx += nfeatLevel;
	}

}


void HSC::ShearletHistogram(Vector<float> *shearHistogramFeatures,int blockW, int blockH, int stepX, int stepY) {

vector<Position> blocks;
int x, y, idx, w, h, i;
Position pos;

	CV_FUNCNAME("HSC::ExtractFeatures");
	
	if (shearMaps.size() == 0)
		DET_ERROR("call AddNewImage() first!");

	// create list of blocks
	h = shearMaps.at(0).at(0)->GetNRows();
	w = shearMaps.at(0).at(0)->GetNCols();

	for (y = 0; y < h - blockH + 1; y += stepY) {
		for (x = 0; x < w - blockW + 1; x += stepX) {
			pos.x0 = x;
			pos.y0 = y;
			pos.y1 = y + blockH - 1;
			pos.x1 = x + blockW - 1;
			pos.h = blockH;
			pos.w = blockW;
			blocks.push_back(pos);
		}
	}
// shearHistogramFeatures = new Vector<float>((int) blocks.size() * GetNFeatures());
	// extract features
	idx = 0;
	for (i = 0; i < (int) blocks.size(); i++) { //blocks.size() = #blocks
		//printf("\nBlock%-4d\n", i);
		fprintf(fid,"\nBlock%-4d\n", i);
		pos = blocks.at(i);
		ShearletHistogram(&pos, shearHistogramFeatures->GetData() + idx);
		idx += GetNFeatures();
	}
}

void HSC::ConcatenateFeatures(Vector<float> *shearFeatures,Vector<float> *shearBlockAveFeatures,Vector<float> *shearHistogramFeatures)
{
	int i;
	float value;

	fprintf(fid,"ConcatenateFeatures\n");
	int dim = shearHistogramFeatures->GetNElements();
	int count=0;
	for (i = 0; i < dim; i++){
		value = shearHistogramFeatures -> GetElement(i);
		fprintf(fid,"%8.3f",value);
		shearFeatures->SetElement(count++,value);
	}

	dim = shearBlockAveFeatures->GetNElements();
	for (i = 0; i < dim; i++){
		value = shearBlockAveFeatures -> GetElement(i);
		fprintf(fid,"%8.3f",value);
		shearFeatures->SetElement(count++,value);
	}

}


float HSC::BlockAverage(Matrix<float> *mat,int startX,int startY,int w,int h)
{
	int i,j;
	float value=0, total=(float) w*h;
	for (i = startX; i < startX+w; i++) for (j=startY;j<startY+h;++j) 
		value+=mat->GetElement(i,j);
	value/=total;
	return value;
}

void HSC::ShearBlockAverage(Vector<float> *shearBlockAveFeatures, int blockW, int blockH, int stepX, int stepY)
{
	int i,j,m,n;
	float value;
	Matrix<float> *mat;

	int count=0;
	for (i = 0; i < nLevels; i++) for (j=0;j<nOrients;++j) {
			mat=shearMaps[i][j];
			int blockWidth=mat->GetNRows()/nPartitionsInWidth,blockHeight=mat->GetNCols()/nPartitionsInWidth;
			for (m= 0; m < nPartitionsInWidth; m++) for (n= 0; n < nPartitionsInWidth; n++) {
				int startX=m*blockWidth, startY=n*blockWidth;			
				value= BlockAverage(mat,startX,startY,blockWidth,blockWidth);	
				fprintf(fid,"%10.3f",value);
				shearBlockAveFeatures->SetElement(count++,value);
			}
	}

}
