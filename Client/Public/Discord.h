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

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();


private:
	STATE_MAIN		m_eMainState = { STATE_MAIN::SPWAN };
	STATE_SUB		m_eSubState = { STATE_SUB::NONE };
	_int		    m_iPhase = { 1 };// 1, 2, 3, death

	// 임시로 일단
	_int			m_iLoopCount = { 3 };
	_int			m_iAttackPattern = { 0 };

public:
	static CDiscord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END