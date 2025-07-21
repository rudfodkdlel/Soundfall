#include "Projectile_Fire.h"
#include "GameInstance.h"
#include "CombatStat.h"
#include "Bounding_Sphere.h"

CProjectile_Fire::CProjectile_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base{pDevice, pContext}
{
}

CProjectile_Fire::CProjectile_Fire(const CProjectile_Fire& Prototype)
	:CProjectile_Base{Prototype}
{
}

HRESULT CProjectile_Fire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Fire::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(30.f, 30.f, 30.f);
	_vector vDir = { 1.f, 0.f,0.f,0.f };
	m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));


	_vector vPos = { 0.f, m_pGameInstance->Compute_Random_Normal() * 0.05f,0.f,0.f };
	vPos += m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE, m_pColliderCom, this);

	return S_OK;
}

void CProjectile_Fire::Priority_Update(_float fTimeDelta)
{
	if (m_fElaspedTime > 5.f)
		Set_Dead();
}

void CProjectile_Fire::Update(_float fTimeDelta)
{
	m_fElaspedTime += fTimeDelta;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

}

void CProjectile_Fire::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_BLEND, this);
}

HRESULT CProjectile_Fire::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fElaspedTime, sizeof(m_fElaspedTime))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_ChannelTexture", 1)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_HighLightTexture", 2)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	


#endif

	return S_OK;
}

HRESULT CProjectile_Fire::On_Collision(CCollider* pCollider)
{
	
	m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));
	

	return S_OK;
}

HRESULT CProjectile_Fire::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Lava"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* For.Com_Collider */
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 0.25f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 1;
	eCombatDesc.iMaxHp = 1;
	eCombatDesc.iDamage = 3;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;
}

CProjectile_Fire* CProjectile_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Fire* pInstance = new CProjectile_Fire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Fire::Clone(void* pArg)
{
	CProjectile_Fire* pInstance = new CProjectile_Fire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectile_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Fire::Free()
{
	__super::Free();
}
