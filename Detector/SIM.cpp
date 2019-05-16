//
//  SIM.cpp
//  AuthenVerifySDK
//
//  Created by authen on 5/7/15.
//  Copyright (c) 2015 CK_S. All rights reserved.
//

//#include "SIM.h"
//
//
///* solve linear equation system of 4th order, Ah=b, h=A^(-1)b */
//void linsolve4(float* A, float* b, float* h)
//{
//
//    float det;
//    float x, y, w, z;
//
//    x = A[0]; y = A[1]; w = A[2]; z = A[3];
//    det = w*z - x*x - y*y;
//
//    /*
//     |-x -y  w  0|
//     1  | y -x  0  w|
//     A^(-1) = -----| z  0 -x  y|
//     det | 0  z -y -x|
//
//     */
//
//    h[0] = -x * b[0] - y * b[1] + w * b[2];
//    h[1] = y * b[0] - x * b[1] + w * b[3];
//    h[2] = z * b[0] - x * b[2] + y * b[3];
//    h[3] = z * b[1] - y * b[2] - x * b[3];
//
//    h[0] /= det;
//    h[1] /= det;
//    h[2] /= det;
//    h[3] /= det;
//}
//
//
//// compute scale, rotation, and translation parameters
//void sim_params_from_points(const TPointF dstKeyPoints[],
//                                   const TPointF srcKeyPoints[], int count,
//                                   float* a, float* b, float* tx, float* ty)
//{
//    int i;
//    float X1, Y1, X2, Y2, Z, C1, C2;
//    float A[4], c[4], h[4];
//
//    X1 = 0.f; Y1 = 0.f;
//    X2 = 0.f; Y2 = 0.f;
//    Z = 0.f;
//    C1 = 0.f; C2 = 0.f;
//    for (i = 0; i < count; i++) {
//        float x1, y1, x2, y2;
//
//        x1 = dstKeyPoints[i].x;
//        y1 = dstKeyPoints[i].y;
//        x2 = srcKeyPoints[i].x;
//        y2 = srcKeyPoints[i].y;
//
//        X1 += x1;
//        Y1 += y1;
//        X2 += x2;
//        Y2 += y2;
//        Z += (x2 * x2 + y2 * y2);
//        C1 += (x1 * x2 + y1 * y2);
//        C2 += (y1 * x2 - x1 * y2);
//    }
//
//    /* solve Ah = c
//     A = 	|X2, -Y2,   w,   0|		b=|X1|
//     |Y2,  X2,   0,   w|       |Y1|
//     | Z,   0,  X2,  Y2|       |C1|
//     | 0,   Z, -Y2,  X2|       |C2|;
//     */
//    A[0] = X2; A[1] = Y2; A[2] = (float)count;  A[3] = Z;
//    c[0] = X1; c[1] = Y1; c[2] = C1; c[3] = C2;
//    linsolve4(A, c, h);
//
//    /* rotation, scaling, and translation parameters */
//    *a = h[0];
//    *b = h[1];
//    *tx = h[2];
//    *ty = h[3];
//}
//
//void sim_transform_image(const unsigned char* gray, int width, int height,
//                                unsigned char* dst, int width1, int height1,
//                                float a, float b, float tx, float ty)
//{
//    int i, j;
//    float x, y;
//    int ix, iy;
//    float u, v;
//
//    for (i = 0; i < height1; i++) {
//        for (j = 0; j < width1; j++) {
//            x = a * j - b * i + tx;
//            y = a * i + b * j + ty;
//
//            ix = (int)x;
//            iy = (int)y;
//
//            u = x - ix;
//            v = y - iy;
//
//            if (ix >= 0 && ix <= width - 2 &&
//                iy >= 0 && iy <= height - 2) {
//                dst[i * width1 + j] = (unsigned char)((1.0f - u) * (1.0f - v) * gray[iy * width + ix] +
//                                                      u * (1.0f - v) * gray[iy * width + ix + 1] +
//                                                      (1.0f - u) * v * gray[(iy + 1) * width + ix] +
//                                                      u * v * gray[(iy + 1) * width + ix + 1] + 0.5f);
//            }
//            else {
//                dst[i * width1 + j] = 0;
//            }
//        }
//    }
//}

/*
float estimate(unsigned char *pData, int width, int height)
{
    int i = 0, j = 0, flag = 0;
    unsigned char *pixel;
    int *image0, *image90, *image45, *image135;
    int a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int threshold = 100;
    int t = 0;
    float *image;
    float sum = 0.0;
    int output_width = 0, output_height = 0;
    double w_ratio = 0.0, h_ratio = 0.0;
    int w = 0, h = 0, ww = 0, hh = 0;
    double u = 0.0, v = 0.0;
    double f11 = 0.0, f12 = 0.0, f21 = 0.0, f22 = 0.0;
    int pixel_value = 0;
    if (width > 80){
        output_width = 80;
        output_height = height * 80 / width + 0.5;
        flag = 1;
    }
    else if (height > 100){
        output_height = 100;
        output_width = width * 100 / height + 0.5;
        flag = 1;
    }
    else{
        output_height = height;
        output_width = width;
    }
    //printf("%d, %d, %d, %d\n", width, height, output_width, output_height);
    
    pixel = new unsigned char[output_width*output_height];
    image0 = new int[output_width*output_height];
    image45 = new int[output_width*output_height];
    image90 = new int[output_width*output_height];
    image135 = new int[output_width*output_height];
    image = new float[output_width*output_height];
    
    memset(pixel, 0, output_width*output_height);
    memset(image0, 0, output_width*output_height * sizeof(int));
    memset(image45, 0, output_width*output_height * sizeof(int));
    memset(image90, 0, output_width*output_height * sizeof(int));
    memset(image135, 0, output_width*output_height * sizeof(int));
    memset(image, 0, output_width*output_height * sizeof(float));
    
  
    //inite
    //pixel = pixel     pucImg_BGR = pData
    if (flag == 1){
        w_ratio = 1.0*output_width / width;
        h_ratio = 1.0*output_height / height;
        for (w = 0; w<output_width; w++)
            for (h = 0; h<output_height; h++)
            {
                ww = floor(w / w_ratio);
                hh = floor(h / h_ratio);
                if (ww >= width - 1)
                    ww = width - 2;
                if (hh >= height - 1)
                    hh = height - 2;
                v = w / w_ratio - ww;
                u = h / h_ratio - hh;
                //B
                f11 = pData[hh*width + ww];
                f12 = pData[hh*width + (ww + 1)];
                f21 = pData[(hh + 1)*width + ww];
                f22 = pData[(hh + 1)*width + (ww + 1)];
                pixel_value = (1 - u)*(1 - v)*f11 + (1 - u)*v*f12 + u*(1 - v)*f21 + u*v*f22;
                if (pixel_value>255) pixel_value = 255;
                if (pixel_value<0) pixel_value = 0;
                pixel[h*output_width + w] = (unsigned char)pixel_value;
            }
        
    }
    else if (pData[height*width] != NULL){
        for (i = 0; i < height; i++){
            for (j = 0; j <width; j++){
                pixel[i*width + j] = pData[t++];
            }
            t = t + 1;
        }
    }
    else{
        for (i = 0; i < height; i++){
            for (j = 0; j <width; j++){
                pixel[i*width + j] = pData[i*width + j];
            }
        }
    }
    
    
    
    
    
    // a1 a2 a3
    // b1 b2 b3
    // c1 c2 c3
    // replicate for the bounds, the nearest one
    for (i = 0; i < output_height; i++)
        for (j = 0; j< output_width; j++){
            if (i == 0){//upper line, left-up and right-up corners
                if (j == 0){//left-up corner
                    a1 = pixel[0];
                    a2 = pixel[0];
                    a3 = pixel[1];
                    b1 = pixel[0];
                    b2 = pixel[0];
                    b3 = pixel[1];
                    c1 = pixel[output_width];
                    c2 = pixel[output_width];
                    c3 = pixel[output_width + 1];
                }
                else if (j == (output_width - 1)){//left-right corner
                    a1 = pixel[output_width - 2];
                    a2 = pixel[output_width - 1];
                    a3 = pixel[output_width - 1];
                    b1 = pixel[output_width - 2];
                    b2 = pixel[output_width - 1];
                    b3 = pixel[output_width] - 1;
                    c1 = pixel[2 * output_width - 2];
                    c2 = pixel[2 * output_width - 1];
                    c3 = pixel[2 * output_width - 1];
                }
                else{
                    a1 = pixel[j - 1];
                    a2 = pixel[j];
                    a3 = pixel[j + 1];
                    b1 = a1;
                    b2 = a2;
                    b3 = a3;
                    c1 = pixel[output_width + j - 1];
                    c2 = pixel[output_width + j];
                    c3 = pixel[output_width + j + 1];
                }
            }
            else if (j == 0){//left row and left-down corner
                if (i == (output_height - 1)){//left-down corner
                    a1 = pixel[(i - 1)*output_width];
                    a2 = a1;
                    a3 = pixel[(i - 1)*output_width + 1];
                    b1 = pixel[i*output_width];
                    b2 = b1;
                    b3 = pixel[i*output_width + 1];
                    c1 = b2;
                    c2 = b2;
                    c3 = b3;
                }
                else{
                    a1 = pixel[(i - 1)*output_width];
                    a2 = a1;
                    a3 = pixel[(i - 1)*output_width + 1];
                    b1 = pixel[i*output_width];
                    b2 = b1;
                    b3 = pixel[i*output_width + 1];
                    c1 = pixel[(i + 1)*output_width];
                    c2 = c1;
                    c3 = pixel[(i + 1)*output_width + 1];
                }
            }
            else if (i == (output_height - 1)){//down line and down-right corner
                if (j == (output_width - 1)){//down-right corner
                    a1 = pixel[(i - 1)*output_width + j - 1];
                    a2 = pixel[(i - 1)*output_width + j];
                    a3 = a2;
                    b1 = pixel[i*output_width + j - 1];
                    b2 = pixel[i*output_width + j];
                    b3 = b2;
                    c1 = b1;
                    c2 = b2;
                    c3 = b2;
                }
                else{
                    a1 = pixel[(i - 1)*output_width + j - 1];
                    a2 = pixel[(i - 1)*output_width + j];
                    a3 = pixel[(i - 1)*output_width + j + 1];
                    b1 = pixel[i*output_width + j - 1];
                    b2 = pixel[i*output_width + j];
                    b3 = pixel[i*output_width + j + 1];
                    c1 = b1;
                    c2 = b2;
                    c3 = b3;
                }
            }
            else if (j == (output_width - 1)){//right row
                a1 = pixel[(i - 1)*output_width + j - 1];
                a2 = pixel[(i - 1)*output_width + j];
                a3 = a2;
                b1 = pixel[i*output_width + j - 1];
                b2 = pixel[i*output_width + j];
                b3 = b2;
                c1 = pixel[(i + 1)*output_width - 1];
                c2 = pixel[(i + 1)*output_width - 1];
                c3 = c2;
            }
            else{
                a1 = pixel[(i - 1)*output_width + j - 1];
                a2 = pixel[(i - 1)*output_width + j];
                a3 = pixel[(i - 1)*output_width + j + 1];
                b1 = pixel[i*output_width + j - 1];
                b2 = pixel[i*output_width + j];
                b3 = pixel[i*output_width + j + 1];
                c1 = pixel[(i + 1)*output_width + j - 1];
                c2 = pixel[(i + 1)*output_width + j];
                c3 = pixel[(i + 1)*output_width + j + 1];
            }
            //corr
            // 0:-1 0 1  45:-2 -1 0  90:1 2 1  135:0 -1 -2
            //   -2 0 2     -1  0 1     0 0 0      1  0 -1
            //   -1 0 1      0  1 2    -1 -2 -1    2  1  0
            
            image0[i*output_width + j] = -a1 + a3 - 2 * b1 + 2 * b3 - c1 + c3;
            
            if (image0[i*output_width + j] < 100)
                image0[i*output_width + j] = 0;
            else image0[i*output_width + j] = 1;
            
       
            if (image45[i*output_width + j] < 100)
                image45[i*output_width + j] = 0;
            else image45[i*output_width + j] = 1;
            
    
            if (image90[i*output_width + j] < 100)
                image90[i*output_width + j] = 0;
            else image90[i*output_width + j] = 1;
            
    
            image135[i*output_width + j] = -a2 - 2 * a3 + b1 - b3 + 2 * c1 + c2;
            
            if (image135[i*output_width + j] < 100)
                image135[i*output_width + j] = 0;
            else image135[i*output_width + j] = 1;

        }
    for (i = 0; i <output_height - 1; i++)
        for (j = 0; j < output_width - 1; j++)
            image[i*output_width + j] = (float)(image0[i*output_width + j] + image45[i*output_width + j] + image90[i*output_width + j] + image135[i*output_width + j]) / 4;
    for (i = 0; i < output_height - 1; i++)
        for (j = 0; j < output_width - 1; j++)
            sum = (float)sum + image[i*output_width + j];
    sum = (float)sum / (output_height*output_width);
    //	for(i = 0; i<10;i++)
    //		printf("%d\n",image0[i]);
    
    delete[] pixel;
    delete[] image0;
    delete[] image45;
    delete[] image90;
    delete[] image135;
    delete[] image;
    return sum;
    
}*/

/*
float estimate(unsigned char* pData, int width, int height, int step)
{
    int i = 0, j = 0;
    unsigned char *pixel;
    int *image0;
    int *image90;
    int *image45;
    int *image135;
    int a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int threshold = 100;
    int t = 0;
    float *image;
    float sum = 0.0f;
    pixel = new unsigned char[width*height];
    image0 = new int[width*height];
    image45 = new int[width*height];
    image90 = new int[width*height];
    image135 = new int[width*height];
    image = new float[width*height];
    
    //inite
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixel[i*width + j] = pData[i*step + j];
        }
        //pData += pitch;
    }
 //   printf(".............pixel[0] = %d,  pixel[1] = %d, (int)pixel[2] = %d\n", (int)pixel[0],  (int)pixel[1], (int)pixel[2]);

    // a1 a2 a3
    // b1 b2 b3
    // c1 c2 c3
    // replicate for the bounds, the nearest one
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
        {
            if (i == 0)
            {
                //upper line, left-up and right-up corners
                if (j == 0)
                {
                    //left-up corner
                    a1 = pixel[0];
                    a2 = pixel[0];
                    a3 = pixel[1];
                    b1 = pixel[0];
                    b2 = pixel[0];
                    b3 = pixel[1];
                    c1 = pixel[width];
                    c2 = pixel[width];
                    c3 = pixel[width + 1];
                }
                else if (j == (width - 1))
                {
                    //left-right corner
                    a1 = pixel[width - 2];
                    a2 = pixel[width - 1];
                    a3 = pixel[width - 1];
                    b1 = pixel[width - 2];
                    b2 = pixel[width - 1];
                    b3 = pixel[width] - 1;
                    c1 = pixel[2*width - 2];
                    c2 = pixel[2*width - 1];
                    c3 = pixel[2*width - 1];
                }
                else
                {
                    a1 = pixel[j - 1];
                    a2 = pixel[j];
                    a3 = pixel[j + 1];
                    b1 = a1;
                    b2 = a2;
                    b3 = a3;
                    c1 = pixel[width + j - 1];
                    c2 = pixel[width + j];
                    c3 = pixel[width + j + 1];
                }
            }
            else if (j == 0)
            {
                //left row and left-down corner
                if (i == (height - 1))
                {
                    //left-down corner
                    a1 = pixel[(i - 1)*width];
                    a2 = a1;
                    a3 = pixel[(i - 1)*width + 1];
                    b1 = pixel[i*width];
                    b2 = b1;
                    b3 = pixel[i*width + 1];
                    c1 = b2;
                    c2 = b2;
                    c3 = b3;
                }
                else
                {
                    a1 = pixel[(i - 1)*width];
                    a2 = a1;
                    a3 = pixel[(i - 1)*width + 1];
                    b1 = pixel[i*width];
                    b2 = b1;
                    b3 = pixel[i*width + 1];
                    c1 = pixel[(i + 1)*width];
                    c2 = c1;
                    c3 = pixel[(i + 1)*width + 1];
                }
            }
            else if (i == (height - 1))
            {
                //down line and down-right corner
                if (j == (width - 1))
                {
                    //down-right corner
                    a1 = pixel[(i - 1)*width + j - 1];
                    a2 = pixel[(i - 1)*width + j];
                    a3 = a2;
                    b1 = pixel[i*width + j - 1];
                    b2 = pixel[i*width + j];
                    b3 = b2;
                    c1 = b1;
                    c2 = b2;
                    c3 = b2;
                }
                else
                {
                    a1 = pixel[(i - 1)*width + j - 1];
                    a2 = pixel[(i - 1)*width + j];
                    a3 = pixel[(i - 1)*width + j + 1];
                    b1 = pixel[i*width + j - 1];
                    b2 = pixel[i*width + j];
                    b3 = pixel[i*width + j + 1];
                    c1 = b1;
                    c2 = b2;
                    c3 = b3;
                }
            }
            else if (j == (width - 1))
            {
                //right row
                a1 = pixel[(i - 1)*width + j - 1];
                a2 = pixel[(i - 1)*width + j];
                a3 = a2;
                b1 = pixel[i*width + j - 1];
                b2 = pixel[i*width + j];
                b3 = b2;
                c1 = pixel[(i + 1)*width - 1];
                c2 = pixel[(i + 1)*width - 1];
                c3 = c2;
            }
            else
            {
                a1 = pixel[(i - 1)*width + j - 1];
                a2 = pixel[(i - 1)*width + j];
                a3 = pixel[(i - 1)*width + j + 1];
                b1 = pixel[i*width + j - 1];
                b2 = pixel[i*width + j];
                b3 = pixel[i*width + j + 1];
                c1 = pixel[(i + 1)*width + j - 1];
                c2 = pixel[(i + 1)*width + j];
                c3 = pixel[(i + 1)*width + j + 1];
            }
            //corr
            // 0:-1 0 1  45:-2 -1 0  90:1 2 1  135:0 -1 -2
            //   -2 0 2     -1  0 1     0 0 0      1  0 -1
            //   -1 0 1      0  1 2    -1 -2 -1    2  1  0
            
            image0[i*width + j] = -a1 + a3 - 2*b1 + 2*b3 - c1 + c3;
            //if(i==0 && j==0)
            // printf("%d\n",pixel[(width+1)]);
            if (image0[i*width + j] < 100)
                image0[i*width + j] = 0;
            else image0[i*width + j] = 1;
            image45[i*width + j] = -2*a1 - a2 - b1 + b3 + c2 + 2*c3;
            if (image45[i*width + j] < 100)
                image45[i*width + j] = 0;
            else image45[i*width + j] = 1;
            image90[i*width + j] = a1 + 2*a2 + a3 - c1 - 2*c2 - c3;
            if (image90[i*width + j] < 100)
                image90[i*width + j] = 0;
            else image90[i*width + j] = 1;
            image135[i*width + j] = -a2 - 2*a3 + b1 - b3 + 2*c1 + c2;
            if (image135[i*width + j] < 100)
                image135[i*width + j] = 0;
            else image135[i*width + j] = 1;
        }
    for (i = 0; i < height - 1; i++)
        for (j = 0; j < width - 1; j++)
            image[i*width + j] =
            (float)
            (image0[i*width + j] + image45[i*width + j] + image90[i*width + j] + image135[i*width + j])/
            4;
    for (i = 0; i < height - 1; i++)
        for (j = 0; j < width - 1; j++)
            sum = sum + image[i*width + j];
    sum = sum/(height*width);
    //	for(i = 0; i<10;i++)
    //		printf("%d\n",image0[i]);
    
    delete [] pixel;
    delete [] image0;
    delete [] image45;
    delete [] image90;
    delete [] image135;
    delete [] image;
    
    return sum;
}



void imgResize(unsigned char* gray, unsigned char* gray_out, int height, int output_height, int width, int output_width)
{
    double w_ratio = (double)output_width / (double)width;// resize_ratio;
    double h_ratio = (double)output_height / (double)height;// resize_ratio;
    //int output_width = (int)(width * resize_ratio);
    // int output_height = (int)(height * resize_ratio);
    int w, h, ww, hh;
    double u, v;
    double f11, f12, f21, f22;
    
    for (w = 0; w < output_width; w++)
        for (h = 0; h < output_height; h++)
        {
            ww = (int)(w / w_ratio);
            hh = (int)(h / h_ratio);
            if (ww >= width - 1)
                ww = width - 2;
            if (hh >= height - 1)
                hh = height - 2;
            v = w / w_ratio - ww;
            u = h / h_ratio - hh;
            
            f11 = gray[hh * width + ww];
            f12 = gray[hh * width + ww + 1];
            f21 = gray[(hh + 1) * width + ww];
            f22 = gray[(hh + 1) * width + ww + 1];
            //pfOutImg[h * output_width + w] = (byte)((1 - u) * (1 - v) * f11 + (1 - u) * v * f12 + u * (1 - v) * f21 + u * v * f22);
            gray_out[w * output_height + h] = (unsigned char)(abs((int)((1 - u) * (1 - v) * f11 + (1 - u) * v * f12 + u * (1 - v) * f21 + u * v * f22)));
            
        }
}

*/
