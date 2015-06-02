/*
 * File name - Refraction.vs
 * Author - CuiXue
 * Updated date - 2013.4.22
 * Desc - 
 *
 */
 cbuffer MatrixBuffer
 {
	matrix world;
	matrix view;
	matrix proj;
 };

 cbuffer CameraBuffer
 {
	float3 cameraPosition;
	float padding;
 };

 cbuffer ClipPlaneBuffer
 {
	float4 clipPlane;
 };

 struct VS_INPUT
 {
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
 };

 struct PS_INPUT
 {
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;	
	float3 normal			: NORMAL;
	float3 viewDirection	: TEXCOORD1;
	float clip		: SV_ClipDistance0;
 };

 PS_INPUT RefractionVS(VS_INPUT input)
 {
	PS_INPUT output = (PS_INPUT)0;

	float4 worldPosition;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	worldPosition = mul(input.position, world);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	output.clip = dot(mul(input.position, world), clipPlane);

	return output;
 };