/*
 * File name - MultiPointLight.vs
 * Author - CuiXue
 * Updated date - 2013.5.2
 * Desc - 
 *
 */

#define NUM_LIGHTS 4

// Matrix
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

// Light position
cbuffer LightPositionBuffer
{
	float4 light_position[NUM_LIGHTS];
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

// VS_OUTPUT
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 light1_pos : TEXCOORD1;
	float3 light2_pos : TEXCOORD2;
	float3 light3_pos : TEXCOORD3;
	float3 light4_pos : TEXCOORD4;
};

// main
PS_INPUT MultiPointLightVS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	float4 worldPos;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	worldPos = mul(input.position, world);

	output.light1_pos.xyz = light_position[0].xyz - worldPos.xyz;
	output.light2_pos.xyz = light_position[1].xyz - worldPos.xyz;
	output.light3_pos.xyz = light_position[2].xyz - worldPos.xyz;
	output.light4_pos.xyz = light_position[3].xyz - worldPos.xyz;

	output.light1_pos = normalize(output.light1_pos);
	output.light2_pos = normalize(output.light2_pos);
	output.light3_pos = normalize(output.light3_pos);
	output.light4_pos = normalize(output.light4_pos);

	return output;
}