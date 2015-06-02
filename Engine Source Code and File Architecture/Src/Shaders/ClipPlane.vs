/*
 * File name - ClipPlane.vs
 * Author - CuiXue
 * Updated date - 2013.3.26
 * Desc - 
 *
 */
 cbuffer MatrixBuffer
 {
	matrix world;
	matrix view;
	matrix proj;
 };

 cbuffer ClipPlaneBuffer
 {
	float4 clipPlane;
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
	float clip		: SV_ClipDistance0;
 };

 PS_INPUT ClipPlaneVS(VS_INPUT input)
 {
	PS_INPUT output = (PS_INPUT)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.clip = dot(mul(input.position, world), clipPlane);

	return output;
 }