//#include "stdafx.h"
#include "Matrix.h"
#include <iostream>
#include "stdio.h"
#include "memory.h"
#include "stdlib.h"
#include "math.h"
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
CMATRIX::CMATRIX(void)
{
	height = 1;
	width = 1;
	p=new float[height*width];
}

CMATRIX::CMATRIX(int n)
{		
	height=n;
	width=n;
	p=new float[height*width];
}

CMATRIX::CMATRIX(int _height,int _width)
{
	height=_height;
	width=_width;
	p=new float[height*width];
}

CMATRIX::CMATRIX(const CMATRIX& SRC)
{
	height=SRC.height;
	width=SRC.width;
	p=new float[height*width];
    memcpy( p,SRC.p, height*width*sizeof(float));
}

CMATRIX::~CMATRIX(void)
{
	delete []p;	
}

void CMATRIX::CopyFromData1D(float *pData)
{
    memcpy(p, pData, height*width*sizeof(float));
}

void CMATRIX::CopyToData1D(float *pData)
{
    memcpy(pData, p, height*width*sizeof(float));
}

void CMATRIX::CopyFromData2D(float **pData)
{
	for (int i=0; i<height; i++)	
		memcpy(((char*)p)+i*width*sizeof(float),pData[i],sizeof(float)*width);	
}

void CMATRIX::CopyToData2D(float **pData)
{
	for (int i=0; i<height; i++)	
		memcpy(pData[i],((char*)p)+i*width*sizeof(float),sizeof(float)*width);	
}

void CMATRIX::CopyFromMatrix(CMATRIX& A)
{
    memcpy( p,A.p, height*width*sizeof(float));
}

void CMATRIX::SetZero(void)
{
    memset(p, 0, sizeof(float)*width*height);
}

void CMATRIX::SetUnit(void)
{
	SetZero();

	for (int i=0; i<min(height, width) ;i++)
	{
		p[i+i*width] = 1;
	}
}

void CMATRIX::Resize(int n)
{
	if (height==n && width==n)
		return;
	delete []p;
	height=n;
	width=n;
	p=new float[height*width];
}

void CMATRIX::Resize(int _height,int _width)
{
	if (height==_height && width==_width)
		return;
	delete []p;
	height=_height;
	width=_width;
	p=new float[height*width];
}

void CMATRIX::SwapRow(int a,int b)
{
	int i;
	float temp;
    float *pa,*pb;
    pa = p+a*width;
    pb = p+b*width;
	for (i=0;i<width;i++)
	{
		temp=pa[i];
		pa[i]=pb[i];
		pb[i]=temp;
	}
}

void CMATRIX::SwapCol(int a,int b)
{
	int i;
	float temp;
    float *pa,*pb;
    pa = p+a;
    pb = p+b;
	for (i=0;i<height;i++)
	{
        temp = *(pa);
        *(pa) = *(pb);
        *(pb) = temp;
        pa+=width;
        pb+=width;
	}
}

void CMATRIX::PlusRow(int a,int b,float r)
{
	int i;
    float *pa,*pb;
    pa = p+a*width;
    pb = p+b*width;
	for (i=0;i<width;i++)
	{
		pb[i]+=pa[i]*r;
	}
}

void CMATRIX::PlusCol(int a,int b,float r)
{
	int i;
    float *pa,*pb;
    pa = p+a;
    pb = p+b;
	for (i=0;i<height;i++)
	{
        *(pb)=*(pa)*r;
        pa+=width;
        pb+=width;
	}
}

void CMATRIX::MultRow(int a,float r)
{
	int i;
    float *pa;
    pa = p+a*width;
	for (i=0;i<width;i++)
	{
		pa[i]*=r;
	}
}

void CMATRIX::MultCol(int a,float r)
{
	int i;
    float *pa;
    pa = p+a;
	for (i=0;i<height;i++)
	{
		*(pa)*=r;
        pa+=width;
	}
}

CMATRIX& CMATRIX::operator =(const CMATRIX& A)
{
	Resize(A.height,A.width);
	memcpy(p, A.p, height*width*sizeof(float));
	return *this;
}

CMATRIX operator ~(CMATRIX& A)
{
	int i,j;
    int _width = A.height;
    int _height= A.width;
	CMATRIX temp(_height,_width);
    float *pa,*pb;
	for (i=0;i<_height;i++)
	{
        pa = temp.p+i*_width;
        pb = A.p+i;
		for (j=0;j<_width;j++)
		{
            *(pa++) = *(pb);
            pb+=_height;
		}
	}
	return temp;
}

CMATRIX operator +(CMATRIX& A,CMATRIX& B)
{
	int i,j;
	CMATRIX temp(A.height,A.width);

	for (i=0;i<temp.height;i++)
	{
		for (j=0;j<temp.width;j++)
		{
			temp.p[i*temp.width+j]=A.p[i*A.width+j]+B.p[i*B.width+j];
		}
	}
	return temp;
}

CMATRIX operator -(CMATRIX& A,CMATRIX& B)
{
	int i,j;
	CMATRIX temp(A.height,A.width);

	for (i=0;i<temp.height;i++)
	{
		for (j=0;j<temp.width;j++)
		{
			temp.p[i*temp.width+j]=A.p[i*A.width+j]-B.p[i*B.width+j];
		}
	}
	return temp;
}

CMATRIX operator *(CMATRIX& A,float r)
{
	int i,j;
	CMATRIX temp(A.height,A.width);

	for (i=0;i<temp.height;i++)
	{
		for (j=0;j<temp.width;j++)
		{
			temp.p[i*temp.width+j]=r*A.p[i*A.width+j];
		}
	}
	return temp;
}

CMATRIX operator *(float r,CMATRIX& A)
{
	int i,j;
	CMATRIX temp(A.height,A.width);

	for (i=0;i<temp.height;i++)
	{
		for (j=0;j<temp.width;j++)
		{
			temp.p[i*temp.width+j]=r*A.p[i*A.width+j];
		}
	}
	return temp;
}

CMATRIX operator *(CMATRIX& A,CMATRIX& B)
{
    CMATRIX temp(A.height,B.width);
    float *outp = temp.p;
    float *inxp = B.p;
    int _width = A.width;
    int _xwidth = B.width;
    float *in_p = A.p-_width;
    
    int total = temp.height*temp.width;
    for (int i=0;i<total;i++)
	{
        float tmp=0;
        float *in_xp = inxp+i%temp.width;
        if(in_xp==inxp) in_p += _width;
        for (float *in_p2=in_p;in_p2<in_p+_width;in_p2++,in_xp+=_xwidth)
        {
            tmp+=(*in_p2)*(*in_xp);
        }
        *(outp++)=tmp;
	}
    return temp;
}

void CMATRIX::SUPER_X(const CMATRIX& in_,const CMATRIX& in_x)
{
    
    Resize(in_.width,in_x.width);
    int _height = in_.height;
    float *outp = p;
    for (int i=0;i<height;i++)
	{
        
		for (int j=0;j<width;j++)
		{
            float tmp=0;
            float *in_xp = in_x.p+j;
            float *in_p = in_.p+i;
			for (int k=0;k<_height;k++)
			{
                tmp+=(*in_p)*(*in_xp);
                in_xp+=in_x.width;
                in_p+=in_.width;
			}
            *(outp++)=tmp;
        }
    }        
}

void CMATRIX::SUPERMUL(const CMATRIX& in_,const CMATRIX& in_x)
{
    Resize(in_.height,in_x.width);

    float *outp = p;
    float *inxp = in_x.p;
    int _width = in_.width;
    int _xwidth = in_x.width;
    float *in_p = in_.p-_width;
    
    int total = height*width;
    for (int i=0;i<total;i++)
	{
        float tmp=0;
        float *in_xp = inxp+i%width;
        if(in_xp==inxp) in_p += _width;
        for (float *in_p2=in_p;in_p2<in_p+_width;in_p2++,in_xp+=_xwidth)
        {
            tmp+=(*in_p2)*(*in_xp);
        }
        *(outp++)=tmp;
	}
}

void CMATRIX::SUPERSUB_(const CMATRIX& in)
{
    int total = height * width;
    float *inp = in.p;
    for(int i=0;i<total;i++)
    {
        p[i]=inp[i]-p[i];
    }
    
}

void CMATRIX::SUPERMULF(float r)
{
    int total = height * width;
    for(int i=0;i<total;i++)
    {
        p[i]*=r;
    }

}

void CMATRIX::SUPERADD(const CMATRIX& in)
{
    int total = height * width;
    float *inp = in.p;
    for(int i=0;i<total;i++)
    {
        p[i]+=inp[i];
    }
}

CMATRIX operator !(CMATRIX& A)
{
	int i,j;
	float ration;
	CMATRIX temp(A.height),tempA;

	temp.SetUnit();
	tempA=A;
	for (i=0;i<temp.height;i++)
	{
		if (tempA.p[i*tempA.width+i]==0)
		{
			for (j=i+1;j<temp.height;j++)
			{
				if (tempA.p[j*tempA.width+i]!=0)
				{
					tempA.SwapRow(i,j);
					temp.SwapRow(i,j);
					break;
				}
			}
		}
		ration=1/tempA.p[i*tempA.width+i];
		tempA.MultRow(i,ration);
		temp.MultRow(i,ration);
		for (j=i+1;j<temp.height;j++)
		{
			ration=-tempA.p[j*tempA.width+i];
			tempA.PlusRow(i,j,ration);
			temp.PlusRow(i,j,ration);
		}
	}
	for (i=0;i<temp.height;i++)
	{
		for (j=0;j<i;j++)
		{
			ration=-tempA.p[j*tempA.width+i];
			tempA.PlusRow(i,j,ration);
			temp.PlusRow(i,j,ration);
		}
	}
	return temp;
}

void CMATRIX::QRResolve(CMATRIX& Q,CMATRIX& R)
{
	CMATRIX P;
	int i,j;
	float g,a,b,v,c,s;

	Q.Resize(height);
	Q.SetUnit();
	P.Resize(height);
	R=*this;
	for (i=0;i<R.height;i++)
	{
		for (j=i+1;j<R.height;j++)
		{
			P.SetUnit();
			g=max(fabs(R.p[i*R.width+i]),fabs(R.p[j*R.width+i]));
			if (g==0) break;
			a=R.p[i*R.width+i]/g;
			b=R.p[j*R.width+i]/g;
			v=sqrt(a*a+b*b);
			c=a/v;
			s=b/v;
			P.p[i*R.width+i]=c;
			P.p[j*R.width+j]=c;
			P.p[j*R.width+i]=-s;
			P.p[i*R.width+j]=s;
			R=P*R;
            P=~P;
			Q=Q*P;
		}
	}
}

float CMATRIX::MaxBelowDiag(void)
{
	int i,j;
	float maxbelowdiag;

	maxbelowdiag=0;
	for (i=0;i<height;i++)
	{
		for (j=0;j<i;j++)
		{
			if (fabs(p[i*width+j])>maxbelowdiag)
			{
				maxbelowdiag=fabs(p[i*width+j]);
			}
		}
	}

	return(maxbelowdiag);
}

void CMATRIX::Rectify(void)
{
	int i;
    int total = height*width;
	for (i=0;i<total;i++)
	{
        if (fabs(p[i])<MINIMUM) p[i]=0;
	}
}

void CMATRIX::SVDResolve(CMATRIX& U,CMATRIX& E,CMATRIX& V)

{
	CMATRIX A,Q,R,B,ATA,AAT,TMPM,TMPM2;

	A=*this;
    TMPM=~A;
	ATA=TMPM*A;
	AAT=A*TMPM;
	U.Resize(A.height);
	U.SetUnit();
	V.Resize(A.width);
	V.SetUnit();
	while (ATA.MaxBelowDiag()>MINIMUM)
	{
		ATA.QRResolve(Q,R);
		ATA=R*Q;
		V=V*Q;
	}
	while (AAT.MaxBelowDiag()>MINIMUM)
	{
		AAT.QRResolve(Q,R);
		AAT=R*Q;
		U=U*Q;
	}
	U.Rectify();
	V.Rectify();
    TMPM = ~U;
    TMPM2 = TMPM*A;
	E=TMPM2*V;
	E.Rectify();
	int n = min(A.height, A.width);
	int i;
	for (i = 0 ; i < n ; i++)
	{
    		if(E.p[i*E.width+i] < 0)   
    		{
        		E.p[i*E.width+i] = -E.p[i*E.width+i];    
        		U.MultCol(i, -1);          
    		}
	}
}

void CMATRIX::SetUnit(int n)
{
	Resize(n);
	SetUnit();
}
