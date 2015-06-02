/*
 * File name - GEModelsManager.h
 * Author - CuiXue
 * Updated date - 2013.2.11
 * Desc -
 *
 * For managing models.
 */
#ifndef _GE_MODEL_MANAGER_H_
#define _GE_MODEL_MANAGER_H_

#ifdef _WIN32
#include "GEModel.h"
#include "Utility.h"
#include <D3DX10math.h>
#include <stdlib.h>
#include <time.h>
#endif

typedef struct _model_node_type
{
	int modelID;
	CGEModel* pModel;
}Mod_Node, pMod_Node, **ppMod_Node;

/*
 * Class name - CGEModelManager
 * Author - CuiXue
 * Updated date - 2013.2.11
 * Desc -
 *
 */
class CGEModelsManager
{
public:
	// Interfaces
	CGEModelsManager();
	CGEModelsManager(const CGEModelsManager&);
	CGEModelsManager& operator=(const CGEModelsManager&);
	CGEModel& operator[](int);
	~CGEModelsManager();

public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int numModels);
	void Shutdown();

public:
	bool AddModel(int, CGEModel**);						// 增加已存在的模型文件到列表
	bool AddModel(int, char*, WCHAR*);					// 从文件中加载模型并添加到列表

	bool ReleaseModel(int);								// 从列表中删除模型文件

	void RenderAllModels();								// 执行列表中所有模型的渲染函数
	void RenderModelByResID(int);						// 根据 资源ID  执行当前ID为modelID模型的渲染函数
	void RenderModelsQueue(int*);						// 执行模型渲染序列，参数指向 资源ID数组（有序）

	int			GetModelCount();						// 获取模型数量
	bool		ResetModelData(int, CGEModel*);
	bool		GetModel(int, CGEModel**);
	CGEModel**	GetModelByIndex(int);					// 根据索引获取模型（渲染队列）
	int			GetModelIDByIndex(int);					// 根据索引获取模型ID号

private:

	int m_nModelCount;
	MyVector<pMod_Node>		m_ModelVec;

	// Bad idea to save device pointer & device context pointer !!
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
};

#endif