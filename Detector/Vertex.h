#include <math.h>  
#ifndef _UTILITY_H
#define _UTILITY_H
#ifndef DBL_EPSILON
#define DBL_EPSILON      2.2204460492503131e-016
#endif
     
#ifndef _BASICGEOMETRY_DEFINE
#define _BASICGEOMETRY_DEFINE

//-----------------------------------------------------------------------------------------
// 2D world point definition
typedef struct tagVERTEX2D
{
    float x;
    float y;
}VERTEX2D, *PVERTEX2D, *LPVERTEX2D;

class CVertex2D : public tagVERTEX2D
{
public:
    // Constructors
    CVertex2D() {}
    CVertex2D(const CVertex2D& s) 
    {
        x = s.x;
        y = s.y;
    }
    CVertex2D(const float sx, const float sy) 
    {
        x = sx;
        y = sy;
    }

    bool operator == (const CVertex2D& s) const 
    {
        return (x == s.x) && (y == s.y);
    }

    bool operator != (const CVertex2D& s) const 
    {
        return (x != s.x) || (y != s.y);
    }

    void operator = (const CVertex2D& st) 
    {
        x = st.x;
        y = st.y;
    }

    void operator += (const CVertex2D& st)     // Coornidate-wise +=
    {
        x += st.x;
        y += st.y;
    }

    void operator -= (const CVertex2D& st)     // Coornidate-wise -=
    {
        x -= st.x;
        y -= st.y;
    }

    void operator *= (const float s)     // Coornidate-wise *=
    {
        x *= s;
        y *= s;
    }

    void operator /= (const float s)     // Coornidate-wise /=
    {
        x /= s;
        y /= s;
    }

	CVertex2D operator + (const CVertex2D& st) const
	{
		return CVertex2D(x + st.x, y + st.y);
	}

	CVertex2D operator - (const CVertex2D& st) const
	{
		return CVertex2D(x - st.x, y - st.y);
	}

	CVertex2D operator * (const float s) const
	{
		return CVertex2D(x * s, y * s);
	}

	CVertex2D operator / (const float s) const
	{
		return CVertex2D(x / s, y / s);
	}

	double Distance(const CVertex2D& st) const
	{
		return (sqrt((x-st.x)*(x-st.x) + (y-st.y)*(y-st.y)));
	}

	double SqrDistance(const CVertex2D& st) const
	{
		return ((x-st.x)*(x-st.x) + (y-st.y)*(y-st.y));
	}
};

//-----------------------------------------------------------------------------------------
// 3D world point definition
typedef struct tagVERTEX3D
{
    float        x;
    float        y;
    float        z;
}VERTEX3D, *PVERTEX3D, *LPVERTEX3D;

class CVertex3D : public tagVERTEX3D
{
public:  
    // Constructors
    CVertex3D() {}
    CVertex3D(const CVertex3D& s) 
    {
        x = s.x;
        y = s.y;
        z = s.z;
    }

    CVertex3D(const float sx, const float sy, const float sz) 
    {
        x = sx;
        y = sy;
        z = sz;
    }

    // Operators
    bool operator == (const CVertex3D& s) const 
    {
        return (x == s.x) && (y == s.y) && (z == s.z);
    }

    bool operator != (const CVertex3D& s) const 
    {
        return (x != s.x) || (y != s.y) || (z != s.z);
    }

    void operator = (const CVertex3D& st) 
    {
        x = st.x;
        y = st.y;
        z = st.z;
    }

    void operator += (const CVertex3D& st)     // Coornidate-wise +=
    {
        x += st.x;
        y += st.y;
        z += st.z;
    }

    void operator -= (const CVertex3D& st)     // Coornidate-wise -=
    {
        x -= st.x;
        y -= st.y;
        z -= st.z;
    }

    void operator *= (const float s)     // Coornidate-wise *=
    {
        x *= s;
        y *= s;
        z *= s;
    }

    void operator /= (const float s)     // Coornidate-wise /=
    {
        x /= s;
        y /= s;
        z /= s;
    }

    CVertex3D operator + (const CVertex3D& st) const    // Coornidate-wise +
    {
        return CVertex3D(x + st.x, y + st.y, z + st.z);
    }

    CVertex3D operator - (const CVertex3D& st) const    // Coornidate-wise -
    {
        return CVertex3D(x - st.x, y - st.y, z - st.z);
    }

    CVertex3D operator * (const float s) const    // Coornidate-wise *
    {
        return CVertex3D(x * s, y * s, z * s);
    }

    CVertex3D operator / (const float s) const    // Coornidate-wise /
    {
        return CVertex3D(x / s, y / s, z / s);
    }

    // Distance Functions
    double    Distance(const CVertex3D& st) const
    {
        return (sqrt((x-st.x)*(x-st.x) + (y-st.y)*(y-st.y) + (z-st.z)*(z-st.z)));
    }

    double    SqrDistance(const CVertex3D& st) const
    {
        return ((x-st.x)*(x-st.x) + (y-st.y)*(y-st.y) + (z-st.z)*(z-st.z));
    }
};

//-----------------------------------------------------------------------------------------
// Triangular mesh structure definition
typedef struct tagTRIMESH
{
    int            pt1;        // index of the first vertex (in vertices set)
    int            pt2;        // index of the second vertex
    int            pt3;        // index of the third vertex
    int            color;        // attribute of the mesh
}TRIMESH, *PTRIMESH, *LPTRIMESH;

class CTriMesh : public tagTRIMESH
{
public:
    // Constructors
    CTriMesh() {}
    CTriMesh(const CTriMesh& s) {pt1 = s.pt1; pt2 = s.pt2; pt3 = s.pt3; color = s.color;}

    bool operator == (const CTriMesh& s) const 
    {
        return (pt1 == s.pt1) && (pt2 == s.pt2) && (pt3 == s.pt3) && (color == s.color);
    }

    bool operator != (const CTriMesh& s) const 
    {
        return (pt1 != s.pt1) || (pt2 != s.pt2) || (pt3 != s.pt3) || (color != s.color);
    }
};

#endif // _BASICGEOMETRY_DEFINE

// wireframe model definition
typedef struct tagWIREFRAMEMODEL
{
    CVertex3D   *pVertex;
    CTriMesh    *pMesh;
    int          nVertex;
    int          nMesh;
}WIREFRAMEMODEL, *PWIREFRAMEMODEL, *LPWIREFRAMEMODEL;

typedef struct tagPOSEPARAM3DOBJECT
{
    double    rot[9];
    double    trans[3];    
}POSEPARAM3DOBJECT, *PPOSEPARAM3DOBJECT, *LPOSEPARAM3DOBJECT;

void Line(
        const float x1,
        const float y1,
        const float x2,
        const float y2,
        float &a,
        float &b,
        float &c);
#endif