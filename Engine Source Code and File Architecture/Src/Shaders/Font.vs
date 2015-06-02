/*
 * File name - Font.vs
 * Author - CuiXue
 * Updated date - 2013.1.5
 * Desc -
 *
 */
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

// Vertex input type
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

PixelInputType FontVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	return output;
}