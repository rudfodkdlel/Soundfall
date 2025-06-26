#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagMeshInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_int	 iMaterialIndex;

	}MESH_INSTANCE_DESC;

public:
	_uint Get_MaterialIndex() { return m_iMaterialIndex; }

private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc,  MESH_DESC meshDesc);
	virtual HRESULT Initialize(void* pArg);


protected:
	VTXMESH_INSTANCE* m_pVertexInstances = { nullptr };
	_uint			m_iMaterialIndex = {};

public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc, MESH_DESC meshDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END