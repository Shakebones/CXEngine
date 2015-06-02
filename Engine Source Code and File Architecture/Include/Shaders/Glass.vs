/*
 * File name - Glass.vs
 * Author - CuiXue
 * Updated date - 2013.5.26
 * Desc - 
 *
 */
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

// Input structure
struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

// Output structure
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
	float4 refractionPosition : TEXCOORD1;
};

PS_INPUT GlassShaderVS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	matrix viewProjectWorld;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	viewProjectWorld = mul(view, proj);
	viewProjectWorld = mul(world, viewProjectWorld);

	output.refractionPosition = mul(input.position, viewProjectWorld);

	return output;
}

