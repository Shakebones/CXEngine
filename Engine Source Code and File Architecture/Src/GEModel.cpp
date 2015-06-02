/*
 * File name - GEModel.cpp
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc
 *
 * Definition
 */
#include "GEModel.h"

////////////////////////////////////////////////////////////////
////////////////////////// Interfaces //////////////////////////

CGEModel::CGEModel()
{
	this->m_switch_vertex_type = NONE;
	this->m_indexCount = 0;
	this->m_vertexCount = 0;
	this->m_pIndex_buffer = 0;
	this->m_pVertex_buffer = 0;
	this->m_instance_buffer = 0;
	this->m_triangleCount = 0;
	this->m_pModel = 0;
	this->m_pBumpMapModel = 0;
	this->m_pTangentModel = 0;
	this->m_pVTModel = 0;
	this->m_pTexture = 0;
	this->m_pTextureSequence = 0;
	this->m_instance_number = 0;
	this->m_instance_type = 0;
}

CGEModel::CGEModel(const CGEModel& source)
{
}

CGEModel& CGEModel::operator=(const CGEModel& source)
{
	return *this;
}

CGEModel::~CGEModel()
{
}

bool CGEModel::CopyFrom(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CGEModel* pSource)
{
	bool result;

	result = this->_copy(pDevice, pDeviceContext, pSource);
	if( !result )
		return false;

	return true;
}

bool CGEModel::InitializeVertexBuffer(ID3D11Device* pDevice, char* model_filename, WCHAR* texture_filename)
{
	bool result;

	// Load model file
	result = this->_load_model(model_filename);
	if( FAILED(result) )
		return false;

	// According to file data to initialize vertex buffer.
	result = this->_initialize_vertex_buffer(pDevice);
	if( !result )
		return false;

	// Load texture file
	result = this->_load_texture(pDevice, texture_filename);
	if( FAILED(result) )
		this->m_pTexture = NULL;

	this->m_switch_vertex_type = PTN;

	return true;
}

bool CGEModel::InitializeBumpMapVertexBuffer(ID3D11Device* pDevice, char* pModelFilename)
{
	bool result;
	result = this->_load_bumpmap_model(pModelFilename);
	if( !result )
		return false;

	this->CalculateModelVectors();

	result = this->_initialize_vertex_buffer_bumpmap(pDevice);
	if( FAILED(result) )
		return false;

	this->m_switch_vertex_type = PTN_Binormal_Tangent;

	return true;
}

bool CGEModel::InitializTangentVertexBuffer(ID3D11Device* pDevice, char* pModelFilename)
{
	bool result;

	result = this->_load_model_tangent(pModelFilename);
	if( !result )
		return false;

	this->CalculateTangentSpace();

	result = this->_inittialize_tangent_vertex_buffer(pDevice);
	if( !result )
		return false;

	this->m_switch_vertex_type = PTN_Tangent;

	return true;
}

bool CGEModel::InitializeVTNVertexBuffer(ID3D11Device* pDevice, char* pModelFilename)
{
	bool result;

	result = this->_load_model(pModelFilename);
	if( !result )
		return false;

	result = this->_initialize_vertex_buffer(pDevice);
	if( !result )
		return false;

	this->m_switch_vertex_type = PTN;

	return true;
}

bool CGEModel::InitializeVTVertexBuffer(ID3D11Device* pDevice, char* pModelFilename)
{
	bool result;

	result = this->_load_vt_model(pModelFilename);
	if( !result )
		return false;

	result = this->_initialize_vertex_vt_buffer(pDevice);
	if( !result )
		return false;

	this->m_switch_vertex_type = PT;

	return true;
}

bool CGEModel::InitializeTriangle(ID3D11Device* pDevice)
{
	if( !this->_initialize_triangle(pDevice) )
		return false;

	this->m_switch_vertex_type = PTN;

	return true;
}

bool CGEModel::InitializeTriangle(ID3D11Device* pDevice, VertexType vt)
{
	if( !this->_initialize_triangle(pDevice, vt) )
		return false;

	this->m_switch_vertex_type = vt;

	return true;
}

bool CGEModel::InitializeBox(ID3D11Device* pDevice, D3DXVECTOR3 shape)
{
	if( !this->_initialize_box(pDevice, shape.x, shape.y, shape.z) )
		return false;

	return true;
}

bool CGEModel::InitializeBox(ID3D11Device* pDevice, D3DXVECTOR3 shape, VertexType vt)
{
	if( !this->_initialize_box(pDevice, shape.x, shape.y, shape.z, vt) )
		return false;

	this->m_switch_vertex_type = vt;

	return true;
}

bool CGEModel::InitializeSphere(ID3D11Device* pDevice, float radius, int slices, int stack)
{
	if( !this->_initialize_sphere(pDevice, radius, slices, stack) )
		return false;

	this->m_switch_vertex_type = PTN;

	return true;
}

bool CGEModel::InitializeSphere(ID3D11Device* pDevice, float radius, int slices, int stack, VertexType vt)
{
	if( !this->_initialize_sphere(pDevice, radius, slices, stack, vt) )
		return false;

	this->m_switch_vertex_type = vt;
	
	return true;
}

bool CGEModel::InitializePlane(ID3D11Device* pDevice, int row, int col, float rowStride, float colStride)
{
	bool result;
	result = this->_initialize_plane(pDevice, row, col, rowStride, colStride);
	if( !result )
		return false;

	this->m_switch_vertex_type = PTN;

	return true;
}

bool CGEModel::InitializePlane(ID3D11Device* pDevice, int row, int col, float rowStride, float colStride, VertexType vt)
{
	if( !this->_initialize_plane(pDevice, row, col, rowStride, colStride, vt) )
		return false;

	this->m_switch_vertex_type = vt;

	return true;
}

bool CGEModel::SetTexture(ID3D11Device* pDevice, WCHAR* texFilename)
{
	bool result;
	result = this->_load_texture(pDevice, texFilename);
	if( !result )
		return false;

	this->m_pTexturePath = texFilename;

	return true;
}

bool CGEModel::SetMultiTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, int numOfTextures, CGEMultiTextureSequence* pTextures)
{
	bool result;

	if( NULL == pDevice || NULL == pTextures || numOfTextures > 128 ||
		numOfTextures <= 0 )
		return false;

	this->m_pTextureSequence = new CGEMultiTextureSequence;
	if( !this->m_pTextureSequence )
		return false;

	result = this->m_pTextureSequence->Initialize(pDevice, pDeviceContext);
	if( !result )
		return false;

	this->m_pTextureSequence->CopyFrom(*pTextures);

	return true;
}

void CGEModel::ReleaseMultiTexture()
{
	this->_release_multitextures();
}

void CGEModel::SetNumberOfInstances(int number_of_instance)
{
	this->_set_instance_number(number_of_instance);
	this->_create_instance();
}

void CGEModel::SetInstanceParameters(InstanceType* instance_parameters)
{
	this->_set_instance_parameters(instance_parameters);
}

bool CGEModel::InitializeInstanceBuffer(ID3D11Device* pDevice, char* model_filename)
{
		bool result;

	// Load model file
	result = this->_load_model(model_filename);
	if( FAILED(result) )
		return false;

	// According to file data to initialize vertex buffer.
	result = this->_initialize_vertex_buffer(pDevice);
	if( !result )
		return false;

	result = this->_initialize_instance_buffer(pDevice);
	if( !result )
		return false;

	this->m_switch_vertex_type = PTN;

	return true;
}

/*
 * Interfaces -
 * Box
 * Sphere
 * Triangle
 * Planes
 * .... Modifying ....
 */

void CGEModel::Shutdown()
{
	this->_release_texture();
	this->_release_buffers();
	this->_release_model();
	this->_release_multitextures();
	return;
}

/*
 * Desc -
 * 作针对不同模型的开关
 * 例如：
 * 1. 针对普通模型的 Render() 函数
 * 2. 针对带有复杂顶点属性的模型 Render() 函数
 *    2.1 Bump mapping Model -> Render(pDeviceContext)
 * 
 * 状态 ： 修改中
 */
void CGEModel::Render(ID3D11DeviceContext* pDeviceContext)
{
	if( Position == this->m_switch_vertex_type )
		this->_render_vertex_type(pDeviceContext, Position);

	if( PTN == this->m_switch_vertex_type )
		this->_render_vertex_type(pDeviceContext, PTN);

	if( PT == this->m_switch_vertex_type )
		this->_render_vertex_type(pDeviceContext, PT);

	if( PTN_Binormal_Tangent == this->m_switch_vertex_type )
		this->_render_vertex_type(pDeviceContext, PTN_Binormal_Tangent);

	return;
}

int CGEModel::GetIndexCount()
{
	return this->m_indexCount;
}

int CGEModel::GetVertexCount()
{
	return this->m_vertexCount;
}

int CGEModel::GetInstanceCount()
{
	return this->m_instance_number;
}

int CGEModel::GetTrianglesCount()
{
	return this->m_triangleCount;
}

ID3D11ShaderResourceView* CGEModel::GetTexture()
{
	return this->m_pTexture->GetTexture();
}

WCHAR* CGEModel::GetTexturePath()
{
	return this->m_pTexturePath;
}

/////////////////////////////////////////////////////////////
////////////////////////// Kernels //////////////////////////

bool CGEModel::_initialize_vertex_buffer(ID3D11Device* pDevice)
{
	HRESULT result;
	D3D11_BUFFER_DESC vbDesc;		// Vertex buffer
	D3D11_BUFFER_DESC ibDesc;		// Index buffer
	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;
	GEVertex3D_Normal_Tex* pVertices = NULL;
	unsigned long* pIndices = NULL;

	// According to m_vertexCount creating vertex buffer
	pVertices = new GEVertex3D_Normal_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	// According to m_indexCount creating index buffer
	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	this->m_triangleCount = this->m_vertexCount / 3;

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		pVertices[i].position = D3DXVECTOR3(this->m_pModel[i].x, this->m_pModel[i].y, this->m_pModel[i].z);
		pVertices[i].texture = D3DXVECTOR2(this->m_pModel[i].tu, this->m_pModel[i].tv);
		pVertices[i].normal = D3DXVECTOR3(this->m_pModel[i].nx, this->m_pModel[i].ny, this->m_pModel[i].nz);

		pIndices[i] = i;
	}
	
	// Fill out vertex buffer description
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * this->m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	vData.pSysMem = pVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&vbDesc, &vData, &this->m_pVertex_buffer);
	if( FAILED(result) )
		return false;

	// Fill out index buffer description
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.StructureByteStride = 0;

	iData.pSysMem = pIndices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&ibDesc, &iData, &this->m_pIndex_buffer);
	if( FAILED( result ) )
		return false;

	DeleteArray<GEVertex3D_Normal_Tex*>(pVertices);
	DeleteArray<unsigned long*>(pIndices);

	return true;
}

bool CGEModel::_initialize_vertex_vt_buffer(ID3D11Device* pDevice)
{
	HRESULT result;
	D3D11_BUFFER_DESC vbDesc;		// Vertex buffer
	D3D11_BUFFER_DESC ibDesc;		// Index buffer
	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;
	GEVertex3D_Tex* pVertices = NULL;
	unsigned long* pIndices = NULL;

	// According to m_vertexCount creating vertex buffer
	pVertices = new GEVertex3D_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	// According to m_indexCount creating index buffer
	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	this->m_triangleCount = this->m_vertexCount / 3;

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		pVertices[i].position = D3DXVECTOR3(this->m_pVTModel[i].x, this->m_pVTModel[i].y, this->m_pVTModel[i].z);
		pVertices[i].texture = D3DXVECTOR2(this->m_pVTModel[i].tu, this->m_pVTModel[i].tv);

		pIndices[i] = i;
	}
	
	// Fill out vertex buffer description
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(GEVertex3D_Tex) * this->m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	vData.pSysMem = pVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&vbDesc, &vData, &this->m_pVertex_buffer);
	if( FAILED(result) )
		return false;

	// Fill out index buffer description
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.StructureByteStride = 0;

	iData.pSysMem = pIndices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&ibDesc, &iData, &this->m_pIndex_buffer);
	if( FAILED( result ) )
		return false;

	DeleteArray<GEVertex3D_Tex*>(pVertices);
	DeleteArray<unsigned long*>(pIndices);

	return true;
}

bool CGEModel::_initialize_vertex_buffer_bumpmap(ID3D11Device* pDevice)
{
	GEVertex3D_AdvancedTextureTech* pVertices;
	unsigned long* pIndices;

	D3D11_BUFFER_DESC vbDesc;
	D3D11_BUFFER_DESC ibDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	pVertices = new GEVertex3D_AdvancedTextureTech[this->m_vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	for(int i = 0; i != this->m_vertexCount; i++)
	{
		pVertices[i].position = D3DXVECTOR3(this->m_pBumpMapModel[i].x, this->m_pBumpMapModel[i].y, this->m_pBumpMapModel[i].z);
		pVertices[i].texture = D3DXVECTOR2(this->m_pBumpMapModel[i].tu, this->m_pBumpMapModel[i].tv);
		pVertices[i].normal = D3DXVECTOR3(this->m_pBumpMapModel[i].nx, this->m_pBumpMapModel[i].ny, this->m_pBumpMapModel[i].nz);
		pVertices[i].tangent = D3DXVECTOR3(this->m_pBumpMapModel[i].tx, this->m_pBumpMapModel[i].ty, this->m_pBumpMapModel[i].tz);
		pVertices[i].binormal = D3DXVECTOR3(this->m_pBumpMapModel[i].bx, this->m_pBumpMapModel[i].by, this->m_pBumpMapModel[i].bz);

		pIndices[i] = i;
	}

	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(GEVertex3D_AdvancedTextureTech) * this->m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&vbDesc, &vertexData, &this->m_pVertex_buffer);
	if( FAILED(result) )
		return false;

	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&ibDesc, &indexData, &this->m_pIndex_buffer);
	if( FAILED(result) )
		return false;

	::DeleteArray<GEVertex3D_AdvancedTextureTech*>(pVertices);
	::DeleteArray<unsigned long*>(pIndices);

	return true;
}

bool CGEModel::_inittialize_tangent_vertex_buffer(ID3D11Device* pDevice)
{
	GEVertex3D_TangentSpace* pVertices;
	unsigned long* pIndices;

	D3D11_BUFFER_DESC vbDesc;
	D3D11_BUFFER_DESC ibDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	pVertices = new GEVertex3D_TangentSpace[this->m_vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	for(int i = 0; i != this->m_vertexCount; i++)
	{
		pVertices[i].position = D3DXVECTOR3(this->m_pTangentModel[i].x, this->m_pTangentModel[i].y, this->m_pTangentModel[i].z);
		pVertices[i].texture = D3DXVECTOR2(this->m_pTangentModel[i].tu, this->m_pTangentModel[i].tv);
		pVertices[i].normal = D3DXVECTOR3(this->m_pTangentModel[i].nx, this->m_pTangentModel[i].ny, this->m_pTangentModel[i].nz);
		pVertices[i].tangent = D3DXVECTOR3(this->m_pTangentModel[i].tx, this->m_pTangentModel[i].ty, this->m_pTangentModel[i].tz);
		pVertices[i].binormal = D3DXVECTOR3(this->m_pTangentModel[i].bx, this->m_pTangentModel[i].by, this->m_pTangentModel[i].bz);

		pIndices[i] = i;
	}

	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(GEVertex3D_TangentSpace) * this->m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&vbDesc, &vertexData, &this->m_pVertex_buffer);
	if( FAILED(result) )
		return false;

	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&ibDesc, &indexData, &this->m_pIndex_buffer);
	if( FAILED(result) )
		return false;

	::DeleteArray<GEVertex3D_TangentSpace*>(pVertices);
	::DeleteArray<unsigned long*>(pIndices);

	return true;
}

/*
 * Kernel -
 * Box
 * Sphere
 * Triangle
 * Plane
 * .... Modifying ....
 */
bool CGEModel::_initialize_triangle(ID3D11Device* pDevice)
{
	GEVertex3D_Normal_Tex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = 3;

	// Set the number of indices in the index array.
	this->m_indexCount = 3;

	// Set number of triangle, here just one.
	this->m_triangleCount = 1;

	// Create the vertex array.
	vertices = new GEVertex3D_Normal_Tex[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);		// Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// Top middle.
	vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		// Bottom right.
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.	
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

 bool CGEModel::_initialize_triangle(ID3D11Device* pDevice, VertexType vt)
 {
	 if( NONE == vt )
		 return false;

	 unsigned long* indices;

	 // Vertex type set
	 GEVertex3D_Position* pVertices_position = 0;
	 GEVertex3D_Normal_Tex* pVertices_PTN = 0;
	 GEVertex3D_Tex* pVertices_PT = 0;
	 GEVertex3D_COLOR* pVertices_PC = 0;
	 GEVertex3D_AdvancedTextureTech* pVertices_PTN_Binormal_Tangent = 0;

	 if( NULL == pDevice )
		 return false;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = 3;

	// Set the number of indices in the index array.
	this->m_indexCount = 3;

	// Set number of triangle, here just one.
	this->m_triangleCount = 1;

	 switch( vt )
	 {
	 case Position:
		 pVertices_position = new GEVertex3D_Position[this->m_vertexCount];
		 if( !pVertices_position )
			 return false;
		 break;

	 case PTN:
		 pVertices_PTN = new GEVertex3D_Normal_Tex[this->m_vertexCount];
		 if( !pVertices_PTN )
			 return false;
		 break;

	 case PT:
		 pVertices_PT = new GEVertex3D_Tex[this->m_vertexCount];
		 if( !pVertices_PT )
			 return false;
		 break;

	 case PN:
		 break;

	 case PC:
		 pVertices_PC = new GEVertex3D_COLOR[this->m_vertexCount];
		 if( !pVertices_PC )
			 return false;
		 break;

	 case PTN_Binormal_Tangent:
		 pVertices_PTN_Binormal_Tangent = new GEVertex3D_AdvancedTextureTech[this->m_vertexCount];
		 if( !pVertices_PTN_Binormal_Tangent )
			 return false;
		 break;
	 };

	 // Create the index array.
	 indices = new unsigned long[m_indexCount];
	 if(!indices)
	 {
		 return false;
	 }

	 switch( vt )
	 {
	 case Position:
		 pVertices_position[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);		// Bottom left.
		 pVertices_position[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// Top middle.
		 pVertices_position[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		// Bottom right.
		 break;

	 case PTN:
		 // Load the vertex array with data.
		 pVertices_PTN[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);			// Bottom left.
		 pVertices_PTN[0].texture = D3DXVECTOR2(0.0f, 1.0f);
		 pVertices_PTN[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		 pVertices_PTN[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// Top middle.
		 pVertices_PTN[1].texture = D3DXVECTOR2(0.5f, 0.0f);
		 pVertices_PTN[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		 pVertices_PTN[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);			// Bottom right.
		 pVertices_PTN[2].texture = D3DXVECTOR2(1.0f, 1.0f);
		 pVertices_PTN[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		 break;

	 case PT:
		 // Load the vertex array with data.
		 pVertices_PT[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);			// Bottom left.
		 pVertices_PT[0].texture = D3DXVECTOR2(0.0f, 1.0f);
		 pVertices_PT[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// Top middle.
		 pVertices_PT[1].texture = D3DXVECTOR2(0.5f, 0.0f);
		 pVertices_PT[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);				// Bottom right.
		 pVertices_PT[2].texture = D3DXVECTOR2(1.0f, 1.0f);
		 break;

	 case PN:
		 break;

	 case PC:
		 pVertices_PC[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);			// Bottom left.
		 pVertices_PC[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		 pVertices_PC[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// Top middle.
		 pVertices_PC[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		 pVertices_PC[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);				// Bottom right.
		 pVertices_PC[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		break;

	case PTN_Binormal_Tangent:
		return false;
		break;
	};

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	 switch( vt )
	 {
	 case Position:
		 // Set up the description of the static vertex buffer.
		 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		 vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Position) * m_vertexCount;
		 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		 vertexBufferDesc.CPUAccessFlags = 0;
		 vertexBufferDesc.MiscFlags = 0;
		 vertexBufferDesc.StructureByteStride = 0;

		 // Give the subresource structure a pointer to the vertex data.
		 vertexData.pSysMem = pVertices_position;
		 vertexData.SysMemPitch = 0;
		 vertexData.SysMemSlicePitch = 0;
		 break;

	 case PTN:
		 // Set up the description of the static vertex buffer.
		 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		 vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * m_vertexCount;
		 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		 vertexBufferDesc.CPUAccessFlags = 0;
		 vertexBufferDesc.MiscFlags = 0;
		 vertexBufferDesc.StructureByteStride = 0;

		 // Give the subresource structure a pointer to the vertex data.
		 vertexData.pSysMem = pVertices_PTN;
		 vertexData.SysMemPitch = 0;
		 vertexData.SysMemSlicePitch = 0;
		 break;

	 case PT:
		 // Set up the description of the static vertex buffer.
		 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		 vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Tex) * m_vertexCount;
		 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		 vertexBufferDesc.CPUAccessFlags = 0;
		 vertexBufferDesc.MiscFlags = 0;
		 vertexBufferDesc.StructureByteStride = 0;

		 // Give the subresource structure a pointer to the vertex data.
		 vertexData.pSysMem = pVertices_PT;
		 vertexData.SysMemPitch = 0;
		 vertexData.SysMemSlicePitch = 0;
		 break;

	 case PN:
		 break;

	 case PC:
		 // Set up the description of the static vertex buffer.
		 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		 vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_COLOR) * m_vertexCount;
		 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		 vertexBufferDesc.CPUAccessFlags = 0;
		 vertexBufferDesc.MiscFlags = 0;
		 vertexBufferDesc.StructureByteStride = 0;

		 // Give the subresource structure a pointer to the vertex data.
		 vertexData.pSysMem = pVertices_PC;
		 vertexData.SysMemPitch = 0;
		 vertexData.SysMemSlicePitch = 0;
		 break;

	 case PTN_Binormal_Tangent:
		 return false;
	 };

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	::DeleteArray<GEVertex3D_Position*>(pVertices_position);
	::DeleteArray<GEVertex3D_Normal_Tex*>(pVertices_PTN);
	::DeleteArray<GEVertex3D_Tex*>(pVertices_PT);
	::DeleteArray<GEVertex3D_COLOR*>(pVertices_PC);
	::DeleteArray<unsigned long*>(indices);

	return true;
 }

bool CGEModel::_initialize_sphere(ID3D11Device* pDevice, float radius, int slices, int stack)	
{
	GEVertex3D_Normal_Tex*	vertices;
	GEVertex3D_Normal_Tex*	pVertices;
	unsigned long*		indices;
	unsigned long*		pIndices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = (stack + 1) * (slices + 1);

	// Set the number of indices in the index array.
	this->m_indexCount = (stack) * (slices + 1) * 2 * 3;

	// Set the number of triangles count.
	this->m_triangleCount = this->m_indexCount / 3;

	pVertices = new GEVertex3D_Normal_Tex[this->m_vertexCount];
	pIndices = new unsigned long[this->m_indexCount];
	vertices = pVertices;
	indices = pIndices;

	if( !vertices || !indices )
		return false;

	// Modifying...
	// Site - 
	float delta1;
	float delta2;

	delta1 = D3DX_PI / stack;
	delta2 = (2.0f * D3DX_PI) / slices;

	for(int i = 0; i < (stack + 1); i++)
	{
		float radius_of_each_stack = radius * sinf(i * delta1);
		float height = radius * cosf(i * delta1);

		for(int j = 0; j < (slices + 1); j++)
		{
			float x = radius_of_each_stack * sinf(j * delta2);
			float z = radius_of_each_stack * cosf(j * delta2);

			// Position data.
			vertices->position.x = x;
			vertices->position.y = height;
			vertices->position.z = z;

			// Normals.
			D3DXVec3Normalize(&vertices->normal, &vertices->position);

			// Texture coordinate.
			vertices->texture.x = (((slices + 1) - j) * delta2) / (2 * D3DX_PI);
			vertices->texture.y = (i * delta1) / (D3DX_PI);
			vertices++;
		}
	}

	// Generate index data.
	// - That's very important !
	// ...
	for(int i = 0; i < stack; i++)
	{
		for(int j = 0; j < slices + 1; j++)
		{
			*indices = j + i * (slices + 1);
			indices++;
			*indices = (j + 1) % (slices + 1) + i * (slices + 1);
			indices++;
			*indices = (j + 1) % (slices + 1) + (i + 1) * (slices + 1);
			indices++;

			*indices = j + i * (slices + 1);
			indices++;
			*indices = (j + 1) % (slices + 1) + (i + 1) * (slices + 1);
			indices++;
			*indices = j + (i + 1) * (slices + 1);
			indices++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_vertexCount * sizeof(GEVertex3D_Normal_Tex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = m_indexCount * sizeof(unsigned long);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] pVertices;
	vertices = 0;

	delete [] pIndices;
	indices = 0;

	return true;
}

bool CGEModel::_initialize_sphere(ID3D11Device* pDevice, float radius, int slices, int stack, VertexType vt)
{
	return true;
}

bool CGEModel::_initialize_box(ID3D11Device* pDevice, float width, float height, float depth)
{
	GEVertex3D_Normal_Tex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = 24;

	// Set the number of indices in the index array.
	this->m_indexCount = 36;

	// Set the number of triangles
	this->m_triangleCount = this->m_indexCount / 3;

	// Create the vertex array.
	vertices = new GEVertex3D_Normal_Tex[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Vertices data
	
	// Front
	vertices[0]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[1]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[2]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[3]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	
	// Back		  
	vertices[4]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / 2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[5]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / 2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[6]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / 2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[7]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / 2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
				 
	// Left		 
	vertices[8]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[9]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[10] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[11] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));

	// Right
	vertices[12] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[13] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[14] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[15] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	
	// Top
	vertices[16] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / 2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[17] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / 2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[18] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / 2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[19] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / 2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// Bottom
	vertices[20] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[21] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[22] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[23] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	
	 // fill in the front face index data
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    // fill in the back face index data
    indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
    indices[9] = 4; indices[10] = 6; indices[11] = 7;

    // fill in the left face index data
    indices[12] = 8; indices[13] =  9; indices[14] = 10;
    indices[15] = 8; indices[16] = 10; indices[17] = 11;

    // fill in the right face index data
    indices[18] = 12; indices[19] = 13; indices[20] = 14;
    indices[21] = 12; indices[22] = 14; indices[23] = 15;

    // fill in the top face index data
    indices[24] = 16; indices[25] = 17; indices[26] = 18;
    indices[27] = 16; indices[28] = 18; indices[29] = 19;

    // fill in the bottom face index data
    indices[30] = 20; indices[31] = 21; indices[32] = 22;
    indices[33] = 20; indices[34] = 22; indices[35] = 23;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.	
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

bool CGEModel::_initialize_box(ID3D11Device* pDevice, float width, float height, float depth, VertexType vt)
{
	GEVertex3D_Normal_Tex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = 24;

	// Set the number of indices in the index array.
	this->m_indexCount = 36;

	// Set the number of triangles
	this->m_triangleCount = this->m_indexCount / 3;

	// Create the vertex array.
	vertices = new GEVertex3D_Normal_Tex[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Vertices data
	
	// Front
	vertices[0]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[1]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[2]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	vertices[3]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	
	// Back		  
	vertices[4]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / 2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[5]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / 2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[6]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / 2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	vertices[7]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / 2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
				 
	// Left		 
	vertices[8]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[9]  = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[10] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	vertices[11] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));

	// Right
	vertices[12] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[13] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[14] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height /  2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	vertices[15] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	
	// Top
	vertices[16] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / 2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[17] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / 2.0f, depth /  2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[18] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / 2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	vertices[19] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / 2.0f, depth / -2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// Bottom
	vertices[20] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[21] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth / -2.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[22] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width /  2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertices[23] = GEVertex3D_Normal_Tex(D3DXVECTOR3(width / -2.0f, height / -2.0f, depth /  2.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	
	 // fill in the front face index data
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    // fill in the back face index data
    indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
    indices[9] = 4; indices[10] = 6; indices[11] = 7;

    // fill in the left face index data
    indices[12] = 8; indices[13] =  9; indices[14] = 10;
    indices[15] = 8; indices[16] = 10; indices[17] = 11;

    // fill in the right face index data
    indices[18] = 12; indices[19] = 13; indices[20] = 14;
    indices[21] = 12; indices[22] = 14; indices[23] = 15;

    // fill in the top face index data
    indices[24] = 16; indices[25] = 17; indices[26] = 18;
    indices[27] = 16; indices[28] = 18; indices[29] = 19;

    // fill in the bottom face index data
    indices[30] = 20; indices[31] = 21; indices[32] = 22;
    indices[33] = 20; indices[34] = 22; indices[35] = 23;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.	
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
	return true;
}

bool CGEModel::_initialize_plane(ID3D11Device* pDevice, int row, int col, float rowStride, float colStride)
{

	GEVertex3D_Normal_Tex* vertices;
	GEVertex3D_Normal_Tex* pVertices;
	unsigned long* indices;
	unsigned long* pIndices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	this->m_vertexCount = (row + 1) * (col + 1);

	// Set the number of indices in the index array.
	this->m_indexCount = row * col * 2 * 3;

	// Set the number of triangles.
	this->m_triangleCount = this->m_indexCount / 3;

	pVertices = new GEVertex3D_Normal_Tex[this->m_vertexCount];
	pIndices = new unsigned long[this->m_indexCount];
	vertices = pVertices;
	indices = pIndices;

	if( !vertices || !indices )
		return false;

	float segmentsRow = 1.0f / col;
	float segmentsCol = 1.0f / row;

	// Vertices.
	// Modifying...

	for(int i = 0; i < row + 1; i++)
	{
		for(int j = 0; j < col + 1; j++)
		{
			vertices->position.x = -((float)col) * (rowStride) / 2 + rowStride * (float)j;
			vertices->position.y = 0.0f;
			vertices->position.z = ((float)row) * (colStride) / 2 - colStride * (float)i;
			vertices->texture = D3DXVECTOR2((float)j, (float)i);
			vertices->normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vertices++;
		}
	}

	// Indices.
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			*indices = i * (col + 1) + j;
			indices++;
			*indices = i * (col + 1)  + j + 1;
			indices++;
			*indices = (i + 1) * (col + 1) + j;
			indices++;
			*indices = i * (col + 1) + (j + 1);
			indices++;
			*indices = (i + 1) * (col + 1) + j;
			indices++;
			*indices = (i + 1) * (col + 1) + (j + 1);
			indices++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Normal_Tex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndex_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.	
	delete [] pVertices;
	pVertices = 0;

	delete [] pIndices;
	pIndices = 0;

	return true;
}

bool CGEModel::_initialize_plane(ID3D11Device* pDevice, int row, int col, float rowStride, float colStride, VertexType vt)
{
	return true;
}

bool CGEModel::_copy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CGEModel* pSource)
{
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pVertex_buffer);
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pIndex_buffer);
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_instance_buffer);

	HRESULT result;

	this->m_vertexCount = pSource->m_vertexCount;
	this->m_indexCount = pSource->m_indexCount;
	this->m_instance_number = pSource->m_instance_number;
	this->m_triangleCount = pSource->m_triangleCount;
	this->m_switch_vertex_type = pSource->m_switch_vertex_type;

	D3D11_BUFFER_DESC vbDesc;
	D3D11_BUFFER_DESC instBufferDesc;
	D3D11_SUBRESOURCE_DATA  pInstanceData;
	D3D11_BUFFER_DESC ibDesc;

	// If source's vertex data & index data are empty, this function will not woking.
	if( NULL == pSource->m_pVertex_buffer ||
		NULL == pSource->m_pIndex_buffer )
		return false;

	// Copy vertex data & index data
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	switch(this->m_switch_vertex_type)
	{
	case Position:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GEVertex3D_Position);
		break;

	case PTN:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GEVertex3D_Normal_Tex);
		break;

	case PT:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GEVertex3D_Tex);
		break;

	case PC:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GEVertex3D_COLOR);
		break;

	case PTN_Binormal_Tangent:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GEBumpMapModel);
		break;

	case PTN_Tangent:
		vbDesc.ByteWidth = this->m_vertexCount * sizeof(GETangentModel);
	}

	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&vbDesc, NULL, &this->m_pVertex_buffer);
	if(FAILED(result) )
		return false;

	pDeviceContext->CopyResource(this->m_pVertex_buffer, pSource->m_pVertex_buffer);
	if( NULL == this->m_pVertex_buffer )
		return false;

	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = this->m_indexCount * sizeof(unsigned long);
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&ibDesc, NULL, &this->m_pIndex_buffer);
	if( FAILED(result) )
		return false;

	pDeviceContext->CopyResource(this->m_pIndex_buffer, pSource->m_pIndex_buffer);
	if( NULL == this->m_pIndex_buffer )
		return false;

	if( NULL != pSource->m_instance_buffer )
	{
		instBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		instBufferDesc.ByteWidth = sizeof(InstanceType) * this->m_instance_number;
		instBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instBufferDesc.CPUAccessFlags = 0;
		instBufferDesc.MiscFlags = 0;
		instBufferDesc.StructureByteStride = 0;

		// Copy instance params
		int capacity = pSource->m_instance_number;

		this->m_instance_type = new CGEModel::InstanceType[capacity];
		if( !this->m_instance_type )
			return false;

		memcpy(this->m_instance_type, pSource->m_instance_type, sizeof(CGEModel::InstanceType) * this->m_instance_number);

		pInstanceData.pSysMem = this->m_instance_type;
		pInstanceData.SysMemPitch = 0;
		pInstanceData.SysMemSlicePitch = 0;

		result = pDevice->CreateBuffer(&instBufferDesc, &pInstanceData, &this->m_instance_buffer);
		if( FAILED(result) )
			return false;

		pDeviceContext->CopyResource(this->m_instance_buffer, pSource->m_instance_buffer);
		if( NULL == this->m_instance_buffer )
			return false;
	}

	if( pSource->m_pTexturePath != NULL )
	{
		result = this->_load_texture(pDevice, pSource->m_pTexturePath);
		if( FAILED(result) )
		return false;
	}

	if( pSource->m_pTextureSequence != NULL )
	{
		this->m_pTextureSequence = new CGEMultiTextureSequence;
		if( !this->m_pTextureSequence )
			return false;

		result = this->m_pTextureSequence->Initialize(pDevice, pDeviceContext);
		if( !result )
			return false;

		result = this->m_pTextureSequence->CopyFrom(*pSource->m_pTextureSequence);
		if( !result )
			return false;
	}	

	// Desc -
	// 如果 “源” 中的模型数据为文件导入，则需拷贝
	// 指向已读取文件数据的指针成员（如m_pModel）
	if( pSource->m_pModel != NULL )
	{
		int size = sizeof(ModelType) * pSource->m_vertexCount;

		this->m_pModel = new ModelType[this->m_vertexCount];
		if( !this->m_pModel )
			return false;

		::memcpy(this->m_pModel, pSource->m_pModel, size);
	}

	if( pSource->m_switch_vertex_type == PTN_Binormal_Tangent )
	{
		int size = sizeof(GEBumpMapModel) * this->m_vertexCount;

		this->m_pBumpMapModel = new GEBumpMapModel[this->m_vertexCount];
		if( !this->m_pBumpMapModel )
			return false;

		::memcpy(this->m_pBumpMapModel, pSource->m_pBumpMapModel, size);
	}

	if( pSource->m_switch_vertex_type == PTN_Tangent )
	{
		int size = sizeof(GETangentModel) * this->m_vertexCount;
		this->m_pTangentModel = new GETangentModel[this->m_vertexCount];
		if( !this->m_pTangentModel )
			return false;

		::memcpy(this->m_pTangentModel, pSource->m_pTangentModel, size);
	}

	return true;
}

inline void CGEModel::_release_buffers()
{
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pVertex_buffer);
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pIndex_buffer);
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_instance_buffer);
}

void CGEModel::_render_buffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_Normal_Tex);
	offset = 0;

	// Set vertex buffer.
	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertex_buffer, &stride, &offset);
	// Set index buffer.
	pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	// Choose primitive.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void CGEModel::_render_buffers_vt(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(ModelTypeVT);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void CGEModel::_render_bumpmap_model(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_AdvancedTextureTech);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool CGEModel::_load_texture(ID3D11Device* pDevice, WCHAR* filename)
{
	bool result;

	this->m_pTexturePath = filename;

	this->m_pTexture = new CGETexture;
	if( !this->m_pTexture )
		return false;

	result = this->m_pTexture->Initialize(pDevice, filename);
	if( !result )
		return false;

	return true;
}

inline void CGEModel::_release_texture()
{
	ShutdownWithDelete<CGETexture*>(this->m_pTexture);
	this->m_pTexturePath = NULL;
}

bool CGEModel::_load_model(char* filename)
{
	/*
	 * Description -
	 * To understand how this func work,
	 * you should know the model file's 
	 * format.
	 * 
	 * PS - We use model file with '*.txt'
	 */
	ifstream fin;
	char input;

	fin.open(filename);
	if( fin.fail() )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin >> this->m_vertexCount;
	this->m_indexCount = this->m_vertexCount;

	this->m_pModel = new ModelType[this->m_vertexCount];
	if( !m_pModel )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin.get(input);
	fin.get(input);

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		fin >> this->m_pModel[i].x >> this->m_pModel[i].y >> this->m_pModel[i].z;
		fin >> this->m_pModel[i].tu >> this->m_pModel[i].tv;
		fin >> this->m_pModel[i].nx >> this->m_pModel[i].ny >> this->m_pModel[i].nz;
	}

	fin.close();

	return true;
}

bool CGEModel::_load_vt_model(char* filename)
{
	/*
	 * Description -
	 * To understand how this func work,
	 * you should know the model file's 
	 * format.
	 * 
	 * PS - We use model file with '*.txt'
	 */
	ifstream fin;
	char input;

	fin.open(filename);
	if( fin.fail() )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin >> this->m_vertexCount;
	this->m_indexCount = this->m_vertexCount;

	this->m_pVTModel = new ModelTypeVT[this->m_vertexCount];
	if( !this->m_pVTModel )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin.get(input);
	fin.get(input);

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		fin >> this->m_pVTModel[i].x >> this->m_pVTModel[i].y >> this->m_pVTModel[i].z;
		fin >> this->m_pVTModel[i].tu >> this->m_pVTModel[i].tv;
	}

	fin.close();

	return true;
}

bool CGEModel::_load_bumpmap_model(char* filename)
{
	/*
	 * Description -
	 * To understand how this func work,
	 * you should know the model file's 
	 * format.
	 * 
	 * PS - We use model file with '*.txt'
	 */
	ifstream fin;
	char input;

	fin.open(filename);
	if( fin.fail() )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin >> this->m_vertexCount;
	this->m_indexCount = this->m_vertexCount;

	this->m_pBumpMapModel = new GEBumpMapModel[this->m_vertexCount];
	if( !this->m_pBumpMapModel )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin.get(input);
	fin.get(input);

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		fin >> this->m_pBumpMapModel[i].x >> this->m_pBumpMapModel[i].y >> this->m_pBumpMapModel[i].z;
		fin >> this->m_pBumpMapModel[i].tu >> this->m_pBumpMapModel[i].tv;
		fin >> this->m_pBumpMapModel[i].nx >> this->m_pBumpMapModel[i].ny >> this->m_pBumpMapModel[i].nz;
	}

	fin.close();

	return true;
}

bool CGEModel::_load_model_tangent(char* filename)
{
		/*
	 * Description -
	 * To understand how this func work,
	 * you should know the model file's 
	 * format.
	 * 
	 * PS - We use model file with '*.txt'
	 */
	ifstream fin;
	char input;

	fin.open(filename);
	if( fin.fail() )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin >> this->m_vertexCount;
	this->m_indexCount = this->m_vertexCount;

	this->m_pTangentModel = new GETangentModel[this->m_vertexCount];
	if( !this->m_pTangentModel )
		return false;

	fin.get(input);
	while( input != ':' )
		fin.get(input);

	fin.get(input);
	fin.get(input);

	for(int i = 0; i < this->m_vertexCount; i++)
	{
		fin >> this->m_pTangentModel[i].x >> this->m_pTangentModel[i].y >> this->m_pTangentModel[i].z;
		fin >> this->m_pTangentModel[i].tu >> this->m_pTangentModel[i].tv;
		fin >> this->m_pTangentModel[i].nx >> this->m_pTangentModel[i].ny >> this->m_pTangentModel[i].nz;
	}

	fin.close();

	return true;
}

void CGEModel::_set_instance_number(int number_of_instance)
{
	this->m_instance_number = number_of_instance;
}

bool CGEModel::_create_instance()
{
	if( 0 == this->m_instance_number ) return false;

	this->m_instance_type = new InstanceType[this->m_instance_number];
	if( !this->m_instance_type )
		return false;

	return true;
}

bool CGEModel::_set_instance_parameters(InstanceType* instance_parameters)
{
	if( this->m_instance_type == 0 || instance_parameters == 0 || this->m_instance_number == 0 )
		return false;

	// It only changes the position so far.
	for(int i = 0; i != this->m_instance_number; i++)
	{
		this->m_instance_type[i].position = instance_parameters[i].position;
	}

	return true;
}

bool CGEModel::_initialize_instance_buffer(ID3D11Device* pDevice)
{
	HRESULT result;

	if( !pDevice )
		return false;

	if( this->m_instance_number == 0 || this->m_instance_type == 0 )
		return false;

	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA  pInstanceData;

	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * this->m_instance_number;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	pInstanceData.pSysMem = this->m_instance_type;
	pInstanceData.SysMemPitch = 0;
	pInstanceData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&instanceBufferDesc, &pInstanceData, &this->m_instance_buffer);
	if( FAILED(result) )
		return false;

	return true;
}

void CGEModel::_release_model()
{
	DeleteArray<ModelType*>(this->m_pModel);
	DeleteArray<GEBumpMapModel*>(this->m_pBumpMapModel);
}

void CGEModel::_release_multitextures()
{
	::Shutdown<CGEMultiTextureSequence*>(this->m_pTextureSequence);
}

ID3D11ShaderResourceView** CGEModel::GetMultiTextureSequence(int textureSeuquenceID)
{
	return this->m_pTextureSequence->GetTextureSequence(textureSeuquenceID);
}

void CGEModel::CalculateModelVectors()
{
	int numFaces;
	int index;

	GEVertex3D_Normal_Tex vertex1;
	GEVertex3D_Normal_Tex vertex2;
	GEVertex3D_Normal_Tex vertex3;

	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;

	numFaces = this->m_vertexCount / 3;

	index = 0;

	for(int i = 0; i != numFaces; i++)
	{
		vertex1.position = D3DXVECTOR3(this->m_pBumpMapModel[index].x, this->m_pBumpMapModel[index].y, this->m_pBumpMapModel[index].z);
		vertex1.texture = D3DXVECTOR2(this->m_pBumpMapModel[index].tu, this->m_pBumpMapModel[index].tv);
		vertex1.normal = D3DXVECTOR3(this->m_pBumpMapModel[index].nx, this->m_pBumpMapModel[index].ny, this->m_pBumpMapModel[index].nz);
		index++;

		vertex2.position = D3DXVECTOR3(this->m_pBumpMapModel[index].x, this->m_pBumpMapModel[index].y, this->m_pBumpMapModel[index].z);
		vertex2.texture = D3DXVECTOR2(this->m_pBumpMapModel[index].tu, this->m_pBumpMapModel[index].tv);
		vertex2.normal = D3DXVECTOR3(this->m_pBumpMapModel[index].nx, this->m_pBumpMapModel[index].ny, this->m_pBumpMapModel[index].nz);
		index++;

		vertex3.position = D3DXVECTOR3(this->m_pBumpMapModel[index].x, this->m_pBumpMapModel[index].y, this->m_pBumpMapModel[index].z);
		vertex3.texture = D3DXVECTOR2(this->m_pBumpMapModel[index].tu, this->m_pBumpMapModel[index].tv);
		vertex3.normal = D3DXVECTOR3(this->m_pBumpMapModel[index].nx, this->m_pBumpMapModel[index].ny, this->m_pBumpMapModel[index].nz);
		index++;

		this->CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		this->CalculateNormal(tangent, binormal, normal);

		this->m_pBumpMapModel[index - 1].nx = normal.x;
		this->m_pBumpMapModel[index - 1].ny = normal.y;
		this->m_pBumpMapModel[index - 1].nz = normal.z;
		this->m_pBumpMapModel[index - 1].tx = tangent.x;
		this->m_pBumpMapModel[index - 1].ty = tangent.y;
		this->m_pBumpMapModel[index - 1].tz = tangent.z;
		this->m_pBumpMapModel[index - 1].bx = binormal.x;
		this->m_pBumpMapModel[index - 1].by = binormal.y;
		this->m_pBumpMapModel[index - 1].bz = binormal.z;

		this->m_pBumpMapModel[index - 2].nx = normal.x;
		this->m_pBumpMapModel[index - 2].ny = normal.y;
		this->m_pBumpMapModel[index - 2].nz = normal.z;
		this->m_pBumpMapModel[index - 2].tx = tangent.x;
		this->m_pBumpMapModel[index - 2].ty = tangent.y;
		this->m_pBumpMapModel[index - 2].tz = tangent.z;
		this->m_pBumpMapModel[index - 2].bx = binormal.x;
		this->m_pBumpMapModel[index - 2].by = binormal.y;
		this->m_pBumpMapModel[index - 2].bz = binormal.z;

		this->m_pBumpMapModel[index - 3].nx = normal.x;
		this->m_pBumpMapModel[index - 3].ny = normal.y;
		this->m_pBumpMapModel[index - 3].nz = normal.z;
		this->m_pBumpMapModel[index - 3].tx = tangent.x;
		this->m_pBumpMapModel[index - 3].ty = tangent.y;
		this->m_pBumpMapModel[index - 3].tz = tangent.z;
		this->m_pBumpMapModel[index - 3].bx = binormal.x;
		this->m_pBumpMapModel[index - 3].by = binormal.y;
		this->m_pBumpMapModel[index - 3].bz = binormal.z;

	}
}

void CGEModel::CalculateTangentSpace()
{
	int numFaces;
	int index;

	GEVertex3D_Normal_Tex vertex1;
	GEVertex3D_Normal_Tex vertex2;
	GEVertex3D_Normal_Tex vertex3;

	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;

	numFaces = this->m_vertexCount / 3;

	index = 0;

	for(int i = 0; i != numFaces; i++)
	{
		vertex1.position = D3DXVECTOR3(this->m_pTangentModel[index].x, this->m_pTangentModel[index].y, this->m_pTangentModel[index].z);
		vertex1.texture = D3DXVECTOR2(this->m_pTangentModel[index].tu, this->m_pTangentModel[index].tv);
		vertex1.normal = D3DXVECTOR3(this->m_pTangentModel[index].nx, this->m_pTangentModel[index].ny, this->m_pTangentModel[index].nz);
		index++;

		vertex2.position = D3DXVECTOR3(this->m_pTangentModel[index].x, this->m_pTangentModel[index].y, this->m_pTangentModel[index].z);
		vertex2.texture = D3DXVECTOR2(this->m_pTangentModel[index].tu, this->m_pTangentModel[index].tv);
		vertex2.normal = D3DXVECTOR3(this->m_pTangentModel[index].nx, this->m_pTangentModel[index].ny, this->m_pTangentModel[index].nz);
		index++;

		vertex3.position = D3DXVECTOR3(this->m_pTangentModel[index].x, this->m_pTangentModel[index].y, this->m_pTangentModel[index].z);
		vertex3.texture = D3DXVECTOR2(this->m_pTangentModel[index].tu, this->m_pTangentModel[index].tv);
		vertex3.normal = D3DXVECTOR3(this->m_pTangentModel[index].nx, this->m_pTangentModel[index].ny, this->m_pTangentModel[index].nz);
		index++;

		this->CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		this->CalculateNormal(tangent, binormal, normal);

		this->m_pTangentModel[index - 1].nx = normal.x;
		this->m_pTangentModel[index - 1].ny = normal.y;
		this->m_pTangentModel[index - 1].nz = normal.z;
		this->m_pTangentModel[index - 1].tx = tangent.x;
		this->m_pTangentModel[index - 1].ty = tangent.y;
		this->m_pTangentModel[index - 1].tz = tangent.z;
		this->m_pTangentModel[index - 1].bx = binormal.x;
		this->m_pTangentModel[index - 1].by = binormal.y;
		this->m_pTangentModel[index - 1].bz = binormal.z;

		this->m_pTangentModel[index - 2].nx = normal.x;
		this->m_pTangentModel[index - 2].ny = normal.y;
		this->m_pTangentModel[index - 2].nz = normal.z;
		this->m_pTangentModel[index - 2].tx = tangent.x;
		this->m_pTangentModel[index - 2].ty = tangent.y;
		this->m_pTangentModel[index - 2].tz = tangent.z;
		this->m_pTangentModel[index - 2].bx = binormal.x;
		this->m_pTangentModel[index - 2].by = binormal.y;
		this->m_pTangentModel[index - 2].bz = binormal.z;

		this->m_pTangentModel[index - 3].nx = normal.x;
		this->m_pTangentModel[index - 3].ny = normal.y;
		this->m_pTangentModel[index - 3].nz = normal.z;
		this->m_pTangentModel[index - 3].tx = tangent.x;
		this->m_pTangentModel[index - 3].ty = tangent.y;
		this->m_pTangentModel[index - 3].tz = tangent.z;
		this->m_pTangentModel[index - 3].bx = binormal.x;
		this->m_pTangentModel[index - 3].by = binormal.y;
		this->m_pTangentModel[index - 3].bz = binormal.z;

	}
}

void CGEModel::CalculateModelVectors_VTN()
{
	int numFaces;
	int index;

	GEVertex3D_Normal_Tex vertex1;
	GEVertex3D_Normal_Tex vertex2;
	GEVertex3D_Normal_Tex vertex3;

	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;

	numFaces = this->m_vertexCount / 3;

	index = 0;

	for(int i = 0; i != numFaces; i++)
	{
		vertex1.position = D3DXVECTOR3(this->m_pModel[index].x, this->m_pModel[index].y, this->m_pModel[index].z);
		vertex1.texture = D3DXVECTOR2(this->m_pModel[index].tu, this->m_pModel[index].tv);
		vertex1.normal = D3DXVECTOR3(this->m_pModel[index].nx, this->m_pModel[index].ny, this->m_pModel[index].nz);
		index++;

		vertex2.position = D3DXVECTOR3(this->m_pModel[index].x, this->m_pModel[index].y, this->m_pModel[index].z);
		vertex2.texture = D3DXVECTOR2(this->m_pModel[index].tu, this->m_pModel[index].tv);
		vertex2.normal = D3DXVECTOR3(this->m_pModel[index].nx, this->m_pModel[index].ny, this->m_pModel[index].nz);
		index++;

		vertex3.position = D3DXVECTOR3(this->m_pModel[index].x, this->m_pModel[index].y, this->m_pModel[index].z);
		vertex3.texture = D3DXVECTOR2(this->m_pModel[index].tu, this->m_pModel[index].tv);
		vertex3.normal = D3DXVECTOR3(this->m_pModel[index].nx, this->m_pModel[index].ny, this->m_pModel[index].nz);
		index++;

		this->CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		this->CalculateNormal(tangent, binormal, normal);

		this->m_pModel[index - 1].nx = normal.x;
		this->m_pModel[index - 1].ny = normal.y;
		this->m_pModel[index - 1].nz = normal.z;


		this->m_pModel[index - 2].nx = normal.x;
		this->m_pModel[index - 2].ny = normal.y;
		this->m_pModel[index - 2].nz = normal.z;

		this->m_pModel[index - 3].nx = normal.x;
		this->m_pModel[index - 3].ny = normal.y;
		this->m_pModel[index - 3].nz = normal.z;

	}
}

void CGEModel::CalculateTangentBinormal(GEVertex3D_Normal_Tex v1,
										GEVertex3D_Normal_Tex v2,
										GEVertex3D_Normal_Tex v3,
										D3DXVECTOR3& _out_tangent,
										D3DXVECTOR3& _out_binormal)
{
	float vector1[3];
	float vector2[3];
	float tu[2];
	float tv[2];

	float length;
	float denominator;

	vector1[0] = v2.position.x - v1.position.x;
	vector1[1] = v2.position.y - v1.position.y;
	vector1[2] = v2.position.z - v1.position.z;

	vector2[0] = v3.position.x - v1.position.x;
	vector2[1] = v3.position.y - v1.position.y;
	vector2[2] = v3.position.z - v1.position.z;

	tu[0] = v2.texture.x - v1.texture.x;
	tv[0] = v2.texture.y - v1.texture.y;

	tu[1] = v3.texture.x - v1.texture.x;
	tv[1] = v3.texture.y - v1.texture.y;

	denominator = 1.0f / (tu[0] * tv[1] - tu[1] * tv[0]);

	_out_tangent.x = denominator * (tv[1] * vector1[0] - tv[0] * vector2[0]);
	_out_tangent.y = denominator * (tv[1] * vector1[1] - tv[0] * vector2[1]);
	_out_tangent.z = denominator * (tv[1] * vector1[2] - tv[0] * vector2[2]);

	_out_binormal.x = denominator * (tu[0] * vector2[0] - tu[1] * vector1[0]);
	_out_binormal.y = denominator * (tu[0] * vector2[1] - tu[1] * vector1[1]);
	_out_binormal.z = denominator * (tu[0] * vector2[2] - tu[1] * vector1[2]);

	length = sqrt((_out_tangent.x * _out_tangent.x) + (_out_tangent.y * _out_tangent.y) + (_out_tangent.z * _out_tangent.z));

	_out_tangent.x /= length;
	_out_tangent.y /= length;
	_out_tangent.z /= length;

	length = sqrt((_out_binormal.x * _out_binormal.x) + (_out_binormal.y * _out_binormal.y) + (_out_binormal.z * _out_binormal.z));

	_out_binormal.x /= length;
	_out_binormal.y /= length;
	_out_binormal.z /= length;

	return;
}

void CGEModel::CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& _out_normal)
{
	float length;

	_out_normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	_out_normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	_out_normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	length = sqrt((_out_normal.x * _out_normal.x) + (_out_normal.y * _out_normal.y) + (_out_normal.z * _out_normal.z));

	_out_normal.x /= length;
	_out_normal.y /= length;
	_out_normal.z /= length;

	return;
}

void CGEModel::RenderSwitch(ID3D11DeviceContext* pDeviceContext, VertexType vt)
{
	if( NULL == pDeviceContext )
		return;

	this->_render_vertex_type(pDeviceContext, vt);
}

void CGEModel::_render_vertex_type(ID3D11DeviceContext* pDeviceContext, VertexType vertex_type)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_Normal_Tex);
	offset = 0;

	if( NULL == pDeviceContext )
		return;

	switch(vertex_type)
	{
	case NONE:
		break;

	case Position:
		stride = sizeof(GEVertex3D_Position);
		break;

	case PTN:
		stride = sizeof(GEVertex3D_Normal_Tex);
		break;

	case PT:
		stride = sizeof(GEVertex3D_Tex);
		break;

	case PN:
		break;

	case PC:
		stride = sizeof(GEVertex3D_COLOR);
		break;

	case PTN_Binormal_Tangent:
		stride = sizeof(GEVertex3D_AdvancedTextureTech);
		break;
	}

	// Set vertex buffer.
	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertex_buffer, &stride, &offset);
	// Set index buffer.
	pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	// Choose primitive.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void CGEModel::RenderInstanceSwitch(ID3D11DeviceContext* pDeviceContext, VertexType vt)
{
	if( NULL == pDeviceContext ) return;

	this->_render_instance_vertex_type(pDeviceContext, vt);
}

void CGEModel::_render_instance_vertex_type(ID3D11DeviceContext* pDeviceContext, VertexType vertex_type)
{
	unsigned int stride[2];
	unsigned int offset[2];
	ID3D11Buffer* pBufferPointer[2];

	offset[0] = 0;
	offset[1] = 0;

	if( NULL == pDeviceContext )
		return;

	// Exept PTN vertex type can work. Others need to be modified.
	switch(vertex_type)
	{
	case NONE:
		break;

	case Position:
		stride[0] = sizeof(GEVertex3D_Position);
		stride[1] = sizeof(InstanceType);
		break;

	case PTN:
		stride[0] = sizeof(GEVertex3D_Normal_Tex);
		stride[1] = sizeof(InstanceType);
		break;

	case PT:
		stride[0] = sizeof(GEVertex3D_Tex);
		stride[1] = sizeof(InstanceType);
		break;

	case PN:
		break;

	case PC:
		stride[0] = sizeof(GEVertex3D_COLOR);
		stride[1] = sizeof(InstanceType);
		break;

	case PTN_Binormal_Tangent:
		stride[0] = sizeof(GEVertex3D_AdvancedTextureTech);
		stride[1] = sizeof(InstanceType);
		break;
	}

	// Set buffer pointers
	pBufferPointer[0] = this->m_pVertex_buffer;
	pBufferPointer[1] = this->m_instance_buffer;

	// Set vertex buffer.
	pDeviceContext->IASetVertexBuffers(0, 2, pBufferPointer, stride, offset);
	// Set index buffer.
	// pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	// Choose primitive.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void CGEModel::RenderTessellation(ID3D11DeviceContext* pDeviceContext, VertexType vertex_type)
{
	if( NULL == pDeviceContext ) return;

	this->_render_tessellation(pDeviceContext, vertex_type);
}

void CGEModel::_render_tessellation(ID3D11DeviceContext* pDeviceContext, VertexType vertex_type)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_TangentSpace);
	offset = 0;

	if( NULL == pDeviceContext )
		return;

	switch(vertex_type)
	{
	case NONE:
		break;

	case Position:
		stride = sizeof(GEVertex3D_Position);
		break;

	case PTN:
		stride = sizeof(GEVertex3D_Normal_Tex);
		break;

	case PT:
		stride = sizeof(GEVertex3D_Tex);
		break;

	case PN:
		break;

	case PC:
		stride = sizeof(GEVertex3D_COLOR);
		break;

	case PTN_Binormal_Tangent:
		stride = sizeof(GEVertex3D_TangentSpace);

	case PTN_Tangent:
		stride = sizeof(GEVertex3D_TangentSpace);
		break;
	}

	// Set vertex buffer.
	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertex_buffer, &stride, &offset);
	// Set index buffer.
	pDeviceContext->IASetIndexBuffer(this->m_pIndex_buffer, DXGI_FORMAT_R32_UINT, 0);
	// Choose primitive.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

}


