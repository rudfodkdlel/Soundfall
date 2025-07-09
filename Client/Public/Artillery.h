#pragma once

#include "Client_Defines.h"
#include "Monster_Base.h"
#include "CombatStat.h"


NS_BEGIN(Engine)
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CArtillery final : public CMonster_Base
{
public:

	CCombatStat::HPBUFFER Get_Hp_Buffer() { return m_pCombatCom->Get_HP_BUFFER(); }

private:
	CArtillery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArtillery(const CArtillery& Prototype);
	virtual ~CArtillery() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Select_State();

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();


private:
	CNavigation* m_pNavigationCom = { nullptr };

	_float		m_fPatternTime = { 2.f };
	_float      m_fDuration = { 0.2f };

	CCombatStat* m_pCombatCom = { nullptr };
	ATTACK		 m_eAttackState = { ATTACK::ATTACK_IN };
	eDirState	 m_eDirState = { eDirState::NONE };


public:
	static CArtillery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END