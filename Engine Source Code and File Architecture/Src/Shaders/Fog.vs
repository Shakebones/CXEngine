/*
 * File name - Fog.vs
 * Author - CuiXue
 * Updated date - 2013.3.25
 * Desc - 
 *
 */
 cbuffer MatrixBuffer
 {
	 matrix world;
	 matrix view;
	 matrix proj;
 };

 cbuffer FogBuffer
 {
	 float fogStart;
	 float fogEnd;
 };

 struct VS_INPUT
 {
	 float4 position : POSITION;
	 float2 tex		 : TEXCOORD0;
 };

 struct PS_INPUT
 {
	 float4 position : SV_POSITION;
	 float2 tex		 : TEXCOORD0;
	 float fogFactor : FOG;
 };

 PS_INPUT FogVS(VS_INPUT input)
 {
	 PS_INPUT output = (PS_INPUT)0;
	 float4 cameraPosition;

	 input.position.w = 1.0f;

	 output.position = mul(input.position, world);
	 output.position = mul(output.position, view);
	 output.position = mul(output.position, proj);

	 output.tex = input.tex;

	 cameraPosition = mul(input.position, world);
	 cameraPosition = mul(cameraPosition, view);

	 output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

	 return output;
 }