/*
 * File name : MGEPlane.cpp
 * Date : 2011.5.27
 * Author : Dormy.ET.C
 * Desc :

 */

#include "MGEPlane.h"

// CONSTRUCTOR LIST
CMGEPlane::CMGEPlane(void)
{
    this->a = 0;
    this->b = 0;
    this->c = 0;
    this->d = 0;
}

CMGEPlane::CMGEPlane(float A, float B, float C, float D)
{
    this->a = A;
    this->b = B;
    this->c = C;
    this->d = D;
}

CMGEPlane::CMGEPlane(CMGEVector3 &v, float D)
{
    this->a = v.x;
    this->b = v.y;
    this->c = v.z;
    this->d = D;
}


// Translate 3 3D Vector to plane.
void CMGEPlane::CreatePlaneFromTri(CMGEVector3 &v1, CMGEVector3 &v2, CMGEVector3 &v3)
{
    CMGEVector3 normal;
    CMGEVector3 elem1;
    CMGEVector3 elem2;

    elem1 = v3 - v1;
    elem2 = v2 - v1;

    elem1.Normal();
    elem2.Normal();

    normal.CrossProduct3(elem1, elem2);
    normal.Normal();

    // Compute D
    this->a = normal.x;
    this->b = normal.y;
    this->c = normal.z;

    this->d = ( this->a * v1.x + this->b * v1.y + this->c * v1.z );
}

void CMGEPlane::SetPlaneNormal(float A, float B, float C){
    this->a = A;
    this->b = B;
    this->c = C;
}

void CMGEPlane::SetPlaneIntercept(float D){
    this->d = D;
}

////////////////////////// INTERSECT OVERLOAD LIST //////////////////////////
bool CMGEPlane::Intersect(CMGEPlane &p1, CMGEPlane &p2, CMGEVector3 *intersectPoint)
{
    CMGEVector3 temp;
    CMGEVector3 normal(this->a, this->b, this->c);
    CMGEVector3 p1Normal(p1.a, p1.b, p1.c);
    CMGEVector3 p2Normal(p2.a, p2.b, p2.c);

    temp.CrossProduct3(p1Normal, p2Normal);

    float dp = normal.DotProduct3(temp);

    if( dp == 0.0f ) 
        return false;

    if( intersectPoint )
    {
        *intersectPoint = temp * this->d;

        temp.CrossProduct3(p1Normal, normal);
        *intersectPoint = *intersectPoint + (p1.d);

        temp.CrossProduct3(normal, p1Normal);
        *intersectPoint = *intersectPoint + (p2.d);

        *intersectPoint = *intersectPoint / -dp;
    }

    return true;
}

bool CMGEPlane::Intersect(CMGEPlane &p, CMGEVector3 *intersectPoint)
{
    CMGEVector3 cross;
    CMGEVector3 normal(this->a, this->b, this->c);
    CMGEVector3 p1Normal(p.a, p.b, p.c);
    float length = 0.0f;

    cross.CrossProduct3(normal, p1Normal);

    length = cross.DotProduct3(cross);

    if( length < 1e-08f ) return false;

    if( intersectPoint )
    {
        float l0 = normal.DotProduct3(normal);
        float l1 = normal.DotProduct3(p1Normal);
        float l2 = p1Normal.DotProduct3(p1Normal);
        float det = l0 * l2 - l1 * l1;
        float invDet = 0;

        if(fabs(det) < 1e-08f) return false;

        invDet = 1 / det;
        float d0 = (l2 * d - l1 * p.d) * invDet;
        float d1 = (l0 * p.d - l1 * d) * invDet;

        (*intersectPoint) = normal * d0 + p1Normal * d1;
    }

    return true;
}

bool CMGEPlane::Intersect(CMGEPlane &p, CMGERay *intersectPoint)
{
    CMGEVector3 cross;
    CMGEVector3 normal(this->a, this->b, this->c);
    CMGEVector3 p1Normal(p.a, p.b, p.c);
    float length = 0.0f;

    cross.CrossProduct3(normal, p1Normal);
    length = cross.DotProduct3(cross);

    if( length < 1e-08f ) return false;

    if( intersectPoint )
    {
        float l0 = normal.DotProduct3(normal);
        float l1 = normal.DotProduct3(p1Normal);
        float l2 = p1Normal.DotProduct3(p1Normal);
        float det = l0 * l2 - l1 * l1;
        float invDet = 0;

        if(fabs(det) < 1e-08f) return false;

        invDet = 1 / det;
        float d0 = (l2 * d - l1 * p.d) * invDet;
        float d1 = (l0 * p.d - l1 * d) * invDet;

        (*intersectPoint).m_origin = normal * d0 + p1Normal * d1;
        (*intersectPoint).m_direction = cross;
    }

    return true;
}

// CLASSIFY POLYGON
int CMGEPlane::ClassifyPolygon(CMGEPolygon &polygon)
{
    int frontPolygon    = 0;
    int backPolygon     = 0;
    int planePolygon    = 0;
    int type            = 0;

    for( int i = 0; i < 3; i++ )
    {
        type = this->ClassifyPoint(polygon.m_vertexList[i]);

        switch(type)
        {
        case MGE_FRONT:
            frontPolygon++;
            break;

        case MGE_BACK:
            backPolygon++;
            break;

        default:
            frontPolygon++;
            backPolygon++;
            planePolygon++;
            break;
        }
    }

    if(planePolygon == 3) return MGE_ONPLANE;
    else if(frontPolygon == 3) return MGE_FRONT;
    else if(backPolygon == 3) return MGE_BACK;

    return MGE_CLIPPED;
}

// CLASSIFY POINT
int CMGEPlane::ClassifyPoint(CMGEVector3 &v)
{
    float distance = a * v.x + b * v.y + c * v.z + d;

    if(distance > 0.001) return MGE_FRONT;
    if(distance < -0.001) return MGE_BACK;

    return MGE_ONPLANE;
}

// CLASSIFY POINT
int CMGEPlane::ClassifyPoint(float x, float y, float z)
{
    float distance = a * x + b * y + c * z + d;

   if(distance > 0.001) return MGE_FRONT;
   if(distance < -0.001) return MGE_BACK;

   return MGE_ONPLANE;
}

// LERP BETWEEN 2 PLANES
void CMGEPlane::Lerp(CMGEPlane &p1, CMGEPlane &p2, float amount)
{
   a = p1.a * (1.0f - amount) + p2.a * amount;
   b = p1.b * (1.0f - amount) + p2.b * amount;
   c = p1.c * (1.0f - amount) + p2.c * amount;
   d = p1.d * (1.0f - amount) + p2.d * amount;

   float length = (float)sqrt((a * a + b * b + c * c));
   
   if(length == 0.0f) length = 1.0f;
   length = 1 / length;

   a = a * length;
   b = b * length;
   c = c * length;
}
