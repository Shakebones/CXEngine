/*
 * File name : MGEPolygon.h
 * Date : 2011.5.26
 * Author : Dormy.ET.C
 * Desc :

 */

#ifndef _MGEPOLYGON_H_
#define _MGEPOLYGON_H_

#include "MathLibrary.h"

/*
 * Clas name : CMGEPlane
 * Date : 2011.5.27
 * Author : Dormy.ET.C
 * Desc :
 
 */
class CMGEPolygon
{
public:
    CMGEPolygon():m_flag(0){}

    void SetFlag(unsigned int flag){ this->m_flag = flag; }
    unsigned int GetFlag(void){ return this->m_flag; }

    void Clip(CMGEPlane &pl, CMGEPolygon *front, CMGEPolygon *back);
    void Copy(CMGEPolygon &polygon);
    bool Intersect(CMGERay &ray, bool isCulled, float *dist);
    
    unsigned int m_flag;
    CMGEVector3 m_vertexList[3];
};

#endif