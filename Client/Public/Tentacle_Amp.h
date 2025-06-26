#pragma once

#include "Client_Defines.h"
#include "Monster_Base.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CTentacle_Amp final : public CMonster_Base
{
private:
	CTentacle_Amp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTentacle_Amp(const CTentacle_Amp& Prototype);
	virtual ~CTentacle_Amp() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual HRESULT On_Collision(class CCollider* pCollider) override;

	void Select_State();


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool	m_IsFinished = { false };

	_int	m_iAnimNum = { 0 };

	CGameObject* m_pTarget = { nullptr };
	_bool		 m_IsTargeted = { false };

	CCollider* m_pColliderCom = { nullptr };

	class CCombatStat* m_pCombatCom = { nullptr };

	_float	m_fAttackTime = {1.5f};
	_float	m_fDelay = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_PartObjects();

public:
	static CTentacle_Amp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END