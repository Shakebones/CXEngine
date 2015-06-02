/*
 * File name - GEModelsManager.cpp
 * Author - CuiXue
 * Updated date - 2013.2.12
 * Desc -
 *
 */
#include "GEModelsManager.h"

CGEModelsManager::CGEModelsManager()
{
	this->m_nModelCount = 0;
	this->m_pDevice = 0;
	this->m_pDeviceContext = 0;
}

CGEModelsManager::CGEModelsManager(const CGEModelsManager& source)
{
}

CGEModelsManager& CGEModelsManager::operator=(const CGEModelsManager& source)
{
	return *this;
}

CGEModel& CGEModelsManager::operator[](int modelID)
{
	CGEModel *error = NULL;
	if( 0 > modelID )
		return *error;


	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( modelID == this->m_ModelVec[i].modelID )
			return *this->m_ModelVec[i].pModel;
	}

	return *(error);
}

CGEModelsManager::~CGEModelsManager()
{
}

bool CGEModelsManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	this->m_pDevice = pDevice;
	this->m_pDeviceContext = pDeviceContext;
	return true;
}

bool CGEModelsManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, int numModels)
{
	this->m_pDevice = pDevice;
	this->m_pDeviceContext = pDeviceContext;
	return true;
}

void CGEModelsManager::Shutdown()
{
	this->m_ModelVec.Reset();
}

bool CGEModelsManager::AddModel(int modelID, CGEModel** pModel)
{
	bool result;
	Mod_Node* pNew = new Mod_Node;
	if( !pNew )
		return false;

	pNew->pModel = new CGEModel;
	if( !pNew->pModel )
		return false;

	pNew->modelID = modelID;
	result = pNew->pModel->CopyFrom(this->m_pDevice, this->m_pDeviceContext, *pModel);
	if( !result )
		return false;

	result = this->m_ModelVec.Pushback(*pNew);
	if( !result )
		return false;

	pNew = NULL;

	this->m_nModelCount++;
	return true;
}

bool CGEModelsManager::AddModel(int modelID, char* pModelPath, WCHAR* pTexturePath)
{
	bool result;

	if( NULL == pModelPath )
		return false;

	Mod_Node* pNew = new Mod_Node;
	if( !pNew )
		return false;

	pNew->modelID = modelID;

	pNew->pModel = new CGEModel;
	if( !pNew->pModel )
		return false;

	if( NULL == pTexturePath )
	{
		result = pNew->pModel->InitializeVertexBuffer(this->m_pDevice, pModelPath, NULL);
		if( !result )
			return false;
	}

	else
	{
		result = pNew->pModel->InitializeVertexBuffer(this->m_pDevice, pModelPath, pTexturePath);
		if( !result )
			return false;
	}

	result = this->m_ModelVec.Pushback(*pNew);
	if( !result )
		return false;

	pNew = NULL;
	
	this->m_nModelCount++;

	return true;
}

bool CGEModelsManager::ReleaseModel(int modelID)
{
	if( this->m_nModelCount == 0 )
		return false;

	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( this->m_ModelVec[i].modelID == modelID )
		{
			::Shutdown<CGEModel*>(this->m_ModelVec[i].pModel);
			this->m_ModelVec.Remove(i);
			this->m_nModelCount--;
			return true;
		}
	}

	return false;
}

bool CGEModelsManager::ResetModelData(int modelID, CGEModel* _pNewModelData)
{
	bool result;

	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( modelID == this->m_ModelVec[i].modelID )
		{
			::Shutdown<CGEModel*>(this->m_ModelVec[i].pModel);
			this->m_ModelVec[i].pModel = _pNewModelData;

			return true;
		}
	}

	Mod_Node* pNew = new Mod_Node;
	if( !pNew )
		return false;

	pNew->modelID = modelID;
	pNew->pModel = _pNewModelData;

	result = this->m_ModelVec.Pushback(*pNew);
	if( !result )
		return false;

	this->m_nModelCount++;

	pNew = NULL;
	return true;
}

void CGEModelsManager::RenderAllModels()
{
	if( 0 == this->m_ModelVec.GetMyVectorLength() )
		return;

	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		this->m_ModelVec[i].pModel->Render(this->m_pDeviceContext);
	}

	return;
}

void CGEModelsManager::RenderModelByResID(int modelID)
{
	if( 0 == this->m_ModelVec.GetMyVectorLength() )
		return;

	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( modelID == this->m_ModelVec[i].modelID )
		{
			this->m_ModelVec[i].pModel->Render(this->m_pDeviceContext);
			break;
		}
	}

	return;
}

void CGEModelsManager::RenderModelsQueue(int *p_models)
{
	if( NULL == p_models ||
		0 == this->m_ModelVec.GetMyVectorLength() )
		return;

	int* p_modelsQueue;
	p_modelsQueue = p_models;

	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( *p_modelsQueue == this->m_ModelVec[i].modelID )
		{
			this->m_ModelVec[i].pModel->Render(this->m_pDeviceContext);
			p_modelsQueue++;
		}
	}

	return;
}

bool CGEModelsManager::GetModel(int modelID, CGEModel** _ppModelData)
{
	for(int i = 0; i != this->m_ModelVec.GetMyVectorLength(); i++)
	{
		if( this->m_ModelVec[i].modelID == modelID )
		{
			*_ppModelData = this->m_ModelVec[i].pModel;
			return true;
		}
	}

	return false;
}

CGEModel** CGEModelsManager::GetModelByIndex(int index)
{
	if( index < 0 || index > this->m_nModelCount - 1 )
		return NULL;

	else
	{
		return &this->m_ModelVec[index].pModel;
	}
}

int CGEModelsManager::GetModelIDByIndex(int index)
{
	if( index < 0 || index > this->m_nModelCount - 1 )
		return -1;

	else
	{
		return this->m_ModelVec[index].modelID;
	}
}

int CGEModelsManager::GetModelCount()
{
	return this->m_nModelCount;
}
