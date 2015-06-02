/********************************************************************************
 * File name - Utility.h
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc
 *
 * Utilities
 ********************************************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10math.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#endif

#include "MyVector.h"
#include "FPS.h"
#include "CPU.h"
#include "TIMER.h"

/****************************** Template functions ******************************
 * Desc -
 * High frequency used template
 ********************************************************************************/

template <typename T>
void ReleaseWithoutDel(T& t)
{
	if(t)
		(t)->Release();
	t = 0;
}

template <typename T>
void Shutdown(T& t)
{
	if(t)
	{
		t->Shutdown();
		delete t;
		t = NULL;
	}
}

template <typename T>
void Release(T& t)
{
	if(t)
	{
		t->Release();
		delete t;
		t = NULL;
	}
}

template <typename T>
void DeleteArray(T& t)
{
	if( t )
	{
		delete [] t;
		t = NULL;
	}
}

template <typename T>
void Delete(T& t)
{
	if( t )
		delete t;
	t = NULL;
}

template <typename T>
void ShutdownWithDelete(T& t)
{
	if( t )
	{
		t->Shutdown();
		delete t;
		t = NULL;
	}
}

template <typename T>
void ReleaseDXInput(T& t)
{
	if( t )
	{
		t->Unacquire();
		t->Release();
		t = 0;
	}
}

/********************************** Structures **********************************
 * Desc -
 * Important structures of Engine
 ********************************************************************************/
typedef struct _GE_VERTEX_3D_POSITION
{
#ifdef __cplusplus
	_GE_VERTEX_3D_POSITION()
	{
		position.x = 0.0f;
		position.y = 0.0f;
		position.z = 0.0f;
	}

	_GE_VERTEX_3D_POSITION(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}
#endif
	D3DXVECTOR3 position;
}GEVertex3D_Position, *pGEVertex3D_Position;

typedef struct _GE_VERTEX_3D_WITH_COLOR
{
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
}GEVertex3D_COLOR, *PGEVertex3D_COLOR;

typedef struct _GE_VERTEX_3D_WITH_NORMAL_TEX
{
#ifdef __cplusplus
public:
	_GE_VERTEX_3D_WITH_NORMAL_TEX(){}
	_GE_VERTEX_3D_WITH_NORMAL_TEX(D3DXVECTOR3 pos, D3DXVECTOR2 tex, D3DXVECTOR3 n)
	{
		position = pos;
		texture = tex;
		normal = n;
	}
#endif

	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;
}GEVertex3D_Normal_Tex, *PGEVertex3D_Normal_Tex;

typedef struct _GE_VERTEX_3D_WITH_TEX
{
#ifdef __cplusplus
public:
	_GE_VERTEX_3D_WITH_TEX(){}
	_GE_VERTEX_3D_WITH_TEX(D3DXVECTOR3 pos, D3DXVECTOR2 tex)
	{
		position = pos;
		texture = tex;
	}
#endif

	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
}GEVertex3D_Tex, *PGEVertex3D_Tex;

typedef struct _GE_VERTEX_3D_TEX_NORMAL_TANGENT_BINORMAL
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;
}GEVertex3D_AdvancedTextureTech, *pGEVertex3D_AdvancedTexgureTech;

typedef struct _tangent_space
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 binormal;
	D3DXVECTOR3 tangent;
}GEVertex3D_TangentSpace, *pGEVertex3D_TangentSpace;

typedef struct _font_type
{
public:
	_font_type()
	{
		left = right = 0.0f;
		int size = 0;
	}

	float	left;
	float	right;
	int		size;
}GEFontType, *PGEFontType;

typedef struct _model_type
{
#ifdef __cpluscplus
public:
	_model_type()
	{
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
	}
#endif
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;

}ModelType;

typedef struct _model_type_vt
{
#ifdef __cpluscplus
public:
	_model_type()
	{
		x = y = z = 0.0f;
		tu = tv = 0.0f;
	}
#endif
	float x, y, z;
	float tu, tv;

}ModelTypeVT;

typedef struct _bumpmap_model_type
{
#ifdef __cplusplus
public:
	_bumpmap_model_type()
	{
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
		tx = ty = tz = 0.0f;
		bx = by = bz = 0.0f;
	}
#endif
	
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;

}GEBumpMapModel, *pGEBumpMapModel;

typedef struct _tangent_model_type
{
#ifdef __cplusplus
public:
	_tangent_model_type()
	{
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
		bx = by = bz = 0.0f;
		tx = ty = tz = 0.0f;
	}
#endif
	
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float bx, by, bz;
	float tx, ty, tz;

}GETangentModel, *pGETangentModel;

typedef struct _matrix_buffer_type
{
	D3DXMATRIX			world;
	D3DXMATRIX			view;
	D3DXMATRIX			proj;
}MatrixBufferType, *pMatrixBufferType;

typedef struct _light_buffer_type
{
	D3DXVECTOR4			ambientColor;
	D3DXVECTOR4			diffuseColor;
	D3DXVECTOR3			direction;
	float				specularPower;
	D3DXVECTOR4			specularColor;
}LightBufferType;

typedef struct _light_buffer_with_diffuse
{
	D3DXVECTOR4 diffuseColor;
	D3DXVECTOR3 lightDir;
	float padding;
}LightDiffuse, *pLightDiffuse;

typedef struct _camera_buffer_type
{
#ifdef __cplusplus
public:
	_camera_buffer_type(){}
	_camera_buffer_type(D3DXVECTOR3 cameraPos)
	{
		cameraPosition = cameraPos;
	}
#endif

	D3DXVECTOR3		cameraPosition;
	float			padding;
}CameraBufferType, *PCameraBufferType;

typedef struct _pixel_buffer_type
{
	D3DXVECTOR4		pixelColor;
}PixelBufferType, PPixelBufferType;

typedef struct _stence_type
{
	ID3D11Buffer*	pVB;
	ID3D11Buffer*	pIB;
	int				vertexCount;
	int				indexCount;
	int				maxLength;
	int				guid;
	float			r;
	float			g;
	float			b;
}SentenceType, *pSentenceType;

typedef struct _wave_header_type
{
	char				chunkID[4];
	unsigned long		chunkSize;
	char				format[4];
	char				subChunkId[4];
	unsigned long		subChunkSize;
	unsigned short		audioFormat;
	unsigned short		numChannels;
	unsigned long		sampleRate;
	unsigned long		bytesPerSecond;
	unsigned short		blockAlign;
	unsigned short		bitsPerSample;
	char				dataChunkId[4];
	unsigned long		dataSize;	
}WaveHeaderType, *pWaveHeaderType;

typedef struct _wave_header_type_ext
{
	char				chunkID[4];
	unsigned long		chunkSize;
	char				format[4];
	char				subChunkId[4];
	unsigned long		subChunkSize;
	unsigned short		audioFormat;
	unsigned short		numChannels;
	unsigned long		sampleRate;
	unsigned long		bytesPerSecond;
	unsigned short		blockAlign;
	unsigned short		bitsPerSample;
	char				fact[12];
	char				dataChunkId[4];
	unsigned long		dataSize;
}WaveHeaderTypeExt, *pWaveHeaderTypeExt;

typedef struct _ssao_params
{
#ifdef __cplusplus
public:
	_ssao_params()
	{
		_random_size = 0.0f;
		_sample_rad = 0.0f;
		_intensity = 0.0f;
		_scale = 0.0f;
		_bias = 0.0f;
	}

#endif
	float _random_size;
	float _screen_size;
	float _sample_rad;
	float _intensity;
	float _scale;
	float _bias;
}GE_SSAO_Params, *pGE_SSAO_Params;

typedef struct _tessellation_buffer_type
{
	float tessellationAmount;
	D3DXVECTOR3 padding;
}GE_TessellationBufferType;

typedef struct _update_constant_buffer
{
	D3DXMATRIXA16    mWorld;                         // World matrix
    D3DXMATRIXA16    mView;                          // View matrix
    D3DXMATRIXA16    mProjection;                    // Projection matrix
    D3DXMATRIXA16    mWorldViewProjection;           // WVP matrix
    D3DXMATRIXA16    mViewProjection;                // VP matrix
    D3DXMATRIXA16    mInvView;                       // Inverse of view matrix
    D3DXVECTOR4      vScreenResolution;              // Screen resolution
    D3DXVECTOR4      vMeshColor;                     // Mesh color
    D3DXVECTOR4      vTessellationFactor;            // Edge, inside, minimum tessellation factor and 1/desired triangle size
    D3DXVECTOR4      vDetailTessellationHeightScale; // Height scale for detail tessellation of grid surface
    D3DXVECTOR4      vGridSize;                      // Grid size
}GE_UpdateConstantBuffer;

typedef struct _material_cb_struct
{
	D3DXVECTOR4     g_materialAmbientColor;  // Material's ambient color
    D3DXVECTOR4     g_materialDiffuseColor;  // Material's diffuse color
    D3DXVECTOR4     g_materialSpecularColor; // Material's specular color
    D3DXVECTOR4     g_fSpecularExponent;     // Material's specular exponent

    D3DXVECTOR4     g_LightPosition;         // Light's position in world space
    D3DXVECTOR4     g_LightDiffuse;          // Light's diffuse color
    D3DXVECTOR4     g_LightAmbient;          // Light's ambient color

    D3DXVECTOR4     g_vEye;                  // Camera's location
    D3DXVECTOR4     g_fBaseTextureRepeat;    // The tiling factor for base and normal map textures
    D3DXVECTOR4     g_fPOMHeightMapScale;    // Describes the useful range of values for the height field

    D3DXVECTOR4     g_fShadowSoftening;      // Blurring factor for the soft shadows computation

    int             g_nMinSamples;           // The minimum number of samples for sampling the height field
    int             g_nMaxSamples;           // The maximum number of samples for sampling the height field
    int             uDummy1;
    int             uDummy2;
}GE_Material_ConstantBufferType;


#endif