/*
 * File name - AlphaMap.vs
 * Author - CuiXue
 * Updated date - 2013.3.10
 * Desc - 
 *
 */
cbuffer _matrix_buffer
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
};

PS_INPUT AlphaMapVS(VS_INPUT input)
{
	PS_INPUT output;
	
	output.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	return output;
}