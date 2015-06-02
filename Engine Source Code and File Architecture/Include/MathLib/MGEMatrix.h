/*
 * File name : MGEMatrix.h
 * Date : 2011.5.23
 * Author : Dormy.ET.C
 * Desc :

   The matrix 4X4 of math library.
 */

#ifndef _MGEMATRIX_H_
#define _MGEMATRIX_H_

#include "MGEVector.h"
#include "MathLibrary.h"

/*
 * Class name : CMGEMatrix4X4
 * Date : 2011.5.24
 * Author : Dormy.ET.C
 */
class CMGEMatrix4X4
{
public:
    // Desc : Constructor list.
    CMGEMatrix4X4(){ this->MGEMatrixIdentity(); }
    CMGEMatrix4X4( const CMGEMatrix4X4 &m );

    // Desc : e[row,col] means elements11, elements12...
    CMGEMatrix4X4( float e11, float e12, float e13, float e14,
                   float e21, float e22, float e23, float e24,
                   float e31, float e32, float e33, float e34,
                   float e41, float e42, float e43, float e44);
    // Desc : Destructors.
    ~CMGEMatrix4X4(){}

    // Desc : Identity matrix.
    void MGEMatrixIdentity(void);

    // Desc : Overload about matrix operators.
    CMGEMatrix4X4 operator+(CMGEMatrix4X4 &m);
    CMGEMatrix4X4 operator-(CMGEMatrix4X4 &m);
    CMGEMatrix4X4 operator*(CMGEMatrix4X4 &m);

    // Desc : Overload about matries operate with float values.
    CMGEMatrix4X4 operator*(float f);
    CMGEMatrix4X4 operator/(float f);

    // Desc : The other overload operators.
    void operator+=(CMGEMatrix4X4 &m);
    void operator-=(CMGEMatrix4X4 &m);
    void operator*=(CMGEMatrix4X4 &m);

    void operator*=(float f);
    void operator/=(float f);

    // Desc : Overload [] operator.
    float * GetValue(int i);

    // Desc : Trnaslate matrix with 3D vector.
    void MGEMatrixTranslate(CMGEVector3 &translateVec3);
    void MGEMatrixTranslate(float x, float y, float z);
    
    void MGEMatrixInverseTranslate(void);

    void MGEMatrixRotation(double angle, bool rotateAboutXAxis, bool rotateAboutYAxis, 
        bool rotateAboutZAxis);
    
    CMGEVector3 MGEVec3MatrixMultiply(CMGEVector3 &v);
    CMGEVector3 MGEVec3MatrixMultiply3X3(CMGEVector3 &v);

    float matrix[16];
};


#endif