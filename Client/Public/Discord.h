#pragma once

#include "Client_Defines.h"
#include "Monster_Base.h"
#include "CombatStat.h"
#include "PartObject.h"


NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CDiscord final : public CMonster_Base
{

private:
	CDiscord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDiscord(const CDiscord& Prototype);
	virtual ~CDiscord() = default;

public:
	_int		 Get_CurrentHp() { return m_pCombatCom->Get_Current_HP(); }
	_bool		 Check_Groggy();
	_bool		 Check_Wall();
	CCombatStat* Get_CombatCom() { return m_pCombatCom; }
	_int		 Get_Phase() { return m_iPhase; }
	void		 Add_Phase() { ++m_iPhase; }
	void         Set_bUseSummon(_bool bUse) { m_bUseSummon = bUse; }
	CPartObject* Get_Body() { return m_PartObjects[0]; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Select_State();

	BOSS_PATTERN	Get_Next_Skill();
	DIR_STATE     Get_Hit_Dir();

	DIR_STATE Get_Dir_Melee();

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();


private:
	CCombatStat* m_pCombatCom = { nullptr };

private:

	CObject_State*	m_pState = { nullptr };
	_int		    m_iPhase = { 0 };// 1, 2, 3, death
	_bool			m_bUseSummon = { false };

	// 임시로 일단
	_int			m_iLoopCount = { 3 };

	_int		   m_iIndex = { 0 };
	vector<_uint>  m_normalSequence = {0,1,2,4,3,0 };

	

public:
	static CDiscord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END