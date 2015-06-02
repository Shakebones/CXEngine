/*
 * File name - Light_Tessellation.hlsl
 * Author - CuiXue
 * Updated date - 2015.5.12
 * Desc -
 *
 */
#include "shader_include.hlsl"
#include "Light_Tessellation_utilities.hlsl"

struct VS_INPUT
{
    float3 inPositionOS   : POSITION;
    float2 inTexCoord     : TEXCOORD0;
    float3 vInNormalOS    : NORMAL;
    float3 vInBinormalOS  : BINORMAL;
    float3 vInTangentOS   : TANGENT;
    
    uint   uVertexID      : SV_VERTEXID;
};

struct VS_OUTPUT_HS_INPUT
{
	float3 vWorldPos: WORLDPOS;
	float3 vNormal: NORMAL;
	float fVertexDistanceFactor : VERTEXDISTANCEFACTOR;
	float2 texCoord : TEXCOORD0;
	float3 vLightTS : LIGHTVECTORTS;
	float3 vViewTS : VIEWVECTORTS;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[3] : SV_TessFactor;
	float Inside   : SV_InsideTessFactor;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 vWorldPos : WORLDPOS;
    float3 vNormal   : NORMAL;
    float2 texCoord  : TEXCOORD0;
    float3 vLightTS  : LIGHTVECTORTS;
    float3 vViewTS   : VIEWVECTORTS; 
};

struct DS_OUTPUT
{
    float2 texCoord          : TEXCOORD0; 
    float3 vLightTS          : LIGHTVECTORTS;
    float3 vViewTS           : VIEWVECTORTS;
	float3 vDiffuseColor	 : COLOR;
    float4 vPosition         : SV_POSITION;
};

struct PS_INPUT
{
   float2 texCoord           : TEXCOORD0;  
   float3 vLightTS           : LIGHTVECTORTS;
   float3 vViewTS            : VIEWVECTORTS;
};

VS_OUTPUT_HS_INPUT VS( VS_INPUT i )
{
    VS_OUTPUT_HS_INPUT Out;
    
    // Compute position in world space
    float4 vPositionWS = mul( i.inPositionOS.xyz, g_mWorld );
                 
    // Compute denormalized light vector in world space
    float3 vLightWS = g_LightPosition.xyz - vPositionWS.xyz;
    // Need to invert Z for correct lighting
    vLightWS.z = -vLightWS.z;
    
    // Propagate texture coordinate through:
    Out.texCoord = i.inTexCoord * g_fBaseTextureRepeat.x;

    // Transform normal, tangent and binormal vectors from object 
    // space to homogeneous projection space and normalize them
    float3 vNormalWS   = mul( i.vInNormalOS,   (float3x3) g_mWorld );
    float3 vTangentWS  = mul( i.vInTangentOS,  (float3x3) g_mWorld );
    float3 vBinormalWS = mul( i.vInBinormalOS, (float3x3) g_mWorld );
    
    // Normalize tangent space vectors
    vNormalWS   = normalize( vNormalWS );
    vTangentWS  = normalize( vTangentWS );
    vBinormalWS = normalize( vBinormalWS );
    
    // Output normal
    Out.vNormal = vNormalWS;

    // Calculate tangent basis
    float3x3 mWorldToTangent = float3x3( vTangentWS, vBinormalWS, vNormalWS );
        
    // Propagate the light vector (in tangent space)
    Out.vLightTS = mul( mWorldToTangent, vLightWS );
    
    // Compute and output the world view vector (unnormalized)
    float3 vViewWS = g_vEye - vPositionWS;
    Out.vViewTS  = mul( mWorldToTangent, vViewWS  );

    // Write world position
    Out.vWorldPos = float3( vPositionWS.xyz );

    return Out;
}

HS_CONSTANT_DATA_OUTPUT ConstantsHS( InputPatch<VS_OUTPUT_HS_INPUT, 3> p, uint PatchID : SV_PrimitiveID )
{
    HS_CONSTANT_DATA_OUTPUT output  = (HS_CONSTANT_DATA_OUTPUT)0;
    float4 vEdgeTessellationFactors = g_vTessellationFactor.xxxy;

	// Get the screen space position of each control point
    float2 f2EdgeScreenPosition0 = 
        GetScreenSpacePosition( p[0].vWorldPos.xyz, g_mViewProjection, g_vScreenResolution.x, g_vScreenResolution.y );
    float2 f2EdgeScreenPosition1 = 
        GetScreenSpacePosition( p[1].vWorldPos.xyz, g_mViewProjection, g_vScreenResolution.x, g_vScreenResolution.y );
    float2 f2EdgeScreenPosition2 = 
        GetScreenSpacePosition( p[2].vWorldPos.xyz, g_mViewProjection, g_vScreenResolution.x, g_vScreenResolution.y );
    
    // Calculate edge tessellation factors based on desired screen space tessellation value
    vEdgeTessellationFactors.x = g_vTessellationFactor.w * distance(f2EdgeScreenPosition2, f2EdgeScreenPosition1);
    vEdgeTessellationFactors.y = g_vTessellationFactor.w * distance(f2EdgeScreenPosition2, f2EdgeScreenPosition0);
    vEdgeTessellationFactors.z = g_vTessellationFactor.w * distance(f2EdgeScreenPosition0, f2EdgeScreenPosition1);
    vEdgeTessellationFactors.w = 0.11 * ( vEdgeTessellationFactors.x + vEdgeTessellationFactors.y + vEdgeTessellationFactors.z );

    // Assign tessellation levels
    output.Edges[0] = vEdgeTessellationFactors.x;
    output.Edges[1] = vEdgeTessellationFactors.y;
    output.Edges[2] = vEdgeTessellationFactors.z;
    output.Inside   = vEdgeTessellationFactors.w;
    
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS")]
[maxtessfactor(15.0)]
HS_CONTROL_POINT_OUTPUT HS( InputPatch<VS_OUTPUT_HS_INPUT, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
    HS_CONTROL_POINT_OUTPUT    output = (HS_CONTROL_POINT_OUTPUT)0;
    
    // Copy inputs to outputs
    output.vWorldPos = inputPatch[uCPID].vWorldPos.xyz;
    output.vNormal =   inputPatch[uCPID].vNormal;
    output.texCoord =  inputPatch[uCPID].texCoord;
    output.vLightTS =  inputPatch[uCPID].vLightTS;
    output.vViewTS =   inputPatch[uCPID].vViewTS;

	return output;
}

[domain("tri")]
DS_OUTPUT DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
             const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> TrianglePatch )
{
    DS_OUTPUT output = (DS_OUTPUT)0;

    // Interpolate world space position with barycentric coordinates
    float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].vWorldPos + 
                       BarycentricCoordinates.y * TrianglePatch[1].vWorldPos + 
                       BarycentricCoordinates.z * TrianglePatch[2].vWorldPos;
    
    // Interpolate world space normal and renormalize it
    float3 vNormal = BarycentricCoordinates.x * TrianglePatch[0].vNormal + 
                     BarycentricCoordinates.y * TrianglePatch[1].vNormal + 
                     BarycentricCoordinates.z * TrianglePatch[2].vNormal;
    vNormal = normalize( vNormal );
    
    // Interpolate other inputs with barycentric coordinates
    output.texCoord = BarycentricCoordinates.x * TrianglePatch[0].texCoord + 
                      BarycentricCoordinates.y * TrianglePatch[1].texCoord + 
                      BarycentricCoordinates.z * TrianglePatch[2].texCoord;
    float3 vLightTS = BarycentricCoordinates.x * TrianglePatch[0].vLightTS + 
                      BarycentricCoordinates.y * TrianglePatch[1].vLightTS + 
                      BarycentricCoordinates.z * TrianglePatch[2].vLightTS;

    // Calculate MIP level to fetch normal from
    float fHeightMapMIPLevel = clamp( ( distance( vWorldPos, g_vEye ) - 100.0f ) / 100.0f, 0.0f, 3.0f);
    
    // Sample normal and height map
    float vNormalHeight = g_nmhTexture.SampleLevel(g_samLinear, output.texCoord, 0).r;
	vNormalHeight *= g_vDetailTessellationHeightScale.x;
	vNormalHeight += g_vDetailTessellationHeightScale.x;

	float3 vDirection = -vNormal;
    
    // Displace vertex along normal
    vWorldPos += vDirection * vNormalHeight; //vNormal * ( g_vDetailTessellationHeightScale.x * ( vNormalHeight.w-1.0 ) );
    
    // Transform world position with viewprojection matrix
    output.vPosition = mul( float4( vWorldPos.xyz, 1.0 ), g_mViewProjection );
    
    // Per-pixel lighting: pass tangent space light vector to pixel shader
    output.vLightTS = vLightTS;
    
    // Also pass tangent space view vector
    output.vViewTS  = BarycentricCoordinates.x * TrianglePatch[0].vViewTS + 
                      BarycentricCoordinates.y * TrianglePatch[1].vViewTS + 
                      BarycentricCoordinates.z * TrianglePatch[2].vViewTS;

    return output;
}

float4 PS( PS_INPUT i ) : SV_TARGET
{ 
    float4 cResultColor = float4( 0, 0, 0, 1 );
    float3 vViewTS = float3( 0, 0, 0 );

    // Normalize tangent space light vector
    float3 vLightTS = normalize( i.vLightTS );

    // Normalize tangent space view vector
    vViewTS = normalize( i.vViewTS );

    // Compute resulting color for the pixel:
    cResultColor = ComputeIllumination( i.texCoord, vLightTS, vViewTS );

	//float fDensity = g_DensityTexture.SampleLevel( g_samLinear, i.texCoord, 0 ).x;
    cResultColor.xyz = cResultColor.xyz; // + fDensity;
   
   // Return color
   return cResultColor;
}  