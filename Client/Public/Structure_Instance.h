

#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Model_Instance.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CStructure_Instance final : public CGameObject
{
public:
	typedef struct tagStructInstanceDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModeltag;
		_wstring strBuffertag;
	}STRUCTURE_INSTANCE_DESC;

	virtual OBJECT_SAVE_DESC Get_Save_Desc() override
	{
		OBJECT_SAVE_DESC eDesc = {}; 

		eDesc.strPrototypetag = m_strProtoTag;
		eDesc.PrototypeLevelIndex = m_iProtoIndex;
		eDesc.strModeltag = m_strModelTag;
		memcpy(&eDesc.matWorld, m_pTransformCom->Get_WorldMatrix(), sizeof(_float4x4));


		return eDesc;
	}

private:
	CStructure_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStructure_Instance(const CStructure_Instance& Prototype);
	virtual ~CStructure_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };

	CVIBuffer_Model_Instance* m_pVIBufferCom = { nullptr };

	_uint	m_iAnimnum = { 0 };

	_bool	m_isAnim = { false };
	_wstring m_strModelTag = {};
	_wstring m_strBufferTag = {};
private:
	HRESULT Ready_Components(STRUCTURE_INSTANCE_DESC* eDesc);

public:
	static CStructure_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END