#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "CombatStat.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Discord final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public CPartObject::PARTOBJECT_DESC {
		CCombatStat* pCombatCom;

	}BODY_DESC;

public:
	CCollider** Get_Colliders() { return m_pColliderCom; }
	void Set_UseRimLight() { m_isUseRimLight = true; }
private:
	CBody_Discord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Discord(const CBody_Discord& Prototype);
	virtual ~CBody_Discord() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual HRESULT On_Collision(CCollider* pCollider);

	eDirState Get_Hit_Dir() { return m_eHitDir; }
	eDirState Calc_Hit_Dir(_vector vDir);
	_bool	  Get_Attack_Hit() { return m_IsAttackHit; }
	void	  Set_Attack_Hit() { m_IsAttackHit = false; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom[5] = {nullptr};
	CTexture* m_pTextureCom = { nullptr };
	CCombatStat* m_pCombatCom = { nullptr };
	_bool	m_IsFinished = { false };
	_bool	m_IsAttackHit = { false };
	_bool	m_isUseRimLight = { false };

	const _float4x4* m_pSocketMatrix = { nullptr };
	eDirState		 m_eHitDir = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Discord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END