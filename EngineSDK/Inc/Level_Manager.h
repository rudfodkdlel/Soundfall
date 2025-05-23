#pragma once

#include "Base.h"
#include "Level.h"

/* 기존 레벨을 파괴하고 새롭게 할당된 레벨의 주소를 들고 있는다. */
/* 위의 작업을 수행 시, 기존 레벨용 자원을 모두 파괴한다. */
/* 현재 할당된 레벨을 갱신하고 보여준다. */

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Update(_float fTimeDelta);
	HRESULT Render();

public:
	_uint Get_Current_Level() const { return m_iCurrentLevelIndex; }

private:
	_uint					m_iCurrentLevelIndex = {};
	CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END