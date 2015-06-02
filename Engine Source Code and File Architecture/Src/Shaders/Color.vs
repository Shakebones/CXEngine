/*
 * File name - Color.vs
 * Author - CuiXue
 * Updated date - 2012.11.29
 * Desc
 */
cbuffer MatrixBuffer
{
	matrix	worldMatrix;
	matrix	viewMatrix;
	matrix	projectionMatrix;
};

// Input types ( Vertex shader & Pixel shader )
struct VSInputType
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct PSInputType
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PSInputType ColorVertexShader(VSInputType _input)
{
	PSInputType _output = (PSInputType)0;

	_input.pos.w = 1.0f;
	_output.pos = mul(_input.pos, worldMatrix);
	_output.pos = mul(_output.pos, viewMatrix);
	_output.pos = mul(_output.pos, projectionMatrix);

	_output.color = _input.color;
	
	return _output;
}

