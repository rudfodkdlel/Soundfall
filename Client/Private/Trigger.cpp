#include "Trigger.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Observer_Trigger.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{pDevice, pContext}
{
}

CTrigger::CTrigger(const CTrigger& Prototype)
	:CGameObject{Prototype}
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));

	m_eType = pDesc->eType;

	m_pGameInstance->Add_Collider(CG_TRIGGER, m_pColliderCom, this);

	return S_OK;
}

void CTrigger::Priority_Update(_float fTimeDelta)
{
	if (m_IsColl)
		Set_Dead();
}

void CTrigger::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CTrigger::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
#endif
}

HRESULT CTrigger::Render()
{

	m_pColliderCom->Render();

	return S_OK;
}

HRESULT CTrigger::On_Collision(CCollider* pCollider)
{
	if (TRIGGERTYPE::ZOOM_IN == m_eType)
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Trigger"))->OnNotify(TEXT("ZOOM_IN"));
	}
	else if (TRIGGERTYPE::ZOOM_OUT == m_eType)
	{
		m_pGameInstance->Find_Observer(TEXT("Observer_Trigger"))->OnNotify(TEXT("ZOOM_OUT"));
	}
	
	m_IsColl = true;

	return S_OK;
}

HRESULT CTrigger::Ready_Components()
{
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 5.f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	return S_OK;
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjecttile_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
