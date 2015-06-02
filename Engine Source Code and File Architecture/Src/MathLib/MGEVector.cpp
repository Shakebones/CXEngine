/*
 * File name : MGEVector.h
 * Date : 2011.5.23
 * Author : Dormy.ET.C
 */

#include "MGEVector.h"
#include "MathDefine.h"

CMGEVector3::CMGEVector3(){
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

CMGEVector3::CMGEVector3(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

CMGEVector3::CMGEVector3(const CMGEVector3 &v)
{
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

void CMGEVector3::operator=( const CMGEVector3 &v )
{
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

CMGEVector3 CMGEVector3::operator+(CMGEVector3 &v){
    return CMGEVector3(this->x + v.x,
        this->y + v.y, this->z + v.z);
}

CMGEVector3 CMGEVector3::operator-(CMGEVector3 &v){
    return CMGEVector3(this->x - v.x,
        this->y - v.y, this->z - v.z);
}

CMGEVector3 CMGEVector3::operator*(CMGEVector3 &v){
    return CMGEVector3(this->x * v.x,
        this->y * v.y, this->z * v.z);
}

CMGEVector3 CMGEVector3::operator/(CMGEVector3 &v){
    return CMGEVector3(this->x / v.x,
        this->y / v.y, this->z / v.z);
}


CMGEVector3 CMGEVector3::operator+(float f){
    return CMGEVector3(this->x + f,
        this->y + f, this->z + f);
}

CMGEVector3 CMGEVector3::operator-(float f){
    return CMGEVector3(this->x - f,
        this->y - f, this->z - f);
}

CMGEVector3 CMGEVector3::operator*(float f){
    return CMGEVector3(this->x * f,
        this->y * f, this->z * f);
}

CMGEVector3 CMGEVector3::operator/(float f){
    f = 1/f;
    return CMGEVector3(this->x * f,
        this->y * f, this->z * f);
}

void CMGEVector3::operator+=(CMGEVector3 &v){
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
}

void CMGEVector3::operator-=(CMGEVector3 &v){
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
}

void CMGEVector3::operator*=(CMGEVector3 &v){
    this->x *= v.x;
    this->y *= v.y;
    this->z *= v.z;
}

void CMGEVector3::operator/=(CMGEVector3 &v){
    this->x /= v.x;
    this->y /= v.y;
    this->z /= v.z;
}

bool CMGEVector3::operator==( CMGEVector3 &v )
{
    if( this->x == v.x &&
        this->y == v.y &&
        this->z == v.z ) return true;

    else return false;
}

void CMGEVector3::operator+=(float f){
    this->x += f;
    this->y += f;
    this->z += f;
}

void CMGEVector3::operator-=(float f){
    this->x -= f;
    this->y -= f;
    this->z -= f;
}

void CMGEVector3::operator*=(float f){
    this->x *= f;
    this->y *= f;
    this->z *= f;
}

void CMGEVector3::operator/=(float f){
    f = 1/f;
    this->x *= f;
    this->y *= f;
    this->z *= f;
}

void CMGEVector3::CrossProduct3(CMGEVector3 &v1, CMGEVector3 &v2){
    this->x = ((v1.y * v2.z) - (v1.z * v2.y));
    this->y = ((v1.z * v2.x) - (v1.x * v2.z));
    this->z = ((v1.x * v2.y) - (v1.y * v2.x));
}

float CMGEVector3::DotProduct3(CMGEVector3 &v){
    return this->x * v.x + this->y * v.y + this->z * v.z;
}

float CMGEVector3::GetLength(void){
    return (float)::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void CMGEVector3::Normal(void){
    float length = this->GetLength();

    if( length == 0.0f )
        length = 1.0f;

    this->x = this->x / length;
    this->y = this->y / length;
    this->z = this->z / length;
}

//////////////////////////////// Vector with 4 float values ////////////////////////////////
CMGEVector4::CMGEVector4(){
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->a = 0.0f;
}

CMGEVector4::CMGEVector4(float x, float y, float z, float a){
    this->x = x;
    this->y = y;
    this->z = z;
    this->a = a;
}

CMGEVector4 CMGEVector4::operator+(CMGEVector4 &v){
    return CMGEVector4(this->x + v.x,
        this->y + v.y, this->z + v.z, this->a + v.a);
}

CMGEVector4 CMGEVector4::operator-(CMGEVector4 &v){
    return CMGEVector4(this->x - v.x,
        this->y - v.y, this->z - v.z, this->a - v.a);
}

CMGEVector4 CMGEVector4::operator*(CMGEVector4 &v){
    return CMGEVector4(this->x * v.x,
        this->y * v.y, this->z * v.z, this->a * v.a);
}

CMGEVector4 CMGEVector4::operator/(CMGEVector4 &v){
    return CMGEVector4(this->x / v.x,
        this->y / v.y, this->z / v.z, this->a / v.a);
}


CMGEVector4 CMGEVector4::operator+(float f){
    return CMGEVector4(this->x + f,
        this->y + f, this->z + f, this->a + f);
}

CMGEVector4 CMGEVector4::operator-(float f){
    return CMGEVector4(this->x - f,
        this->y - f, this->z - f, this->a - f);
}

CMGEVector4 CMGEVector4::operator*(float f){
    return CMGEVector4(this->x * f,
        this->y * f, this->z * f, this->a * f);
}

CMGEVector4 CMGEVector4::operator/(float f){
    f = 1/f;
    return CMGEVector4(this->x * f,
        this->y * f, this->z * f, this->a * f);
}

void CMGEVector4::operator+=(CMGEVector4 &v){
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    this->a += v.a;
}

void CMGEVector4::operator-=(CMGEVector4 &v){
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    this->a -= v.a;
}

void CMGEVector4::operator*=(CMGEVector4 &v){
    this->x *= v.x;
    this->y *= v.y;
    this->z *= v.z;
    this->a *= v.a;
}

void CMGEVector4::operator/=(CMGEVector4 &v){
    this->x /= v.x;
    this->y /= v.y;
    this->z /= v.z;
    this->a /= v.a;
}

void CMGEVector4::operator+=(float f){
    this->x += f;
    this->y += f;
    this->z += f;
    this->a += f;
}

void CMGEVector4::operator-=(float f){
    this->x -= f;
    this->y -= f;
    this->z -= f;
    this->a -= f;
}

void CMGEVector4::operator*=(float f){
    this->x *= f;
    this->y *= f;
    this->z *= f;
    this->a *= f;
}

void CMGEVector4::operator/=(float f){
    f = 1/f;
    this->x *= f;
    this->y *= f;
    this->z *= f;
    this->a *= f;
}

// Desc : No vector which with 4 float vlaues CROSSPRODUCT 
//void CMGEVector4::CrossProduct4(CMGEVector4 &v1, CMGEVector4 &v2){
//    this->x = ((v1.y * v2.z) - (v1.z * v2.y));
//    this->y = ((v1.z * v2.x) - (v1.x * v2.z));
//    this->z = ((v1.x * v2.y) - (v1.y * v2.x));
//
//}

float CMGEVector4::DotProduct4(CMGEVector4 &v){
    return this->x * v.x + this->y * v.y + this->z * v.z + this->a * v.a;
}

float CMGEVector4::GetLength(void){
    return (float)::sqrt(this->x * this->x + this->y * this->y + this->z * this->z +
        this->a * this->a);
}

void CMGEVector4::Normal(void){
    float length = this->GetLength();

    if( length == 0.0f )
        length = 1.0f;

    this->x = this->x / length;
    this->y = this->y / length;
    this->z = this->z / length;
    this->a = this->a / length;
}
