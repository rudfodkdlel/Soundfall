#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "Model.h"
#include "Observer_Animation.h"


NS_BEGIN(Client)
class CObject_State abstract
{ 
public:
	virtual ~CObject_State() = default;

public:
	virtual void Enter(CGameObject* pObj, OBJTYPE eType) { m_eType = eType; m_pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(m_pGameInstance); }
	virtual void Update(CGameObject* pObj, float fTimeDelta) = 0;
	virtual void Exit(CGameObject* pObj) = 0;
	virtual CObject_State* Check_Transition(CGameObject* pObj) = 0;

protected:
	OBJTYPE		m_eType = { OBJTYPE::END};
	CGameInstance* m_pGameInstance = { nullptr };

	// 필요하면 쓰기?
	CGameObject* m_pObject = { nullptr };
	CModel* m_pModel = { nullptr };
};

NS_END

