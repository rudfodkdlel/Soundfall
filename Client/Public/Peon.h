#pragma once

#include "Client_Defines.h"
#include "Monster_Base.h"
#include "CombatStat.h"


NS_BEGIN(Engine)
class CModel;
class CNavigation;

NS_END

NS_BEGIN(Client)

class CPeon final : public CMonster_Base
{
public:

	CCombatStat::HPBUFFER Get_Hp_Buffer() { return m_pCombatCom->Get_HP_BUFFER(); }

private:
	CPeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPeon(const CPeon& Prototype);
	virtual ~CPeon() = default;

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
	_float		m_fPatternTime = { 2.f };

	CCombatStat* m_pCombatCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	

public:
	static CPeon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END