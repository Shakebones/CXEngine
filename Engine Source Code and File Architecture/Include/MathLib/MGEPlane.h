/*
 * File name : MGEPlane.h
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

   This file declar plane
 */

#ifndef _MGEPLANE_H_
#define _MGEPLANE_H_

#include "MathLibrary.h"

/*
 * Claa name : CMGEPlane
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :
 
 */
class CMGEPlane
{
public:
    // CONSTRUCTOR LIST
    CMGEPlane();
    CMGEPlane(float A, float B, float C, float D);
    CMGEPlane(CMGEVector3 &n, float D);

    // CREATE PLANE FROM TRIANGLE
    void CreatePlaneFromTri(CMGEVector3 &v1, CMGEVector3 &v2, CMGEVector3 &v3);
    void SetPlaneNormal(float A, float B, float C);
    void SetPlaneIntercept(float D);
    bool Intersect(CMGEPlane &p1, CMGEPlane &p2, CMGEVector3 *intersectPoint);
    bool Intersect(CMGEPlane &p, CMGEVector3 *intersectPoint);
    bool Intersect(CMGEPlane &p, CMGERay *intersectPoint);

    // CLASSIFYS
    int ClassifyPolygon(CMGEPolygon &polygon);
    int ClassifyPoint(CMGEVector3 &v);
    int ClassifyPoint(float x, float y, float z);

    // LERP
    void Lerp(CMGEPlane &p1, CMGEPlane &p, float amount);

    // GET DISTANCET
    float GetDistance(CMGEVector3 &dist);
    float GetDistance(float x, float y, float z);

    float a;
    float b;
    float c;
    float d;

    // a, b, c refer the plane normal(a, b, c)
};

#endif