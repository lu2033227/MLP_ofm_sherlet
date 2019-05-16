#include "imtransform.h"

/* solve linear equation system of 4th order, Ah=b, h=A^(-1)b */
void linsolve4(float* A, float* b, float* h)
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

// compute scale, rotation, and translation parameters
void sim_params_from_points(const TPointF dstKeyPoints[],  
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

void sim_transform_landmark(const TLandmark1* landmark, TPointF* dst, 
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

void sim_transform_image(const unsigned char* gray, int width, int height, int pitch,
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

			ix = MIN(width - 2, MAX(0, ix));
			iy = MIN(height - 2, MAX(0, iy));

			dst[i * width1 + j] = (unsigned char)((1.0f - u) * (1.0f - v) * gray[iy * pitch + ix] +
				u * (1.0f - v) * gray[iy * pitch + ix + 1] +
				(1.0f - u) * v * gray[(iy + 1) * pitch + ix] +
				u * v * gray[(iy + 1) * pitch + ix + 1] + 0.5f);
		}
	}
} 

cv::Mat sim_transform_image_3channels(cv::Mat img, int left_eye_x, 
					int left_eye_y, int right_eye_x, int right_eye_y)
{
	float a1,b1,tx1,ty1;
	TPointF dstPoints[2] = {
		{ 92, 96 },
		{ 164, 96 } };
	TPointF srcPoints[2] = {
			{left_eye_x, left_eye_y}, 
			{right_eye_x, right_eye_y}};
	sim_params_from_points(srcPoints,dstPoints, 2, &a1, &b1, &tx1, &ty1);

//	cout<<a1<<"  "<<b1<<" "<<tx1<<"  "<<ty1<<endl;

	int width = img.cols;
	int height = img.rows;
	uchar *channel0= new uchar[width*height];
	uchar *channel1= new uchar[width*height];
	uchar *channel2= new uchar[width*height];

	vector<cv::Mat> split_mat;
	cv::split(img,split_mat);

	Mat2uchar(split_mat[0],channel0,width,height);
	Mat2uchar(split_mat[1],channel1,width,height);
	Mat2uchar(split_mat[2],channel2,width,height);

	uchar *dst_channel0= new uchar[STD_WIDTH*STD_HEIGHT];
	uchar *dst_channel1= new uchar[STD_WIDTH*STD_HEIGHT];
	uchar *dst_channel2= new uchar[STD_WIDTH*STD_HEIGHT];


	sim_transform_image(channel0, width, height, width,
				dst_channel0, STD_WIDTH, STD_HEIGHT, a1, b1, tx1, ty1);
	sim_transform_image(channel1, width, height, width,
				dst_channel1, STD_WIDTH, STD_HEIGHT, a1, b1, tx1, ty1);
	sim_transform_image(channel2, width, height, width,
				dst_channel2, STD_WIDTH, STD_HEIGHT, a1, b1, tx1, ty1);

	cv::Mat img_transform0=uchar2Mat(dst_channel0,STD_WIDTH,STD_HEIGHT);
	cv::Mat img_transform1=uchar2Mat(dst_channel1,STD_WIDTH,STD_HEIGHT);
	cv::Mat img_transform2=uchar2Mat(dst_channel2,STD_WIDTH,STD_HEIGHT);

	vector<cv::Mat> channels;
    channels.push_back(img_transform0);
    channels.push_back(img_transform1);
    channels.push_back(img_transform2);

	cv::Mat img_transform;
	cv::merge(channels,img_transform);
	delete [] channel0;
	delete [] channel1;
	delete [] channel2;
	delete [] dst_channel0;
	delete [] dst_channel1;
	delete [] dst_channel2;

	//imwrite("xxx.jpg", img_transform);
	//cout<<"xxxxxxxxxxxxxxxxxxxxx"<<endl;
	return img_transform.clone();
}
cv::Mat sim_transform_image_1channel(cv::Mat img,int left_eye_x, 
				int left_eye_y, int right_eye_x, int right_eye_y)
{
	int width = img.cols / 4 * 4;
	int height = img.rows / 4 * 4;

	resize(img, img, cv::Size(width, height));
	imwrite("xxx.bmp", img);

	uchar *img_data= new uchar[width*height];
	
	Mat2uchar(img,img_data,width,height);

	uchar *dst_data= new uchar[STD_WIDTH*STD_HEIGHT];
	float a1,b1,tx1,ty1;
	TPointF dstPoints[2] = {
		{ 30, 30 },
		{ 90, 30 } };
	TPointF srcPoints[2] = {
			{left_eye_x, left_eye_y}, 
			{right_eye_x, right_eye_y}};
	sim_params_from_points(srcPoints, dstPoints, 2, &a1, &b1, &tx1, &ty1);

	ofstream out("result.dat", ios::binary);
	out.write((char*)img_data, STD_WIDTH*STD_HEIGHT);
	out.close();

	sim_transform_image(img_data, width, height, width,
				dst_data, STD_WIDTH, STD_HEIGHT, a1, b1, tx1, ty1);

	//for (int k = 0; k < STD_HEIGHT * STD_WIDTH; k++)
	//cout << int(dst_data[k]) << " ";

	cv::Mat img_transform=uchar2Mat(dst_data,STD_WIDTH,STD_HEIGHT);
	//cv::Mat img_transform = cv::Mat(STD_HEIGHT, STD_WIDTH, CV_8UC1, dst_data);
	//imwrite("xxxx.bmp", img_transform);
	
	delete [] img_data;
	delete [] dst_data;

	return img_transform.clone();
}

void Mat2uchar(cv::Mat img,uchar* dst,int width,int height)
{
	uchar* p;
	for (int i = 0; i < height; ++i) {
		p = img.ptr<uchar>(i);
		for (int j = 0; j < width; ++j) {
			dst[i * width + j] = p[j];
		}
	}
}
cv::Mat uchar2Mat(uchar *buffer,int width,int height) {
	cv::Mat img(height, width, CV_8UC1);
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
			img.at<uchar>(x, y) = buffer[x * width + y]; 
        }
    }
	return img;
}