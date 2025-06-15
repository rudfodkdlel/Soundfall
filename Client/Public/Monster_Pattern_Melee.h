#include "Monster_State_Base.h"

NS_BEGIN(Client)

class CMonster_Pattern_Melee : public CMonster_State_Base
{
public:
	virtual ~CMonster_Pattern_Melee() = default;

public:
	virtual void Enter(CGameObject* pObj) override;
	virtual void Update(CGameObject* pObj, float fTimeDelta);
	virtual void Exit(CGameObject* pObj);
	virtual CObject_State* Check_Transition(CGameObject* pObj);

private:
	_float		m_fDuration = { 1.5f };
	DIR_STATE m_eDir = { DIR_STATE::NONE };
	_int		m_iAnimnum = {};
	_bool		m_isColl = { false };

};

NS_END