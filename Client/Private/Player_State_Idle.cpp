#include "Player_State_Idle.h"
#include "Player.h"
#include "Model.h"

void Player_State_Idle::Enter(CGameObject* pObj)
{
	
}

void Player_State_Idle::Update(CGameObject* pObj, float fTimeDelta)
{
	CModel* pModel = static_cast<CModel*>(pObj->Get_Component(TEXT("Com_Model")));

	if(NONE == static_cast<CPlayer*>(pObj)->Calc_Dir())
		pModel->Set_Animation(4, true);

}

void Player_State_Idle::Exit(CGameObject* pObj)
{
	
}
