/*
 * File name - Reflection.vs
 * Author - CuiXue
 * Updated date - 2013.4.10
 * Desc - 
 *
 */
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position		: SV_POSITION;
	float2 tex			: TEXCOORD0;
	float4 reflection	: TEXCOORD1;
};

PS_INPUT ReflectionVS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	matrix	 reflectionProjWorld;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	reflectionProjWorld = mul(reflectionMatrix, proj);
	reflectionProjWorld = mul(world, reflectionProjWorld);

	output.reflection = mul(input.position, reflectionProjWorld);

	return output;
}