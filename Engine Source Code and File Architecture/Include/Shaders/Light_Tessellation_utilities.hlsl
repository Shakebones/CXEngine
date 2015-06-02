/*
 * File name - Light_Tessellation_utilites.hlsl
 * Author - CuiXue
 * Updated date - 2015.5.12
 * Desc -
 *
 */
float GetEdgeDotProduct(float3 f3EdgeNormal0, float3 f3EdgeNormal1, float3 f3ViewVector)
{
	float3 f3EdgeNormal = normalize( (f3EdgeNormal0 + f3EdgeNormal1) * 0.5f );
	float fEdgeDotProduct = dot( f3EdgeNormal, f3ViewVector );

	return fEdgeDotProduct;
}

float2 GetScreenSpacePosition(float3 f3Position, float4x4 f4x4ViewProjection, float fScreenWidth, float fScreenHeight)
{
	float4 f4ProjectedPosition = mul( float4( f3Position, 1.0f ), f4x4ViewProjection );
	float2 f2ScreenPosition = f4ProjectedPosition.xy / f4ProjectedPosition.ww;
	f2ScreenPosition = (f2ScreenPosition + 1.0f ) * 0.5f * float2( fScreenWidth, -fScreenHeight);

	return f2ScreenPosition;
}

float DistanceFromPlane(float3 f3Position, float4 f4PlaneEquation)
{
	float fDistance = dot( float4( f3Position, 1.0f), f4PlaneEquation );

	return fDistance;
}

float GetDistanceAdaptiveScaleFactor(float3 f3Eye, float3 f3EdgePosition0, float3 f3EdgePosition1, float fMinDistance, float fRange)
{
	float3 f3MidPoint = (f3EdgePosition0 + f3EdgePosition1) * 0.5f;
	float fDistance = distance( f3MidPoint, f3Eye ) - fMinDistance;
	float fScale = 1.0f - saturate( fDistance / fRange );

	return fScale;
}

float GetOrientationAdaptiveScaleFactor (float fEdgeDotProduct, float fSilhouetteEpsilon)
{
    float fScale = 1.0f - abs( fEdgeDotProduct );
        
    fScale = saturate( ( fScale - fSilhouetteEpsilon ) / ( 1.0f - fSilhouetteEpsilon ) );

    return fScale;
}

float GetScreenResolutionAdaptiveScaleFactor(float fCurrentWidth, float fCurrentHeight, float fMaxWidth, float fMaxHeight)
{
    float fMaxArea = fMaxWidth * fMaxHeight;
    
    float fCurrentArea = fCurrentWidth * fCurrentHeight;

    float fScale = saturate( fCurrentArea / fMaxArea );

    return fScale;
}

bool BackFaceCull(float fEdgeDotProduct0, float fEdgeDotProduct1, float fEdgeDotProduct2, float fBackFaceEpsilon)
{
    float3 f3BackFaceCull;
    
    f3BackFaceCull.x = ( fEdgeDotProduct0 > -fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    f3BackFaceCull.y = ( fEdgeDotProduct1 > -fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    f3BackFaceCull.z = ( fEdgeDotProduct2 > -fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    
    return all( f3BackFaceCull );
}

bool ViewFrustumCull(float3 f3EdgePosition0,         // World space position of patch control point 0
					 float3 f3EdgePosition1,         // World space position of patch control point 1
					 float3 f3EdgePosition2,         // World space position of patch control point 2
					 float4 f4ViewFrustumPlanes[4],  // 4 plane equations (left, right, top, bottom)
					 float fCullEpsilon)             // Epsilon to determine the distance outside the view frustum is still considered inside
{    
    float4 f4PlaneTest;
    float fPlaneTest;
    
    // Left clip plane
    f4PlaneTest.x = ( ( DistanceFromPlane( f3EdgePosition0, f4ViewFrustumPlanes[0]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition1, f4ViewFrustumPlanes[0]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition2, f4ViewFrustumPlanes[0]) > -fCullEpsilon ) ? 1.0f : 0.0f );
    // Right clip plane
    f4PlaneTest.y = ( ( DistanceFromPlane( f3EdgePosition0, f4ViewFrustumPlanes[1]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition1, f4ViewFrustumPlanes[1]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition2, f4ViewFrustumPlanes[1]) > -fCullEpsilon ) ? 1.0f : 0.0f );
    // Top clip plane
    f4PlaneTest.z = ( ( DistanceFromPlane( f3EdgePosition0, f4ViewFrustumPlanes[2]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition1, f4ViewFrustumPlanes[2]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition2, f4ViewFrustumPlanes[2]) > -fCullEpsilon ) ? 1.0f : 0.0f );
    // Bottom clip plane
    f4PlaneTest.w = ( ( DistanceFromPlane( f3EdgePosition0, f4ViewFrustumPlanes[3]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition1, f4ViewFrustumPlanes[3]) > -fCullEpsilon ) ? 1.0f : 0.0f ) +
                    ( ( DistanceFromPlane( f3EdgePosition2, f4ViewFrustumPlanes[3]) > -fCullEpsilon ) ? 1.0f : 0.0f );
        
    // Triangle has to pass all 4 plane tests to be visible
    return !all( f4PlaneTest );
}

