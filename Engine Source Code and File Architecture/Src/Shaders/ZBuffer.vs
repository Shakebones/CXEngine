/*
 * File name - ZBuffer.vs
 * Author - CuiXue
 * Updated date - 2013.7.16
 * Desc - 
 *
 */
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 position : POSITION;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

PS_INPUT ZBufferVS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.depthPosition = output.position;

	return output;
}