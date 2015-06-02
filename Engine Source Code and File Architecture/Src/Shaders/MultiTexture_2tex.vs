/* 
 * File name - MultiTexture.vs
 * Author - CuiXue
 * Updated date - 2013.3.1
 * Desc -
 *
 */

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

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

PixelInputType MultiTextureVS(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	return output;
}