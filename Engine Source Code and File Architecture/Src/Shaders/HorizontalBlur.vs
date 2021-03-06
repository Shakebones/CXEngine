/*
 * File name - HorizontalBlur.vs
 * Author - CuiXue
 * Updated date - 2013.7.18
 * Desc - 
 *
 */
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer screenWidthBuffer
{
	float screenWidth;
	float3 padding;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;
	float2 texCoord1		: TEXCOORD1;
	float2 texCoord2		: TEXCOORD2;
	float2 texCoord3		: TEXCOORD3;
	float2 texCoord4		: TEXCOORD4;
	float2 texCoord5		: TEXCOORD5;
	float2 texCoord6		: TEXCOORD6;
	float2 texCoord7		: TEXCOORD7;
	float2 texCoord8		: TEXCOORD8;
	float2 texCoord9		: TEXCOORD9;
};

PS_INPUT HorizontalBlurVS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	input.position.w = 1.0f;
	float texelSize;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	texelSize = 1.0f / screenWidth;

	output.texCoord1 = input.tex + float2(texelSize * -4.0f, 0.0f);
	output.texCoord2 = input.tex + float2(texelSize * -3.0f, 0.0f);
	output.texCoord3 = input.tex + float2(texelSize * -2.0f, 0.0f);
	output.texCoord4 = input.tex + float2(texelSize * -1.0f, 0.0f);
	output.texCoord5 = input.tex + float2(texelSize *  0.0f, 0.0f);
	output.texCoord6 = input.tex + float2(texelSize *  1.0f, 0.0f);
	output.texCoord7 = input.tex + float2(texelSize *  2.0f, 0.0f);
	output.texCoord8 = input.tex + float2(texelSize *  3.0f, 0.0f);
	output.texCoord9 = input.tex + float2(texelSize *  4.0f, 0.0f);

	return output;
}