#pragma once
#define SAFE_DELETE(ptrMem)	         if(ptrMem){ ::delete ptrMem; ptrMem = NULL;}
#define SAFE_ARRAY_DELETE(ptrMem)	 if(ptrMem){ ::delete[] ptrMem; ptrMem = NULL;}

#define HUE_LUT_SIZE                181
#define FRONTAL_MDL_IDX              1 
#define ALLOWED_LOST_FRAME_CNT      1
#define CMOS_PIXEL_SIZE        5.6f 
#define MIR_K                       66 
#define ZOOM_3D_MODEL_PARA     100 
#define LUTS_2D3DAAM_NUM         3
#define AAM_MODEL_NUM   3
#define MAINDIR_MIN_MOTION_THR      8 
#define FRONTAL_INIT_NUM            3
#define MaxAdjacentTrianglesNum     20
#define MIR_J                       62
#define NO_TRACKFEATURE             80 
#define FOCAL_LENGTH   -474.074f
#ifndef ImageProcessing_WindowsHeader_h
#define ImageProcessing_WindowsHeader_h
#ifndef IMAGE_PROCESS_NULLPOINTER
#define IMAGE_PROCESS_NULLPOINTER    0x00000041
#endif
#ifndef IMAGE_PROCESS_OK
#define IMAGE_PROCESS_OK             0x00000001
#endif

#ifndef MAX
#define MAX(a,b) (a)>(b)?(a):(b)
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
//
//#ifndef abs
//#define abs(x) ((x)>(0)?(x):(-x))
//#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef _WINDOWS
   #ifndef FACEDETECT_MAXZOOMLEVEL
	 #define  FACEDETECT_MAXZOOMLEVEL                (70)        //��������������Ŵ�Ĳ���
   #endif
   #ifndef FACEAAM_MAXITERATECOUNT
	 #define  FACEAAM_MAXITERATECOUNT                (20)        //������������
   #endif
#else
   #ifndef FACEDETECT_MAXZOOMLEVEL
	 #define  FACEDETECT_MAXZOOMLEVEL                (70)        //��������������Ŵ�Ĳ���
   #endif
   #ifndef FACEAAM_MAXITERATECOUNT
     #define  FACEAAM_MAXITERATECOUNT                (20)         //������������
   #endif

#endif

#ifndef _WINDOWS
#define MAX_PATH          260




typedef long HRESULT;
typedef long LONG;

typedef unsigned int        UINT;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#define far
typedef void far            *LPVOID;

typedef void *HANDLE;
#define TRUE		1
#define FALSE		0
#ifndef MIN
#define MIN(a,b)	(((a)<(b))?(a):(b))	
#endif
	/* find maximum of a and b */
#ifndef MAX
#define MAX(a,b)	(((a)>(b))?(a):(b))	
#endif
typedef unsigned char BYTE;
#ifndef __CPLUSPLUS_CLASS_H__
#define __CPLUSPLUS_CLASS_H__ 
#endif
#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)
#define ERROR_INVALID_BLOCK              9L
#define ERROR_NO_SYSTEM_RESOURCES        1450L
#define ERROR_FILE_NOT_FOUND             2L
#define ERROR_PATH_NOT_FOUND             3L


typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Spare[ 2 ];
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

#define RTL_CRITSECT_TYPE 0
#define RTL_RESOURCE_TYPE 1

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;
typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef assert
#define assert(_Expression)     ((void)0)
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
typedef struct tagRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} RECT;

typedef struct tagPOINT
{
    long  x;
    long  y;
} POINT;

#else
#include "windows.h"
#endif

void *LoadDataFromResource_FD(const char *FileName,int Pos);
unsigned char* down_scale_image(unsigned char* gray, int width, int height);
//void *LoadDataFromResource_FD(WORD resourseID);

class W_Size
{
public:
    W_Size()
    {
        Width = Height = 0;
    }

    W_Size(const W_Size& size)
    {
        Width = size.Width;
        Height = size.Height;
    }

    W_Size(int width,
         int height)
    {
        Width = width;
        Height = height;
    }

    W_Size operator+(const W_Size& sz) const
    {
        return W_Size(Width + sz.Width,
                    Height + sz.Height);
    }

    W_Size operator-(const W_Size& sz) const
    {
        return W_Size(Width - sz.Width,
                    Height - sz.Height);
    }

    bool Equals(const W_Size& sz) const
    {
        return (Width == sz.Width) && (Height == sz.Height);
    }

    bool Empty() const
    {
        return (Width == 0 && Height == 0);
    }

public:

    int Width;
    int Height;
};

class W_Point
{
public:
   W_Point()
   {
       X = Y = 0;
   }

   W_Point(const W_Point &point)
   {
       X = point.X;
       Y = point.Y;
   }

   W_Point(const W_Size &size)
   {
       X = size.Width;
       Y = size.Height;
   }

   W_Point(int x,
         int y)
   {
       X = x;
       Y = y;
   }

   W_Point operator+(const W_Point& point) const
   {
       return W_Point(X + point.X,
                    Y + point.Y);
   }

   W_Point operator-(const W_Point& point) const
   {
       return W_Point(X - point.X,
                    Y - point.Y);
   }

   bool Equals(const W_Point& point)
   {
       return (X == point.X) && (Y == point.Y);
   }

public:

    int X;
    int Y;
};

class W_Rect
{
public:

    W_Rect()
    {
        X = Y = Width = Height = 0;
    }

    W_Rect(int x,
         int y,
         int width,
         int height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    W_Rect(const W_Point& location,
         const W_Size& size)
    {
        X = location.X;
        Y = location.Y;
        Width = size.Width;
        Height = size.Height;
    }

    W_Rect* Clone() const
    {
        return new W_Rect(X, Y, Width, Height);
    }

    void GetLocation(W_Point* point) const
    {
        point->X = X;
        point->Y = Y;
    }

    void GetSize(W_Size* size) const
    {
        size->Width = Width;
        size->Height = Height;
    }

    void GetBounds(W_Rect* rect) const
    {
        rect->X = X;
        rect->Y = Y;
        rect->Width = Width;
        rect->Height = Height;
    }

    int GetLeft() const
    {
        return X;
    }

    int GetTop() const
    {
        return Y;
    }

    int GetRight() const
    {
        return X+Width;
    }

    int GetBottom() const
    {
        return Y+Height;
    }

    bool IsEmptyArea() const
    {
        return (Width <= 0) || (Height <= 0);
    }

    bool Equals(const W_Rect & rect) const
    {
        return X == rect.X &&
               Y == rect.Y &&
               Width == rect.Width &&
               Height == rect.Height;
    }

    bool Contains(int x,
                  int y) const
    {
        return x >= X && x < X+Width &&
               y >= Y && y < Y+Height;
    }

    bool Contains(const W_Point& pt) const
    {
        return Contains(pt.X, pt.Y);
    }

    bool Contains(W_Rect& rect) const
    {
        return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
               (Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
    }

    void Inflate(int dx,
                 int dy)
    {
        X -= dx;
        Y -= dy;
        Width += 2*dx;
        Height += 2*dy;
    }

    void Inflate(const W_Point& point)
    {
        Inflate(point.X, point.Y);
    }

    bool Intersect(const W_Rect& rect)
    {
        return Intersect(*this, *this, rect);
    }

    static bool Intersect(W_Rect& c,
                          const W_Rect& a,
                          const W_Rect& b)
    {
        int right = min(a.GetRight(), b.GetRight());
        int bottom = min(a.GetBottom(), b.GetBottom());
        int left = max(a.GetLeft(), b.GetLeft());
        int top = max(a.GetTop(), b.GetTop());

        c.X = left;
        c.Y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    bool IntersectsWith(const W_Rect& rect) const
    {
        return (GetLeft() < rect.GetRight() &&
                GetTop() < rect.GetBottom() &&
                GetRight() > rect.GetLeft() &&
                GetBottom() > rect.GetTop());
    }

    static bool Union(W_Rect& c,
                      const W_Rect& a,
                       const W_Rect& b)
    {
        int right = max(a.GetRight(), b.GetRight());
        int bottom = max(a.GetBottom(), b.GetBottom());
        int left = min(a.GetLeft(), b.GetLeft());
        int top = min(a.GetTop(), b.GetTop());

        c.X = left;
        c.Y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    void Offset(const W_Point& point)
    {
        Offset(point.X, point.Y);
    }

    void Offset(int dx,
                int dy)
    {
        X += dx;
        Y += dy;
    }

public:

    int X;
    int Y;
    int Width;
    int Height;
};


#endif
