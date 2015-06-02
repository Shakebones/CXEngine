/*
 * File name : MGEQuaternion.h
 * Date : 2011.5.24
 * Author : Dormy.ET.C
 * Desc :

   Quaternion of math library.
 */

#include <math.h>
#include "MGEVector.h"
#include "MathDefine.h"

/*
 * Class name : CMGEQuaternion.
 * Date : 2011.5.24.
 * Author : Dormy.ET.C
 * Desc :

   Quternion deal with rotation matrix is
   faster than rotation matrix To some extent.

   Quaternion of math library.
 */
class CMGEQuaternion
{
public:
    CMGEQuaternion();
    CMGEQuaternion(float xAxis, float yAxis,
                   float zAxis, float wAxis);

    void operator=(const CMGEQuaternion &q);
    CMGEQuaternion operator*(const CMGEQuaternion &q);
    CMGEQuaternion operator+(const CMGEQuaternion &q);

    void CreateQuatFromAxis(CMGEVector3 &v, float radians);
    
    float GetLength(void);
    void Normal(void);

    CMGEQuaternion Conjugate(void);
    CMGEQuaternion CrossProduct(const CMGEQuaternion &q);

    // ROTATION (FASTER THAN MATRIX)
    void Rotatef(float amout, float xAxis,
        float yAxis, float zAxis);

    // ACCORDING TO RADIANS TO ROTATE.(FASTER THAN MATRIX) 
    void RotateRadiansf(double X, double Y, double Z);
    
    // ACCORDING TO QUATERNION TO CREATE MATRIX.
    void CreateMatrix(float *pMatrix);

    // SPHERE LERP.
    void SphereSlerp(const CMGEQuaternion &q1, const CMGEQuaternion &q2,
        float t);

    // Desc : 4 float values.
    float x;
    float y;
    float z;
    float w;
};