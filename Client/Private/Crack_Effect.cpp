#include "Crack_Effect.h"
#include "GameInstance.h"
#include "Collider.h"
#include "CombatStat.h"

CCrack_Effect::CCrack_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{pDevice, pContext}
{
}

CCrack_Effect::CCrack_Effect(const CCrack_Effect& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CCrack_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrack_Effect::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Scaling(50.f, 50.f, 50.f);
	_vector vDir = { 1.f, 0.f,0.f,0.f };
	m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));

	_vector vPos = XMLoadFloat4(&pDesc->vPos);
	vPos.m128_f32[1] += 0.1f;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE, m_pColliderCom, this);

	return S_OK;
}

void CCrack_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_fElapsedTime > 1.f)
		Set_Dead();

	if (m_IsColl)
		m_pColliderCom->Set_Active(false);
}

void CCrack_Effect::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CCrack_Effect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONLIGHT, this);
}

HRESULT CCrack_Effect::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_vector m_vColor = { 1.f,1.f,1.f,1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fElapsedTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrack_Effect::On_Collision(CCollider* pCollider)
{
	m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));

	m_IsColl = true;

	return S_OK;
}

HRESULT CCrack_Effect::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crack_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 0.5f;
	eDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 1;
	eCombatDesc.iMaxHp = 1;
	eCombatDesc.iDamage = 100;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;
	return S_OK;
}

CCrack_Effect* CCrack_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCrack_Effect* pInstance = new CCrack_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrack_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrack_Effect::Clone(void* pArg)
{
	CCrack_Effect* pInstance = new CCrack_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrack_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrack_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
