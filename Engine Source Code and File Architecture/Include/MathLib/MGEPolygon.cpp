/*
 * File name : MGEPolygon.cpp
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

 */

#include "MGEPolygon.h"

// COPY POLYGON
void CMGEPolygon::Copy(CMGEPolygon &p)
{
    this->m_vertexList[0] = p.m_vertexList[0];
    this->m_vertexList[1] = p.m_vertexList[1];
    this->m_vertexList[2] = p.m_vertexList[2];
}

// INTERSECT CHECKING
bool CMGEPolygon::Intersect(CMGERay &ray, bool isCulled, float *dist)
{
    if( !ray.Intersect(this->m_vertexList[0], this->m_vertexList[1],
        this->m_vertexList[2], false, dist) ) return false;

    if( !isCulled )
    {
        if( !ray.Intersect(this->m_vertexList[0], this->m_vertexList[1],
            this->m_vertexList[2], false, dist) ) return false;
    }

    return true;
}

// CLIP
void CMGEPolygon::Clip(CMGEPlane &pl, CMGEPolygon *front, CMGEPolygon *back)
{
   CMGEVector3 frontVecs[3], backVecs[3];
   int frontIndex = 0;
   int backIndex = 0;

   if(!front && !back) return;

   switch(pl.ClassifyPoint(m_vertexList[0]))
      {
         case MGE_FRONT:
            frontVecs[frontIndex++] = m_vertexList[0];
            break;
         
         case MGE_BACK:
            backVecs[backIndex++] = m_vertexList[0];
            break;
         
         case MGE_ONPLANE:
            frontVecs[frontIndex++] = m_vertexList[0];
            backVecs[backIndex++] = m_vertexList[0];
            //frontVecs[backIndex++] = m_vertexList[0];
            break;
         
         default:
            return;
      }

   int index = 0;
   int v1Type = 0, v2Type = 0;
   CMGEVector3 v1, v2, intersectPoint;
   CMGERay ray;

   for(int i = 1; i < 4; i++)
      {
         if(i == 3) index = 0;
         else index = i;

         v1 = m_vertexList[i - 1];
         v2 = m_vertexList[index];

         v1Type = pl.ClassifyPoint(v1);
         v2Type = pl.ClassifyPoint(v2);

         if(v2Type == MGE_ONPLANE)
            {
               frontVecs[frontIndex++] = m_vertexList[index];
               backVecs[backIndex++] = m_vertexList[index];
            }
         else
            {
               float length = 0;
               ray.m_origin = v1;
               ray.m_direction = v2 - v1;
               length = ray.m_direction.GetLength();
               ray.m_direction.Normal();
               
               if(ray.Intersect(pl, false, length, &intersectPoint, 0) &&
                  (v1Type != MGE_ONPLANE))
                  {
                     frontVecs[frontIndex++] = intersectPoint;
                     backVecs[backIndex++] = intersectPoint;
                  }
               
               if(index == 0) continue;
               if(v2Type == MGE_FRONT) frontVecs[frontIndex++] = m_vertexList[index];
               else if(v2Type == MGE_BACK) backVecs[backIndex++] = m_vertexList[index];
            }
      }
   
   if(front)
      {
         CMGEPlane fPl;
         CMGEVector3 n, n2;

         // Be sure to keep same orientation.
         fPl.CreatePlaneFromTri(frontVecs[0], frontVecs[1], frontVecs[2]);
         n.x = fPl.a; n.y = fPl.b; n.z = fPl.c;
         n2.x = pl.a; n2.y = pl.b; n2.z = pl.c;

         if(n.DotProduct3(n2) >= 0.0f)
            {
               front->m_vertexList[0] = frontVecs[0];
               front->m_vertexList[1] = frontVecs[1];
               front->m_vertexList[2] = frontVecs[2];
            }
         else
            {
               front->m_vertexList[0] = frontVecs[2];
               front->m_vertexList[1] = frontVecs[1];
               front->m_vertexList[2] = frontVecs[0];
            }
      }
   
   if(back)
      {
         CMGEPlane fPl;
         CMGEVector3 n, n2;

         // Be sure to keep same orientation.
         fPl.CreatePlaneFromTri(backVecs[0], backVecs[1], backVecs[2]);
         n.x = fPl.a; n.y = fPl.b; n.z = fPl.c;
         n2.x = pl.a; n2.y = pl.b; n2.z = pl.c;

         if(n.DotProduct3(n2) >= 0.0f)
            {
               back->m_vertexList[0] = backVecs[0];
               back->m_vertexList[1] = backVecs[1];
               back->m_vertexList[2] = backVecs[2];
            }
         else
            {
               back->m_vertexList[0] = backVecs[2];
               back->m_vertexList[1] = backVecs[1];
               back->m_vertexList[2] = backVecs[0];
            }
      }
}