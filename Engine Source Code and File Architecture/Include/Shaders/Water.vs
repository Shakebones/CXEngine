/*
 * File name - Water.vs
 * Author - CuiXue
 * Updated date - 2013.4.21
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

 cbuffer CameraBuffer
 {
	float3 cameraPosition;
	float padding;
 };

 struct VS_INPUT
 {
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
 };

 struct PS_INPUT
 {
	float4 position : SV_POSITION;
	float2 tex1		: TEXCOORD0;
	float2 tex2		: TEXCOORD4;
	float4 reflectionPos : TEXCOORD1;
	float4 refractionPos : TEXCOORD2;
	float3 viewDirection : TEXCOORD3;
	float3 normal	: NORMAL;
	float3 tangent		 : TANGENT;
	float3 binormal		 : BINORMAL;
 };

 PS_INPUT WaterVS(VS_INPUT input)
 {
	PS_INPUT output = (PS_INPUT)0;

	float4 worldPos;

	matrix reflectionProjWorld;
	matrix viewProjWorld;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)world);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)world);
	output.binormal = normalize(output.binormal);

	reflectionProjWorld = mul(reflectionMatrix, proj);
	reflectionProjWorld = mul(world, reflectionProjWorld);
	output.reflectionPos = mul(input.position, reflectionProjWorld);

	viewProjWorld = mul(view, proj);
	viewProjWorld = mul(world, viewProjWorld);
	output.refractionPos = mul(input.position, viewProjWorld);

	worldPos = mul(input.position, world);
	output.viewDirection = cameraPosition.xyz - worldPos.xyz;
	output.viewDirection = normalize(output.viewDirection);

	output.tex1 = input.tex / 1.0f;
	output.tex2 = input.tex / 1.0f;

	return output;
 }