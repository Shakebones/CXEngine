/*
 * File name - Light_Tesselation.vs
 * Author - CuiXue
 * Updated date - 2015.5.7
 * Desc -
 *
 */

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

struct VertexInputType
{
	float4 position : WORLDPOS;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;	
};

struct HullInputType
{
	float4 position			: POSITION;
	float2 tex				: TEXCOORD0;	
	float3 normal			: NORMAL;
};

HullInputType LightVertexShader(VertexInputType input)
{
	HullInputType output = (HullInputType)0;
	return output;
}
