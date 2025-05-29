#include "Monster_Base.h"
#include "GameInstance.h"
#include "PartObject.h"


CMonster_Base::CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CMonster_Base::CMonster_Base(const CMonster_Base& Prototype)
	: CContainerObject{ Prototype }
{
}

HRESULT CMonster_Base::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CMonster_Base::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC* pDesc = static_cast<CONTAINEROBJECT_DESC*>(pArg);

	m_iNumPartObjects = pDesc->iNumPartObjects;

	m_PartObjects.resize(m_iNumPartObjects);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Base::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMonster_Base::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMonster_Base::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMonster_Base::Render()
{
	return S_OK;
}

void CMonster_Base::Look_Target()
{
	if (nullptr == m_pTarget)
		return;

	m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(STATE::POSITION));

}



DIR_STATE CMonster_Base::Calc_Dir()
{
	
	return DIR_STATE::NONE;
}

HRESULT CMonster_Base::Ready_PartObjects()
{


	return S_OK;
}

HRESULT CMonster_Base::Ready_Components()
{
	return S_OK;
}

void CMonster_Base::Free()
{
	__super::Free();
}
