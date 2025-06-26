#pragma once

#include "VIBuffer_Instance.h"
#include "VIBuffer_Mesh_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagModelInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_wstring strModelDesc;

	}Model_INSTANCE_DESC;

public:
	_uint	Get_MeshCount() { return m_MeshIntances.size(); }

private:
	CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& Prototype);
	virtual ~CVIBuffer_Model_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _int eType, _uint iTextureIndex = 0);
	
protected:
	vector<CVIBuffer_Mesh_Instance*> m_MeshIntances = {};
	vector<class CMaterial*>	m_Materials;

public:
	static CVIBuffer_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END