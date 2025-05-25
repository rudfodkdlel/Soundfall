#include "Player_State_Move.h"
#include "Player.h"
#include "Model.h"

void Player_State_Move::Enter(CGameObject* pObj)
{
}

void Player_State_Move::Update(CGameObject* pObj, float fTimeDelta)
{
	CModel* pModel = static_cast<CModel*>(pObj->Get_Component(TEXT("Com_Model")));

	switch (static_cast<CPlayer*>(pObj)->Calc_Dir())
	{
	case F:
		pModel->Set_Animation(10, true);
		break;
	case B:
		pModel->Set_Animation(9, true);
		break;
	case L:
		pModel->Set_Animation(54, true);
		break;
	case R:
		pModel->Set_Animation(55, true);
		break;
	case FL:
		pModel->Set_Animation(52, true);
		break;
	case FR:
		pModel->Set_Animation(53, true);
		break;
	case BL:
		pModel->Set_Animation(50, true);
		break;
	case BR:
		pModel->Set_Animation(51, true);
		break;

	default:
		break;
	}
}

void Player_State_Move::Exit(CGameObject* pObj)
{
}
