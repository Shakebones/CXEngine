/*
 * File name - Fade.vs
 * Author - CuiXue
 * Updated date - 2013.4.15
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
	float2 tex		: TEXCOORD0;
 };

 struct PS_INPUT
 {
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
 };

 PS_INPUT FadeVS(VS_INPUT input)
 {
	PS_INPUT output = (PS_INPUT)0;

	input.position.w = 1.0;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	return output;
 }