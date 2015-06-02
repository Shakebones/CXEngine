/*
 * File name : MGERay.h
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

   This file declar the ray class
 */

#ifndef _MGERAY_H_
#define _MGERAY_H_

#include "MathLibrary.h"

/*
 * Class name : CMGERay.
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

 */
class CMGERay
{
public:
    CMGERay(){}

    // Overload list.

      bool Intersect(CMGEVector3 &pos, float radius, float *dist);
      bool Intersect(CMGEVector3 &p1, CMGEVector3 &p2, CMGEVector3 &p3,
                     bool cull, float *dist);
      bool Intersect(CMGEVector3 &p1, CMGEVector3 &p2, CMGEVector3 &p3,
                     bool cull, float length, float *dist);
      bool Intersect(CMGEPlane &pl, bool cull,
                     CMGEVector3 *intersectPoint, float *dist);
      bool Intersect(CMGEPlane &pl, bool cull, float length,
                     CMGEVector3 *intersectPoint, float *dist);

    CMGEVector3 m_origin;
    CMGEVector3 m_direction;
};

#endif