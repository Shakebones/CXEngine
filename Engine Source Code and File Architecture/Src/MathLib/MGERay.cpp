/*
 * File name : MGERay.cpp
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

 */

#include "MGERay.h"

////////////// OVERLOAD LIST //////////////

bool CMGERay::Intersect(CMGEVector3 &pos, float radius, float *dist)
{
   CMGEVector3 RSDir;

   float RayToSphereLength = 0.0f;
   float IntersectPoint = 0.0f;
   float SquaredPoint = 0.0f;

   // Get the ray/sphere direction.
   RSDir = pos - m_origin;

   RayToSphereLength = RSDir.DotProduct3(RSDir);
   IntersectPoint = RSDir.DotProduct3(m_direction);

   // If true no collision.
   if(IntersectPoint < 0 ) return false;

   SquaredPoint = (radius * radius) - RayToSphereLength +
                  (IntersectPoint * IntersectPoint);

   // If this is less than zero then the ray does not hit.
   if(SquaredPoint < 0) return false;

   // Else it does hit the sphere and we record the results.
   if(dist) *dist = IntersectPoint - (float)sqrt(SquaredPoint);

   return true;
}


bool CMGERay::Intersect(CMGEVector3 &p1, CMGEVector3 &p2, CMGEVector3 &p3,
                     bool cull, float *dist)
{
   // Store these vectors between the points for future use.
   CMGEVector3 vecAB = p2 - p1;
   CMGEVector3 vecAC = p3 - p1;

   // Get the corss product bewteen the p3 - p1 vec and the ray dir.
   CMGEVector3 cross;
   cross.CrossProduct3(m_direction, vecAC);

   // Calculate the determinate.
   float det = vecAB.DotProduct3(cross);
   
   // If either true, no collision.
   if(cull && det < 0.0001f) return false;
   else if(det < 0.0001f && det > -0.0001f) return false;

   CMGEVector3 rayPointVec = m_origin - p1;
   float test1 = rayPointVec.DotProduct3(cross);
   
   // Ray behind, no collision.
   if(test1 < 0.0f || test1 > det) return false;
   
   CMGEVector3 cross2;
   cross2.CrossProduct3(rayPointVec , vecAB);
   float test2 = m_direction.DotProduct3(cross2);

   // No collision.
   if(test2 < 0.0f || test1 + test2 > det) return false;
   
   // Get collision dist.
   float inverseDet = 1.0f / det;
   
   if(dist)
      {
         *dist = vecAC.DotProduct3(cross2);
         *dist *= inverseDet;
      }

   return true;
}


bool CMGERay::Intersect(CMGEVector3 &p1, CMGEVector3 &p2, CMGEVector3 &p3,
                     bool cull, float length, float *dist)
{
   // Store these vectors between the points for future use.
   CMGEVector3 vecAB = p2 - p1;
   CMGEVector3 vecAC = p3 - p1;

   // Get the corss product bewteen the p3 - p1 vec and the ray dir.
   CMGEVector3 cross;
   cross.CrossProduct3(m_direction, vecAC);

   // Calculate the determinate.
   float det = vecAB.DotProduct3(cross);
   
   // If either true, no collision.
   if(cull && det < 0.0001f) return false;
   else if(det < 0.0001f && det > -0.0001f) return false;
      
   CMGEVector3 rayPointVec = m_origin - p1;
   float test1 = rayPointVec.DotProduct3(cross);
   
   // Ray behind, no collision.
   if(test1 < 0.0f || test1 > det) return false;
   
   CMGEVector3 cross2;
   cross2.CrossProduct3(rayPointVec , vecAB);
   float test2 = m_direction.DotProduct3(cross2);

   // No collision.
   if(test2 < 0.0f || test1 + test2 > det) return false;
   
   // Get distance. If smaller than ray length then collision.
   float inverseDet = 1.0f / det;
   
   if(dist)
      {
         *dist = vecAC.DotProduct3(cross2);
         *dist *= inverseDet;

         if(*dist > length) return false;
      }
   else
      {
         float l = vecAC.DotProduct3(cross2) * inverseDet;
         if(l > length) return false;
      }

   return true;
}


bool CMGERay::Intersect(CMGEPlane &pl, bool cull,
                     CMGEVector3 *intersectPoint, float *dist){
   CMGEVector3 normal(pl.a, pl.b, pl.c);

   float rayD = normal.DotProduct3(m_direction);
   
   // If true then ray is parallel to plane, no intersection.
   if(fabs(rayD) < 0.00001f) return false;
   if(cull && rayD > 0.0f) return false;

   float originD = -(normal.DotProduct3(m_origin) + pl.d);
   float intersectDist = originD / rayD;
   
   // If less then 0, no intersection.  Behind ray origin.
   if(intersectDist < 0.001f) return false;
   
   // Else we have a hit.
   if(dist) *dist = intersectDist;
   if(intersectPoint)
      *intersectPoint = m_origin + (m_direction * intersectDist);
   
   return true;
}


bool CMGERay::Intersect(CMGEPlane &pl, bool cull, float length,
                     CMGEVector3 *intersectPoint, float *dist)
{
   CMGEVector3 normal(pl.a, pl.b, pl.c);

   float rayD = normal.DotProduct3(m_direction);

   // If true then ray is parallel to plane, no intersection.
   if(fabs(rayD) < 0.00001f) return false;
   if(cull && rayD > 0.0f) return false;

   float originD = -(normal.DotProduct3(m_origin) + pl.d);
   
   float intersectDist = originD / rayD;
   
   // If less then 0, no intersection.  Behind ray origin.
   if(intersectDist < 0.001f || intersectDist > length) return false;
   
   // Else we have a hit.
   if(dist) *dist = intersectDist;
   if(intersectPoint)
      *intersectPoint = m_origin + (m_direction * intersectDist);
   
   return true;
}

