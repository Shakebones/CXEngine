/*
 * File name - Fire.vs
 * Author - CuiXue
 * Updated date - 2013.7.8
 * Desc - 
 *
 */
 cbuffer  MatrixBufferType
 {
	matrix world;
	matrix view;
	matrix proj;
 };

 cbuffer NoiseBuffer
 {
	float frameTime;
	float3 scrollSpeeds;
	float3 scales;
	float padding;
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
	float2 texcoord1: TEXCOORD1;
	float2 texcoord2: TEXCOORD2;
	float2 texcoord3: TEXCOORD3;
 };

 PS_INPUT FireVS(VS_INPUT input)
 {
	PS_INPUT output = (PS_INPUT)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.texcoord1 = input.tex * scales.x;
	output.texcoord1.y = output.texcoord1.y + (frameTime * scrollSpeeds.x);

	output.texcoord2 = input.tex * scales.y;
	output.texcoord2.y = output.texcoord2.y + (frameTime * scrollSpeeds.y);
	
	output.texcoord3 = input.tex * scales.z;
	output.texcoord3.y = output.texcoord3.y + (frameTime * scrollSpeeds.z);

	return output;
 }