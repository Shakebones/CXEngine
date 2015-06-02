/*
 * File name - Texture.vs
 * Author - CuiXue
 * Updated date - 2012.12.21
 * Desc -
 *
 */

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
}

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;	
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;
	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.tex = input.tex;
	return output;
}
