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
	bool AddModel(int, CGEModel**);						// �����Ѵ��ڵ�ģ���ļ����б�
	bool AddModel(int, char*, WCHAR*);					// ���ļ��м���ģ�Ͳ���ӵ��б�

	bool ReleaseModel(int);								// ���б���ɾ��ģ���ļ�

	void RenderAllModels();								// ִ���б�������ģ�͵���Ⱦ����
	void RenderModelByResID(int);						// ���� ��ԴID  ִ�е�ǰIDΪmodelIDģ�͵���Ⱦ����
	void RenderModelsQueue(int*);						// ִ��ģ����Ⱦ���У�����ָ�� ��ԴID���飨����

	int			GetModelCount();						// ��ȡģ������
	bool		ResetModelData(int, CGEModel*);
	bool		GetModel(int, CGEModel**);
	CGEModel**	GetModelByIndex(int);					// ����������ȡģ�ͣ���Ⱦ���У�
	int			GetModelIDByIndex(int);					// ����������ȡģ��ID��

private:

	int m_nModelCount;
	MyVector<pMod_Node>		m_ModelVec;

	// Bad idea to save device pointer & device context pointer !!
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
};

#endif