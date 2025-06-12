#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_float4		vPos;
		_int		iProtoIndex;
		_wstring    strPrototag;
		_tchar		szName[MAX_PATH];
	}GAMEOBJECT_DESC;



	OBJECT_SAVE_DESC Get_Save_Desc()
	{
		OBJECT_SAVE_DESC eDesc = {};
		eDesc.szPrototypetag = m_strProtoTag;
		eDesc.PrototypeLevelIndex = m_iProtoIndex;
		memcpy(&eDesc.matWorld,m_pTransformCom->Get_WorldMatrix(), sizeof(_float4x4));
		
		return eDesc;
	}

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	CComponent* Get_Component(const _wstring& strComponentTag);
	CTransform* Get_Transform() { return m_pTransformCom; }

	void        Set_Dead() { m_bDead = true; }
	_bool		Get_Dead() { return m_bDead; }
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	// 필요하면 무조건 구현하기
	virtual HRESULT On_Collision(CGameObject* Other, class CCollider* pCollider) { return E_FAIL; }

	void Billboarding();
	

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	_tchar										m_szName[MAX_PATH] = {};
	map<const _wstring, class CComponent*>		m_Components;
	class CTransform*							m_pTransformCom = { nullptr };
	_bool										m_bDead = { false };

	_wstring									m_strProtoTag = {};
	_int										m_iProtoIndex = {};

	// 충돌 처리때 사용할 것들, 충돌할 물체면 mass 값을 줘야됨. 
	_int										m_iMass = {0};
	vector<_float4>								m_pushVectors;


protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END