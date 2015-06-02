/*
 * File name : MGEVector.h
 * Date : 2011.5.23
 * Author : Dormy.ET.C
 */
#ifndef _MGEVECTOR_H_
#define _MGEVECTOR_H_

#include <math.h>

/*
 * Desc : Class vector3 ( with 3 float values )
 */
class CMGEVector3
{
public:
    CMGEVector3();
    CMGEVector3(float x, float y, float z);
    CMGEVector3(const CMGEVector3 &c);

    // Desc : Operators
    void operator=(const CMGEVector3 &v);
    CMGEVector3 operator+(CMGEVector3 &v);
    CMGEVector3 operator-(CMGEVector3 &v);
    CMGEVector3 operator*(CMGEVector3 &v);
    CMGEVector3 operator/(CMGEVector3 &v);

    CMGEVector3 operator+(float f);
    CMGEVector3 operator-(float f);
    CMGEVector3 operator*(float f);
    CMGEVector3 operator/(float f);

    void operator+=( CMGEVector3 &v );
    void operator-=( CMGEVector3 &v );
    void operator*=( CMGEVector3 &v );
    void operator/=( CMGEVector3 &v );
    bool operator==( CMGEVector3 &v );

    void operator+=(float f);
    void operator-=(float f);
    void operator*=(float f);
    void operator/=(float f);

    // Desc : Vector cross product.
    void CrossProduct3(CMGEVector3 &v1, CMGEVector3 &v2);
    float DotProduct3(CMGEVector3 &v);
    float GetLength(void);
    void  Normal(void);
    void  Normalize( CMGEVector3 &v1, CMGEVector3 &v2, CMGEVector3 &v3 );

    float x;
    float y;
    float z;
};

/*
 * Desc :
 * Vector with 4 float values.
 */
class CMGEVector4
{
public:
    CMGEVector4();
    CMGEVector4(float x, float y, float z, float a);

    // Desc : Operators
    void operator=(CMGEVector4 &v);
    CMGEVector4 operator+(CMGEVector4 &v);
    CMGEVector4 operator-(CMGEVector4 &v);
    CMGEVector4 operator*(CMGEVector4 &v);
    CMGEVector4 operator/(CMGEVector4 &v);

    CMGEVector4 operator+(float f);
    CMGEVector4 operator-(float f);
    CMGEVector4 operator*(float f);
    CMGEVector4 operator/(float f);

    void operator+=( CMGEVector4 &v );
    void operator-=( CMGEVector4 &v );
    void operator*=( CMGEVector4 &v );
    void operator/=( CMGEVector4 &v );

    void operator+=(float f);
    void operator-=(float f);
    void operator*=(float f);
    void operator/=(float f);

    // Desc : Vector cross product.
    // Desc : Not sure 4 float values do cross product.
    // void  CrossProduct4(CMGEVector4 &v1, CMGEVector4 &v2);
    float DotProduct4(CMGEVector4 &v);
    float GetLength(void);
    void  Normal(void);
    void  Normalize( CMGEVector4 &v1, CMGEVector4 &v2, CMGEVector4 &v3, CMGEVector4 &v4 );

    float x;
    float y;
    float z;
    float a;
};
#endif