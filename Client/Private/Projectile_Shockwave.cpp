#include "Projectile_Shockwave.h"
#include "GameInstance.h"
#include "CombatStat.h"

CProjectile_Shockwave::CProjectile_Shockwave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base(pDevice, pContext)
{
}

CProjectile_Shockwave::CProjectile_Shockwave(const CProjectile_Shockwave& Prototype)
	:CProjectile_Base(Prototype)
{
}

HRESULT CProjectile_Shockwave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Shockwave::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(8.f, 8.f, 8.f);
	_vector vDir = { 1.f, 0.f,0.f,0.f };
	m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));


	_vector vPos = { 0.f,0.2f + m_pGameInstance->Compute_Random_Normal() * 0.005f,0.f,0.f };
	vPos += m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE, m_pInnerCollider, this);
	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE, m_pOuterCollider, this);
	

	return S_OK;
}

void CProjectile_Shockwave::Priority_Update(_float fTimeDelta)
{
	// 삭제할 기준 만들어야됨?
	if (m_fDuration < 0.f)
		Set_Dead();

	
}

void CProjectile_Shockwave::Update(_float fTimeDelta)
{
	m_fScale += fTimeDelta * 15.f;
	m_pTransformCom->Scaling(m_fScale, m_fScale, m_fScale);

	m_fDuration -= fTimeDelta;
	
	m_pOuterCollider->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
	m_pInnerCollider->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CProjectile_Shockwave::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Shockwave::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG

	m_pOuterCollider->Render();
	m_pInnerCollider->Render();

#endif

	return S_OK;
}

HRESULT CProjectile_Shockwave::On_Collision(CGameObject* Other, class CCollider* pCollider)
{
	if (m_IsColl)
		return S_OK;

	if (static_cast<CCombatStat*>(Other->Get_Component(TEXT("Com_Combat")))->Get_bInvinsible())
		return S_OK;

	if (m_pOuterCollider->Get_IsColl() && !m_pInnerCollider->Get_IsColl())
	{
		m_pCombatCom->Attack(static_cast<CCombatStat*>(Other->Get_Component(TEXT("Com_Combat"))));
		m_IsColl = true;
	}
	
	
	return S_OK;
}


HRESULT CProjectile_Shockwave::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shockwave"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 0.5f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Outer"), reinterpret_cast<CComponent**>(&m_pOuterCollider), &eDesc)))
		return E_FAIL;

	eDesc.fRadius = 0.4f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Inner"), reinterpret_cast<CComponent**>(&m_pInnerCollider), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 1;
	eCombatDesc.iMaxHp = 1;
	eCombatDesc.iDamage = 10;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;

	return S_OK;
}

CProjectile_Shockwave* CProjectile_Shockwave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Shockwave* pInstance = new CProjectile_Shockwave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjecttile_Shockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Shockwave::Clone(void* pArg)
{
	CProjectile_Shockwave* pInstance = new CProjectile_Shockwave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjecttile_Shockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Shockwave::Free()
{
	__super::Free();

	Safe_Release(m_pOuterCollider);
	Safe_Release(m_pInnerCollider);
	
}
