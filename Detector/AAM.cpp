//#include "stdafx.h"
#include "AAM.h"

#ifndef MAX 
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define EPSILON 0.000001f
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float PYTHAG(float a, float b)
{
    float at = fabs(a), bt = fabs(b), ct, result;

    if (at > bt)       { ct = bt / at; result = at * sqrt(1.0F + ct * ct); }
    else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0F + ct * ct); }
    else result = 0.0;
    return(result);
}

int dsvd(float *a, int m, int n, float *w, float *v)
{
    int flag, i, its, j, jj, k, l, nm;
    float c, f, h, s, x, y, z;
    float anorm = 0.0, g = 0.0, scale = 0.0;
    float *rv1;
    
    if (m < n) 
    {        
        return(0);
    }
    rv1 = (float *)malloc((unsigned int) n*sizeof(float));
    for (i = 0; i < n; i++) 
    {
        
        l = i + 1;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        if (i < m) 
        {
            for (k = i; k < m; k++) 
                scale += fabs((float)a[k*m+i]);
            if (scale) 
            {
				float scale1 = 1.0F/scale;
                float *pa = a+i*m+i;
              
                for (k = i; k < m; k++,pa+=m) 
                {
                    float tapa = *pa*scale1;
                    s += tapa*tapa;
                    *pa = tapa;
                    
                }
                f = (float)a[i*m+i];
                g = -SIGN(sqrt(s), f);
                h = 1.0f/(f * g - s);
                a[i*m+i] = (float)(f - g);
                if (i != n - 1) 
                {
                    for (j = l; j < n; j++) 
                    {
                        float *api= a+i*m+i;
                        float *apj= api+j-i;
                        
                        for (s = 0.0, k = i; k < m; k++,api+=m,apj+=m) 
                            s += (*api * (*apj));
                        f = s * h;
                        
                        api= a+i*m+i;
                        apj= api+j-i;
                        for (k = i; k < m; k++,api+=m,apj+=m) 
                           *apj += f * (*api);
                    }
                }
                float *ap= a+i*m+i;
                for (k = i; k < m; k++,ap+=m) 
                {
                    *ap = *ap*scale;
                }
            }
        }
        w[i] = (float)(scale * g);
        
        
        g = s = scale = 0.0;
        if (i < m && i != n - 1) 
        {
            for (k = l; k < n; k++) 
                scale += fabs((float)a[i*m+k]);
            if (scale) 
            {
				float scale1 = 1.0F/scale;
                for (k = l; k < n; k++) 
                {
                    a[i*m+k] = (float)((float)a[i*m+k]*scale1);///scale);
                    s += ((float)a[i*m+k] * (float)a[i*m+k]);
                }
                f = (float)a[i*m+l];
                g = -SIGN(sqrt(s), f);
                h = f * g - s;
                a[i*m+l] = (float)(f - g);
				float h1 = 1.0F/h;
                float *ap = a+i*m+l;
                for (k = l; k < n; k++) 
                    rv1[k] = (*ap++)*h1;// / h;
                if (i != m - 1) 
                {
                    for (j = l; j < m; j++) 
                    {
                        float *api = a+i*m+l;
                        float *apj = a+j*m+l;
                        for (s = 0.0, k = l; k < n; k++) 
                            s += (*api++) * (*apj++);
                        apj = a+j*m+l;
                        for (k = l; k < n; k++) 
                            (*apj++) += (s * rv1[k]);
                    }
                }
                ap = a+i*m+l;
                for (k = l; k < n; k++,ap++) 
                    (*ap) = ((*ap)*scale);
            }
        }
        anorm = MAX(anorm, (fabs((float)w[i]) + fabs(rv1[i])));
    }
    
    for (i = n - 1; i >= 0; i--) 
    {
        if (i < n - 1) 
        {
            if (g) 
            {				
				float g1 = 1.0F/((float)a[i*m+l]*g);
                for (j = l; j < n; j++)
                    v[j*m+i] = (float)((float)a[i*m+j]*g1);
                
                for (j = l; j < n; j++) 
                {
                    for (s = 0.0, k = l; k < n; k++) 
                        s += ((float)a[i*m+k] * (float)v[k*m+j]);
                    for (k = l; k < n; k++) 
                        v[k*m+j] += (float)(s * (float)v[k*m+i]);
                }
            }
            for (j = l; j < n; j++) 
                v[i*m+j] = v[j*m+i] = 0.0;
        }
        v[i*m+i] = 1.0;
        g = rv1[i];
        l = i;
    }
    
    for (i = n - 1; i >= 0; i--) 
    {
        l = i + 1;
        g = (float)w[i];
        if (i < n - 1) 
            for (j = l; j < n; j++) 
                a[i*m+j] = 0.0;
        if (g) 
        {
            g = 1.0F / g;
            if (i != n - 1) 
            {
				float temp1 = g/(float)a[i*m+i];
                for (j = l; j < n; j++) 
                {
                    float *api = a+l*m+i;
                    float *apj = api+j-i;
                    for (s = 0.0, k = l; k < m; k++,api+=m,apj+=m) 
                        s += (*api) * (*apj);
                    f = s*temp1;//(s / (float)a[i][i]) * g;
                    
                    api = a+i*m+i;
                    apj = api+j-i;
                    for (k = i; k < m; k++,api+=m,apj+=m) 
                        *apj += f * (*api);
                }
            }
            float *api = a+i*m+i;
            for (j = i; j < m; j++,api+=m) 
               *api = *api*g;
            
        }
        else 
        {
            for (j = i; j < m; j++) 
                a[j*m+i] = 0.0;
        }
        ++a[i*m+i];
    }
    
    for (k = n - 1; k >= 0; k--) 
    {                             
        for (its = 0; its < 30; its++) 
        {                        
            flag = 1;
            for (l = k; l >= 0; l--) 
            {                     
                nm = l - 1;
                if (fabs(rv1[l]) + anorm == anorm) 
                {
                    flag = 0;
                    break;
                }
                if (fabs((float)w[nm]) + anorm == anorm) 
                    break;
            }
            if (flag) 
            {
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++) 
                {
                    f = s * rv1[i];
                    if (fabs(f) + anorm != anorm) 
                    {
                        g = (float)w[i];
                        h = PYTHAG(f, g);
                        w[i] = (float)h; 
                        h = 1.0F / h;
                        c = g * h;
                        s = (- f * h);
                        for (j = 0; j < m; j++) 
                        {
                            y = (float)a[j*m+nm];
                            z = (float)a[j*m+i];
                            a[j*m+nm] = (float)(y * c + z * s);
                            a[j*m+i] = (float)(z * c - y * s);
                        }
                    }
                }
            }
            z = (float)w[k];
            if (l == k) 
            {                  
                if (z < 0.0) 
                {              
                    w[k] = (float)(-z);
                    for (j = 0; j < n; j++) 
                        v[j*m+k] = (-v[j*m+k]);
                }
                break;
            }
            if (its >= 30) {
                free((void*) rv1);              
                return(0);
            }
            
            
            x = (float)w[l];
            nm = k - 1;
            y = (float)w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0F * h * y);
            g = PYTHAG(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
            
            
            c = s = 1.0;
            for (j = l; j <= nm; j++) 
            {
                i = j + 1;
                g = rv1[i];
                y = (float)w[i];
                h = s * g;
                g = c * g;
                z = PYTHAG(f, h);
                rv1[j] = z;
                
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y = y * c;
                float *pvi = v+i;
                float *pvj = v+j;
                for (jj = 0; jj < n; jj++,pvj+=m,pvi+=m) 
                {
                    x = *pvj;
                    z = *pvi;
                    *pvj = (float)(x * c + z * s);
                    *pvi = (float)(z * c - x * s);
                }
                z = PYTHAG(f, h);
                w[j] = (float)z;
                if (z) 
                {
                    z = 1.0F / z;
                    c = f * z;
                    s = h * z;
                }
                f = (c * g) + (s * y);
                x = (c * y) - (s * g);
                float *api = a+i;
                float *apj = a+j;
                for (jj = 0; jj < m; jj++,api+=m,apj+=m) 
                {
                    y = *apj;
                    z = *api;
                    *apj = (float)(y * c + z * s);
                    *api = (float)(z * c - y * s);
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = (float)x;
        }
    }
    free((void*) rv1);
    return(1);
}

int pinv(float *a, float *a_pinv, int m, int n)
{
	//����svd�ֽ� a= s*v*d'
	float *s = new float[m*n];
    memcpy(s,a,sizeof(float)*n*m);

    
	float *d = new float[n*n];

    
	float *v = new float[n];
    
	int rst = dsvd(s,m,n,v,d);
    
	//������� a_inv = d*inv(v)*s'
	for (int i=0; i<n; i++)
	{
		if(fabs(v[i])<EPSILON)
			v[i] = 0;
		else
			v[i] = 1.0f/v[i];
	}	
    
	for (int i=0; i<n; i++)	
		for(int j=0; j<n; j++)
			d[i*n+j] *= v[j];
	
    float *dp = d;
    float *a_pinvp = a_pinv;
    
    
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<m; j++,a_pinvp++)
		{
			*a_pinvp = 0;
            float *dpp = dp;
            float *sp = s+j*n;
			for(int k=0; k<n; k++)
				*(a_pinvp) += (*dpp++)*(*sp++);
		}
        dp += m;
	}
	delete []s;
	delete []d;
	delete []v;
	return rst;
}


int g_nSigmoidTimes = 2;            
int g_nGraySigmoidTimes = 1;        
int g_nDxDySigmoidTimes = 2;        
int g_nECornerSigmoidTimes = 2;     
int g_nWholeGMSigmoidTimes = 1;    

int g_nOnlyCalcQTimes = 11;                                        
int g_nInitScaleComp = 120;		
int g_nInitXOffset = 0;			
int g_nInitYOffset = -1;		 
int g_nEdgeCnrSmoothTimes = 0;  

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


void CalTheTan(unsigned char* grayImg,int width,int height,int *A,int *B,int *C,int windowssize, double dbSigma)
{   
	int i=0,j=0;
	
	int imgEndx = 0;
	int imgEndy = 0;
	int w=0,h=0;
	int dx=0,dy=0;

	imgEndx   = width-1;
	imgEndy   = height-1;
	w = imgEndx +1;
	h = imgEndy +1;

	int size = w*h;
	int size3 = size*3;

	int *A0 = new int[size3]; //store dx*dx
	int *B0 = A0+size; //store dy*dy
	int *C0 = B0+size; //store dx*dy

	memset(A0,0,sizeof(int)*size3);

	for(i=/*imgStarty+*/1; i<imgEndy; i++)     //��imgStarty+1��ʼ�㣬Ҳ����˵����1���ؿ�ȫ����
	{
		unsigned char* pointer1;
		unsigned char* pointer1_;
		unsigned char* pointer2;                      
		unsigned char* pointer3;                   
		int pointer4;
		int *Ap4,*Bp4,*Cp4;

		pointer1 = i*width+grayImg+1;                     // X  pointer3  X
		pointer1_= i*width+grayImg-1;
		pointer2 = pointer1+width;                      // X  pointer1  X
		pointer3 = pointer1-width;                      // X  pointer2  X
		pointer4 = (i/*-imgStarty*/)*w+1;
		Ap4 = A0+pointer4;
		Bp4 = B0+pointer4;
		Cp4 = C0+pointer4;
		for(j=/*imgStartx+*/1; j<imgEndx; j++)
		{
			dx = *(pointer1+j) - *(pointer1_+j);
			dy = *(pointer2+j) - *(pointer3+j);

		
			*(Ap4++) = dx*dx;
			*(Bp4++) = dy*dy;
			*(Cp4++) = dx*dy;
		}
	}
	if(windowssize==3 && dbSigma < 0.1)
	{
		for(i=1; i<=h-2; i++)
		{
			int pointer2 = i*w;
			int pointer1 = pointer2-w;
			int pointer3 = pointer2+w;

			for(j=1; j<=w-2; j++)
			{
				int pointer1_j = pointer1+j;
				int pointer2_j = pointer2+j;
				int pointer3_j = pointer3+j;

				int pointer1_jA = pointer1_j+1;
				int pointer2_jA = pointer2_j+1;
				int pointer3_jA = pointer3_j+1;

				int pointer1_jS = pointer1_j-1;
				int pointer2_jS = pointer2_j-1;
				int pointer3_jS = pointer3_j-1;
				A[pointer2_j] = A0[pointer1_jS]+ (A0[pointer1_j]<<1)+ A0[pointer1_jA]	+(A0[pointer2_jS]<<1)
					+ (A0[pointer2_j]<<2)+ (A0[pointer2_jA]<<1)+ A0[pointer3_jS]+ (A0[pointer3_j]<<1)	
					+ A0[pointer3_jA];			

				B[pointer2_j] = B0[pointer1_jS]+ (B0[pointer1_j]<<1)+ B0[pointer1_jA]	+ (B0[pointer2_jS]<<1)		
					+ (B0[pointer2_j]<<2)+ (B0[pointer2_jA]<<1)+ B0[pointer3_jS]+ (B0[pointer3_j]<<1)
					+ B0[pointer3_jA];	

				C[pointer2_j] = C0[pointer1_jS]+ (C0[pointer1_j]<<1)+ C0[pointer1_jA]	+ (C0[pointer2_jS]<<1)		
					+ (C0[pointer2_j]<<2)+ (C0[pointer2_jA]<<1)+ C0[pointer3_jS]+ (C0[pointer3_j]<<1)			
					+ C0[pointer3_jA];			
			}
		}

	} 
	delete []A0;

}

/********************************************************************************
������   : DetectCornerAndEdgeDouble
����     : �Ӹ�ͼƬ��ָ�������ҳ��ǵ�ͱ�Ե
*******************************************************************************/
bool GetCornerAndEdge(unsigned char* grayImg, int width, int height, float *RE, double dbSigma)
{   

	int i = 0;
	int *A=NULL,*B=NULL,*C=NULL;
	int w=width,h=height;

	if(w<4||h<4)
		return FALSE;

	int size = w*h;
	A = new int[size*3]; 
	B = A + size;
	C = B + size;

	CalTheTan(grayImg,width,height,A,B,C,3, dbSigma); 

	float tmp1=0.0,tmp2=0.0;

	for(i=0; i<h; i++)
	{
		int pointer1 = i*w;
		for(int j=pointer1; j<pointer1+w; j++)
		{    
			tmp1 = float( A[j]*B[j]-C[j]*C[j] ); //AB-C*C
			tmp2 = float( A[j]+B[j] ); 
			RE[j] = tmp2 * tmp2 - tmp1 - tmp1;
		}
	}

	delete []A;
	return TRUE;
}

CAAMRunPara::CAAMRunPara()
{
	tbUseICOVTracking          = true  ; 
	tbUseMouthInequRestrict    = true  ;
	tbUse3DConstraint          = true;                   
	tbUseFeatureTrack          = true  ;
	tbUseCompGlbAAMConstraint  = false ;
	tbUseBEBCompGlbAAMConstraint   = false;
	tbUseHueOnlineConstr       = false ;
	tbUse3DPoseConstr          = false ;
	tbUseProfileMotionConstr   = false ;
	tflOnlineK         = 10.0f     ;       
	tflMIRCoefK        = 0.010f    ;        
	tfl3DConstrK       = 000.03000f;      
	tflFeaTrackK       = 8.0f      ;          
	tflCompGlbAamK     = 0.0500f  ;       
	tflHueOnlineK      = 5.0f      ;           
	tfl3DPoseConstrK   = 001000.0f ;       
	tflProfileMotionK  = 10.0f     ;     
} 

CAAMRunPara::~CAAMRunPara()
{

}

void CAAM::GetRelativePos(float *S,TrianglesStruct &tri,AAMPoint &point,float &alpha,float &beta,float &gamma)
{
	x = point.x;
	y = point.y;

	x1 = S[tri.xi];
	x2 = S[tri.xj];
	x3 = S[tri.xk];

	y1 = S[tri.xi+1];
	y2 = S[tri.xj+1];
	y3 = S[tri.xk+1];

	float tdD = ((x1-x2)*(y3-y2)+(x3-x2)*(y2-y1));   
	if (fabs(tdD)<0.000001f)
	{
		tdD = tdD<0.0f? -0.000001f:0.000001f;
	}

	tdD = 1.0f/tdD;
	alpha = (x-x2)*(y3-y)+(x-x3)*(y-y2);			
	beta  = (x-x1)*(y-y3)+(x-x3)*(y1-y);          

	alpha *= tdD;
	beta  *= tdD;
	gamma = 1.0f - alpha - beta;
}


void CAAM::FreeModelData()
{   
	delete []aammodel.ShapeEigenvalue;
	delete []aammodel.mean_appearance;
	delete []aammodel.MeanShape;
	delete []aammodel.aamAppearance;
	int i;

	delete []aammodel.pseudoInv;      


	for (i=0; i<aammodel.ShapeComp_p_r;i++)
	{
		delete []aammodel.ShapeComp_p[i];
	}
	delete []aammodel.ShapeComp_p;

	for (i=0; i<aammodel.ShapeComp_q_r; i++)
	{
		delete []aammodel.ShapeComp_q[i];
	}
	delete []aammodel.ShapeComp_q;


	delete [] aammodel.triangle_vertex;
	delete [] aammodel.triangle_index;
	delete [] aammodel.triangles;
	delete [] aammodel.dpq0;


	delete []aammodel.mask;
	delete []aammodel.pAlpha;
	delete []aammodel.pBeta;
	delete []aammodel.pGamma;

	delete [] aammodel.appearancePCA[0];
	delete [] aammodel.appearancePCA;

	delete [] aammodel.tpppHessian[0][0];
	delete [] aammodel.tpppHessian[0];
	delete [] aammodel.tpppHessian;

	delete [] aammodel.tppHessianPo[0];
	delete [] aammodel.tppHessianPo;

	delete [] aammodel.tpInvTriangleArea;

	delete [] aammodel.JWp[0];
	delete [] aammodel.JWp;

	DeleteParameters();
}

bool CAAM::LoadModelData(void *p)
{
	if (p == NULL)
	{
		return false;
	}
	char* pModelData = (char*)p;

	memcpy(&aammodel, pModelData, sizeof(AAMModelStruct));
	pModelData += sizeof(AAMModelStruct);

	aammodel.ShapeEigenvalue = new float[aammodel.ShapeEigenvalue_num];
	memcpy(aammodel.ShapeEigenvalue, pModelData, sizeof(float) * aammodel.ShapeEigenvalue_num);
	pModelData += sizeof(float) * aammodel.ShapeEigenvalue_num;

	aammodel.mean_appearance = new float[aammodel.appearance_len];
	memcpy(aammodel.mean_appearance, pModelData, sizeof(float) * aammodel.appearance_len);
	pModelData += sizeof(float) * aammodel.appearance_len;

	aammodel.MeanShape = new float[aammodel.MeanShape_len];
	memcpy(aammodel.MeanShape, pModelData, sizeof(float) * aammodel.MeanShape_len);
	pModelData += sizeof(float) * aammodel.MeanShape_len;

	aammodel.aamAppearance = new AAMAppearance[aammodel.pixelnum];
	memcpy(aammodel.aamAppearance, pModelData, sizeof(AAMAppearance) * aammodel.pixelnum);
	pModelData += sizeof(AAMAppearance) * aammodel.pixelnum;

	int i, j;

	aammodel.pseudoInv = new float[aammodel.pseudoInv_r*aammodel.pseudoInv_c];
	memcpy(aammodel.pseudoInv, pModelData, aammodel.pseudoInv_r*sizeof(float) * aammodel.pseudoInv_c);
	pModelData+=aammodel.pseudoInv_r*sizeof(float) * aammodel.pseudoInv_c;

	aammodel.ShapeComp_p = new float*[aammodel.ShapeComp_p_r];
	for (i=0; i<aammodel.ShapeComp_p_r; i++)
	{
		aammodel.ShapeComp_p[i] = new float[aammodel.ShapeComp_p_c];
		memcpy(aammodel.ShapeComp_p[i], pModelData, sizeof(float) * aammodel.ShapeComp_p_c);
		pModelData += sizeof(float) * aammodel.ShapeComp_p_c;
	}

	aammodel.ShapeComp_q = new float*[aammodel.ShapeComp_q_r];
	for (i=0; i<aammodel.ShapeComp_q_r; i++)
	{   
		aammodel.ShapeComp_q[i] = new float[aammodel.ShapeComp_q_c];
		memcpy(aammodel.ShapeComp_q[i], pModelData, sizeof(float) * aammodel.ShapeComp_q_c);
		pModelData += sizeof(float) * aammodel.ShapeComp_q_c;
	}

	aammodel.triangle_vertex = new int[aammodel.vertexnum];
	memcpy(aammodel.triangle_vertex, pModelData, sizeof(int) * aammodel.vertexnum);
	pModelData += sizeof(int) * aammodel.vertexnum;

	aammodel.triangle_index  = new int[aammodel.vertexnum][MaxAdjacentTrianglesNum];
	memcpy(&(aammodel.triangle_index[0][0]), pModelData, sizeof(int) * aammodel.vertexnum * MaxAdjacentTrianglesNum);
	pModelData += sizeof(int) * aammodel.vertexnum * MaxAdjacentTrianglesNum;

	aammodel.triangles = new TrianglesStruct[aammodel.triangleNum];
	memcpy(aammodel.triangles, pModelData, sizeof(TrianglesStruct) * aammodel.triangleNum);
	pModelData += sizeof(TrianglesStruct) * aammodel.triangleNum;

	aammodel.dpq0 = new float[aammodel.pseudoInv_r];
	memcpy(aammodel.dpq0, pModelData, sizeof(float) * aammodel.pseudoInv_r);
	pModelData += sizeof(float) * aammodel.pseudoInv_r;

	aammodel.mask = new unsigned char[aammodel.sampleImgW*aammodel.sampleImgH];
	memcpy(aammodel.mask, pModelData, sizeof(char) * aammodel.sampleImgW*aammodel.sampleImgH);
	pModelData += sizeof(char) * aammodel.sampleImgW*aammodel.sampleImgH;

	aammodel.pAlpha = new int[aammodel.pixelnum];
	aammodel.pBeta = new int[aammodel.pixelnum];
	aammodel.pGamma = new int[aammodel.pixelnum];

	memcpy(aammodel.pAlpha, pModelData, sizeof(int) * aammodel.pixelnum);
	pModelData += sizeof(int) * aammodel.pixelnum;

	memcpy(aammodel.pBeta, pModelData, sizeof(int) * aammodel.pixelnum);
	pModelData += sizeof(int) * aammodel.pixelnum;

	memcpy(aammodel.pGamma, pModelData, sizeof(int) * aammodel.pixelnum);
	pModelData += sizeof(int) * aammodel.pixelnum;


	//Dr.Lei modified
	aammodel.appearancePCA      = new float *[aammodel.appearanceAxiNum];
	aammodel.appearancePCA[0]   = new float[aammodel.appearanceAxiNum * aammodel.appearance_len];
	for ( i = 1; i < aammodel.appearanceAxiNum; i++ )
		aammodel.appearancePCA[i]   = aammodel.appearancePCA[i - 1] + aammodel.appearance_len;

	short int *temp = new short int[aammodel.appearanceAxiNum * aammodel.appearance_len];
	memcpy((char*)temp, pModelData, sizeof(short int) * aammodel.appearanceAxiNum * aammodel.appearance_len);
	pModelData += sizeof(short int) * aammodel.appearanceAxiNum * aammodel.appearance_len; //1678336

	for(i=0; i<aammodel.appearanceAxiNum * aammodel.appearance_len; i++)
	{
		aammodel.appearancePCA[0][i] = 1.0*temp[i]/300000;
	}
	delete temp;


	/*aammodel.appearancePCA      = new float *[aammodel.appearanceAxiNum];
	aammodel.appearancePCA[0]   = new float[aammodel.appearanceAxiNum * aammodel.appearance_len];
	for ( i = 1; i < aammodel.appearanceAxiNum; i++ )
		aammodel.appearancePCA[i]   = aammodel.appearancePCA[i - 1] + aammodel.appearance_len;
	memcpy(aammodel.appearancePCA[0], pModelData, sizeof(float) * aammodel.appearanceAxiNum * aammodel.appearance_len);
	pModelData += sizeof(float) * aammodel.appearanceAxiNum * aammodel.appearance_len;*/

	int iTotalShapeNum = aammodel.shapeAxiNum + 4;


	aammodel.tpppHessian  = new float**[aammodel.m_iHessianNum];;     
	aammodel.tpppHessian[0] = new float*[aammodel.m_iHessianNum * iTotalShapeNum];
	for(i=1; i<aammodel.m_iHessianNum; i++)
	{
		aammodel.tpppHessian[i] = aammodel.tpppHessian[i-1] + iTotalShapeNum;
	}
	aammodel.tpppHessian[0][0] = new float[aammodel.m_iHessianNum * iTotalShapeNum * iTotalShapeNum];
	for(i=0; i<aammodel.m_iHessianNum; i++)
	{
		for(j=0; j<iTotalShapeNum; j++)
		{
			aammodel.tpppHessian[i][j] = aammodel.tpppHessian[0][0] + \
				i * iTotalShapeNum * iTotalShapeNum + j * iTotalShapeNum;
		}
	}
	memcpy(aammodel.tpppHessian[0][0], pModelData, sizeof(float) * aammodel.m_iHessianNum * \
		iTotalShapeNum * iTotalShapeNum);
	pModelData += sizeof(float) * aammodel.m_iHessianNum * iTotalShapeNum * iTotalShapeNum;

	aammodel.tppHessianPo = new float*[iTotalShapeNum];
	aammodel.tppHessianPo[0] = new float[iTotalShapeNum * iTotalShapeNum];
	for(i=1; i<iTotalShapeNum; i++)
	{
		aammodel.tppHessianPo[i] = aammodel.tppHessianPo[i-1] + iTotalShapeNum;
	}
	memcpy(aammodel.tppHessianPo[0], pModelData, sizeof(float) * iTotalShapeNum * iTotalShapeNum);
	pModelData += sizeof(float) * iTotalShapeNum * iTotalShapeNum;


	aammodel.tpInvTriangleArea = new float[aammodel.triangleNum];
	memcpy(aammodel.tpInvTriangleArea, pModelData, sizeof(float) * aammodel.triangleNum);
	pModelData += sizeof(float) * aammodel.triangleNum;

	int     JWp_r    = (aammodel.shapeAxiNum + 4) * 2;    
	int     JWp_c    = aammodel.pixelnum;
	aammodel.JWp     = new float*[JWp_r];              
	aammodel.JWp[0]  = new float[JWp_r * JWp_c];        
	for ( i = 1; i < JWp_r; i++ )                      
	{                                                 
		aammodel.JWp[i] = aammodel.JWp[i - 1] + JWp_c;  
	} 
	memcpy(aammodel.JWp[0], pModelData, sizeof(float) * JWp_r * JWp_c);
	pModelData += sizeof(float) * JWp_r * JWp_c;

	InitParameters();
	return true;
}
CMATRIX cvPartPseudoInv;
CMATRIX cvSDTMulErr;
CMATRIX tpMatSDTMulErr2DTmp2;

void CAAM::RunAAM(unsigned char* inputGrayImg, RECT *pFaceRect, float* pflShape, EyePosition *pEyePos, 
						  AAMResult* pAAMRlt, bool bTracking,
						  InitParaComp* initParaComp, unsigned char* inputHueImg)
{      

	if (inputGrayImg == NULL)
	{
		pAAMRlt->tbAlignSuccessful = false; 
	}

	RECT    rcROI, rcFaceRect;

	// set initial parameters and some flags
	if ( pFaceRect || pflShape || pEyePos )     // thoroughly lost, need face detection
	{
		bTracking = false;

		if( pflShape )
		{
			CalcuParaByShape( pflShape, pAAMRlt->tpfParameters );
			UpdateFaceRect( pflShape, rcFaceRect );
		}
		else
			InitWithFaceRectOrEyePos(pFaceRect, pEyePos, initParaComp, rcFaceRect);

		m_MotionHistory.Reset();

		if ( m_AAMRunPara.tbUseICOVTracking )
		{
			m_iTemplAccCnt      = 0;
			tbHasOnlineTempl   = false;
		}

		// 3D model parameters
		if ( m_AAMRunPara.tbUse3DConstraint )
		{
			memset( tpVarsOf2D3DAAM->m_RotAngleXYZ, 0, sizeof(tpVarsOf2D3DAAM->m_RotAngleXYZ) );
			memset( tpVarsOf2D3DAAM->m_TranslXYZ, 0, sizeof(tpVarsOf2D3DAAM->m_TranslXYZ) );
			memset( tpVarsOf2D3DAAM->tpSUCoef, 0, sizeof(float) * tpCandideMdl->m_iSUNum );
			memset( tpVarsOf2D3DAAM->tpAUCoef, 0, sizeof(float) * tpCandideMdl->m_iAUNum );
		}
	}
	else    // tracking
	{
		bTracking = true;

		memcpy(coef_p_new, &(pAAMRlt->tpfParameters[4]), sizeof(float)*aammodel.shapeAxiNum);
		coef_q_new[0] = pAAMRlt->tpfParameters[0];
		coef_q_new[1] = pAAMRlt->tpfParameters[1];
		coef_q_new[2] = pAAMRlt->tpfParameters[2];
		coef_q_new[3] = pAAMRlt->tpfParameters[3];
		rcFaceRect = pAAMRlt->trcFaceRect;
	}

	// if face rect outside the input image, return false
	if ( rcFaceRect.left > InputImgW || rcFaceRect.right < 0 || 
		rcFaceRect.top > InputImgH  || rcFaceRect.bottom < 0 )
	{
		pAAMRlt->tbAlignSuccessful = false; 
		return ;
	}

	if ( tbHasOnlineTempl == false )
	{
		tpMatHessian2DPre.CopyFromData1D(aammodel.tppHessianPo[0]);
	}

	GetROIGrayImage(inputGrayImg, tpbyGrayImg, rcFaceRect,  rcROI);   

	coef_q_new[0] -= rcROI.left;    
	coef_q_new[1] -= rcROI.top;

	int i, j;
	if ( m_AAMRunPara.tbUseFeatureTrack )
	{
		for ( j = 0; j < NO_TRACKFEATURE && bTracking; j++ )
		{
			if ( tpCurrFeatures[j].w <= 0 )
				continue;
			tpCurrFeatures[j].x -= rcROI.left;
			tpCurrFeatures[j].y -= rcROI.top;
		}
	}


	for (j=0; j<ImgWidth * ImgHeight; j++)
	{
		tpbyGrayImg[j] = tpbyLUT[ tpbyGrayImg[j] ];
	}

	GetCornerAndEdge(tpbyGrayImg, ImgWidth, ImgHeight, tpflEdgeData, tdbSigma);

	for (j=0; j<g_nEdgeCnrSmoothTimes; j++)
	{
		GaussSmooth(tpflEdgeData, ImgWidth, ImgHeight, NULL, NULL);
	}

	memset(shape_bak,0,sizeof(float)*aammodel.vertexnum*2);

	m_iCurShapeNum = 5; 

	int     loopCount = 0;
	for(; ; )
	{
		loopCount ++;
		SynthesizeShape_qp( coef_q_new, coef_p_new, shapeCrd );

		RECT    rcBoundBox;
		CalcuBoundRect(shapeCrd, aammodel.vertexnum, rcBoundBox);
		rcBoundBox.left     += rcROI.left;
		rcBoundBox.right    += rcROI.left;
		rcBoundBox.top      += rcROI.top;
		rcBoundBox.bottom   += rcROI.top;

		if ( rcBoundBox.left >= InputImgW || rcBoundBox.right <= 0 || rcBoundBox.top >= InputImgH || rcBoundBox.bottom <= 0 || 
			(rcBoundBox.right - rcBoundBox.left) > InputImgW || (rcBoundBox.bottom - rcBoundBox.top) > InputImgH)
		{
			pAAMRlt->tbAlignSuccessful = false;
			m_MotionHistory.Reset();
			return;
		}

		for(i=0; i < aammodel.vertexnum * 2; i++)
		{
			shapeCrdInt[i] = int( shapeCrd[i]*8 + 0.5);  
		}

		if ( m_AAMRunPara.tbUse3DConstraint )
		{
			if( !bTracking && loopCount == 1 )  
			{
				tpVarsOf2D3DAAM->m_TranslXYZ[2] = tpVarsOf2D3DAAM->tfFocalLen * (tpCandideMdl->tpMSVertex[21].x - tpCandideMdl->tpMSVertex[54].x) / (shapeCrd[10*2 + 0] - shapeCrd[2*2 + 0]);

				tpVarsOf2D3DAAM->m_TranslXYZ[0] =  ( 0.5f * (shapeCrd[38*2 + 0] + shapeCrd[45*2 + 0] - InputImgW + 1) + rcROI.left ) * tpVarsOf2D3DAAM->m_TranslXYZ[2] / tpVarsOf2D3DAAM->tfFocalLen;
				tpVarsOf2D3DAAM->m_TranslXYZ[1] = -( 0.5f * (shapeCrd[38*2 + 1] + shapeCrd[45*2 + 1] - InputImgH + 1) + rcROI.top ) * tpVarsOf2D3DAAM->m_TranslXYZ[2] / tpVarsOf2D3DAAM->tfFocalLen;
			}
		}


		if ( loopCount == 1 )  
		{

			if( m_AAMRunPara.tbUse3DConstraint )     
			{
				Calcu3DPara_By_2DShape( bTracking || pflShape, rcROI, false );
			}
		}

		int nstate = WarpImage();  

		if ( nstate != -1 )
			nstate = EvaluateShape(loopCount, pAAMRlt->tdbScore, tbHasOnlineTempl);

		if (nstate == 1) 
		{                         
			pAAMRlt->tdbReconErrFinal  = pAAMRlt->tdbScore;

			pAAMRlt->tnIteCount=loopCount;
			pAAMRlt->tpfParameters[0] = coef_q_new[0];
			pAAMRlt->tpfParameters[1] = coef_q_new[1];
			pAAMRlt->tpfParameters[2] = coef_q_new[2];
			pAAMRlt->tpfParameters[3] = coef_q_new[3];

			memcpy(&(pAAMRlt->tpfParameters[4]), coef_p_new, sizeof(float)*aammodel.shapeAxiNum);
			pAAMRlt->tpfParameters[0] += rcROI.left; 
			pAAMRlt->tpfParameters[1] += rcROI.top;
			pAAMRlt->tnOffsetX = rcROI.left;
			pAAMRlt->tnOffsetY = rcROI.top;

			for (i=0; i<aammodel.MeanShape_len; i+=2)
			{
				shapeCrd[i]     += rcROI.left;
				shapeCrd[i + 1] += rcROI.top;
			}

			UpdateFaceRect(shapeCrd, pAAMRlt->trcFaceRect);    

			pAAMRlt->tbAlignSuccessful = true;
			memcpy(pAAMRlt->tpfShape, shapeCrd, 2*aammodel.vertexnum*sizeof(float));

			SynthesizeShape_p( coef_p_new, pAAMRlt->tpfShapeNoRST );

			if ( m_AAMRunPara.tbUse3DConstraint )
			{
				Calc3DModelProj2DImgCrd( pAAMRlt->tpf3DProj2DShape );
				memcpy( pAAMRlt->m_TranslXYZ, tpVarsOf2D3DAAM->m_TranslXYZ, sizeof(tpVarsOf2D3DAAM->m_TranslXYZ) );
				RotRad2Degree( tpVarsOf2D3DAAM->m_RotAngleXYZ, pAAMRlt->m_RotAngleXYZ);

				memcpy( pAAMRlt->tpSUCoef, tpVarsOf2D3DAAM->tpSUCoef, sizeof(float) * tpCandideMdl->m_iSUNum );
			}

			if ( m_AAMRunPara.tbUseICOVTracking )
			{
				if (m_iTemplAccCnt < m_iTemplAccCntThr)
					CalcuTemplateOnline();
			}

			m_MotionHistory.Update(pAAMRlt->tpfParameters);

			return ;
		}
		else if (nstate == -1) 
		{

			pAAMRlt->tbAlignSuccessful = false;

			m_MotionHistory.Reset();

			return ;

		} 

		int iTotalShapeNum = aammodel.shapeAxiNum + 4;
		tpMatHessian2DFnl = tpMatHessian2DPre;

		tbHasCalcuDerivSd_dqp  = false;                   
		if (!bTracking && loopCount < g_nOnlyCalcQTimes) 
			m_iCurShapeNum = 5; 
		else                                                    
		{
			m_iCurShapeNum = iTotalShapeNum;

		}
		float*  pfRhoSDTEnorm = new float[m_iCurShapeNum];
		tpMatSDTMulErr2DFnl.SetZero();

		for(j = 0; j < aammodel.appearance_len; j++)
		{
			tpMatAppearanceCrd.p[j*tpMatAppearanceCrd.width+0] = aammodel.mean_appearance[j] - appearanceCrd[j];
		}

		cvPartPseudoInv.Resize(m_iCurShapeNum, aammodel.pseudoInv_c);
		cvPartPseudoInv.CopyFromData1D(aammodel.pseudoInv);

		cvSDTMulErr.SUPERMUL(cvPartPseudoInv, tpMatAppearanceCrd);
		tpMatSDTMulErr2DFnl.CopyFromMatrix(cvSDTMulErr);

		for( i = 0; i < m_iCurShapeNum; i++)
		{
			pfRhoSDTEnorm[i] = tpMatSDTMulErr2DFnl.p[i*tpMatSDTMulErr2DFnl.width+0];
		}	


		if ( m_AAMRunPara.tbUseICOVTracking )
		{
			if ( tbHasOnlineTempl )
			{
				for(j = 0; j < m_iTemplOnlineLen; j++)
				{
					tpMatApprnOnline.p[j*tpMatTemplOnline.width+0] = tpMatTemplOnline.p[j*tpMatTemplOnline.width+0] - appearanceCrd[j];
				}

				tpMatSDTMulErr2DTmp.SUPERMUL(tpMatSDOnLine, tpMatApprnOnline);
				tpMatSDTMulErr2DTmp.SUPERMULF(m_AAMRunPara.tflOnlineK);
				tpMatSDTMulErr2DFnl.SUPERADD(tpMatSDTMulErr2DTmp);

			}
		}
		//----------------------- use feature matching restriction for fitting
		if ( m_AAMRunPara.tbUseFeatureTrack )    
		{
			if( bTracking )
			{
				AddConstraint_FeaTrack();
			}
		}

		//----------------------- use mouth position inequation restriction for fitting
		float flLipDist;
		if ( m_AAMRunPara.tbUseMouthInequRestrict )
		{
			if ( m_iCurShapeNum == iTotalShapeNum )
			{
				AddConstraint_MouthInequ( bTracking, flLipDist );
			}
		}

		CMATRIX matHessian3D;
		CMATRIX matSDTMulErr3D;

		if( m_AAMRunPara.tbUse3DConstraint )
		{
			if (m_iCurShapeNum == iTotalShapeNum)
			{                                                      
				AddConstraint_3DModel( matHessian3D, matSDTMulErr3D, bTracking, rcROI );
			}
		}

		memset(dpq, 0, sizeof(float) * iTotalShapeNum);
		int m, n;
		int iHIndex = (m_iCurShapeNum == 2) ? 0 : m_iCurShapeNum - 3;
		for(m=0; m<m_iCurShapeNum; m++)
		{
			for(n=0; n<m_iCurShapeNum; n++)
			{
				dpq[m] -= float(aammodel.tpppHessian[iHIndex][m][n] * pfRhoSDTEnorm[n]);    
			}
		}


		////------------------ some constraints ---------------------//
		if ( !m_AAMRunPara.tbUse3DConstraint )  
		{
			bool bCalcuWithContraint = false;

			if ( m_AAMRunPara.tbUseMouthInequRestrict )
			{
				if ( m_iCurShapeNum == iTotalShapeNum && flLipDist < 0 ) bCalcuWithContraint = true;
			}

			if ( m_AAMRunPara.tbUseICOVTracking || m_AAMRunPara.tbUseFeatureTrack || m_AAMRunPara.tbUseCompGlbAAMConstraint)
			{
				if ( bTracking ) bCalcuWithContraint = true;
			}

			if ( bCalcuWithContraint )
			{

				CMATRIX pMatInvHessianFnl(aammodel.pseudoInv_r, aammodel.pseudoInv_r);
				pMatInvHessianFnl = !tpMatHessian2DFnl;
				tpMatSDTMulErr2DFnl = pMatInvHessianFnl*tpMatSDTMulErr2DFnl;

				float *tmpp = tpMatSDTMulErr2DFnl.p;
				for(m=0; m<iTotalShapeNum; m++)
				{
					dpq[m] = -(*tmpp);
					tmpp+=tpMatSDTMulErr2DFnl.width;
				}
			} 
		}
		else   
		{

			if (m_iCurShapeNum == iTotalShapeNum)
			{                
				pinv(matHessian3D.p,matHessian3D.p,matHessian3D.height,matHessian3D.width);
				matSDTMulErr3D = matHessian3D*matSDTMulErr3D;

				for (m = 0; m < m_iCurShapeNum; m++)
				{
					dpq[m] = -matSDTMulErr3D.p[m*matSDTMulErr3D.width+0];
				}

				for (m = 0; m < 3; m++)
				{
					tpVarsOf2D3DAAM->m_RotAngleXYZ[m] -= matSDTMulErr3D.p[(m_iCurShapeNum + m)*matSDTMulErr3D.width+0];    
					tpVarsOf2D3DAAM->m_TranslXYZ[m]   -= matSDTMulErr3D.p[(m_iCurShapeNum + 3 + m)*matSDTMulErr3D.width+0];
				}

				float   fl3DParaThr = 1.f;
				int iParaStart = m_iCurShapeNum + 6;
				if (!bTracking)
				{
					for (m = 0; m < tpCandideMdl->m_iSUNum; m++)
					{
						tpVarsOf2D3DAAM->tpSUCoef[m] -= matSDTMulErr3D.p[(iParaStart + m)*matSDTMulErr3D.width+0];//float(cvmGet(&matSDTMulErr3D, iParaStart + m, 0));

						if( tpVarsOf2D3DAAM->tpSUCoef[m] < -fl3DParaThr ) tpVarsOf2D3DAAM->tpSUCoef[m] = -fl3DParaThr;
						else if (  tpVarsOf2D3DAAM->tpSUCoef[m] > fl3DParaThr )   tpVarsOf2D3DAAM->tpSUCoef[m] = fl3DParaThr;
					}
				}

				iParaStart = ( bTracking ? 6 + m_iCurShapeNum : 6 + m_iCurShapeNum + tpCandideMdl->m_iSUNum );
			}

		}

		delete [] pfRhoSDTEnorm;


		//step(9)
		for ( i=0; i<aammodel.shapeAxiNum+4; i++)
		{  
			dpq[i] = -dpq[i];
		} 

		//calculate the destination
		SynthesizeShape_qp( dpq, &dpq[4], SpdS0 );

		//2.calculate the destination
		//shapeCrd		
		//3. (x0i,y0i)+delta(x0i,y0i) --> (xi,yi)+delta(xi,yi)
		{
			memset(SpdS,0,sizeof(float)*aammodel.MeanShape_len);
			for (i=0; i<aammodel.vertexnum; i++)
			{    
				int triangle_num = aammodel.triangle_vertex[i];
				int i2 = i+i;
				for (j=0; j<triangle_num ; j++)
				{   
					float _alpha = 0.0f;
					float _belta = 0.0f;
					float _gamma = 0.0f;

					AAMPoint point={ SpdS0[i2],SpdS0[i2+1]};

					int tri_index = aammodel.triangle_index[i][j];
					GetRelativePos(aammodel.MeanShape,aammodel.triangles[tri_index],point,_alpha,_belta,_gamma);

					int xi = aammodel.triangles[tri_index].xi;
					int xj = aammodel.triangles[tri_index].xj;
					int xk = aammodel.triangles[tri_index].xk;

					SpdS[i2  ] += _alpha*shapeCrd[xi  ] + _belta*shapeCrd[xj  ] + _gamma*shapeCrd[xk  ];
					SpdS[i2+1] += _alpha*shapeCrd[xi+1] + _belta*shapeCrd[xj+1] + _gamma*shapeCrd[xk+1];
				}
				SpdS[i2  ] = SpdS[i2  ]/triangle_num;
				SpdS[i2+1] = SpdS[i2+1]/triangle_num;
			}
		}

		{  
			float q0 = 0.0f;
			float q1 = 0.0f;
			float q2 = 0.0f;
			float q3 = 0.0f;

			for (j=0; j<aammodel.vertexnum*2; j++)
			{   
				SpdS[j] -= aammodel.MeanShape[j];

				q0 += aammodel.ShapeComp_q[0][j]*SpdS[j];
				q1 += aammodel.ShapeComp_q[1][j]*SpdS[j];
				q2 += aammodel.ShapeComp_q[2][j]*SpdS[j];
				q3 += aammodel.ShapeComp_q[3][j]*SpdS[j];
			}

			{
				coef_q_new[0] = q0*aammodel.ShapeComp_q[0][aammodel.ShapeComp_q_c-1];
				coef_q_new[1] = q1*aammodel.ShapeComp_q[1][aammodel.ShapeComp_q_c-1];			
				coef_q_new[2] = q2*aammodel.ShapeComp_q[2][aammodel.ShapeComp_q_c-1];
				coef_q_new[3] = q3*aammodel.ShapeComp_q[3][aammodel.ShapeComp_q_c-1];
			}

		}

		for (j=0; j<aammodel.vertexnum*2; j++)
		{
			SpdS[j] -= (coef_q_new[0]*aammodel.ShapeComp_q[0][j] + coef_q_new[1]*aammodel.ShapeComp_q[1][j] 
			+  coef_q_new[2]*aammodel.ShapeComp_q[2][j] + coef_q_new[3]*aammodel.ShapeComp_q[3][j]);
		}

		float _s = 1.0f/((1+coef_q_new[2])*(1+coef_q_new[2])+coef_q_new[3]*coef_q_new[3]);
		float _a = (1+coef_q_new[2])*_s;
		float _b =  coef_q_new[3]*_s;

		for ( j=0; j<aammodel.vertexnum; j++)
		{   
			int j2 = j+j;
			float x =  _a*SpdS[j2]+_b*SpdS[j2+1];
			float y = -_b*SpdS[j2]+_a*SpdS[j2+1];

			SpdS[j2  ] = x;
			SpdS[j2+1] = y;
		}

		for ( i=0; i<aammodel.shapeAxiNum; i++)
		{
			float p = 0.0f;

			for ( j=0; j<2*aammodel.vertexnum; j++)
			{  
				p += aammodel.ShapeComp_p[i][j]*SpdS[j];
			}

			coef_p_new[i] = p*aammodel.ShapeComp_p[i][aammodel.ShapeComp_p_c-1];
		}

		int nOutRangCoefNum = 0;
		for ( i=0; i<aammodel.shapeAxiNum; i++)
		{  
			float threshold = 3.0f*aammodel.ShapeEigenvalue[i];

			if (coef_p_new[i]<-threshold)
			{
				coef_p_new[i] = -threshold;
				nOutRangCoefNum++;
			}
			else if (coef_p_new[i]>threshold)
			{
				coef_p_new[i] = threshold;
				nOutRangCoefNum++;
			}
		}

		if ( nOutRangCoefNum == aammodel.shapeAxiNum )
		{
			pAAMRlt->tbAlignSuccessful = false;
			m_MotionHistory.Reset();
			return ;
		}

	}

}
int CAAM::WarpImage()
{
	//some temporary pointers
	BYTE*   grayImg     = tpbyGrayImg;
	int     *gradXdata  = tpnGradData;
	int     *gradYdata  = tpnGradData + SampleImgW * SampleImgH;
	float   *edgedata   = tpflEdgeData;

	int     i, j;

	int     index_x , index_y;
	int     iOffset;
	BYTE    byGrayVal;


	std::vector<int> vecOutsideIdx;
	vecOutsideIdx.clear();
	vecOutsideIdx = m_vecInLipIdx;
	memcpy(tpbyMaskCalcAvg, tpbyMaskSubBorder, aammodel.pixelnum * sizeof(BYTE) );    //reset tpbyMaskCalcAvg
	memset(tpIplValidPixesMask,0,sizeof(BYTE)*SampleImgH*SampleImgW);

	int nValidPixCnt = 0;

	for( i=0; i < aammodel.pixelnum ; i++)
	{   
		AAMAppearance &t = aammodel.aamAppearance[i];

		appearanceCrdInt[i+i  ] = 
			(  pAlpha[i]*shapeCrdInt[t.vi  ] 
		+ pBeta[i]*shapeCrdInt[t.vj  ] 
		+ pGamma[i]*shapeCrdInt[t.vk  ] )>>13;   
		appearanceCrdInt[i+i+1] = 
			( pAlpha[i]*shapeCrdInt[t.vi+1] 
		+ pBeta[i]*shapeCrdInt[t.vj+1] 
		+ pGamma[i]*shapeCrdInt[t.vk+1] )>>13;
	}


	for( i = 0; i < aammodel.pixelnum ; i++)
	{
		index_x = appearanceCrdInt[i+i  ];
		index_y = appearanceCrdInt[i+i+1];


		if (index_x <0 || index_x >= ImgWidth || index_y <0 || index_y >= ImgHeight)  
		{  
			continue;
		}
		else
		{                                 
			iOffset = index_x + index_y * ImgWidth;
			byGrayVal = grayImg[iOffset];

			nValidPixCnt++;

			{

				*(tpIplValidPixesMask + aammodel.aamAppearance[i].imgy * SampleImgW + 
					aammodel.aamAppearance[i].imgx) = 1;  
				//gray
				appearanceCrd[tnGrayIndex + i] = byGrayVal;         
				//gray for calcu dxdy
				tpbyWarpGrayImg[aammodel.aamAppearance[i].imgy * SampleImgW + 
					aammodel.aamAppearance[i].imgx] = byGrayVal;   
				appearanceCrd[tnEdgeCnrIndex + i] = edgedata[iOffset]; //edge
			}
		}                                                  
	}


	for( i = 0; i < aammodel.pixelnum ; i++)
	{
		if ( *(tpIplValidPixesMask + aammodel.aamAppearance[i].imgy *SampleImgW + 
			aammodel.aamAppearance[i].imgx) == 0 )
		{
			tpbyMaskCalcAvg[i] = 0;
			vecOutsideIdx.push_back(i);
		}
	}

	CalcuGradient(tpbyWarpGrayImg, SampleImgW, SampleImgH, gradXdata, gradYdata);
	CalcuBorderGradient(tpbyWarpGrayImg, SampleImgW, SampleImgH, aammodel.mask, 
		tpbyMask_Border, gradXdata, gradYdata);

	for(j=0; j<SampleImgW * SampleImgH; j++)
	{
		gradXdata[j] *= gradXdata[j];      
		gradYdata[j] *= gradYdata[j];      
	}

	float   *pDx = appearanceCrd + tnDxDyIndex; 
	float   *pDy = appearanceCrd + tnDxDyIndex + aammodel.pixelnum ;  
	for(j=0; j<aammodel.pixelnum; j++)
	{		
		int t = aammodel.aamAppearance[j].imgy * SampleImgW + aammodel.aamAppearance[j].imgx;
		pDx[j]  = float( gradXdata[t] );     
		pDy[j]  = float( gradYdata[t] );    
	}


	for (i=0; i<g_nDxDySigmoidTimes; i++)  
	{
		SigmoidalPositive(pDx, aammodel.pixelnum, 1, tpbyMaskCalcAvg);      
		SigmoidalPositive(pDy, aammodel.pixelnum, 1, tpbyMaskCalcAvg);    
	}


	for(j=0; j<g_nGraySigmoidTimes; j++)  //g_nSigmoidTimes
		SigmoidalPositive(appearanceCrd + tnGrayIndex, aammodel.pixelnum, 1, tpbyMaskCalcAvg);


	for(i=0; i<g_nECornerSigmoidTimes; i++)  // g_nSigmoidTimes
		SigmoidalPositive(appearanceCrd + tnEdgeCnrIndex, aammodel.pixelnum, 1, tpbyMaskCalcAvg); 

	for ( i = 0; i < int( vecOutsideIdx.size() ); i++ )
	{
		int num =vecOutsideIdx[i];
		for (j=0; j<aammodel.featureNum; j++)
		{
			appearanceCrd[num] = aammodel.mean_appearance[num];
			num+=aammodel.pixelnum;
		}
	}

	int nstate = 0;
	if ( int( vecOutsideIdx.size() ) > int( aammodel.pixelnum * 0.7 ) )
		nstate = -1;

	return nstate;
}

void  CAAM::InitParameters()
{  
	tnRunMode  = rmONLINE_TRACK;  

	tpContext  = NULL;

	SampleImgW  = aammodel.sampleImgW;
	SampleImgH  = aammodel.sampleImgH;

	tnGrayIndex      = 0;    
	tnDxDyIndex      = 0;    
	tnEdgeCnrIndex   = 0;    
	tnWholeGMagIndex = 0;  
	tnWholeDstMIndex = 0;   

	tnDxDyIndex      += 1;
	tnEdgeCnrIndex   += 1;
	tnWholeGMagIndex += 1;
	tnWholeDstMIndex += 1;

	tnEdgeCnrIndex   += 2;
	tnWholeGMagIndex += 2;
	tnWholeDstMIndex += 2;

	tnWholeGMagIndex += 1;
	tnWholeDstMIndex += 1;

	tnGrayIndex      *= aammodel.pixelnum;
	tnDxDyIndex      *= aammodel.pixelnum;
	tnEdgeCnrIndex   *= aammodel.pixelnum;
	tnWholeGMagIndex *= aammodel.pixelnum;
	tnWholeDstMIndex *= aammodel.pixelnum;
	// ----------- precalculate the index for each feature ------------------//

	pAlpha = aammodel.pAlpha;
	pBeta  = aammodel.pBeta;
	pGamma = aammodel.pGamma;

	shape_bak = new float[aammodel.vertexnum*2];

	appearanceCrd = new float[aammodel.appearance_len];
	memset(appearanceCrd,0,aammodel.appearance_len*sizeof(float));	
	appearanceCrdInt = new int[aammodel.pixelnum * 2];

	shapeCrd      = new float[aammodel.vertexnum * 2];  //N(W(s0;p),q) 
	shapeCrdInt   = new int[aammodel.vertexnum * 2];

	SpdS0         = new float[aammodel.MeanShape_len]; //S0+dS0
	SpdS          = new float[aammodel.MeanShape_len];   //S+dS
	dpq           = new float[aammodel.shapeAxiNum + 4];
	SDErr         = new float[aammodel.shapeAxiNum + 4];

	coef_p_new = new float[aammodel.shapeAxiNum];

	float fMinX = 1.0e+10, fMaxX = -1.0e+10, fMinY = 1.0e+10, fMaxY = -1.0e+10;
	int	i;
	for(i = 0; i < aammodel.MeanShape_len; i += 2)
	{
		if ( fMinX > aammodel.MeanShape[i] ) fMinX = aammodel.MeanShape[i];
		if ( fMaxX < aammodel.MeanShape[i] ) fMaxX = aammodel.MeanShape[i];
		if ( fMinY > aammodel.MeanShape[i+1] ) fMinY = aammodel.MeanShape[i+1];
		if ( fMaxY < aammodel.MeanShape[i+1] ) fMaxY = aammodel.MeanShape[i+1];
	}
	trcfBaseShapeExtent.fX	    = fMinX;
	trcfBaseShapeExtent.fY	    = fMinY;
	trcfBaseShapeExtent.fWidth	= fMaxX - fMinX;
	trcfBaseShapeExtent.fHeight= fMaxY - fMinY;

	m_vecInLipIdx.clear();
	if ( aammodel.vertexnum >= 87 )
	{
		memcpy( shape_bak, aammodel.MeanShape, sizeof(float) * aammodel.vertexnum*2 );
		for( i = 60; i <= 67; i++ )
		{
			shape_bak[i*2 + 0] -= fMinX;
			shape_bak[i*2 + 1] -= fMinY;
		}
		fMinX = 1.0e+10, fMaxX = -1.0e+10, fMinY = 1.0e+10, fMaxY = -1.0e+10;
		for( i = 60 * 2; i <= 67 * 2; i += 2 )
		{
			if ( fMinX > shape_bak[i] ) fMinX = shape_bak[i];
			if ( fMaxX < shape_bak[i] ) fMaxX = shape_bak[i];
			if ( fMinY > shape_bak[i+1] ) fMinY = shape_bak[i+1];
			if ( fMaxY < shape_bak[i+1] ) fMaxY = shape_bak[i+1];
		}
		shape_bak[68*2 + 0] = shape_bak[60*2 + 0];  
		shape_bak[68*2 + 1] = shape_bak[60*2 + 1];

		int x, y, j;
		float flY;
		for( i = 0; i < aammodel.pixelnum ; i++ )
		{
			x = aammodel.aamAppearance[i].imgx;
			y = aammodel.aamAppearance[i].imgy;

			if ( y < fMinY || y > fMaxY || x < fMinX || x > fMaxX )
				continue;

			bool bIsInLipIdx = false;
			for ( j = 60; j < 64; j++ )
			{
				if ( x >= shape_bak[j*2] && x <= shape_bak[(j+1)*2] )
				{
					flY = shape_bak[j*2 + 1] + ( shape_bak[(j+1)*2 + 1] - shape_bak[j*2 + 1] ) * ( x - shape_bak[j*2] ) / ( shape_bak[(j+1)*2] - shape_bak[j*2] );
					if ( flY <= y )
						bIsInLipIdx = true;
					break;
				}
			}
			for ( j = 64; bIsInLipIdx && j < 68; j++ )
			{
				if ( x <= shape_bak[j*2] && x >= shape_bak[(j+1)*2] )
				{
					flY = shape_bak[j*2 + 1] + ( shape_bak[(j+1)*2 + 1] - shape_bak[j*2 + 1] ) * ( x - shape_bak[j*2] ) / ( shape_bak[(j+1)*2] - shape_bak[j*2] );
					if ( flY >= y )
						m_vecInLipIdx.push_back( i );
					break;
				}
			}
		}
	}

	tpbyGrayImg    = NULL;
	//warped gray image
	tpbyWarpGrayImg   = new unsigned char [SampleImgW * SampleImgH];    
	memset(tpbyWarpGrayImg, 0 , SampleImgW * SampleImgH * sizeof(BYTE));
	tpnGradData = new int [SampleImgW * SampleImgH * 2];
	memset(tpnGradData, 0, SampleImgW * SampleImgH * 2 * sizeof(int));
	tpflEdgeData = NULL; 
	//create look up table begin: ///////////////
	tpbyLUT = new BYTE[256];
	tpbyLUT[0] = 0;

	double fScale = 255 / log(255.0);
	for (i=1; i<=255; i++)
		tpbyLUT[i] = BYTE( log(double(i)) * fScale );

	tpbyMaskSubBorder  = new BYTE[aammodel.pixelnum];
	tpbyMaskCalcAvg    = new BYTE[aammodel.pixelnum];
	memset(tpbyMaskSubBorder, 255, aammodel.pixelnum * sizeof(BYTE) );
	memset(tpbyMaskCalcAvg, 255, aammodel.pixelnum * sizeof(BYTE) );

	tpbyMask_Border = new BYTE[SampleImgW * SampleImgH];         
	memset(tpbyMask_Border, 0, SampleImgW * SampleImgH * sizeof(BYTE));
	ErodeMask(aammodel.mask, SampleImgW, SampleImgH, tpbyMask_Border);           
	MaskSub(aammodel.mask, SampleImgW, SampleImgH, tpbyMask_Border, tpbyMask_Border);  

	int m = 0;
	for (i=0; i<SampleImgW * SampleImgH; i++)
	{
		if (aammodel.mask[i])
		{
			if (tpbyMask_Border[i])
			{
				tpbyMaskSubBorder[m]   = 0;
				tpbyMaskCalcAvg[m]     = 0;
			}
			m++;
		}
	}
	//create mask of calculating average for sigmoid  function end! ///
	tpMatAppearanceCrd.Resize(aammodel.pseudoInv_c,1);
	int iTotalShapeNum = aammodel.shapeAxiNum + 4;
	// some common vairable for following constraint
	tpMatHessian2DPre.Resize(iTotalShapeNum, iTotalShapeNum);
	tpMatHessian2DFnl.Resize(iTotalShapeNum, iTotalShapeNum);
	tpMatHessian2DTmp.Resize(iTotalShapeNum, iTotalShapeNum);
	tpMatSDTMulErr2DFnl.Resize(iTotalShapeNum,1);
	tpMatSDTMulErr2DTmp.Resize(iTotalShapeNum,i);
	tpMatHessian2DPre.CopyFromData1D(aammodel.tppHessianPo[0]);
	tbHasCalcuDerivSd_dqp  = false;
	// end

	m_iTemplOnlineLen  = aammodel.pixelnum;
	tpMatSDOnLine.Resize(aammodel.pseudoInv_r, m_iTemplOnlineLen);
	tpMatTemplOnline.Resize(m_iTemplOnlineLen,1);
	tpMatApprnOnline.Resize(m_iTemplOnlineLen,1);

	tbHasOnlineTempl    = false;
	m_iTemplAccCnt       = 0;
	m_iTemplAccCntThr    = 1;

	m_iHueOnlineLen         = aammodel.pixelnum;     // aammodel.pixelnum
	tpMatHueApprnOnline.Resize(m_iHueOnlineLen,1);

	tpbyHueImgOnline       = NULL;
	m_iHueTemplAccCnt       = 0;
	m_iHueTemplAccCntThr    = 5;
	tpbyHueLUT             = new BYTE[ HUE_LUT_SIZE ];
	tbHueLUTOK             = false;

	tpIplValidPixesMask = new unsigned char[SampleImgH*SampleImgW];

	tpMatFstDerB_p.Resize(1,aammodel.shapeAxiNum);	
	tpMatSndDerB_p.Resize(aammodel.shapeAxiNum, aammodel.shapeAxiNum);
	tpMatDerP_dqp.Resize(aammodel.shapeAxiNum, aammodel.shapeAxiNum+4);
	tpMatDerP_Sd.Resize(aammodel.shapeAxiNum, aammodel.vertexnum * 2);

	tppflDerSd_dqp         = new float*[aammodel.vertexnum * 2];
	tppflDerSd_dqp[0]      = new float[ aammodel.vertexnum * 2 * ( aammodel.shapeAxiNum + 4 ) ];
	for ( i = 1; i < aammodel.vertexnum * 2; i++ ) tppflDerSd_dqp[i] = tppflDerSd_dqp[i-1] + aammodel.shapeAxiNum + 4;

	tflFeaTrackR       = 1000;
	tpMatFeaTrackSD.Resize(NO_TRACKFEATURE * 2, iTotalShapeNum);
	tpMatFeaTrackSD_T.Resize(iTotalShapeNum, NO_TRACKFEATURE * 2);
	tpMatFeaTrackErr.Resize(NO_TRACKFEATURE * 2, 1);

	m_MotionHistory.Initialize(2, 0.9f);

	const int nUsedAppearanceAxiNum = 20;
	matAppearancePCA.Resize(nUsedAppearanceAxiNum, aammodel.appearance_len);
	matAppearancePCA.CopyFromData1D(aammodel.appearancePCA[0]);
}

void  CAAM::DeleteParameters()
{   

	delete [] shape_bak;            shape_bak       = NULL;    
	delete [] appearanceCrd;	    appearanceCrd   = NULL;
	delete [] appearanceCrdInt;	    appearanceCrdInt= NULL;

	delete [] shapeCrd;	    	    shapeCrd		= NULL;
	delete [] shapeCrdInt;	    	shapeCrdInt		= NULL;

	delete [] SpdS0;				SpdS0			= NULL;
	delete [] SpdS;				    SpdS			= NULL;
	delete [] dpq;				    dpq				= NULL;
	delete [] SDErr;				SDErr			= NULL;

	delete [] coef_p_new;		    coef_p_new		= NULL;

	SAFE_ARRAY_DELETE( tpbyGrayImg );
	delete [] tpbyWarpGrayImg;
	delete [] tpnGradData;
	SAFE_ARRAY_DELETE( tpflEdgeData );

	delete [] tpbyLUT;
	delete [] tpbyMaskSubBorder;
	delete [] tpbyMaskCalcAvg;
	delete [] tpbyMask_Border;

	if ( tpbyHueImgOnline ) delete [] tpbyHueImgOnline;
	delete [] tpbyHueLUT;

	delete []tpIplValidPixesMask;

	delete [] tppflDerSd_dqp[0];
	delete [] tppflDerSd_dqp;

}

void  CAAM::SetParameters(int width,int height, int nMaxIteCnt, double dbScoreThr)
{

	tnMaxIteCnt    = nMaxIteCnt;
	tdbScoreThr	= dbScoreThr;
	tdbSigma       = 0;

	if ( (InputImgW != width || InputImgH != height) && tpbyGrayImg )
	{
		SAFE_ARRAY_DELETE( tpbyGrayImg         );
		SAFE_ARRAY_DELETE( tpflEdgeData        );
		SAFE_ARRAY_DELETE( tpbyHueImgOnline    );
	}
	if ( !tpbyGrayImg && !tpflEdgeData && !tpbyHueImgOnline )
	{
		InputImgW   = width;
		InputImgH   = height;

		tpbyGrayImg   = new(std::nothrow) BYTE[ InputImgW * InputImgH ];
		memset( tpbyGrayImg, 0, sizeof(BYTE) * InputImgW * InputImgH );

		tpflEdgeData   = new(std::nothrow) float[ InputImgW * InputImgH ];
		memset( tpflEdgeData, 0, sizeof(float) * InputImgW * InputImgH );

		tpbyHueImgOnline  = new BYTE[ InputImgW * InputImgH ];
		memset( tpbyHueImgOnline, 0, sizeof(BYTE) * InputImgW * InputImgH );
	}
}

int CAAM::EvaluateShape(const int nCount, double& dbScore, const bool tbHasOnlineTempl)
{ 

	float dst = 0.0f;
	int i;

	for(i=0; i<aammodel.vertexnum*2; i++)
	{  
		dst += fabs(shapeCrd[i]-shape_bak[i]);
		shape_bak[i] = shapeCrd[i];
	}

	if (nCount < 2)	
	{
		return 0; //go on iterating
	}

	if (tnRunMode == rmOFFLINE_TEST)
	{
		if(nCount <= tnMaxIteCnt)  
			return 0;   
		else
			return 1;   
	}

	else if (tnRunMode == rmONLINE_TRACK)
	{
		if(dst > 20.0f && nCount < tnMaxIteCnt)  
			return 0; //go on iterating
		else
		{
			if (nCount < tnMaxIteCnt && m_iCurShapeNum < (aammodel.shapeAxiNum + 4)) 
			{
				return 0;
			}

			dbScore = CalcuReconstructErr();
			// 18.5
			if(dbScore > tdbRecErrThr)	// use Reconstruct Error to judge
			{
				return -1;
			}
			else
			{
				return 1;
			}

		}
	}
	return 1;   
}

double CAAM::CalcuReconstructErr()
{
	int     i, j;
	for(j = 0; j < aammodel.appearance_len; j++)
	{
		tpMatAppearanceCrd.p[j*tpMatAppearanceCrd.width+0] = appearanceCrd[j] - aammodel.mean_appearance[j];
	}

	pmatLambda.SUPERMUL(matAppearancePCA,tpMatAppearanceCrd);
	pmatAppearanceTmp.SUPER_X(matAppearancePCA,pmatLambda);
	pmatAppearanceTmp.SUPERSUB_(tpMatAppearanceCrd);
	for ( j = 0; j < aammodel.pixelnum; j++ )
	{
		if (  tpbyMaskSubBorder[j] == 0 )
		{
			for ( i = 0; i < aammodel.featureNum; i++ )
			{
				int nOffset = i * aammodel.pixelnum + j;
				pmatAppearanceTmp.p[nOffset*pmatAppearanceTmp.width+0] = 0;
			}
		}
	}

	pmatRst.SUPER_X(pmatAppearanceTmp, pmatAppearanceTmp);
	double  dbSqErr = pmatRst.p[0+0];
	double  dbErr   =sqrt( dbSqErr );

	return dbErr;
}

void CAAM::GetROIGrayImage(BYTE* pInputGrayImg, BYTE* pbyROIGrayImg, RECT &faceRect, RECT &rcROI)
{
	int faceRectW = faceRect.right - faceRect.left;
	int faceRectH = faceRect.bottom - faceRect.top;

	float fExtRate = 0.5f * 1.5f;
	rcROI.left      =  (faceRect.right + faceRect.left) / 2 - int(fExtRate * faceRectW + 0.5f);   
	rcROI.right     =  (faceRect.right + faceRect.left) / 2 + int(fExtRate * faceRectW + 0.5f);   
	rcROI.top       =  (faceRect.top + faceRect.bottom) / 2 - int(fExtRate * faceRectH + 0.5f);   
	rcROI.bottom    =  (faceRect.top + faceRect.bottom) / 2 + int(fExtRate * faceRectH + 0.5f);

	rcROI.left  = max(0, rcROI.left);
	rcROI.right = min(InputImgW, rcROI.right);
	rcROI.top   = max(0, rcROI.top);
	rcROI.bottom= min(InputImgH, rcROI.bottom);

	faceRect.left  -= rcROI.left;
	faceRect.right -= rcROI.left;
	faceRect.top   -= rcROI.top;
	faceRect.bottom-= rcROI.top;

	ImgWidth    = rcROI.right - rcROI.left;
	ImgHeight   = rcROI.bottom - rcROI.top;

	int iFirstPixPos = rcROI.top * InputImgW + rcROI.left;
	int i;
	for (i=0; i<ImgHeight; i++)
	{
		memcpy( pbyROIGrayImg + i * ImgWidth, pInputGrayImg + iFirstPixPos + i * InputImgW, 
			ImgWidth * sizeof(BYTE) );
	}

}

bool CAAM::UpdateFaceRect(float* shape, RECT& faceRect)
{

	CalcuBoundRect(shape, aammodel.vertexnum, faceRect);

	RECT&   rcROI = faceRect;

	if ( rcROI.left >= InputImgW || rcROI.right <= 0 || 
		rcROI.top >= InputImgH  || rcROI.bottom <= 0 )
	{
		return false;
	}

	rcROI.left  = max(0, rcROI.left);
	rcROI.right = min(InputImgW, rcROI.right);
	rcROI.top   = max(0, rcROI.top);
	rcROI.bottom= min(InputImgH, rcROI.bottom);

	return true;
}

void CAAM::InitWithFaceRectOrEyePos(RECT *pFaceRect, EyePosition *pEyePos, InitParaComp* initParaComp, RECT& rcFaceRect)
{
	float   fDx = trcfBaseShapeExtent.fX + trcfBaseShapeExtent.fWidth / 2;
	float   fDy = trcfBaseShapeExtent.fY + trcfBaseShapeExtent.fHeight / 2;  
	float   fScaleComp = ( (initParaComp) ? initParaComp->fScaleComp : 0.01f * g_nInitScaleComp ); 

	if (pEyePos)    
	{
		float  fDenominator;
		float x1 = m_eyePosition.fLeft_eye_x,  y1 = m_eyePosition.fLeft_eye_y; 
		float x2 = m_eyePosition.fRight_eye_x, y2 = m_eyePosition.fRight_eye_y; 

		float x1p = pEyePos->fLeft_eye_x,  y1p = pEyePos->fLeft_eye_y; 
		float x2p = pEyePos->fRight_eye_x, y2p = pEyePos->fRight_eye_y; 

		fDenominator = (x1 - x2) * (x1 - x2)  + (y1 - y2) * (y1 - y2);
		float a = ( (x1 - x2) * (x1p - x2p) + (y1 - y2) * (y1p - y2p) ) / fDenominator;
		float b = ( (x1 - x2) * (y1p - y2p) - (y1 - y2) * (x1p - x2p) ) / fDenominator;

		float tx = x1p - ( a * x1 - b * y1 );
		float ty = y1p - ( b * x1 + a * y1 );
		float scale = float( sqrt( a * a + b * b ) );


		coef_q_new[ 0 ] = tx + scale * ( (initParaComp) ? initParaComp->fOffsetXComp :g_nInitYOffset );
		coef_q_new[ 1 ] = ty + scale * ( (initParaComp) ? initParaComp->fOffsetYComp :g_nInitYOffset );
		coef_q_new[ 2 ] = fScaleComp * a - 1.0f;
		coef_q_new[ 3 ] = fScaleComp * b;

		float Xc = tx + fDx * scale;
		float Yc = ty + fDy * scale;

		rcFaceRect.left     = int( Xc - scale * trcfBaseShapeExtent.fWidth / 2 + 0.5f );
		rcFaceRect.right    = int( Xc + scale * trcfBaseShapeExtent.fWidth / 2 + 0.5f );
		rcFaceRect.top      = int( Yc - scale * trcfBaseShapeExtent.fHeight / 2 + 0.5f );
		rcFaceRect.bottom   = int( Yc + scale * trcfBaseShapeExtent.fHeight / 2 + 0.5f );
	}
	else            
	{
		float fScale = (pFaceRect->bottom - pFaceRect->top) / trcfBaseShapeExtent.fHeight;

		coef_q_new[ 2 ] = fScaleComp * fScale - 1.0f;
		coef_q_new[ 3 ] = 0.0;

		coef_q_new[ 0 ] = (pFaceRect->left + pFaceRect->right)/2.0f - fScale * fDx 
			+ fScale * ( (initParaComp) ? initParaComp->fOffsetXComp : g_nInitYOffset );
		coef_q_new[ 1 ] = (pFaceRect->bottom + pFaceRect->top) /2.0f  - fScale * fDy 
			+ fScale * ( (initParaComp) ? initParaComp->fOffsetYComp : g_nInitYOffset );

		rcFaceRect = *pFaceRect;

	}

	memset(coef_p_new,0,sizeof(float)*aammodel.shapeAxiNum);
	coef_p_new[0] = (initParaComp) ? initParaComp->fInitP1 : 0.0f;

}

// memory is allocated outside
void CAAM::CalcuParaByShape( float* pflShape, float* pflParameters )
{

	int i,j;
	memcpy( SpdS, pflShape, sizeof(float) * aammodel.vertexnum*2 );

	float q0 = 0.0f;
	float q1 = 0.0f;
	float q2 = 0.0f;
	float q3 = 0.0f;


	for (j=0; j<aammodel.vertexnum*2; j++)
	{   
		SpdS[j] -= aammodel.MeanShape[j];

		q0 += aammodel.ShapeComp_q[0][j]*SpdS[j];
		q1 += aammodel.ShapeComp_q[1][j]*SpdS[j];
		q2 += aammodel.ShapeComp_q[2][j]*SpdS[j];
		q3 += aammodel.ShapeComp_q[3][j]*SpdS[j];

	}

	{
		coef_q_new[0] = q0*aammodel.ShapeComp_q[0][aammodel.ShapeComp_q_c-1];
		coef_q_new[1] = q1*aammodel.ShapeComp_q[1][aammodel.ShapeComp_q_c-1];			
		coef_q_new[2] = q2*aammodel.ShapeComp_q[2][aammodel.ShapeComp_q_c-1];
		coef_q_new[3] = q3*aammodel.ShapeComp_q[3][aammodel.ShapeComp_q_c-1];
	}

	for (j=0; j<aammodel.vertexnum*2; j++)
	{
		SpdS[j] -= (coef_q_new[0]*aammodel.ShapeComp_q[0][j] + coef_q_new[1]*aammodel.ShapeComp_q[1][j] 
		+  coef_q_new[2]*aammodel.ShapeComp_q[2][j] + coef_q_new[3]*aammodel.ShapeComp_q[3][j]);
	}


	float _s = 1.0f/((1+coef_q_new[2])*(1+coef_q_new[2])+coef_q_new[3]*coef_q_new[3]);
	float _a = (1+coef_q_new[2])*_s;
	float _b =  coef_q_new[3]*_s;

	for ( j=0; j<aammodel.vertexnum; j++)
	{   
		int j2 = j+j;
		float x =  _a*SpdS[j2]+_b*SpdS[j2+1];
		float y = -_b*SpdS[j2]+_a*SpdS[j2+1];

		SpdS[j2  ] = x;
		SpdS[j2+1] = y;
	}

	for ( i=0; i<aammodel.shapeAxiNum; i++)
	{
		float p = 0.0f;

		for ( j=0; j<2*aammodel.vertexnum; j++)
		{  
			p += aammodel.ShapeComp_p[i][j]*SpdS[j];
		}

		coef_p_new[i] = p*aammodel.ShapeComp_p[i][aammodel.ShapeComp_p_c-1];
	}

	memcpy( &(pflParameters[0]), coef_q_new, sizeof(float)*4 );
	memcpy( &(pflParameters[4]), coef_p_new, sizeof(float)*aammodel.shapeAxiNum );
}


void CAAM::CalcuTemplateOnline()
{
	tpMatApprnOnline.CopyFromData1D(appearanceCrd);

	if ( m_iTemplAccCnt == 0 )
	{
		tpMatTemplOnline.SetZero();
	}

	tpMatTemplOnline = tpMatApprnOnline + tpMatTemplOnline;
	m_iTemplAccCnt++;

	if ( m_iTemplAccCnt < m_iTemplAccCntThr )
		return;

	tpMatTemplOnline = tpMatTemplOnline*(1.0f/m_iTemplAccCntThr);


	float   *pflWarpImg;
	float   *pflWarpImgDx, *pflWarpImgDy;

	pflWarpImg  = new float[SampleImgW * SampleImgH * 3];

	pflWarpImgDx = pflWarpImg + SampleImgW * SampleImgH;
	pflWarpImgDy = pflWarpImgDx + SampleImgW * SampleImgH;

	tpMatSDOnLine.SetZero();

	int i, j, k;
	for ( k = 0; k < m_iTemplOnlineLen / aammodel.pixelnum; k++ ) 
	{
		memset( pflWarpImg, 0, sizeof(float) * SampleImgW * SampleImgH * 3 );

		for( j = 0; j < aammodel.pixelnum; j++ )
		{		
			int t = aammodel.aamAppearance[j].imgy * SampleImgW + aammodel.aamAppearance[j].imgx;
			pflWarpImg[t] = tpMatTemplOnline.p[(k * aammodel.pixelnum + j)*tpMatTemplOnline.width+0];
		}

		GaussSmooth(pflWarpImg, SampleImgW, SampleImgH, aammodel.mask, tpbyMask_Border);

		CalcuGradient(pflWarpImg, SampleImgW, SampleImgH, aammodel.mask, pflWarpImgDx, pflWarpImgDy);
		CalcuBorderGradient(pflWarpImg, SampleImgW, SampleImgH, aammodel.mask, 
			tpbyMask_Border, pflWarpImgDx, pflWarpImgDy);


		for ( j = 0; j < aammodel.pseudoInv_r; j++ )
		{
			float*  pCurJWxp = aammodel.JWp[2*j    ];
			float*  pCurJWyp = aammodel.JWp[2*j + 1];
			for( i = 0; i < aammodel.pixelnum ; i++ )
			{
				int index = aammodel.aamAppearance[i].imgy*SampleImgW + aammodel.aamAppearance[i].imgx; 

				float flSD = pCurJWxp[i]*pflWarpImgDx[index] + pCurJWyp[i]*pflWarpImgDy[index];
				tpMatSDOnLine.p[j*tpMatSDOnLine.width+k * aammodel.pixelnum + i] = flSD*4;
			}
		}
	}

	CMATRIX  pMatSD(m_iTemplOnlineLen, aammodel.pseudoInv_r);
	pMatSD = ~tpMatSDOnLine;

	tpMatHessian2DTmp = tpMatSDOnLine*pMatSD;
	tpMatHessian2DTmp = tpMatHessian2DTmp*m_AAMRunPara.tflOnlineK;
	tpMatHessian2DPre = tpMatHessian2DPre + tpMatHessian2DTmp;

	delete [] pflWarpImg;

	tbHasOnlineTempl   = true;
}


void CAAM::CalcRotationTrigVal()
{
	float e_x      = tpVarsOf2D3DAAM->m_RotAngleXYZ[0];
	float e_y      = tpVarsOf2D3DAAM->m_RotAngleXYZ[1];
	float e_z      = tpVarsOf2D3DAAM->m_RotAngleXYZ[2];

	tpVarsOf2D3DAAM->m_Trig.Sx   = sin( e_x );
	tpVarsOf2D3DAAM->m_Trig.Cx   = cos( e_x );
	tpVarsOf2D3DAAM->m_Trig.Sy   = sin( e_y );
	tpVarsOf2D3DAAM->m_Trig.Cy   = cos( e_y );
	tpVarsOf2D3DAAM->m_Trig.Sz   = sin( e_z );
	tpVarsOf2D3DAAM->m_Trig.Cz   = cos( e_z );
}


void CAAM::SynPersKeyPoint3D(float*  pKeyPoint3D)
{
	SynPersKeyPoint3D( pKeyPoint3D, tpVarsOf2D3DAAM->tpSUCoef, tpVarsOf2D3DAAM->tpAUCoef );
}

void CAAM::SynPersKeyPoint3D( float*  pKeyPoint3D, float* pSUCoef, float* pAUCoef )
{
	int i, j, k;

	float*  pData = (float*)(tpCandideMdl->tpMSVertex);
	memcpy( pKeyPoint3D, pData, sizeof(float) * tpCandideMdl->m_iMSVertexNum * 3 );  

	int iVertexNumStart = 0;
	for ( i = 0; i < tpCandideMdl->m_iSUNum; i++ )          
	{
		int iVtxNum = tpCandideMdl->tpiEach_SUVertexNum[i];

		for ( j = 0; j < iVtxNum; j++ )
		{
			int i3DPtsIdx = tpCandideMdl->tpiEach_SUVertexIdx[iVertexNumStart + j];
			pData = (float*)( tpCandideMdl->tpEach_SUVertexVal + (iVertexNumStart + j) );

			for ( k = 0; k < 3; k++ )
			{                                                  
				pKeyPoint3D[ i3DPtsIdx * 3 + k ] += pData[k] * pSUCoef[i];
			}
		}
		iVertexNumStart += iVtxNum;
	}

	iVertexNumStart = 0;
	for ( i = 0; i < tpCandideMdl->m_iAUNum; i++ )          
	{
		int iVtxNum = tpCandideMdl->tpiEach_AUVertexNum[i];

		for ( j = 0; j < iVtxNum; j++ )
		{
			int i3DPtsIdx = tpCandideMdl->tpiEach_AUVertexIdx[iVertexNumStart + j];
			pData = (float*)( tpCandideMdl->tpEach_AUVertexVal + (iVertexNumStart + j) );

			for ( k = 0; k < 3; k++ )
			{                                                  
				pKeyPoint3D[ i3DPtsIdx * 3 + k ] += pData[k] * pAUCoef[i];
			}
		}
		iVertexNumStart += iVtxNum;
	}
}

void CAAM::GetRotMatrix(float pRotMat[], int nRotType)
{
	switch( nRotType) 
	{
	case rtNormRotMatrix:     // Transform Rotation
		{
			//[                       cos(y)*cos(z),                      -cos(y)*sin(z),          sin(y)]
			//[  sin(x)*sin(y)*cos(z)+cos(x)*sin(z), -sin(x)*sin(y)*sin(z)+cos(x)*cos(z),  -sin(x)*cos(y)]
			//[ -cos(x)*sin(y)*cos(z)+sin(x)*sin(z),  cos(x)*sin(y)*sin(z)+sin(x)*cos(z),   cos(x)*cos(y)]

			pRotMat[0] =  tpVarsOf2D3DAAM->m_Trig.Cy * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[1] = -tpVarsOf2D3DAAM->m_Trig.Cy * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[2] =  tpVarsOf2D3DAAM->m_Trig.Sy;

			float Temp = tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[3] =  Temp * tpVarsOf2D3DAAM->m_Trig.Cz     + tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[4] = -Temp * tpVarsOf2D3DAAM->m_Trig.Sz     + tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[5] = -tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cy;

			Temp     = tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[6] = -Temp * tpVarsOf2D3DAAM->m_Trig.Cz     + tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[7] =  Temp * tpVarsOf2D3DAAM->m_Trig.Sz     + tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[8] =  tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cy;
		}
		break;
	case rtFstDeriv_x: // 
		{
			//[                                   0,                                   0,              0]
			//[  cos(x)*sin(y)*cos(z)-sin(x)*sin(z), -cos(x)*sin(y)*sin(z)-sin(x)*cos(z), -cos(x)*cos(y)]
			//[  sin(x)*sin(y)*cos(z)+cos(x)*sin(z), -sin(x)*sin(y)*sin(z)+cos(x)*cos(z), -sin(x)*cos(y)]

			pRotMat[0] =  0;
			pRotMat[1] =  0;
			pRotMat[2] =  0;

			float Temp = tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[3] =  Temp * tpVarsOf2D3DAAM->m_Trig.Cz     - tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[4] = -Temp * tpVarsOf2D3DAAM->m_Trig.Sz     - tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[5] = -tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cy;

			Temp     = tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[6] =  Temp * tpVarsOf2D3DAAM->m_Trig.Cz     + tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[7] = -Temp * tpVarsOf2D3DAAM->m_Trig.Sz     + tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[8] = -tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cy;
		}
		break;
	case rtFstDeriv_y:     // 
		{
			//[        -sin(y)*cos(z),         sin(y)*sin(z),                cos(y)]
			//[  sin(x)*cos(y)*cos(z), -sin(x)*cos(y)*sin(z),         sin(x)*sin(y)]
			//[ -cos(x)*cos(y)*cos(z),  cos(x)*cos(y)*sin(z),        -cos(x)*sin(y)]

			pRotMat[0] = -tpVarsOf2D3DAAM->m_Trig.Sy * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[1] =  tpVarsOf2D3DAAM->m_Trig.Sy * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[2] =  tpVarsOf2D3DAAM->m_Trig.Cy;

			float Temp = tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cy;
			pRotMat[3] =  Temp * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[4] = -Temp * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[5] =  tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sy;

			Temp     = tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cy;
			pRotMat[6] = -Temp * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[7] =  Temp * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[8] = -tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sy;
		}
		break;
	case rtFstDeriv_z:     // 
		{
			//[                      -cos(y)*sin(z),                      -cos(y)*cos(z), 0]
			//[ -sin(x)*sin(y)*sin(z)+cos(x)*cos(z), -sin(x)*sin(y)*cos(z)-cos(x)*sin(z), 0]
			//[  cos(x)*sin(y)*sin(z)+sin(x)*cos(z),  cos(x)*sin(y)*cos(z)-sin(x)*sin(z), 0]

			pRotMat[0] = -tpVarsOf2D3DAAM->m_Trig.Cy * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[1] = -tpVarsOf2D3DAAM->m_Trig.Cy * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[2] =  0;

			float Temp = tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[3] = -Temp * tpVarsOf2D3DAAM->m_Trig.Sz     + tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[4] = -Temp * tpVarsOf2D3DAAM->m_Trig.Cz     - tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[5] =  0;

			Temp     = tpVarsOf2D3DAAM->m_Trig.Cx * tpVarsOf2D3DAAM->m_Trig.Sy;
			pRotMat[6] =  Temp * tpVarsOf2D3DAAM->m_Trig.Sz     + tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Cz;
			pRotMat[7] =  Temp * tpVarsOf2D3DAAM->m_Trig.Cz     - tpVarsOf2D3DAAM->m_Trig.Sx * tpVarsOf2D3DAAM->m_Trig.Sz;
			pRotMat[8] =  0;
		}
		break;
	default:
		memset( pRotMat, 0, sizeof( float ) * 9 );
	}
}

void CAAM::RotateTransPoint3D(const float pRotMat[], const float pTrans[], const float*  pPnt3DSrc, float*  pPnt3DDst)
{
	pPnt3DDst[0]  = pRotMat[0] * pPnt3DSrc[0] + pRotMat[1] * pPnt3DSrc[1] + pRotMat[2] * pPnt3DSrc[2] + pTrans[0];
	pPnt3DDst[1]  = pRotMat[3] * pPnt3DSrc[0] + pRotMat[4] * pPnt3DSrc[1] + pRotMat[5] * pPnt3DSrc[2] + pTrans[1];
	pPnt3DDst[2]  = pRotMat[6] * pPnt3DSrc[0] + pRotMat[7] * pPnt3DSrc[1] + pRotMat[8] * pPnt3DSrc[2] + pTrans[2];
}

void CAAM::RotatePoint3D(const float pRotMat[], const float*  pPnt3DSrc, float*  pPnt3DDst)
{
	pPnt3DDst[0]  = pRotMat[0] * pPnt3DSrc[0] + pRotMat[1] * pPnt3DSrc[1] + pRotMat[2] * pPnt3DSrc[2];
	pPnt3DDst[1]  = pRotMat[3] * pPnt3DSrc[0] + pRotMat[4] * pPnt3DSrc[1] + pRotMat[5] * pPnt3DSrc[2];
	pPnt3DDst[2]  = pRotMat[6] * pPnt3DSrc[0] + pRotMat[7] * pPnt3DSrc[1] + pRotMat[8] * pPnt3DSrc[2];
}


void CAAM::Calc3DModelProj2DImgCrd(float*  pfl3DProj2DShape)
{
	float  dbaNormRotMat[9], dbaPoint3DCam[3], dbInvPz;
	CalcRotationTrigVal();          
	GetRotMatrix( dbaNormRotMat, rtNormRotMatrix  );

	memset(tpVarsOf2D3DAAM->tpflKeyPoint3D, 0, sizeof(float) * tpCandideMdl->m_iMSVertexNum * 3);
	SynPersKeyPoint3D(tpVarsOf2D3DAAM->tpflKeyPoint3D);

	int j;
	for ( j = 0; j < tpCandideMdl->m_iMSVertexNum; j++ )
	{
		float* pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + j * 3;
		RotateTransPoint3D( dbaNormRotMat, tpVarsOf2D3DAAM->m_TranslXYZ, pdbPoint3DMdl, dbaPoint3DCam); 

		dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
		pfl3DProj2DShape[j*2 + 0] = tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[0] * dbInvPz + 0.5f * (InputImgW - 1);      // point projected onto image plane
		pfl3DProj2DShape[j*2 + 1] = -tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[1] * dbInvPz + 0.5f * (InputImgH - 1);      //because of using ROI

	}
}

void CAAM::Calc3DModelProj2DImgCrd(float*  pfl3DProj2DShape,RECT rcROI)
{
	float  dbaNormRotMat[9], dbaPoint3DCam[3], dbInvPz;
	CalcRotationTrigVal();          
	GetRotMatrix( dbaNormRotMat, rtNormRotMatrix  );

	memset(tpVarsOf2D3DAAM->tpflKeyPoint3D, 0, sizeof(float) * tpCandideMdl->m_iMSVertexNum * 3);
	SynPersKeyPoint3D(tpVarsOf2D3DAAM->tpflKeyPoint3D);

	int j;
	for ( j = 0; j < tpCandideMdl->m_iMSVertexNum; j++ )
	{
		float* pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + j * 3;
		RotateTransPoint3D( dbaNormRotMat, tpVarsOf2D3DAAM->m_TranslXYZ, pdbPoint3DMdl, dbaPoint3DCam); 

		dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
		pfl3DProj2DShape[j*2 + 0] =  tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[0] * dbInvPz + 0.5f * (InputImgW - 1)-rcROI.left;      // point projected onto image plane
		pfl3DProj2DShape[j*2 + 1] = -tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[1] * dbInvPz + 0.5f * (InputImgH - 1)-rcROI.top;      //because of using ROI

	}
}


void CAAM::FilterFeatureByMainDirection( AAMResult* pAAMRlt )
{
	int     j;

	//----------------  stat. the main direction of motion ---------------//
#define     NO_MOTIONDIR_BINS  36
	int*    pnDirCntHist      = new int[NO_MOTIONDIR_BINS + 1];     // feature counts for each direction, additional one for no motion
	int*    pnDirCntHistBak   = new int[NO_MOTIONDIR_BINS + 1];     // ditto
	float*  pflDirDistAcc     = new float[NO_MOTIONDIR_BINS + 1];   // accumulated distances of motion for each direction

	memset( pnDirCntHist, 0, sizeof(int) * ( NO_MOTIONDIR_BINS + 1 ) );
	memset( pnDirCntHistBak, 0, sizeof(int) * ( NO_MOTIONDIR_BINS + 1 ) );
	memset( pflDirDistAcc, 0, sizeof(float) * ( NO_MOTIONDIR_BINS + 1 ) );

	int*    pnFeaMotionDirIdx = new int[NO_TRACKFEATURE];

	float   flBinRng = float( 2 * M_PI / NO_MOTIONDIR_BINS );
	float   flDistThr = 1.0f;
	int     nValidPtsCnt = 0;

	for ( j = 0; j < NO_TRACKFEATURE; j++ )
	{
		if ( tpCurrFeatures[j].w <= 0 )
		{
			pnFeaMotionDirIdx[ j ]  = -1;
			continue;
		}

		nValidPtsCnt++;

		float   x, y;
		x = tpCurrFeatures[j].x - tpPrevFeatures[j].x;
		y = tpCurrFeatures[j].y - tpPrevFeatures[j].y;

		float   flDist = x*x + y*y;

		if( flDist <= flDistThr )
		{
			pnDirCntHistBak[ 0 ]   += 1;
			pflDirDistAcc[ 0 ]     += flDist;
			pnFeaMotionDirIdx[ j ]  = 0;
			continue;
		}

		flDist = sqrt( flDist );

		float   flTheta = atan2( y, x );
		if( flTheta < 0 )
			flTheta += float( 2 * M_PI );

		int     nIdx = int( flTheta / flBinRng ) + 1;   // index 0 for no motion

		pnDirCntHistBak[ nIdx ]     += 1;
		pflDirDistAcc[ nIdx ]       += flDist;
		pnFeaMotionDirIdx[ j ]       = nIdx;
	}

	// seach the main direction
	memcpy( pnDirCntHist, pnDirCntHistBak, sizeof(int) * NO_MOTIONDIR_BINS );
	pnDirCntHist[1] += pnDirCntHistBak[NO_MOTIONDIR_BINS] + pnDirCntHistBak[ 2 ];
	for ( j = 2; j < NO_MOTIONDIR_BINS; j++ )
	{
		pnDirCntHist[j] += pnDirCntHistBak[j - 1] + pnDirCntHistBak[j + 1];
	}
	pnDirCntHist[NO_MOTIONDIR_BINS] += pnDirCntHistBak[NO_MOTIONDIR_BINS - 1] + pnDirCntHistBak[ 1 ];

	int nMaxIdx = 1;
	for ( j = 2; j < NO_MOTIONDIR_BINS + 1; j++ )   // search the main direction
	{
		if ( pnDirCntHist[j] > pnDirCntHist[nMaxIdx] )
		{
			nMaxIdx = j;
		}
	}

	int nCnt = int( pnDirCntHist[nMaxIdx] );

	if ( pnDirCntHistBak[0] > nCnt )     
	{
		pAAMRlt->tflMainDir = 0;
		pAAMRlt->tflMainDirMag = pflDirDistAcc[ 0 ] / ( pnDirCntHistBak[0] );
	}
	else
	{
		// average distance of main direction
		float   flDistSum = pflDirDistAcc[nMaxIdx];
		if ( nMaxIdx == 1 )
			flDistSum += ( pflDirDistAcc[NO_MOTIONDIR_BINS] + pflDirDistAcc[2] );
		else if ( nMaxIdx == NO_MOTIONDIR_BINS )
			flDistSum += ( pflDirDistAcc[NO_MOTIONDIR_BINS - 1] + pflDirDistAcc[ 1 ] );
		else 
			flDistSum += ( pflDirDistAcc[nMaxIdx - 1] + pflDirDistAcc[nMaxIdx + 1] );

		pAAMRlt->tflMainDir    = ( nMaxIdx - 1 + 0.5f ) * flBinRng;
		pAAMRlt->tflMainDirMag = flDistSum / ( nCnt == 0 ? 1 : nCnt );

		if ( pAAMRlt->tflMainDirMag > MAINDIR_MIN_MOTION_THR )
		{
			for ( j = 0; j < NO_TRACKFEATURE; j++ )   // set 
			{
				if ( pnFeaMotionDirIdx[j] == 0 )
				{
					tpCurrFeatures[j].w = -tpCurrFeatures[j].w;
				}
				else if ( pnFeaMotionDirIdx[j] != -1 )
				{

					int nErr = abs( pnFeaMotionDirIdx[j] - nMaxIdx );
					if ( nErr > NO_MOTIONDIR_BINS / 2 )     
						nErr = NO_MOTIONDIR_BINS - nErr;

					if( nErr > 1 )
						tpCurrFeatures[j].w = -tpCurrFeatures[j].w;
				}
			}
		}
	}


	delete [] pnDirCntHist      ;   
	delete [] pnDirCntHistBak   ;
	delete [] pflDirDistAcc     ;
	delete [] pnFeaMotionDirIdx ;

}


bool CAAM::PreAlignByFeatureTrack( unsigned char* inputGrayImg, AAMResult* pAAMRlt )
{

	memcpy(coef_p_new, &(pAAMRlt->tpfParameters[4]), sizeof(float)*aammodel.shapeAxiNum);
	memcpy(coef_q_new, pAAMRlt->tpfParameters, sizeof(float)*4);

	float   r = 1000;

	int nCurShapeNum = 0;           
	int     nDispInterimRlt = 0;
	int     j, k;

	if ( tpContext && tpContext->tbDispPreAlignByFeaTrack )
		nDispInterimRlt = 1;


	//----------------  stat. the main direction of motion ---------------//
	int     nValidPtsCnt = 0;
	FilterFeatureByMainDirection( pAAMRlt );
	//----------------  stat. the main direction of motion ---------------//

	SynthesizeShape_p(coef_p_new, shape_bak);
	SynthesizeShape_q(coef_q_new, shape_bak, shapeCrd);


	for ( k = 0; k < 4 && r > 1; k++ )
	{
		if ( k >= 2 )
			nCurShapeNum = aammodel.shapeAxiNum;

		CMATRIX pMatSD(NO_TRACKFEATURE*2, nCurShapeNum+4);
		CMATRIX pMatSD_T( nCurShapeNum+4, NO_TRACKFEATURE*2);
		CMATRIX pMatXYErr( NO_TRACKFEATURE*2, 1);
		CMATRIX pMatSDTMulErr( nCurShapeNum+4, 1);
		CMATRIX pMatHessian(nCurShapeNum+4, nCurShapeNum+4);

		pMatSD.SetZero();
		pMatSD_T.SetZero();
		pMatXYErr.SetZero();
		pMatSDTMulErr.SetZero();
		pMatHessian.SetZero();

		memset( tppflDerSd_dqp[0], 0, sizeof(float) * aammodel.vertexnum * 2 * (aammodel.shapeAxiNum + 4) );
		AAMPoint*   pCurSPts = (AAMPoint*)shape_bak;  // current aam shape

		float q3, q4, flTmp;
		q3 = coef_q_new[2];
		q4 = coef_q_new[3];

		int m, n;
		for ( j = 0; j < aammodel.vertexnum; j++ )       // canculate jacobian for each point
		{
			tppflDerSd_dqp[j*2 + 0][0] =  1;
			tppflDerSd_dqp[j*2 + 0][1] =  0;
			tppflDerSd_dqp[j*2 + 0][2] =  pCurSPts[j].x;
			tppflDerSd_dqp[j*2 + 0][3] = -pCurSPts[j].y;

			for ( m = 0; m < nCurShapeNum; m++ )
			{
				flTmp = (1 + q3) * aammodel.ShapeComp_p[m][j*2 + 0] - q4 * aammodel.ShapeComp_p[m][j*2 + 1];
				tppflDerSd_dqp[j*2 + 0][m + 4] = flTmp;     
			}


			tppflDerSd_dqp[j*2 + 1][0] =  0;
			tppflDerSd_dqp[j*2 + 1][1] =  1;
			tppflDerSd_dqp[j*2 + 1][2] =  pCurSPts[j].y;
			tppflDerSd_dqp[j*2 + 1][3] =  pCurSPts[j].x;

			for ( m = 0; m < nCurShapeNum; m++ )
			{
				flTmp = q4 * aammodel.ShapeComp_p[m][j*2 + 0] + (1 + q3) * aammodel.ShapeComp_p[m][j*2 + 1];
				tppflDerSd_dqp[j*2 + 1][m + 4] = flTmp;     
			}
		}
		nValidPtsCnt = 0;
		for ( j = 0; j < NO_TRACKFEATURE; j++ )
		{
			if ( tpCurrFeatures[j].w <= 0 )
				continue;

			float   x, y;
			x = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 0] +
				tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 0] +
				tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 0];

			y = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 1] +
				tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 1] +
				tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 1];

			x -= tpCurrFeatures[j].x;
			y -= tpCurrFeatures[j].y;

			if ( x*x + y*y >= r*r )
			{
				tpCurrFeatures[j].w = 0;
				continue;
			}

			nValidPtsCnt++;
			pMatXYErr.p[(j*2)*pMatXYErr.width+0] = x;
			pMatXYErr.p[(j*2+1)*pMatXYErr.width+0] = y;

			for ( n = 0; n < 2; n++ )
			{
				for ( m = 0; m < nCurShapeNum + 4; m++ )
				{
					flTmp = tpBaryCoords[j].x * tppflDerSd_dqp[ tpFeaTriMesh[j].xi + n ][m] +
						tpBaryCoords[j].y * tppflDerSd_dqp[ tpFeaTriMesh[j].xj + n ][m] +
						tpBaryCoords[j].z * tppflDerSd_dqp[ tpFeaTriMesh[j].xk + n ][m];

					pMatSD.p[(j*2+n)*pMatSD.width+m] = flTmp;
				}
			}
		}

		if ( nValidPtsCnt == 0 )
		{
			return false;
		}

		pMatSD_T = ~pMatSD;

		for ( j = 0; j < NO_TRACKFEATURE; j++ )     // weight for each feature point
		{
			if ( tpCurrFeatures[j].w <= 0 )
				continue;
			for ( n = 0; n < 2; n++ )
			{
				for ( m = 0; m < nCurShapeNum + 4; m++ )
				{
					flTmp = pMatSD_T.p[m*pMatSD_T.width+j*2+n];
					pMatSD_T.p[m*pMatSD_T.width+2*j+n] = flTmp * tpCurrFeatures[j].w;
				}
			}
		}

		pMatSDTMulErr.SUPERMUL(pMatSD_T, pMatXYErr);
		pMatHessian.SUPERMUL(pMatSD_T, pMatSD);

		pMatHessian = !pMatHessian;
		pMatSDTMulErr.SUPERMUL(pMatHessian, pMatSDTMulErr);



		for ( m = 0; m < nCurShapeNum + 4; m++ )
		{
			dpq[m] = -pMatSDTMulErr.p[m*pMatSDTMulErr.width+0];   
		}



		for ( m = 0; m < 4; m++ )
			coef_q_new[m] += dpq[m];

		for ( m = 0; m < nCurShapeNum; m++ )
			coef_p_new[m] += dpq[m + 4];

		for ( m = 0; m < nCurShapeNum; m++ )   
		{
			float threshold = 3.0f*aammodel.ShapeEigenvalue[m];

			if ( coef_p_new[m] < -threshold )
			{
				coef_p_new[m] = -threshold;
			}
			else if ( coef_p_new[m] > threshold )
			{
				coef_p_new[m] = threshold;
			}
		}

		// synthesis shape
		SynthesizeShape_p(coef_p_new, shape_bak);

		SynthesizeShape_q(coef_q_new, shape_bak, shapeCrd);

		double dbDistSum = 0;
		CalcuSqDistSum( r, dbDistSum, nValidPtsCnt );

		if ( nValidPtsCnt != 0 ) 
		{
			dbDistSum /= nValidPtsCnt;
			dbDistSum = sqrt( dbDistSum );
			if ( k <= 1 )
				r = float( dbDistSum * 2.0 );
		}
		else
		{
			r *= 0.5f;
		}
	}

	tflFeaTrackR = max( 1.f, r );
	tnInlierFeatures = nValidPtsCnt;

	memcpy( &(pAAMRlt->tpfParameters[4]), coef_p_new, sizeof(float)*aammodel.shapeAxiNum );
	memcpy( pAAMRlt->tpfParameters, coef_q_new, sizeof(float)*4 );
	memcpy( (AAMPoint*)pAAMRlt->tpfShape, shapeCrd, sizeof(float) * pAAMRlt->tnLandmarkNum * 2 );
	bool bSuccess = UpdateFaceRect(shapeCrd, pAAMRlt->trcFaceRect);    
	return bSuccess;

}

// NoW derivative for forwards-additive algorithm
void CAAM::CalcuDerivativeNW_qp_FA( const int nCurShapeNum )
{

	memset( tppflDerSd_dqp[0], 0, sizeof(float) * aammodel.vertexnum * 2 * (aammodel.shapeAxiNum + 4) );
	AAMPoint*   pCurSPts = (AAMPoint*)shape_bak;  

	float q3, q4, flTmp;
	q3 = coef_q_new[2];
	q4 = coef_q_new[3];

	int j, m;
	for ( j = 0; j < aammodel.vertexnum; j++ )       
	{
		tppflDerSd_dqp[j*2 + 0][0] =  1;
		tppflDerSd_dqp[j*2 + 0][1] =  0;
		tppflDerSd_dqp[j*2 + 0][2] =  pCurSPts[j].x;
		tppflDerSd_dqp[j*2 + 0][3] = -pCurSPts[j].y;

		for ( m = 0; m < nCurShapeNum; m++ )
		{
			flTmp = (1 + q3) * aammodel.ShapeComp_p[m][j*2 + 0] - q4 * aammodel.ShapeComp_p[m][j*2 + 1];
			tppflDerSd_dqp[j*2 + 0][m + 4] = flTmp;     
		}


		tppflDerSd_dqp[j*2 + 1][0] =  0;
		tppflDerSd_dqp[j*2 + 1][1] =  1;
		tppflDerSd_dqp[j*2 + 1][2] =  pCurSPts[j].y;
		tppflDerSd_dqp[j*2 + 1][3] =  pCurSPts[j].x;

		for ( m = 0; m < nCurShapeNum; m++ )
		{
			flTmp = q4 * aammodel.ShapeComp_p[m][j*2 + 0] + (1 + q3) * aammodel.ShapeComp_p[m][j*2 + 1];
			tppflDerSd_dqp[j*2 + 1][m + 4] = flTmp;     
		}
	}
}



void CAAM::CalcuSqDistSum( const float r, double& dbDistSum, int& nValidPtsCnt )
{
	dbDistSum = 0;
	nValidPtsCnt = 0;
	int j;
	for ( j = 0; j < NO_TRACKFEATURE; j++ )
	{
		if ( tpCurrFeatures[j].w <= 0 )
			continue;

		float   x, y;
		x = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 0] +
			tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 0] +
			tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 0];

		y = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 1] +
			tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 1] +
			tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 1];

		x -= tpCurrFeatures[j].x;
		y -= tpCurrFeatures[j].y;

		if ( x*x + y*y >= r*r )
			continue;

		dbDistSum += ( x*x + y*y );
		nValidPtsCnt++;
	}
}

void CAAM::CalcuDerivativeSd_dqp()
{
	if ( tbHasCalcuDerivSd_dqp )
		return;

	int i, j;
	memset( tppflDerSd_dqp[0], 0, sizeof(float) * aammodel.vertexnum * 2 * (aammodel.shapeAxiNum + 4) );

	for ( j = 0; j < aammodel.vertexnum; j++ )      
	{
		int j2=j+j;

		AAMPoint*   p2DMSPts = (AAMPoint*)aammodel.MeanShape;  // mean shape
		int     iNk = aammodel.triangle_vertex[ j ] ;       // iNk connected triangles

		for ( i = 0; i < iNk; i++ )                     // for each triangle include this point
		{
			int iTrigIdx = aammodel.triangle_index[ j ][i];
			int k1, k2, k3;
			k1 = aammodel.triangles[iTrigIdx].xi >> 1;  
			k2 = aammodel.triangles[iTrigIdx].xj >> 1; 
			k3 = aammodel.triangles[iTrigIdx].xk >> 1;

			float   flInv2D = aammodel.tpInvTriangleArea[iTrigIdx] / iNk;   // divided by iNk

			float   flAlphaY, flAlphaX, flBetaY, flBetaX, flGammaY, flGammaX;

			flAlphaY = p2DMSPts[k2].y - p2DMSPts[k3].y; 
			flAlphaX = p2DMSPts[k3].x - p2DMSPts[k2].x;

			flBetaY = p2DMSPts[k3].y - p2DMSPts[k1].y; 
			flBetaX = p2DMSPts[k1].x - p2DMSPts[k3].x;

			flGammaY = p2DMSPts[k1].y - p2DMSPts[k2].y; 
			flGammaX = p2DMSPts[k2].x - p2DMSPts[k1].x;


			float   flTmp;

			int m, n;
			for ( n = 0; n < 2; n++ ) // aS+ix/a(deltaq), aS+iy/a(deltaq)
			{
				flTmp = -flAlphaY * shapeCrd[k1*2 + n] - flBetaY * shapeCrd[k2*2 + n] - flGammaY * shapeCrd[k3*2 + n];
				tppflDerSd_dqp[j2 + n][0] += flInv2D * flTmp;     // aS+i/a(deltaq1)

				flTmp = -flAlphaX * shapeCrd[k1*2 + n] - flBetaX * shapeCrd[k2*2 + n] - flGammaX * shapeCrd[k3*2 + n];
				tppflDerSd_dqp[j2 + n][1] += flInv2D * flTmp;     // aS+i/a(deltaq2)

				flTmp = - (flAlphaY * p2DMSPts[j].x + flAlphaX * p2DMSPts[j].y) * shapeCrd[k1*2 + n] 
				- (flBetaY  * p2DMSPts[j].x + flBetaX  * p2DMSPts[j].y) * shapeCrd[k2*2 + n]
				- (flGammaY * p2DMSPts[j].x + flGammaX * p2DMSPts[j].y) * shapeCrd[k3*2 + n];
				tppflDerSd_dqp[j2 + n][2] += flInv2D * flTmp;     // aS+i/a(deltaq3)

				flTmp =   (flAlphaY * p2DMSPts[j].y - flAlphaX * p2DMSPts[j].x) * shapeCrd[k1*2 + n] 
				+ (flBetaY  * p2DMSPts[j].y - flBetaX  * p2DMSPts[j].x) * shapeCrd[k2*2 + n]
				+ (flGammaY * p2DMSPts[j].y - flGammaX * p2DMSPts[j].x) * shapeCrd[k3*2 + n];
				tppflDerSd_dqp[j2 + n][3] += flInv2D * flTmp;     // aS+i/a(deltaq4)


				for ( m = 0; m < m_iCurShapeNum - 4; m++ )
				{
					int j21 = j2+1;
					flTmp = - (flAlphaY * aammodel.ShapeComp_p[m][j2 + 0] + flAlphaX * aammodel.ShapeComp_p[m][j21]) *        shapeCrd[k1*2 + n] 
					- (flBetaY  * aammodel.ShapeComp_p[m][j2 + 0] + flBetaX  * aammodel.ShapeComp_p[m][j21]) * shapeCrd[k2*2 + n]
					- (flGammaY * aammodel.ShapeComp_p[m][j2 + 0] + flGammaX * aammodel.ShapeComp_p[m][j21]) * shapeCrd[k3*2 + n];
					tppflDerSd_dqp[j2 + n][m + 4] += flInv2D * flTmp;     // aS+i/a(deltaPm)

				}
			}

		}
	}

	tbHasCalcuDerivSd_dqp = true;
}

void CAAM::SynthesizeShape_p(float* pflP, float* pflShape)
{
	if ( !pflP || !pflShape )
		return;
	memcpy( pflShape, aammodel.MeanShape, sizeof(float) * aammodel.vertexnum * 2 );
	int i, m;

	for ( m = 0; m < aammodel.shapeAxiNum; m++ )
	{
		for ( i = 0; i < aammodel.vertexnum * 2; i++ )
		{
			pflShape[i] += pflP[m] * aammodel.ShapeComp_p[m][i];
		}
	}
}
// pflQ[4]: Tx, Ty, s*cos(theta) - 1, s*sin(theta) 
void CAAM::SynthesizeShape_q(float* pflQ, float* pflShapeIn, float* pflShapeOut)
{
	if ( !pflQ || !pflShapeIn || !pflShapeOut )
		return;

	int i;
	float x, y;

	for ( i = 0; i < aammodel.vertexnum; i++ )
	{ 
		int i2 = i + i;    
		x = pflShapeIn[i2 + 0];
		y = pflShapeIn[i2 + 1];

		pflShapeOut[i2 + 0] = (1 + pflQ[2]) * x - pflQ[3] * y + pflQ[0];
		pflShapeOut[i2 + 1] = pflQ[3] * x + (1 + pflQ[2]) * y + pflQ[1];
	}
}


void CAAM::SynthesizeShape_qp(float* pflQ, float* pflP, float* pflShape)
{
	if ( !pflQ || !pflP || !pflShape )
		return;

	SynthesizeShape_p(pflP, pflShape);

	SynthesizeShape_q(pflQ, pflShape, pflShape);
}

void CAAM::ResetOnlineTempl()
{ 
	tbHasOnlineTempl   = false; 
	m_iTemplAccCnt      = 0;
}


void CAAM::Calcu3DPara_By_2DShape( const bool bTracking, const RECT rcROI, bool bCalcuOnlyPose )
{

	int     i, j;
	int     i3DParaNum = 3;
	int iCurShapeNum = 0;


	float   flDeltaErrThr = 1; 
	float   flSqErrCur = 999999, flSqErrPre = 0;
	int     nMaxIterNum = bCalcuOnlyPose ? 1 : 10;

	LUTsOf2D3DAAM*  pLUTsOf2D3DAAM = tpLUTsOf2D3DAAM[tnCurLUTsOf2D3DAAMIdx];

	int iIterN;
	for ( iIterN = 0; iIterN < nMaxIterNum && abs(flSqErrPre - flSqErrCur) > flDeltaErrThr; iIterN++ )
	{
		flSqErrPre = flSqErrCur;
		flSqErrCur = 0;

		if ( iIterN == 1)
		{
			i3DParaNum = 6;
		}
		else if ( iIterN >= 2)   
		{
			if (!bTracking) 
				i3DParaNum = 6 + tpCandideMdl->m_iSUNum;
			else
				i3DParaNum = 6 + tpCandideMdl->m_iAUNum;
		}

		for ( i = 1; i < (pLUTsOf2D3DAAM->m_iComVertexNum * 2); i++ )
			tpVarsOf2D3DAAM->tppflMatSD3D[i] = tpVarsOf2D3DAAM->tppflMatSD3D[i-1] + (iCurShapeNum + i3DParaNum);

		CMATRIX matSD3D(pLUTsOf2D3DAAM->m_iComVertexNum * 2, iCurShapeNum + i3DParaNum);
		matSD3D.SetZero();
		matSD3D.CopyToData1D(tpVarsOf2D3DAAM->tppflMatSD3D[0]);

		CMATRIX mat2DPtsErr(pLUTsOf2D3DAAM->m_iComVertexNum * 2, 1);
		mat2DPtsErr.SetZero();

		CMATRIX matHessian3D(iCurShapeNum+i3DParaNum,iCurShapeNum+i3DParaNum);
		matHessian3D.SetZero();

		CMATRIX matSDTMulErr3D(iCurShapeNum + i3DParaNum, 1);
		matSDTMulErr3D.SetZero();

		//3D jacobian related variable
		float  dbaNormRotMat[9], dbaRotMatX1[9], dbaRotMatY1[9], dbaRotMatZ1[9];
		float  *pdbaRotMatDer[3];
		float  *pdbPoint3DMdl, dbaPoint3DCam[3], dba3DVect[3];
		float  dba2DAAMPnt[2], dba3DAAMPnt[2];
		float  dbaTranslate[3] = {tpVarsOf2D3DAAM->m_TranslXYZ[0], tpVarsOf2D3DAAM->m_TranslXYZ[1], tpVarsOf2D3DAAM->m_TranslXYZ[2]};
		float  dbTmp1, dbTmp2, dbTmp3, dbInvPz;

		CalcRotationTrigVal();         
		GetRotMatrix( dbaNormRotMat, rtNormRotMatrix  );
		GetRotMatrix( dbaRotMatX1, rtFstDeriv_x  );
		GetRotMatrix( dbaRotMatY1, rtFstDeriv_y );
		GetRotMatrix( dbaRotMatZ1, rtFstDeriv_z );

		pdbaRotMatDer[0] = dbaRotMatX1;
		pdbaRotMatDer[1] = dbaRotMatY1;
		pdbaRotMatDer[2] = dbaRotMatZ1;

		memset(tpVarsOf2D3DAAM->tpflKeyPoint3D, 0, sizeof(float) * tpCandideMdl->m_iMSVertexNum * 3);
		SynPersKeyPoint3D(tpVarsOf2D3DAAM->tpflKeyPoint3D);

		for ( j = 0; j < pLUTsOf2D3DAAM->m_iComVertexNum; j++ )       // canculate jacobian for each point
		{

			// 2D jacobian
			int     i2DIdx = pLUTsOf2D3DAAM->tpiComTo2DIdx[j];               //corresponding 2d aam point index

			int     ia2DIndices[2]      = {i2DIdx, -1};
			float   fla2DIntplCoef[2]   = {1.f, 0.f};

			bool    bNeedIntpl = false;
			for ( i = 0; i < pLUTsOf2D3DAAM->m_i2DNeedIntplNum; i++ )
			{
				if ( pLUTsOf2D3DAAM->tpi2DNeedIntplIdx[i] == j )
				{
					bNeedIntpl = true;
					ia2DIndices[1]      = pLUTsOf2D3DAAM->tpi2DIntplIdx[i];
					fla2DIntplCoef[0]   = pLUTsOf2D3DAAM->tpfl2DIntplCoef[i][0];
					fla2DIntplCoef[1]   = pLUTsOf2D3DAAM->tpfl2DIntplCoef[i][1];
					break;
				}
			}

			dba2DAAMPnt[0] = 0;
			dba2DAAMPnt[1] = 0;

			int     k, iPtsNum = ( bNeedIntpl ? 2 : 1 );
			float   flIntplCoef;
			for ( i = 0; i < iPtsNum; i++ )
			{
				k = ia2DIndices[i];
				flIntplCoef = fla2DIntplCoef[i];

				dba2DAAMPnt[0] += shapeCrd[k*2 + 0] * flIntplCoef;
				dba2DAAMPnt[1] += shapeCrd[k*2 + 1] * flIntplCoef;
			}

			// 3D jacobian
			k = pLUTsOf2D3DAAM->tpiComTo3DIdx[j];               //corresponding 3d model point index



			pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + k * 3;
			RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 

			dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
			dba3DAAMPnt[0] = tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[0] * dbInvPz + 0.5f * (InputImgW - 1) - rcROI.left;      // point projected onto image plane
			dba3DAAMPnt[1] = -tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[1] * dbInvPz + 0.5f * (InputImgH - 1) - rcROI.top;      //because of using ROI

			dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
			dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
			dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)


			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 0] = -(dbTmp1);
			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 0] = 0;

			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 1] = 0;
			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 1] = -(-dbTmp1);

			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 2] = -(dbTmp2);
			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 2] = -(dbTmp3);

			for ( i = 0; i < 3 && (i3DParaNum > 3); i++ )
			{
				RotatePoint3D(pdbaRotMatDer[i], pdbPoint3DMdl, dba3DVect);   

				tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 3 + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]);    //∂‘x,y,z–˝◊™Ω«µƒ∆´µº
				tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 3 + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
			}

			mat2DPtsErr.p[(j*2)*mat2DPtsErr.width+0] = dba2DAAMPnt[0] - dba3DAAMPnt[0];
			mat2DPtsErr.p[(j*2+1)*mat2DPtsErr.width+0] = dba2DAAMPnt[1] - dba3DAAMPnt[1];

			flSqErrCur += (dba2DAAMPnt[0] - dba3DAAMPnt[0]) * (dba2DAAMPnt[0] - dba3DAAMPnt[0]);
			flSqErrCur += (dba2DAAMPnt[1] - dba3DAAMPnt[1]) * (dba2DAAMPnt[1] - dba3DAAMPnt[1]);
		}

		int iVtxNumStart = 0;
		int iParaStart = iCurShapeNum + 6;

		for ( i = 0; i < tpCandideMdl->m_iSUNum && !bTracking && i3DParaNum > 6; i++ )
		{
			int iVtxNum = tpCandideMdl->tpiEach_SUVertexNum[i];
			for ( j = 0; j < iVtxNum; j++ )
			{
				int i3DPtsIdx = tpCandideMdl->tpiEach_SUVertexIdx[iVtxNumStart + j];

				int iComIdx = pLUTsOf2D3DAAM->tpi3DToComIdx[i3DPtsIdx];
				if ( iComIdx == -1 ) 
					continue;


				pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + i3DPtsIdx * 3;
				RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 
				dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
				dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
				dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
				dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)

				float* pData = (float*)( tpCandideMdl->tpEach_SUVertexVal + (iVtxNumStart + j) );
				RotatePoint3D(dbaNormRotMat, pData, dba3DVect);

				tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 0][iParaStart + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]); 
				tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 1][iParaStart + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
			}
			iVtxNumStart += iVtxNum;
		}


		//AU
		iVtxNumStart = 0;
		iParaStart = iCurShapeNum + 6;
		for ( i = 0; i < tpCandideMdl->m_iAUNum && bTracking  && i3DParaNum > 6; i++ )
		{
			int iVtxNum = tpCandideMdl->tpiEach_AUVertexNum[i];
			for ( j = 0; j < iVtxNum; j++ )
			{
				int i3DPtsIdx = tpCandideMdl->tpiEach_AUVertexIdx[iVtxNumStart + j];

				int iComIdx = pLUTsOf2D3DAAM->tpi3DToComIdx[i3DPtsIdx];
				if ( iComIdx == -1 ) 
					continue;

				pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + i3DPtsIdx * 3;
				RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 
				dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
				dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
				dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
				dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)


				float* pData = (float*)( tpCandideMdl->tpEach_AUVertexVal + (iVtxNumStart + j) );
				RotatePoint3D(dbaNormRotMat, pData, dba3DVect);

				tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 0][iParaStart + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]); 
				tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 1][iParaStart + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
			}
			iVtxNumStart += iVtxNum;
		}

		matSD3D.CopyFromData1D(tpVarsOf2D3DAAM->tppflMatSD3D[0]);

		matSDTMulErr3D.SUPER_X(matSD3D, mat2DPtsErr);

		matHessian3D.SUPER_X(matSD3D, matSD3D);
		pinv(matHessian3D.p,matHessian3D.p,iCurShapeNum+i3DParaNum,iCurShapeNum+i3DParaNum);

		matSDTMulErr3D = matHessian3D*matSDTMulErr3D;      

		int m;
		for (m = 0; m < 3; m++)
		{
			tpVarsOf2D3DAAM->m_TranslXYZ[m]   -= matSDTMulErr3D.p[(iCurShapeNum + m)*matSDTMulErr3D.width+0];//float(cvmGet(&matSDTMulErr3D, iCurShapeNum + m, 0));
			if ( i3DParaNum > 3 )
			{
				tpVarsOf2D3DAAM->m_RotAngleXYZ[m] -= matSDTMulErr3D.p[(iCurShapeNum + 3 + m)*matSDTMulErr3D.width+0];//float(cvmGet(&matSDTMulErr3D, iCurShapeNum + 3 + m, 0));
			}
		}

		float   fl3DParaThr = 1.f;
		iParaStart = iCurShapeNum + 6;
		if (!bTracking  && i3DParaNum > 6)
		{
			for (m = 0; m < tpCandideMdl->m_iSUNum; m++)
			{
				tpVarsOf2D3DAAM->tpSUCoef[m] -= matSDTMulErr3D.p[(iParaStart + m)*matSDTMulErr3D.width+0];//float(cvmGet(&matSDTMulErr3D, iParaStart + m, 0));

				if( tpVarsOf2D3DAAM->tpSUCoef[m] < -fl3DParaThr ) tpVarsOf2D3DAAM->tpSUCoef[m] = -fl3DParaThr;
				else if (  tpVarsOf2D3DAAM->tpSUCoef[m] > fl3DParaThr )   tpVarsOf2D3DAAM->tpSUCoef[m] = fl3DParaThr;
			}
		}

		iParaStart = iCurShapeNum + 6;
		for (m = 0; m < tpCandideMdl->m_iAUNum && bTracking  && i3DParaNum > 6; m++)
		{
			tpVarsOf2D3DAAM->tpAUCoef[m] -= matSDTMulErr3D.p[(iParaStart + m)*matSDTMulErr3D.width+0];//float(cvmGet(&matSDTMulErr3D, iParaStart + m, 0));

			if( tpVarsOf2D3DAAM->tpAUCoef[m] < -fl3DParaThr ) tpVarsOf2D3DAAM->tpAUCoef[m] = -fl3DParaThr;
			else if (  tpVarsOf2D3DAAM->tpAUCoef[m] > fl3DParaThr )   tpVarsOf2D3DAAM->tpAUCoef[m] = fl3DParaThr;
		}
	}                                                                  
}


// feature tracking constraint
void CAAM::AddConstraint_FeaTrack()
{   
	CalcuDerivativeSd_dqp();

	float   r = tflFeaTrackR;
	int     nCurShapeNum = aammodel.shapeAxiNum;        
	int     nValidPtsCnt = 0;

	tpMatFeaTrackSD.SetZero();
	tpMatFeaTrackSD_T.SetZero();
	tpMatFeaTrackErr.SetZero();

	int     j, m, n;
	float   flTmp;
	int     jtpMatFeaTrackErr_width2 = 0;
	for ( j = 0; j < NO_TRACKFEATURE; j++ )
	{
		if ( tpCurrFeatures[j].w <= 0 )
			continue;

		float   x, y;
		x = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 0] +
			tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 0] +
			tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 0];

		y = tpBaryCoords[j].x * shapeCrd[ tpFeaTriMesh[j].xi + 1] +
			tpBaryCoords[j].y * shapeCrd[ tpFeaTriMesh[j].xj + 1] +
			tpBaryCoords[j].z * shapeCrd[ tpFeaTriMesh[j].xk + 1];

		x -= tpCurrFeatures[j].x;
		y -= tpCurrFeatures[j].y;

		if ( x*x + y*y >= r*r )
			continue;

		nValidPtsCnt++;
		tpMatFeaTrackErr.p[jtpMatFeaTrackErr_width2] = x;
		jtpMatFeaTrackErr_width2+=tpMatFeaTrackErr.width;
		tpMatFeaTrackErr.p[jtpMatFeaTrackErr_width2] = y;
		jtpMatFeaTrackErr_width2+=tpMatFeaTrackErr.width; 


		for ( n = 0; n < 2; n++ )
		{
			int jtpMatFeaTrackSD2= (j*2+n)*tpMatFeaTrackSD.width;
			for ( m = 0; m < nCurShapeNum + 4; m++ )
			{
				flTmp = tpBaryCoords[j].x * tppflDerSd_dqp[ tpFeaTriMesh[j].xi + n ][m] +
					tpBaryCoords[j].y * tppflDerSd_dqp[ tpFeaTriMesh[j].xj + n ][m] +
					tpBaryCoords[j].z * tppflDerSd_dqp[ tpFeaTriMesh[j].xk + n ][m];

				tpMatFeaTrackSD.p[jtpMatFeaTrackSD2] = flTmp;
				jtpMatFeaTrackSD2++;
			}
		}
	}

	tpMatFeaTrackSD_T = ~tpMatFeaTrackSD;

	for ( j = 0; j < NO_TRACKFEATURE; j++ )     // weight for each feature point
	{
		if ( tpCurrFeatures[j].w <= 0 )
			continue;
		for ( n = 0; n < 2; n++ )
		{
			for ( m = 0; m < nCurShapeNum + 4; m++ )
			{
				flTmp = tpMatFeaTrackSD_T.p[m*tpMatFeaTrackSD_T.width+j*2+n];
				tpMatFeaTrackSD_T.p[m*tpMatFeaTrackSD_T.width+j*2+n] = flTmp*tpCurrFeatures[j].w;
			}
		}
	}

	tpMatSDTMulErr2DTmp.SUPERMUL(tpMatFeaTrackSD_T, tpMatFeaTrackErr);
	tpMatHessian2DTmp.SUPERMUL(tpMatFeaTrackSD_T, tpMatFeaTrackSD);

	float scale = m_AAMRunPara.tflFeaTrackK * 3 / (4*r*r*r);
	tpMatHessian2DTmp = tpMatHessian2DTmp*scale;
	tpMatSDTMulErr2DTmp = tpMatSDTMulErr2DTmp*scale;

	tpMatSDTMulErr2DFnl.SUPERADD(tpMatSDTMulErr2DTmp);
	tpMatHessian2DFnl.SUPERADD(tpMatHessian2DTmp);

	double dbDistSum = 0;
	CalcuSqDistSum( r, dbDistSum, nValidPtsCnt );

	if ( nValidPtsCnt != 0 ) 
	{
		dbDistSum /= nValidPtsCnt;
		dbDistSum = sqrt( dbDistSum );
		r = float( dbDistSum * 2.0 );
	}
	else
	{
		r *= 1.0f;
	}
	tflFeaTrackR = r;
}

// mouth inequation constraint
void CAAM::AddConstraint_MouthInequ( bool bTracking, float& flLipDist )
{
	int     i, j;
	if ( aammodel.vertexnum < 87 ) 
		return;
	flLipDist = aammodel.MeanShape[MIR_K*2 + 1] - aammodel.MeanShape[MIR_J*2 + 1];
	for ( i = 0; i < aammodel.shapeAxiNum; i++ )
		flLipDist += coef_p_new[i] * ( aammodel.ShapeComp_p[i][MIR_K*2 + 1] - aammodel.ShapeComp_p[i][MIR_J*2 + 1] );

	float   flMIRCoefK ;
	if ( flLipDist >= 0.f )
		flMIRCoefK = 0.f;
	else
		flMIRCoefK = 00000001000.00000f;

	if ( flLipDist < 0 )
	{
		tpMatFstDerB_p.SetZero();
		for ( i = 0; i < aammodel.shapeAxiNum; i++ )
		{
			tpMatFstDerB_p.p[0+i] = aammodel.ShapeComp_p[i][MIR_K * 2 + 1] - aammodel.ShapeComp_p[i][MIR_J * 2 + 1];
		}

		float a1 = coef_q_new[2]+1, b = coef_q_new[3];
		int     j2;
		float   flSxxyy, flSxyyx;
		float   flInvSqS_3 = aammodel.ShapeComp_q[2][aammodel.vertexnum * 2];
		float   flInvSqS_4 = aammodel.ShapeComp_q[3][aammodel.vertexnum * 2];
		for ( i = 0; i < aammodel.shapeAxiNum; i++ )
		{
			float   flInvSqS_i = aammodel.ShapeComp_p[i][aammodel.vertexnum * 2];

			flSxxyy = 0.f;
			flSxyyx = 0.f;
			for ( j = 0; j < aammodel.vertexnum; j++ )
			{
				j2 = j + j;
				flSxxyy += aammodel.ShapeComp_p[i][j2] * shapeCrd[j2] + aammodel.ShapeComp_p[i][j2 + 1] * shapeCrd[j2 + 1];
				flSxyyx += aammodel.ShapeComp_p[i][j2] * shapeCrd[j2 + 1] - aammodel.ShapeComp_p[i][j2 + 1] * shapeCrd[j2];
			}

			for ( j = 0; j < aammodel.vertexnum; j++ )
			{    
				j2 = j + j;
				float flTmp;
				flTmp = flInvSqS_3 * aammodel.ShapeComp_q[2][j2] * flSxxyy + flInvSqS_4 * aammodel.ShapeComp_q[3][j2] * flSxyyx + 
					a1 * aammodel.ShapeComp_p[i][j2] - b * aammodel.ShapeComp_p[i][j2 + 1] - 
					2 * coef_p_new[i] * ( flInvSqS_3 * aammodel.ShapeComp_q[2][j2] * a1 + flInvSqS_4 * aammodel.ShapeComp_q[3][j2] * b);


				tpMatDerP_Sd.p[i*tpMatDerP_Sd.width+j2] = flTmp * flInvSqS_i;


				flTmp = flInvSqS_3 * aammodel.ShapeComp_q[2][j2 + 1] * flSxxyy + flInvSqS_4 * aammodel.ShapeComp_q[3][j2 + 1] * flSxyyx + 
					b * aammodel.ShapeComp_p[i][j2] + a1 * aammodel.ShapeComp_p[i][j2 + 1] - 
					2 * coef_p_new[i] * ( flInvSqS_3 * aammodel.ShapeComp_q[2][j2 + 1] * a1 + flInvSqS_4 * aammodel.ShapeComp_q[3][j2 + 1] * b);


				tpMatDerP_Sd.p[i*tpMatDerP_Sd.width+j2+1] = flTmp * flInvSqS_i;
			}
		}

		tpMatDerP_Sd = tpMatDerP_Sd*(1.0f / (a1 * a1 + b * b));


		CMATRIX matDerSd_dqp(aammodel.vertexnum * 2, aammodel.shapeAxiNum + 4);
		matDerSd_dqp.CopyFromData1D(tppflDerSd_dqp[0]);

		CalcuDerivativeSd_dqp();    // may not calculate actually if has calculated before in one iteration


		tpMatDerP_dqp = tpMatDerP_Sd*matDerSd_dqp;
		CMATRIX tmpm1,tmpm2;
		tmpm1 = ~tpMatDerP_dqp;
		tmpm2 = ~tpMatFstDerB_p;
		tpMatSDTMulErr2DTmp = tmpm1 * tmpm2;  
		tmpm1 = ~tpMatSDTMulErr2DTmp;
		tpMatHessian2DTmp = tpMatSDTMulErr2DTmp*tmpm1; 

		tpMatSDTMulErr2DTmp = tpMatSDTMulErr2DTmp*(flMIRCoefK * flLipDist);
		tpMatHessian2DTmp = tpMatHessian2DTmp*flMIRCoefK;

		tpMatSDTMulErr2DFnl = tpMatSDTMulErr2DFnl + tpMatSDTMulErr2DTmp;
		tpMatHessian2DFnl = tpMatHessian2DFnl + tpMatHessian2DTmp;
	}
}


// mouth inequation constraint
void CAAM::AddConstraint_3DModel( CMATRIX& matHessian3D, CMATRIX& matSDTMulErr3D, const bool bTracking, const RECT rcROI )
{   
	int     i, j;

	int     i3DParaNum = 6 + tpCandideMdl->m_iSUNum;
	if (!bTracking) 
		i3DParaNum = 6 + tpCandideMdl->m_iSUNum;
	else
		i3DParaNum = 6 + tpCandideMdl->m_iAUNum;

	int     iCurShapeNum = m_iCurShapeNum;

	LUTsOf2D3DAAM*  pLUTsOf2D3DAAM = tpLUTsOf2D3DAAM[tnCurLUTsOf2D3DAAMIdx];

	for ( i = 1; i < (pLUTsOf2D3DAAM->m_iComVertexNum * 2); i++ )
		tpVarsOf2D3DAAM->tppflMatSD3D[i] = tpVarsOf2D3DAAM->tppflMatSD3D[i-1] + (iCurShapeNum + i3DParaNum);

	CMATRIX matSD3D(pLUTsOf2D3DAAM->m_iComVertexNum * 2, iCurShapeNum + i3DParaNum);
	matSD3D.SetZero();
	matSD3D.CopyToData1D(tpVarsOf2D3DAAM->tppflMatSD3D[0]);

	CMATRIX mat2DPtsErr(pLUTsOf2D3DAAM->m_iComVertexNum * 2, 1);
	mat2DPtsErr.SetZero();
	matHessian3D.Resize(iCurShapeNum + i3DParaNum, iCurShapeNum + i3DParaNum);
	matHessian3D.SetZero();
	matSDTMulErr3D.Resize(iCurShapeNum + i3DParaNum, 1);
	matSDTMulErr3D.SetZero();

	float  dbaNormRotMat[9], dbaRotMatX1[9], dbaRotMatY1[9], dbaRotMatZ1[9];
	float  *pdbaRotMatDer[3];
	float  *pdbPoint3DMdl, dbaPoint3DCam[3], dba3DVect[3];
	float  dba2DAAMPnt[2], dba3DAAMPnt[2];
	float  dbaTranslate[3] = {tpVarsOf2D3DAAM->m_TranslXYZ[0], tpVarsOf2D3DAAM->m_TranslXYZ[1], tpVarsOf2D3DAAM->m_TranslXYZ[2]};
	float  dbTmp1, dbTmp2, dbTmp3, dbInvPz;

	CalcRotationTrigVal();          
	GetRotMatrix( dbaNormRotMat, rtNormRotMatrix  );
	GetRotMatrix( dbaRotMatX1, rtFstDeriv_x  );
	GetRotMatrix( dbaRotMatY1, rtFstDeriv_y );
	GetRotMatrix( dbaRotMatZ1, rtFstDeriv_z );

	pdbaRotMatDer[0] = dbaRotMatX1;
	pdbaRotMatDer[1] = dbaRotMatY1;
	pdbaRotMatDer[2] = dbaRotMatZ1;

	memset(tpVarsOf2D3DAAM->tpflKeyPoint3D, 0, sizeof(float) * tpCandideMdl->m_iMSVertexNum * 3);   // S0 + Sigma(Si*a) + Sigma(Ai*b) 
	SynPersKeyPoint3D(tpVarsOf2D3DAAM->tpflKeyPoint3D);

	CalcuDerivativeSd_dqp();    // may not calculate actually if has calculated before in one iteration

	for ( j = 0; j < pLUTsOf2D3DAAM->m_iComVertexNum; j++ )       // canculate jacobian for each point
	{

		// 2D jacobian
		int     i2DIdx = pLUTsOf2D3DAAM->tpiComTo2DIdx[j];               //corresponding 2d aam point index

		int     ia2DIndices[2]      = {i2DIdx, -1};
		float   fla2DIntplCoef[2]   = {1.f, 0.f};

		bool    bNeedIntpl = false;
		for ( i = 0; i < pLUTsOf2D3DAAM->m_i2DNeedIntplNum; i++ )
		{
			if ( pLUTsOf2D3DAAM->tpi2DNeedIntplIdx[i] == j )
			{
				bNeedIntpl = true;
				ia2DIndices[1]      = pLUTsOf2D3DAAM->tpi2DIntplIdx[i];
				fla2DIntplCoef[0]   = pLUTsOf2D3DAAM->tpfl2DIntplCoef[i][0];
				fla2DIntplCoef[1]   = pLUTsOf2D3DAAM->tpfl2DIntplCoef[i][1];
				break;
			}
		}

		dba2DAAMPnt[0] = 0;
		dba2DAAMPnt[1] = 0;

		int     k, iPtsNum = ( bNeedIntpl ? 2 : 1 );
		float   flIntplCoef;
		for ( i = 0; i < iPtsNum; i++ )
		{
			k = ia2DIndices[i];
			flIntplCoef = fla2DIntplCoef[i];

			dba2DAAMPnt[0] += shapeCrd[k*2 + 0] * flIntplCoef;
			dba2DAAMPnt[1] += shapeCrd[k*2 + 1] * flIntplCoef;

			int m, n;
			for ( n = 0; n < 2; n++ ) // aS+ix/a(deltaq), aS+iy/a(deltaq)
			{
				for ( m = 0; m < iCurShapeNum; m++ )
				{
					tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + n][m] += flIntplCoef * tppflDerSd_dqp[k*2 + n][m];     // aPi/a(deltaq, deltaPm)
				}
			}
		}


		// 3D jacobian

		k = pLUTsOf2D3DAAM->tpiComTo3DIdx[j];              

		pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + k * 3;
		RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 

		dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
		dba3DAAMPnt[0] = tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[0] * dbInvPz + 0.5f * (InputImgW - 1) - rcROI.left;      // point projected onto image plane
		dba3DAAMPnt[1] = -tpVarsOf2D3DAAM->tfFocalLen * dbaPoint3DCam[1] * dbInvPz + 0.5f * (InputImgH - 1) - rcROI.top;      //because of using ROI


		dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
		dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
		dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)

		for ( i = 0; i < 3; i++ )
		{
			RotatePoint3D(pdbaRotMatDer[i], pdbPoint3DMdl, dba3DVect);   

			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]);    //∂‘x,y,z–˝◊™Ω«µƒ∆´µº
			tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
		}

		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 3] = -(dbTmp1);
		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 3] = 0;

		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 4] = 0;
		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 4] = -(-dbTmp1);

		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 0][iCurShapeNum + 5] = -(dbTmp2);
		tpVarsOf2D3DAAM->tppflMatSD3D[j*2 + 1][iCurShapeNum + 5] = -(dbTmp3);

		mat2DPtsErr.p[(j*2)*mat2DPtsErr.width+  0] = dba2DAAMPnt[0] - dba3DAAMPnt[0];
		mat2DPtsErr.p[(j*2+1)*mat2DPtsErr.width+0] = dba2DAAMPnt[1] - dba3DAAMPnt[1];

	}

	int iVtxNumStart = 0;              
	int iParaStart = iCurShapeNum + 6;
	for ( i = 0; i < tpCandideMdl->m_iSUNum && !bTracking; i++ )
	{
		int iVtxNum = tpCandideMdl->tpiEach_SUVertexNum[i];
		for ( j = 0; j < iVtxNum; j++ )
		{
			int i3DPtsIdx = tpCandideMdl->tpiEach_SUVertexIdx[iVtxNumStart + j];

			int iComIdx = pLUTsOf2D3DAAM->tpi3DToComIdx[i3DPtsIdx];
			if ( iComIdx == -1 ) 
				continue;


			pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + i3DPtsIdx * 3;
			RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 
			dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
			dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
			dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
			dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)

			float* pData = (float*)( tpCandideMdl->tpEach_SUVertexVal + (iVtxNumStart + j) );
			RotatePoint3D(dbaNormRotMat, pData, dba3DVect);

			tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 0][iParaStart + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]); 
			tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 1][iParaStart + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
		}
		iVtxNumStart += iVtxNum;
	}

	//AU
	iVtxNumStart = 0;
	iParaStart = iCurShapeNum + 6;
	for ( i = 0; i < tpCandideMdl->m_iAUNum && bTracking; i++ )
	{
		int iVtxNum = tpCandideMdl->tpiEach_AUVertexNum[i];
		for ( j = 0; j < iVtxNum; j++ )
		{
			int i3DPtsIdx = tpCandideMdl->tpiEach_AUVertexIdx[iVtxNumStart + j];

			int iComIdx = pLUTsOf2D3DAAM->tpi3DToComIdx[i3DPtsIdx];
			if ( iComIdx == -1 ) 
				continue;

			pdbPoint3DMdl = tpVarsOf2D3DAAM->tpflKeyPoint3D + i3DPtsIdx * 3;
			RotateTransPoint3D( dbaNormRotMat, dbaTranslate, pdbPoint3DMdl, dbaPoint3DCam); 
			dbInvPz = 1 / dbaPoint3DCam[2];                             // 1/Pz
			dbTmp1 =  tpVarsOf2D3DAAM->tfFocalLen * dbInvPz;                    //a(Pxi)/a(Px)  =  a(Pyi)/a(Py)
			dbTmp2 = -dbTmp1 * dbaPoint3DCam[0] * dbInvPz;      //a(Pxi)/a(Pz)
			dbTmp3 =  dbTmp1 * dbaPoint3DCam[1] * dbInvPz;      //a(Pyi)/a(Pz)


			float* pData = (float*)( tpCandideMdl->tpEach_AUVertexVal + (iVtxNumStart + j) );
			RotatePoint3D(dbaNormRotMat, pData, dba3DVect);

			tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 0][iParaStart + i] = -( dbTmp1 * dba3DVect[0] + dbTmp2 * dba3DVect[2]); 
			tpVarsOf2D3DAAM->tppflMatSD3D[iComIdx*2 + 1][iParaStart + i] = -(-dbTmp1 * dba3DVect[1] + dbTmp3 * dba3DVect[2]);
		}
		iVtxNumStart += iVtxNum;
	}

	matSD3D.CopyFromData1D(tpVarsOf2D3DAAM->tppflMatSD3D[0]);


	matSDTMulErr3D.SUPER_X(matSD3D, mat2DPtsErr);
	matHessian3D.SUPER_X(matSD3D, matSD3D);

	float   fl3DConstrK = m_AAMRunPara.tfl3DConstrK;

	matHessian3D = matHessian3D*fl3DConstrK;

	for ( j = 0; j < iCurShapeNum; j++ )          // combine 2D and 3D hessian
		for ( i = 0; i < iCurShapeNum; i++ )
			matHessian3D.p[j*matHessian3D.width+i] = matHessian3D.p[j*matHessian3D.width+i] + tpMatHessian2DFnl.p[j*tpMatHessian2DFnl.width+i];//cvmSet( &matHessian3D, j, i, cvmGet(&matHessian3D, j, i) + cvmGet(tpMatHessian2DFnl, j, i) );

	matSDTMulErr3D = matSDTMulErr3D*fl3DConstrK;

	for ( i = 0; i < iCurShapeNum; i++ )          // combine 2D and 3D SDTMulErr
	{
		matSDTMulErr3D.p[i*matSDTMulErr3D.width+0] = matSDTMulErr3D.p[i*matSDTMulErr3D.width+0] + tpMatSDTMulErr2DFnl.p[i*tpMatSDTMulErr2DFnl.width+0];
	}
}


// called outside
void CAAM::GetPosePara3DObject( double* pRot, double* pTrans )  
{
	float   dbaNormRotMat[9];
	int     i;

	CalcRotationTrigVal();          
	GetRotMatrix( dbaNormRotMat, rtNormRotMatrix  );

	for ( i = 0; i < 9; i++ )
		pRot[i] = dbaNormRotMat[i];

	for ( i = 0; i < 3; i++ )
		pTrans[i] = tpVarsOf2D3DAAM->m_TranslXYZ[i];

}

void CAAM::RotMat2AngleDegree( const double *pInRotMat, float *pOutRotAngle )
{
	const double* pRot = pInRotMat;
	pOutRotAngle[0] = float( -atan( pRot[5] / pRot[8]) * 180 / M_PI );
	pOutRotAngle[1] = float( asin(pRot[2]) * 180 / M_PI );
	pOutRotAngle[2] = float(-atan(pRot[1] / pRot[0]) * 180 / M_PI );
}


void CAAM::RotRad2Degree( float *pInRotAngle, float *pOutRotAngle )
{
	pOutRotAngle[0] = float( pInRotAngle[0] * 180 / M_PI );
	pOutRotAngle[1] = float( pInRotAngle[1] * 180 / M_PI );
	pOutRotAngle[2] = float( pInRotAngle[2] * 180 / M_PI );
}


