#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"

NS_BEGIN(Client)
class CObject_State abstract
{ 
public:
	virtual ~CObject_State() = default;

public:
	virtual void Enter(CGameObject* pObj) = 0;
	virtual void Update(CGameObject* pObj, float fTimeDelta) = 0;
	virtual void Exit(CGameObject* pObj) = 0;
};

NS_END

