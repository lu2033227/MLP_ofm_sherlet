
typedef struct tagPointF {
	float x;
	float y;
} TPointF;
class ImageUtil
{

public:
	static void BGR2Gray(const unsigned char* bgr, int width, int height, int pitch, unsigned char* gray);
	static void AlignGray(const unsigned char* gray1, int width, int height, int pitch, unsigned char* gray);
	static bool solve(float* A, int m, float* b);
	static void linsolve4(float* A, float* b, float* h);
	static void sim_params_from_points(const TPointF dstKeyPoints[],
										 const TPointF srcKeyPoints[], int count,
										 float* a, float* b, float* tx, float* ty);
	static void sim_transform_landmark(const TPointF* landmark, TPointF* dst,
									   int count, float a, float b, float tx, float ty);

	static void sim_transform_image(const unsigned char* gray, int width, int height,
									unsigned char* dst, int width1, int height1,
									float a, float b, float tx, float ty);

	static void down_scale_image(const unsigned char* gray, int width, int height, unsigned char* dst);
};
