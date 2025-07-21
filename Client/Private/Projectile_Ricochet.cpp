#include "Projectile_Ricochet.h"
#include "GameInstance.h"
#include "Music_Note.h"
#include "CombatStat.h"

CProjectile_Ricochet::CProjectile_Ricochet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base{pDevice, pContext}
{
}

CProjectile_Ricochet::CProjectile_Ricochet(const CProjectile_Ricochet& Prototype)
	:CProjectile_Base{ Prototype }
{
}

HRESULT CProjectile_Ricochet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Ricochet::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE, m_pColliderCom, this);

	PROJECTILE_DESC* pDesc = static_cast<PROJECTILE_DESC*>(pArg);

	float yaw = atan2f(XMVectorGetX(XMLoadFloat4(&m_vDir)), XMVectorGetZ(XMLoadFloat4(&m_vDir)));

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), yaw, 0.0f);

	_matrix matRotaion = XMMatrixRotationQuaternion(vQuaternion);

	XMStoreFloat4x4(m_pTransformCom->Get_WorldMatrix(), matRotaion * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

	_vector vPos = XMLoadFloat4(&pDesc->vPos);

	vPos.m128_f32[1] += 0.5f;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pTransformCom->Scaling(1.f, 2.f, 1.f);


	m_fMaxDistance = pDesc->fMaxDistance;


	return S_OK;
}

void CProjectile_Ricochet::Priority_Update(_float fTimeDelta)
{
	if (m_iCountChangeDir > 5 || m_fDuration < 0.f)
	{
		Set_Dead();
		CMusic_Note::MUSICNOTE_DESC eDesc = {};
		eDesc.fRotationPerSec = 90.f;
		eDesc.isSpread = true;

		XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));

		if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Music_Note"),
			m_pGameInstance->Get_Current_Level(), TEXT("Layer_Paticle"), &eDesc)))
			return;
	}
		
}

void CProjectile_Ricochet::Update(_float fTimeDelta)
{
	if (m_bChange)
	{
		m_fCoolTime -= fTimeDelta;
	}

	if (m_fCoolTime < 0.f)
	{
		m_bChange = false;
		m_fCoolTime = 1.f;
	}

	if (m_fMaxDistance > m_fDistance)
	{
		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vDir = XMLoadFloat4(&m_vDir);
		vPos += vDir * 10 * fTimeDelta;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);
		m_fDistance += 10 * fTimeDelta;
	}
	else
	{
		m_fDuration -= fTimeDelta;
	}

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CProjectile_Ricochet::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONLIGHT, this);
}

HRESULT CProjectile_Ricochet::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG

#endif

	return S_OK;
}

HRESULT CProjectile_Ricochet::On_Collision(CCollider* pCollider)
{
	m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));

	if(!m_bChange)
		Change_Dir();

	return S_OK;
}

void CProjectile_Ricochet::Change_Dir()
{
	_vector vDir = {};

	vDir = XMLoadFloat4(&m_vDir);

	vDir *= -1;

	_float fAngle = m_pGameInstance->Compute_Random(-30.f, 30.f);
	
	_vector vAxis = { 0.f,1.f,0.f,0.f };

	vDir = XMVector3TransformNormal(vDir, XMMatrixRotationAxis(vAxis,XMConvertToRadians(fAngle)));

	XMStoreFloat4(&m_vDir, vDir);


	++m_iCountChangeDir;

	m_fDistance = 0.f;

	m_bChange = true;

	CMusic_Note::MUSICNOTE_DESC eDesc = {};
	eDesc.fRotationPerSec = 90.f;
	eDesc.isSpread = true;

	XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Music_Note"),
		m_pGameInstance->Get_Current_Level(), TEXT("Layer_Paticle"), &eDesc)))
		return;


}

HRESULT CProjectile_Ricochet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* For.Com_Collider */
	CBounding_Sphere::SPHERE_DESC eDesc{};
	eDesc.fRadius = 0.5f;
	eDesc.vCenter = { 0.f,0.f,0.f };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eCombatDesc = {};
	eCombatDesc.iCurrentHp = 1;
	eCombatDesc.iMaxHp = 1;
	eCombatDesc.iDamage = 20;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
		return E_FAIL;


	return S_OK;
}

CProjectile_Ricochet* CProjectile_Ricochet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Ricochet* pInstance = new CProjectile_Ricochet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Ricochet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Ricochet::Clone(void* pArg)
{
	CProjectile_Ricochet* pInstance = new CProjectile_Ricochet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectile_Ricochet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Ricochet::Free()
{
	__super::Free();
}