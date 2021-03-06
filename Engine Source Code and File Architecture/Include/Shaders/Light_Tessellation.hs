/*
 * File name - Light_Tesselation.hs
 * Author - CuiXue
 * Updated date - 2015.5.7
 * Desc -
 *
 */
cbuffer TesselationBuffer
{
	float tessellationAmount;
	float3 padding;
}

struct HullInputType
{
	float4 position			: POSITION;
	float2 tex				: TEXCOORD0;	
	float3 normal			: NORMAL;
};

struct ConstantOutputType
{
	float edges[3]: SV_TessFactor;
	float inside: SV_InsideTessFactor;
};

struct HullOutputType
{
	float4 position			: POSITION;
	float2 tex				: TEXCOORD0;	
	float3 normal			: NORMAL;
	float3 viewDirection	: TEXCOORD1;
};

// Patch function
ConstantOutputType LightPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchID: SV_PrimitiveID)
{
	ConstantOutputType output;

	output.edges[0] = tessellationAmount;
	output.edges[1] = tessellationAmount;
	output.edges[2] = tessellationAmount;

	output.inside = tessellationAmount;
	
	return output;
}

// Hull Shader
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("LightPatchConstantFunction")]

HullOutputType LightHullShader(InputPatch<HullOutputType, 3> patch, uint pointID: SV_OutputControlPointID, uint patchID: SV_PrimitiveID)
{
	HullOutputType output;

	output.position = patch[pointID].position;
	
	output.tex = patch[pointID].tex;
	output.normal = patch[pointID].normal;
	output.viewDirection = patch[pointID].viewDirection;
	
	return output;
}