/*
 * Desc :
 * Math defines.
 */

#ifndef _MATH_DEFINE_H_
#define _MATH_DEFINE_H_

#define MGE_FRONT 0
#define MGE_BACK 1
#define MGE_ONPLANE 2
#define MGE_CLIPPED 3
#define MGE_CULLED 4
#define MGE_VISIBLE 5

#ifndef MGE_PI
#define MGE_PI 3.14159265358979323846
#define MGE_GET_RADIANS(degree) (float)(degree / 180 * MGE_PI)
#endif

#endif