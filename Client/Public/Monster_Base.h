#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CMonster_Base abstract  : public CContainerObject
{
public:

protected:
	CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Base(const CMonster_Base& Prototype);
	virtual ~CMonster_Base() = default;

public:
	void Set_State(STATE_MAIN eState, STATE_SUB eSubState = STATE_SUB::NONE) {
		m_eMainState = eState; m_eSubState = eSubState;
	}
	pair<STATE_MAIN, STATE_SUB> Get_State() { return { m_eMainState, m_eSubState }; }
	map<STATE_MAIN, _int>& Get_AnimIndexMap() { return m_AnimIndexMap; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Look_Target();
	virtual void Select_State() = 0;

	DIR_STATE Calc_Dir();

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();


protected:
	DIR_STATE		m_eDirState = { NONE };
	STATE_MAIN		m_eMainState = { STATE_MAIN::SPWAN };
	STATE_SUB		m_eSubState = { STATE_SUB::NONE };
	CGameObject*	m_pTarget = { nullptr };
	class CObject_State* m_pState = { nullptr };

	map<STATE_MAIN, _int >	m_AnimIndexMap = {};

	_int			m_iHp = { 100 };

public:
	virtual CGameObject* Clone(void* pArg) =0;
	virtual void Free() override;

};

NS_END