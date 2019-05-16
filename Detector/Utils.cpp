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

#include "FaceAlignment.h"


Mat_<float> GetMeanShape(const vector<Mat_<float> >& shapes,
                          const vector<BoundingBox>& bounding_box){
    Mat_<float> result = Mat::zeros(shapes[0].rows,2,CV_32FC1);
    for(int i = 0;i < shapes.size();i++){
        result = result + ProjectShape(shapes[i],bounding_box[i]);
    }
    result = 1.0 / shapes.size() * result;

    return result;
}

Mat_<float> ProjectShape(const Mat_<float>& shape, const BoundingBox& bounding_box){
    Mat_<float> temp(shape.rows,2);
    for(int j = 0;j < shape.rows;j++){
    	/*temp(j,0) = ((FixedNumber(shape(j,0))-FixedNumber(bounding_box.centroid_x)) /
    			(FixedNumber(bounding_box.width) / FixedNumber(2))).toFloat();
    	temp(j,1) = ((FixedNumber(shape(j,1))-FixedNumber(bounding_box.centroid_y)) /
    			(FixedNumber(bounding_box.height) / FixedNumber(2))).toFloat();*/
        temp(j,0) = (shape(j,0)-bounding_box.centroid_x) / (bounding_box.width / 2.0);
        temp(j,1) = (shape(j,1)-bounding_box.centroid_y) / (bounding_box.height / 2.0);
    } 
    return temp;  
}


Mat_<float> ReProjectShape(const Mat_<float>& shape, const BoundingBox& bounding_box){
    Mat_<float> temp(shape.rows,2);
    for(int j = 0;j < shape.rows;j++)
    {
        temp(j,0) = (shape(j,0) * bounding_box.width / 2.0 + bounding_box.centroid_x);
        temp(j,1) = (shape(j,1) * bounding_box.height / 2.0 + bounding_box.centroid_y);

        //temp(j,0) = (FixedNumber(shape(j,0)) * FixedNumber(bounding_box.width) / FixedNumber(2) + FixedNumber(bounding_box.centroid_x)).toFloat();
        //temp(j,1) = (FixedNumber(shape(j,1)) * FixedNumber(bounding_box.height) / FixedNumber(2) + FixedNumber(bounding_box.centroid_y)).toFloat();
    } 
    return temp; 
}


void SimilarityTransform(const Mat_<float>& shape1, const Mat_<float>& shape2,
                         Mat_<float>& rotation,float& scale){
    rotation = Mat::zeros(2,2,CV_32FC1);
    scale = 0;
    
    // center the data
    float center_x_1 = 0;
    float center_y_1 = 0;
    float center_x_2 = 0;
    float center_y_2 = 0;
    for(int i = 0;i < shape1.rows;i++){
        center_x_1 += shape1(i,0);
        center_y_1 += shape1(i,1);
        center_x_2 += shape2(i,0);
        center_y_2 += shape2(i,1); 
    }
    center_x_1 /= shape1.rows;
    center_y_1 /= shape1.rows;
    center_x_2 /= shape2.rows;
    center_y_2 /= shape2.rows;
    
    Mat_<float> temp1 = shape1.clone();
    Mat_<float> temp2 = shape2.clone();
    for(int i = 0;i < shape1.rows;i++){
        temp1(i,0) -= center_x_1;
        temp1(i,1) -= center_y_1;
        temp2(i,0) -= center_x_2;
        temp2(i,1) -= center_y_2;
    }

     
    Mat_<float> covariance1, covariance2;
    Mat_<float> mean1,mean2;
    // calculate covariance matrix
    calcCovarMatrix(temp1,covariance1,mean1,CV_COVAR_COLS, CV_32FC1);
    calcCovarMatrix(temp2,covariance2,mean2,CV_COVAR_COLS, CV_32FC1);

    float s1 = sqrt(norm(covariance1));
    float s2 = sqrt(norm(covariance2));
    scale = s1 / s2; 
    temp1 = 1.0 / s1 * temp1;
    temp2 = 1.0 / s2 * temp2;

    float num = 0;
    float den = 0;
    for(int i = 0;i < shape1.rows;i++){
        num = num + temp1(i,1) * temp2(i,0) - temp1(i,0) * temp2(i,1);
        den = den + temp1(i,0) * temp2(i,0) + temp1(i,1) * temp2(i,1);      
    }
    
    float norm = sqrt(num*num + den*den);
    float sin_theta = num / norm;
    float cos_theta = den / norm;
    rotation(0,0) = cos_theta;
    rotation(0,1) = -sin_theta;
    rotation(1,0) = sin_theta;
    rotation(1,1) = cos_theta;
}

float calculate_covariance(const vector<float>& v_1,
                            const vector<float>& v_2){
    assert(v_1.size() == v_2.size());
    assert(v_1.size() != 0);
    float sum_1 = 0;
    float sum_2 = 0;
    float exp_1 = 0;
    float exp_2 = 0;
    float exp_3 = 0;
    for(int i = 0;i < v_1.size();i++){
        sum_1 += v_1[i];
        sum_2 += v_2[i];
    }
    exp_1 = sum_1 / v_1.size();
    exp_2 = sum_2 / v_2.size();
    for(int i = 0;i < v_1.size();i++){
        exp_3 = exp_3 + (v_1[i] - exp_1) * (v_2[i] - exp_2);
    }
    return exp_3 / v_1.size();
}


