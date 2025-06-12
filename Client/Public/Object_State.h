#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "Observer_Animation.h"


NS_BEGIN(Client)
class CObject_State abstract
{ 
public:
	virtual ~CObject_State() = default;

public:
	virtual void Enter(CGameObject* pObj) = 0;
	virtual void Update(CGameObject* pObj, float fTimeDelta) = 0;
	virtual void Exit(CGameObject* pObj) = 0;
	virtual CObject_State* Check_Transition(CGameObject* pObj) = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };

	
	CModel* m_pModel = { nullptr };
	_bool   m_IsFinish = { false };
};

NS_END

