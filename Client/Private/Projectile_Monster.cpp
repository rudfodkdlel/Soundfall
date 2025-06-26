#include "Projectile_Monster.h"
#include "GameInstance.h"
#include "CombatStat.h"

CProjectile_Monster::CProjectile_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile_Base(pDevice, pContext)
{
}

CProjectile_Monster::CProjectile_Monster(const CProjectile_Monster& Prototype)
    :CProjectile_Base(Prototype)
{
}

HRESULT CProjectile_Monster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Monster::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return S_OK;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE, m_pColliderCom, this);

    return S_OK;
}

void CProjectile_Monster::Priority_Update(_float fTimeDelta)
{
	if (m_IsColl)
		Set_Dead();
}

void CProjectile_Monster::Update(_float fTimeDelta)
{

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = XMLoadFloat4(&m_vDir);
	vPos += vDir * 10 * fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	// 카메라 쳐다보도록
	__super::Billboarding();

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CProjectile_Monster::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Monster::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
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

	//m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CProjectile_Monster::On_Collision( class CCollider* pCollider)
{
	m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));

	m_IsColl = true;

	m_pColliderCom->Set_Active(false);

	CGameObject::GAMEOBJECT_DESC eDesc = {};

	_float3 vMtv = m_pColliderCom->Get_Bounding()->Get_Mtv();

	XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&vMtv));

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Hit_Effect_Texture"), m_pGameInstance->Get_Current_Level(),
		TEXT("Layer_Effect"), &eDesc);
	
	return S_OK;
}

HRESULT CProjectile_Monster::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 1.f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 1;
	eCombatDesc.iMaxHp = 1;
	eCombatDesc.iDamage = 5;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;



    return S_OK;
}

CProjectile_Monster* CProjectile_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Monster* pInstance = new CProjectile_Monster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjecttile_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Monster::Clone(void* pArg)
{
	CProjectile_Monster* pInstance = new CProjectile_Monster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjecttile_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Monster::Free()
{
	__super::Free();
}
