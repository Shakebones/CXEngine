/*
 * File name - Light.vs
 * Author - CuiXue
 * Updated date - 2012.12.21
 * Desc -
 *
 * The equation for specular lighting is the following:

 * SpecularLighting = SpecularColor * (SpecularColorOfLight * ((NormalVector dot HalfWayVector) power SpecularReflectionPower) * Attentuation * Spotlight)
 
 * We will modify the equation to produce just the basic specular lighting effect as follows:
 * SpecularLighting = SpecularLightColor * (ViewingDirection dot ReflectionVector) power SpecularReflectionPower

 * The reflection vector in this equation has to be produced by multiplying double the light intensity by the vertex normal. 

 * The direction of the light is subtracted which then gives the reflection vector between the light source and the viewing angle:
 * ReflectionVector = 2 * LightIntensity * VertexNormal - LightDirection

 * The viewing direction in the equation is produced by subtracting the location of the camera by the position of the vertex:

 * ViewingDirection = CameraPosition - VertexPosition
 * Lets take a look at the modified light shader to see how this is implemented:
 */

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct PixelInputType
{
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;	
	float3 normal			: NORMAL;
	float3 viewDirection	: TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;
	float4 worldPosition;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}
