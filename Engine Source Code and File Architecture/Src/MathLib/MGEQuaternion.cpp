/*
 * File name : MGEQuaternion.cpp
 * Date : 2011.5.24
 * Author : Dormy.ET.C
 * Desc :

 */

#include "MGEQuaternion.h"

// CONSTRUCTOR
CMGEQuaternion::CMGEQuaternion(){
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->w = 0.0f;
}

CMGEQuaternion::CMGEQuaternion(float xAxis, float yAxis,
    float zAxis, float wAxis)
{
    this->x = xAxis;
    this->y = yAxis;
    this->z = zAxis;
    this->w = wAxis;
}

////////////////////// OPERATORS //////////////////////
CMGEQuaternion CMGEQuaternion::operator+(const CMGEQuaternion &q)
{
    CMGEQuaternion result;

    CMGEVector3 v1(this->x, this->y, this->z);
    CMGEVector3 v2(q.x, q.y, q.z);
    CMGEVector3 cross;
    CMGEVector3 v3;
    float newW = 0.0f;

    cross.CrossProduct3(v2, v1);

    v1 *= q.w;
    v2 *= this->w;

    v3 = v1 + v2;
    v3 = cross + v3;
    
    newW = this->w * q.w - (x * q.x + y * q.y + z * q.z);

    result.x = v3.x;
    result.y = v3.y;
    result.z = v3.z;
    result.w = newW;

    return result;
}

// EQUALS
void CMGEQuaternion::operator=(const CMGEQuaternion &q)
{
    this->x = q.x;
    this->y = q.y;
    this->z = q.z;
    this->w = q.w;
}

// MULTIPLY
CMGEQuaternion CMGEQuaternion::operator*(const CMGEQuaternion &q)
{
    return CMGEQuaternion(w * q.x + x * q.w + y * q.z - z * q.y,
                          w * q.y - x * q.z + y * q.w + z * q.x,
                          w * q.z + x * q.y - y * q.x + z * q.w,
                          w * q.w - x * q.x - y * q.y - z * q.z);
}

// CREATE QUATERNION
void CMGEQuaternion::CreateQuatFromAxis(CMGEVector3 &v, float radians)
{
    float sine = 0.0f;
    radians = radians * 0.5f;

    sine = (float)sin(radians);

    v.Normal();

    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    
    x *= sine;
    y *= sine;
    z *= sine;
    
    this->w = (float)cos(radians);
}

// GET LENGTH OF QUATERNION
float CMGEQuaternion::GetLength(void)
{
    return (float)::sqrt(this->x * this->x +
                         this->y * this->y +
                         this->z * this->z +
                         this->w * this->w);
}

// NORMALIZE QUATERNION
void CMGEQuaternion::Normal(void)
{
    float len = this->GetLength();
    len = 1/len;

    this->x *= len;
    this->y *= len;
    this->z *= len;
    this->w *= len;
}

// CONJUGATE
CMGEQuaternion CMGEQuaternion::Conjugate(void)
{
    return CMGEQuaternion( -this->x, -this->y, -this->z, this->w );
}

// CROSS PRODUCT
CMGEQuaternion CMGEQuaternion::CrossProduct(const CMGEQuaternion &q)
{
    CMGEQuaternion crossProduct;

    crossProduct.x = w * q.x + x * q.w + y * q.z - z * q.y;
    crossProduct.y = w * q.y + x * q.z + y * q.w - z * q.x;
    crossProduct.z = w * q.z + x * q.y + y * q.x - z * q.w;
    crossProduct.w = w * q.w - x * q.x - y * q.y - z * q.z;

    return crossProduct;
}

// ROTATE BY FLOAT VALUE
void CMGEQuaternion::Rotatef(float amount, float xAxis, float yAxis, float zAxis)
{
    if( (xAxis + yAxis + zAxis) != 1 )
    {
        float length = (float)::sqrt(xAxis * xAxis +
                                     yAxis * yAxis +
                                     zAxis * zAxis);

        xAxis /= length; yAxis /= length; zAxis /= length;
    }

    float angle = MGE_GET_RADIANS(amount);
    float sine  = (float)sin( angle / 2.0f );

    this->w = (float)cos( angle / 2.0f );
    this->x = xAxis * sine;
    this->y = yAxis * sine;
    this->z = zAxis * sine;
}

// ROTATTE BY RADIANS FLOAT VALUE
void CMGEQuaternion::RotateRadiansf(double X, double Y, double Z)
{
    double cosX, cosY, cosZ;
    double sinX, sinY, sinZ;
    double cosXY, sinXY;

    sinX = sin(X * 0.5);
    cosX = cos(X * 0.5);

    sinY = sin(Y * 0.5);
    cosY = cos(Y * 0.5);

    sinZ = sin(Z * 0.5);
    cosZ = cos(Z * 0.5);

    cosXY = cosX * cosY;
    sinXY = sinX * sinY;

    this->x = (float)(sinX * cosY * cosZ - cosX * sinY * sinZ);
    this->y = (float)(cosX * sinY * cosZ + sinX * cosY * sinZ);
    this->z = (float)(cosXY * sinZ - sinXY * cosZ);
    this->w = (float)(cosXY * cosZ + sinXY * sinZ); 
}

// CREATE 4X4 MATRIX
void CMGEQuaternion::CreateMatrix(float *pMatrix)
{
    if( !pMatrix ) return;

    pMatrix[0]  = 1.0f - 2.0f * (y * y + z * z); 
    pMatrix[1]  = 2.0f * (x * y + z * w);
    pMatrix[2]  = 2.0f * (x * z - y * w);
    pMatrix[3]  = 0.0f;  

    pMatrix[4]  = 2.0f * (x * y - z * w);  
    pMatrix[5]  = 1.0f - 2.0f * (x * x + z * z); 
    pMatrix[6]  = 2.0f * (z * y + x * w);  
    pMatrix[7]  = 0.0f;  

    pMatrix[8]  = 2.0f * (x * z + y * w);
    pMatrix[9]  = 2.0f * (y * z - x * w);
    pMatrix[10] = 1.0f - 2.0f * (x * x + y * y);  
    pMatrix[11] = 0.0f;  

    pMatrix[12] = 0;  
    pMatrix[13] = 0;  
    pMatrix[14] = 0;  
    pMatrix[15] = 1.0f; 
}

// SPHERE LERP
void CMGEQuaternion::SphereSlerp(const CMGEQuaternion &q1, const CMGEQuaternion &q2, float t)
{
    float cosTheta = 0.0f;
    float sinTheta = 0.0f;
    float beta = 0.0f;
    float q2Array[4];

    q2Array[0] = q2.x; q2Array[1] = q2.y;
    q2Array[2] = q2.z; q2Array[3] = q2.w;

    cosTheta = q1.x * q2.x + q1.y * q2.y +
        q1.z * q2.z + q1.w * q2.w;

    if(cosTheta < 0.0f)
    {
        q2Array[0] = -q2Array[0]; q2Array[1] = -q2Array[1];
        q2Array[2] = -q2Array[2]; q2Array[3] = -q2Array[3];
        cosTheta = -cosTheta;
    }

    beta = 1.0f - t;

    if(1.0f - cosTheta > 0.001f)
    {
        cosTheta = (float)acos(cosTheta);
        sinTheta = 1.0f / (float)sin(cosTheta);
        beta = (float)sin(cosTheta * beta) * sinTheta;
        t = (float)sin(cosTheta * t) * sinTheta;
    }

    this->x = beta * q1.x + t * q2Array[0];
    this->y = beta * q1.y + t * q2Array[1];
    this->z = beta * q1.z + t * q2Array[2];
    this->w = beta * q1.w + t * q2Array[3];
}
