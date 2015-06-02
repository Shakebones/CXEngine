/*
 * File name : MGEMatrix.cpp
 * Date : 2011.5.23
 * Author : Dormy.ET.C
 * Desc :

   The matrix 4X4 of math library.
 */

#include "MGEMatrix.h"
#include "MathDefine.h"

// Desc : Construtors.
CMGEMatrix4X4::CMGEMatrix4X4(const CMGEMatrix4X4 &m)
{
    this->matrix[0]  = m.matrix[0];  this->matrix[1] = m.matrix[1];
    this->matrix[2]  = m.matrix[2];  this->matrix[3] = m.matrix[3];
    this->matrix[4]  = m.matrix[4];  this->matrix[5] = m.matrix[5];
    this->matrix[6]  = m.matrix[6];  this->matrix[7] = m.matrix[7];
    this->matrix[8]  = m.matrix[8];  this->matrix[9] = m.matrix[9];
    this->matrix[10] = m.matrix[10]; this->matrix[11]= m.matrix[11];
    this->matrix[12] = m.matrix[12]; this->matrix[13]= m.matrix[13];
    this->matrix[14] = m.matrix[14]; this->matrix[15]= m.matrix[15];
}

// Desc : Constructor.
CMGEMatrix4X4::CMGEMatrix4X4(float e11, float e12, float e13, float e14,
                             float e21, float e22, float e23, float e24,
                             float e31, float e32, float e33, float e34,
                             float e41, float e42, float e43, float e44)
{
    matrix[0]  = e11; matrix[1]  = e12; matrix[2]  = e13; matrix[3]  = e14;
	matrix[4]  = e21; matrix[5]  = e22; matrix[6]  = e23; matrix[7]  = e24;
	matrix[8]  = e31; matrix[9]  = e32; matrix[10] = e33; matrix[11] = e34;
	matrix[12] = e41; matrix[13] = e42; matrix[14] = e43; matrix[15] = e44;
}

/*
 * Desc :
 * Identity matrix.
 */
void CMGEMatrix4X4::MGEMatrixIdentity(void)
{
    matrix[0]  = 1.0f; matrix[1]  = 0.0f; matrix[2]  = 0.0f; matrix[3]  = 0.0f;
    matrix[4]  = 0.0f; matrix[5]  = 1.0f; matrix[6]  = 0.0f; matrix[7]  = 0.0f;
    matrix[8]  = 0.0f; matrix[9]  = 0.0f; matrix[10] = 1.0f; matrix[11] = 0.0f;
    matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = 0.0f; matrix[15] = 1.0f;
}

////////////////////////////////// OVERLOAD OPERATOS //////////////////////////////////
CMGEMatrix4X4 CMGEMatrix4X4::operator+(CMGEMatrix4X4 &m)
{
    return CMGEMatrix4X4(
        matrix[0]  + m.matrix[0],  matrix[1]  + m.matrix[1],  matrix[2]  + m.matrix[2],
        matrix[3]  + m.matrix[3],  matrix[4]  + m.matrix[4],  matrix[5]  + m.matrix[5],
        matrix[6]  + m.matrix[6],  matrix[7]  + m.matrix[7],  matrix[8]  + m.matrix[8],
        matrix[9]  + m.matrix[9],  matrix[10] + m.matrix[10], matrix[11] + m.matrix[11],
        matrix[12] + m.matrix[12], matrix[13] + m.matrix[13],
        matrix[14] + m.matrix[14], matrix[15] + m.matrix[15]
    );
}

CMGEMatrix4X4 CMGEMatrix4X4::operator-(CMGEMatrix4X4 &m)
{
    return CMGEMatrix4X4(
        matrix[0]  - m.matrix[0],  matrix[1]  - m.matrix[1],  matrix[2]  - m.matrix[2],
        matrix[3]  - m.matrix[3],  matrix[4]  - m.matrix[4],  matrix[5]  - m.matrix[5],
        matrix[6]  - m.matrix[6],  matrix[7]  - m.matrix[7],  matrix[8]  - m.matrix[8],
        matrix[9]  - m.matrix[9],  matrix[10] - m.matrix[10], matrix[11] - m.matrix[11],
        matrix[12] - m.matrix[12], matrix[13] - m.matrix[13],
        matrix[14] - m.matrix[14], matrix[15] - m.matrix[15]);
}

// Desc : Mastrix the dot pruduct.
CMGEMatrix4X4 CMGEMatrix4X4::operator*(CMGEMatrix4X4 &m)
{
    return CMGEMatrix4X4(
        matrix[0] * m.matrix[0] + matrix[4]  * m.matrix[1] + 
        matrix[8] * m.matrix[2] + matrix[12] * m.matrix[3] ,

        matrix[1] * m.matrix[0] + matrix[5]  * m.matrix[1] + 
        matrix[9] * m.matrix[2] + matrix[13] * m.matrix[3] ,

        matrix[2]  * m.matrix[0] + matrix[6]  * m.matrix[1] + 
        matrix[10] * m.matrix[2] + matrix[14] * m.matrix[3] ,

        matrix[3]  * m.matrix[0] + matrix[7]  * m.matrix[1] + 
        matrix[11] * m.matrix[2] + matrix[15] * m.matrix[3] ,

        matrix[0] * m.matrix[4] + matrix[4]  * m.matrix[5] + 
        matrix[8] * m.matrix[6] + matrix[12] * m.matrix[7] ,

        matrix[1] * m.matrix[4] + matrix[5]  * m.matrix[5] + 
        matrix[9] * m.matrix[6] + matrix[13] * m.matrix[7] ,

        matrix[2]  * m.matrix[4] + matrix[6]  * m.matrix[5] + 
        matrix[10] * m.matrix[6] + matrix[14] * m.matrix[7] ,

        matrix[3]  * m.matrix[4] + matrix[7]  * m.matrix[5] + 
        matrix[11] * m.matrix[6] + matrix[15] * m.matrix[7] ,

        matrix[0] * m.matrix[8]  + matrix[4]  * m.matrix[9] + 
        matrix[8] * m.matrix[10] + matrix[12] * m.matrix[11],

        matrix[1] * m.matrix[8]  + matrix[5]  * m.matrix[9] + 
        matrix[9] * m.matrix[10] + matrix[13] * m.matrix[11],

        matrix[2]  * m.matrix[8]  + matrix[6]  * m.matrix[9] + 
        matrix[10] * m.matrix[10] + matrix[14] * m.matrix[11],

        matrix[3]  * m.matrix[8]  + matrix[7]  * m.matrix[9] + 
        matrix[11] * m.matrix[10] + matrix[15] * m.matrix[11],

        matrix[0] * m.matrix[12] + matrix[4]  * m.matrix[13] + 
        matrix[8] * m.matrix[14] + matrix[12] * m.matrix[15] ,

        matrix[1] * m.matrix[12] + matrix[5]  * m.matrix[13] + 
        matrix[9] * m.matrix[14] + matrix[13] * m.matrix[15] ,

        matrix[2]  * m.matrix[12] + matrix[6]  * m.matrix[13] + 
        matrix[10] * m.matrix[14] + matrix[14] * m.matrix[15] ,

        matrix[3]  * m.matrix[12] + matrix[7]  * m.matrix[13] + 
        matrix[11] * m.matrix[14] + matrix[15] * m.matrix[15]);
}

// Desc : Dot product with float value.
CMGEMatrix4X4 CMGEMatrix4X4::operator*(float f)
{
    return CMGEMatrix4X4(
        matrix[0]  * f, matrix[1]  * f, matrix[2]  * f, matrix[3]  * f,
        matrix[4]  * f, matrix[5]  * f, matrix[6]  * f, matrix[7]  * f,
        matrix[8]  * f, matrix[9]  * f, matrix[10] * f, matrix[11] * f,
        matrix[12] * f, matrix[13] * f, matrix[14] * f, matrix[15] * f);
}

// Desc : Divide with float value.
CMGEMatrix4X4 CMGEMatrix4X4::operator/(float f)
{
    if( f == 0.0f )
        f = 1.0f;

    f = 1/f;
    return CMGEMatrix4X4(
        matrix[0]  * f, matrix[1]  * f, matrix[2]  * f, matrix[3]  * f,
        matrix[4]  * f, matrix[5]  * f, matrix[6]  * f, matrix[7]  * f,
        matrix[8]  * f, matrix[9]  * f, matrix[10] * f, matrix[11] * f,
        matrix[12] * f, matrix[13] * f, matrix[14] * f, matrix[15] * f);
}

void CMGEMatrix4X4::operator+=(CMGEMatrix4X4 &m)
{
    (*this) = (*this) + m;
}

void CMGEMatrix4X4::operator-=(CMGEMatrix4X4 &m)
{
    (*this) = (*this) - m;
}

void CMGEMatrix4X4::operator*=(CMGEMatrix4X4 &m)
{
    (*this) = (*this) * m;
}

void CMGEMatrix4X4::operator*=(float f)
{
    (*this) = (*this) * f;
}

void CMGEMatrix4X4::operator/=(float f)
{
    (*this) = (*this) / f;
}

void CMGEMatrix4X4::MGEMatrixTranslate(CMGEVector3 &translateVec3)
{
    matrix[12] = translateVec3.x;
    matrix[13] = translateVec3.y;
    matrix[14] = translateVec3.z;
    matrix[15] = 1.0f;
}

void CMGEMatrix4X4::MGEMatrixTranslate(float x,float y, float z)
{
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
    matrix[15] = 1.0f;
}

// Desc : Inverse matrix 4x4
void CMGEMatrix4X4::MGEMatrixInverseTranslate(void)
{
    matrix[12] = -matrix[12];
    matrix[13] = -matrix[13];
    matrix[14] = -matrix[14];
}

// Desc : Rotation matrix
void CMGEMatrix4X4::MGEMatrixRotation(double angle, bool rotateAboutXAxis, 
    bool rotateAboutYAxis, bool rotateAboutZAxis)
{
    angle = angle * MGE_PI / 180;
    float cosAngle = cosf((float)angle);
    float sineAngle = sinf((float)angle);

    // IF USER ACTIVED Z AXIS.
    if( rotateAboutZAxis )
    {
        this->matrix[0] =  cosAngle;
        this->matrix[1] =  sineAngle;
        this->matrix[4] = -sineAngle;
        this->matrix[5] =  cosAngle;
    }

    // IF USER ACITIVED Y AXIS.
    if( rotateAboutYAxis )
    {
        this->matrix[0] =  cosAngle;
        this->matrix[2] = -sineAngle;
        this->matrix[8] =  sineAngle;
        this->matrix[10]=  cosAngle;
    }

    // IF USER ACTIVED X AXIS.
    if( rotateAboutXAxis )
    {
        this->matrix[5] = cosAngle;
        this->matrix[6] = sineAngle;
        this->matrix[9] = -sineAngle;
        this->matrix[10]=  cosAngle;
    }
}

// Desc : Multyply of matrix
CMGEVector3 CMGEMatrix4X4::MGEVec3MatrixMultiply(CMGEVector3 &v)
{
    CMGEVector3 out;

    out.x = (v.x * this->matrix[0]) + (v.y * this->matrix[4]) + 
        (v.z + this->matrix[8]) + this->matrix[12];

    out.y = (v.x * this->matrix[1]) + (v.y * this->matrix[5]) + 
        (v.z * this->matrix[9]) + this->matrix[13];

    out.z = (v.x * this->matrix[2]) + (v.y * this->matrix[6]) +
        (v.z * this->matrix[10]) + this->matrix[14];

    return out;
}

CMGEVector3 CMGEMatrix4X4::MGEVec3MatrixMultiply3X3(CMGEVector3 &v)
{
    CMGEVector3 out;

    out.x = (v.x * this->matrix[0]) + (v.y * this->matrix[4]) +
        (v.z * this->matrix[8]);

    out.y = (v.x * this->matrix[1]) + (v.y * this->matrix[5]) +
        (v.z * this->matrix[9]);

    out.z = (v.x * this->matrix[2]) + (v.y * this->matrix[6]) +
        (v.z * this->matrix[10]);

    return out;
}

float * CMGEMatrix4X4::GetValue(int i)
{
    if( i > 15 )
        return NULL;

    return &this->matrix[i];
}