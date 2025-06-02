#pragma once

#include "Client_Defines.h"
#include "Monster_Base.h"


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
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Select_State();

	BOSS_PATTERN	Get_Next_Skill();

	DIR_STATE Get_Dir_Melee();

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();


private:
	CObject_State*	m_pState = { nullptr };
	_int		    m_iPhase = { 1 };// 1, 2, 3, death

	// 임시로 일단
	_int			m_iLoopCount = { 3 };

	vector<_float>	m_fSkillCoolTimes = {};

public:
	static CDiscord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END