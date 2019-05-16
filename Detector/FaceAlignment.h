/*
Author: Bi Sai 
Date: 2014/06/18
This program is a reimplementation of algorithms in "Face Alignment by Explicit 
Shape Regression" by Cao et al.
If you find any bugs, please email me: soundsilencebisai-at-gmail-dot-com

Copyright (c) 2014 Bi Sai 
The MIT License (MIT)
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef FACE_ALIGNMENT_H
#define FACE_ALIGNMENT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <ctime>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>
#include <vector>
#include <fstream>
#include <numeric>   
#include <utility> 

using namespace std;
using namespace cv;

class BoundingBox{
    public:
        float start_x;
        float start_y;
        float width;
        float height;
        float centroid_x;
        float centroid_y;
        BoundingBox(){
            start_x = 0;
            start_y = 0;
            width = 0;
            height = 0;
            centroid_x = 0;
            centroid_y = 0;
        }; 
};


class Fern{
    private:
        int fern_pixel_num_;
        int landmark_num_;
        Mat_<int> selected_nearest_landmark_index_;
        Mat_<float> threshold_;
        Mat_<int> selected_pixel_index_;
        Mat_<float> selected_pixel_locations_;
        vector<Mat_<float> > bin_output_;
    public:
        vector<Mat_<float> > Train(const vector<vector<float> >& candidate_pixel_intensity,
                                    const Mat_<float>& covariance,
                                    const Mat_<float>& candidate_pixel_locations,
                                    const Mat_<int>& nearest_landmark_index,
                                    const vector<Mat_<float> >& regression_targets,
                                    int fern_pixel_num);
        Mat_<float> Predict(const Mat_<uchar>& image,
                             const Mat_<float>& shape,
                             const Mat_<float>& rotation,
                             const BoundingBox& bounding_box,
                             float scale);
        void Read(ifstream& fin);
		void ReadBinary(istream& fin);
		void ReadBinary(istream& fin, float norm_coeff);
        void Write(ofstream& fout);
		void WriteBinary(ofstream& fout);
};

class FernCascade{
    public:
        vector<Mat_<float> > Train(const vector<Mat_<uchar> >& images,
                                    const vector<Mat_<float> >& current_shapes,
                                    const vector<Mat_<float> >& ground_truth_shapes,
                                    const vector<BoundingBox> & bounding_box,
                                    const Mat_<float>& mean_shape,
                                    int second_level_num,
                                    int candidate_pixel_num,
                                    int fern_pixel_num);  
        Mat_<float> Predict(const Mat_<uchar>& image,
                          const BoundingBox& bounding_box, 
                          const Mat_<float>& mean_shape,
                          const Mat_<float>& shape);
        void Read(ifstream& fin);
		void ReadBinary(istream& fin);
		void ReadBinary(istream& fin, float norm_coeff);
        void Write(ofstream& fout);
		void WriteBinary(ofstream& fout);
    private:
        vector<Fern> ferns_;
        int second_level_num_;
};

class ShapeRegressor{
    public:
        ShapeRegressor(); 
        void Train(const vector<Mat_<uchar> >& images, 
                   const vector<Mat_<float> >& ground_truth_shapes,
                   const vector<BoundingBox>& bounding_box,
                   int first_level_num, int second_level_num,
                   int candidate_pixel_num, int fern_pixel_num,
                   int initial_num);
        Mat_<float> Predict(const Mat_<uchar>& image, const BoundingBox& bounding_box, int initial_num);
        void Read(ifstream& fin);
		void ReadBinary(istream& fin);
		//void ReadBinary(ibstream& fin, float norm_coeff);
        void Write(ofstream& fout);
		void WriteBinary(ofstream& fout);
        //void Load(string path);
        void Load(const char* buffer);
        void Save(string path);
    private:
        int first_level_num_;
        int landmark_num_;
        vector<FernCascade> fern_cascades_;
        Mat_<float> mean_shape_;
        vector<Mat_<float> > training_shapes_;
        vector<BoundingBox> bounding_box_;
};

Mat_<float> GetMeanShape(const vector<Mat_<float> >& shapes,
                          const vector<BoundingBox>& bounding_box);
Mat_<float> ProjectShape(const Mat_<float>& shape, const BoundingBox& bounding_box);
Mat_<float> ReProjectShape(const Mat_<float>& shape, const BoundingBox& bounding_box);
void SimilarityTransform(const Mat_<float>& shape1, const Mat_<float>& shape2,
                         Mat_<float>& rotation,float& scale);
float calculate_covariance(const vector<float>& v_1,
                            const vector<float>& v_2);

#endif
