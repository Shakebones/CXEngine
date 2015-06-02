/*
 * File name - GEModel.h
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc - 
 *
 */
#ifndef _GE_MODEL_H_
#define _GE_MODEL_H_

#ifdef _WIN32
#include <d3dx9.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include "Utility.h"
#include "GETexture.h"
#include "GEMultiTextureSequence.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEModel
 * Author - CuiXue
 * Updated date - 2012.11.29
 * Desc
 *
 *
 */
class CGEModel
{
public:
	enum VertexType
	{
		NONE,
		Position,
		PTN,
		PT,
		PN,
		PC,
		PTN_Binormal_Tangent,
		PTN_Tangent
	};

	typedef struct _instanceType
	{
		D3DXVECTOR3 position;
	}InstanceType, *pInstanceType;

public:
	
	// Constructors & destructor
	CGEModel();
	CGEModel(const CGEModel&);
	CGEModel& operator=(const CGEModel&);
	~CGEModel();

	// Initialize buffers
	bool InitializeBox(ID3D11Device*, D3DXVECTOR3);
	bool InitializeBox(ID3D11Device*, D3DXVECTOR3, VertexType);
	bool InitializeSphere(ID3D11Device*, float, int, int);
	bool InitializeSphere(ID3D11Device*, float, int, int, VertexType);
	bool InitializeTriangle(ID3D11Device*);
	bool InitializeTriangle(ID3D11Device*, VertexType);
	bool InitializePlane(ID3D11Device*, int, int, float, float);
	bool InitializePlane(ID3D11Device*, int, int, float, float, VertexType);

	// From model file.
	bool InitializeBumpMapVertexBuffer(ID3D11Device*, char*);
	bool InitializeVTNVertexBuffer(ID3D11Device*, char*);
	bool InitializeVTVertexBuffer(ID3D11Device*, char*);
	bool InitializeVertexBuffer(ID3D11Device*, char*, WCHAR*);
	bool InitializTangentVertexBuffer(ID3D11Device*, char*);

	// About model's texture.
	bool SetTexture(ID3D11Device*, WCHAR*);
	bool SetMultiTexture(ID3D11Device*, ID3D11DeviceContext*, int, CGEMultiTextureSequence*);
	void ReleaseMultiTexture();

	// Start up instancing (DX 11 Features)
	void SetNumberOfInstances(int);
	void SetInstanceParameters(InstanceType*);
	bool InitializeInstanceBuffer(ID3D11Device*, char*);

	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void RenderSwitch(ID3D11DeviceContext*, VertexType);
	void RenderInstanceSwitch(ID3D11DeviceContext*, VertexType);
	void RenderTessellation(ID3D11DeviceContext*, VertexType);

	// Calculate
	void CalculateTangentSpace();
	void CalculateModelVectors();
	void CalculateModelVectors_VTN();
	void CalculateTangentBinormal(GEVertex3D_Normal_Tex, GEVertex3D_Normal_Tex, GEVertex3D_Normal_Tex,
								  D3DXVECTOR3&, D3DXVECTOR3&);
	void CalculateNormal(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR3&);

	// Access functions.
	bool CopyFrom(ID3D11Device*, ID3D11DeviceContext*, const CGEModel*);
	int GetIndexCount();
	int GetVertexCount();
	int GetTrianglesCount();
	int GetInstanceCount();
	ID3D11ShaderResourceView** GetMultiTextureSequence(int);
	ID3D11ShaderResourceView* GetTexture();
	WCHAR* GetTexturePath();

private:

	// Kernels

	// Doing.
	bool _initialize_vertex_buffer_bumpmap(ID3D11Device*);

	// Done
	bool _initialize_vertex_buffer(ID3D11Device*);
	bool _initialize_vertex_vt_buffer(ID3D11Device*);
	bool _inittialize_tangent_vertex_buffer(ID3D11Device*);

	// Fundamental elements
	bool _initialize_box(ID3D11Device*, float, float, float);
	bool _initialize_box(ID3D11Device*, float, float, float, VertexType);
	bool _initialize_sphere(ID3D11Device*, float, int, int);
	bool _initialize_sphere(ID3D11Device*, float, int, int, VertexType);
	bool _initialize_triangle(ID3D11Device*);
	bool _initialize_triangle(ID3D11Device*, VertexType);
	bool _initialize_plane(ID3D11Device*, int, int, float, float);
	bool _initialize_plane(ID3D11Device*, int, int, float, float, VertexType);
	// Fundamental elements END

	bool _load_model(char*);
	bool _load_vt_model(char*);
	bool _load_bumpmap_model(char*);
	bool _load_model_tangent(char*);

	// Instancing feature by DirectX 11 followed the following step to use.
	void _set_instance_number(int);
	bool _create_instance();
	bool _set_instance_parameters(InstanceType*);
	bool _initialize_instance_buffer(ID3D11Device*);

	bool _copy(ID3D11Device*, ID3D11DeviceContext*, const CGEModel*);
	void _release_model();
	bool _load_texture(ID3D11Device*, WCHAR*);
	bool _load_multitexture_sequence(ID3D11Device*, ID3D11ShaderResourceView**);
	void _release_texture();
	void _release_buffers();
	void _release_multitextures();
	void _render_buffers(ID3D11DeviceContext*);
	void _render_buffers_vt(ID3D11DeviceContext*);
	void _render_bumpmap_model(ID3D11DeviceContext*);
	void _render_vertex_type(ID3D11DeviceContext*, VertexType);
	void _render_instance_vertex_type(ID3D11DeviceContext*, VertexType);
	void _render_tessellation(ID3D11DeviceContext*, VertexType);

private:
	ID3D11Buffer*				m_pVertex_buffer;
	ID3D11Buffer*				m_pIndex_buffer;
	ID3D11Buffer*				m_instance_buffer;
	ModelType*					m_pModel;
	GEBumpMapModel*				m_pBumpMapModel;
	GETangentModel*				m_pTangentModel;
	ModelTypeVT*				m_pVTModel;
	CGETexture*					m_pTexture;
	CGEMultiTextureSequence*	m_pTextureSequence;
	WCHAR*						m_pTexturePath;
	VertexType					m_switch_vertex_type;
	int							m_vertexCount;
	int							m_indexCount;
	int							m_triangleCount;
	int							m_instance_number;
	InstanceType*				m_instance_type;
};

#endif