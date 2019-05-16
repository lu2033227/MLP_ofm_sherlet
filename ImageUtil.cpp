#include "ImageUtil.h"
#include "math.h"
#include <string.h>

#define FLT_MIN -99999999
void ImageUtil::BGR2Gray(const unsigned char* bgr, int width, int height, int pitch, unsigned char* gray)
{
	int i, j;

	// bgr to gray
	const unsigned char* pSrc = bgr;
	unsigned char* dst = gray;
	for (i = 0 ; i < height; i++) {
		for (j = 0; j < width; j++) {
			dst[j] = (306 * pSrc[j * 3 + 2] + 601 * pSrc[j * 3 + 1] + 117 * pSrc[j * 3]) >> 10;
		}
		dst += width;
		pSrc += pitch;
	}
}

void ImageUtil::AlignGray(const unsigned char* gray1, int width, int height, int pitch, unsigned char* gray)
{
	int i;

	// bgr to gray
	const unsigned char* pSrc = gray1;
	unsigned char* dst = gray;
	for (i = 0 ; i < height; i++) {
		memcpy(dst, pSrc, width);
		dst += width;
		pSrc += pitch;
	}
}
bool ImageUtil::solve(float* A, int m, float* b)
{
    float* L = A;
    int i, j, k;
    float s;

    for( i = 0; i < m; i++ ) {
        for( j = 0; j < i; j++ ) {
            s = A[i * m + j];
            for( k = 0; k < j; k++ )
                s -= L[i * m + k] * L[j * m + k];
            L[i * m + j] = s * L[j * m + j];
        }
        s = A[i * m + i];
        for( k = 0; k < j; k++ ) {
            float t = L[i * m + k];
            s -= t * t;
        }
        if( s < FLT_MIN )
            return false;
        L[i * m + i] = 1.0f / sqrtf(s);
    }

    if( !b )
        return true;

    // LLt x = b
    // 1: L y = b
    // 2. Lt x = y

    /*
     [ L00             ]  y0   b0
     [ L10 L11         ]  y1 = b1
     [ L20 L21 L22     ]  y2   b2
     [ L30 L31 L32 L33 ]  y3   b3

     [ L00 L10 L20 L30 ]  x0   y0
     [     L11 L21 L31 ]  x1 = y1
     [         L22 L32 ]  x2   y2
     [             L33 ]  x3   y3
    */

    for( i = 0; i < m; i++ ) {
        s = b[i];
        for( k = 0; k < i; k++ )
            s -= L[i * m + k] * b[k];
        b[i] = s * L[i * m + i];
    }

    for( i = m - 1; i >= 0; i-- ) {
        s = b[i];
        for( k = m - 1; k > i; k-- )
            s -= L[k * m + i] * b[k];
        b[i] = s * L[i * m + i];
    }

    return true;
}

/* solve linear equation system of 4th order, Ah=b, h=A^(-1)b */
void ImageUtil::linsolve4(float* A, float* b, float* h)
{

	float det;
	float x, y, w, z;

	x =  A[0]; y =  A[1]; w =  A[2]; z =  A[3];
	det = w*z - x*x - y*y;

	/*
	                 |-x -y  w  0|
	              1  | y -x  0  w|
       A^(-1) = -----| z  0 -x  y|
	             det | 0  z -y -x|

	*/

	h[0] = -x * b[0] - y * b[1] + w * b[2];
	h[1] =  y * b[0] - x * b[1] + w * b[3];
	h[2] =  z * b[0] - x * b[2] + y * b[3];
	h[3] =  z * b[1] - y * b[2] - x * b[3];

	h[0] /= det;
	h[1] /= det;
	h[2] /= det;
	h[3] /= det;
}

void ImageUtil::sim_params_from_points(const TPointF dstKeyPoints[],
									 const TPointF srcKeyPoints[], int count,
									 float* a, float* b, float* tx, float* ty)
{
	int i;
	float X1, Y1, X2, Y2, Z, C1, C2;
	float A[4], c[4], h[4];

	X1 = 0.f; Y1 = 0.f;
	X2 = 0.f; Y2 = 0.f;
	Z = 0.f;
	C1 = 0.f; C2 = 0.f;
	for(i = 0; i < count; i++) {
		float x1, y1, x2, y2;

		x1 = dstKeyPoints[i].x;
		y1 = dstKeyPoints[i].y;
		x2 = srcKeyPoints[i].x;
		y2 = srcKeyPoints[i].y;

		X1 += x1;
		Y1 += y1;
		X2 += x2;
		Y2 += y2;
		Z  += (x2 * x2 + y2 * y2);
		C1 += (x1 * x2 + y1 * y2);
		C2 += (y1 * x2 - x1 * y2);
	}

	/* solve Ah = c
	   A = 	|X2, -Y2,   w,   0|		b=|X1|
	        |Y2,  X2,   0,   w|       |Y1|
	        | Z,   0,  X2,  Y2|       |C1|
	        | 0,   Z, -Y2,  X2|       |C2|;
	*/
	A[0] = X2; A[1] = Y2; A[2] = (float)count;  A[3] =  Z;
	c[0] = X1; c[1] = Y1; c[2] = C1; c[3] = C2;
	linsolve4(A, c, h);

	/* rotation, scaling, and translation parameters */
	*a = h[0];
	*b = h[1];
    *tx = h[2];
	*ty = h[3];
}
void ImageUtil::sim_transform_landmark(const TPointF* landmark, TPointF* dst,
								   int count, float a, float b, float tx, float ty)
{
	int i;
	float x, y;

	// transform last shape to current shape
	for(i = 0; i < count; i++) {
		x = landmark[i].x;
		y = landmark[i].y;

		dst[i].x = a * x - b * y + tx;
		dst[i].y = a * y + b * x + ty;
	}
}

void ImageUtil::sim_transform_image(const unsigned char* gray, int width, int height,
								unsigned char* dst, int width1, int height1,
								float a, float b, float tx, float ty)
{
	int i, j;
	float x, y;
	int ix, iy;
	float u, v;

	for (i = 0; i < height1; i++) {
		for (j = 0; j < width1; j++) {
			x = a * j - b * i + tx;
			y = a * i + b * j + ty;

			ix = (int)x;
			iy = (int)y;

			u = x - ix;
			v = y - iy;

			if (ix >= 0 && ix <= width - 2 &&
				iy >= 0 && iy <= height - 2) {
				dst[i * width1 + j] = (unsigned char)((1.0f - u) * (1.0f - v) * gray[iy * width + ix] +
					u * (1.0f - v) * gray[iy * width + ix + 1] +
					(1.0f - u) * v * gray[(iy + 1) * width + ix] +
					u * v * gray[(iy + 1) * width + ix + 1] + 0.5f);
			} else {
				dst[i * width1 + j] = 0;
			}
		}
	}
}

void ImageUtil::down_scale_image(const unsigned char* gray, int width, int height, unsigned char* dst)
{
	int width1 = width >> 1;
	int height1 = height >> 1;

	int i, j;
	int k = 0;
	for( j = 0; j < height1; j++ )
	{
		for( i = 0; i < width1; i++ )
		{
			int idx = j * 2 * width + i * 2;
			dst[k] = ( gray[idx]  + gray[idx + 1] + gray[idx + width] + gray[idx + width + 1] + 2 ) >> 2;
			k++;
		}
	}
}
