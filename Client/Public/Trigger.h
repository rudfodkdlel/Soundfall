#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CTrigger final : public CGameObject
{
public:
	typedef struct tagTriggerDesc : public CGameObject::GAMEOBJECT_DESC {
		TRIGGERTYPE eType;
	}TRIGGER_DESC;
private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& Prototype);
	virtual ~CTrigger() = default;

public:
	_int Get_TriggerType() { return static_cast<_int>(m_eType); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual HRESULT On_Collision(CCollider* pCollider) override;

private:
	CCollider* m_pColliderCom = { nullptr };

	TRIGGERTYPE m_eType = { TRIGGERTYPE::END };

	_bool		m_IsColl = { false };
	_bool		m_IsChange = { false };

private:
	HRESULT Ready_Components();

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END