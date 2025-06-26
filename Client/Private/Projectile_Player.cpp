#include "Projectile_Player.h"
#include "GameInstance.h"
#include "CombatStat.h"

CProjectile_Player::CProjectile_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base(pDevice, pContext)
{
}

CProjectile_Player::CProjectile_Player(const CProjectile_Player& Prototype)
	:CProjectile_Base(Prototype)
{
}

HRESULT CProjectile_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Player::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE, m_pColliderCom, this);

	PROJECTILE_DESC* pDesc = static_cast<PROJECTILE_DESC*>(pArg);

	m_pTransformCom->Scaling(2.f, 1.f, 1.f);

	float yaw = atan2f(XMVectorGetX(XMLoadFloat4(&m_vDir)), XMVectorGetZ(XMLoadFloat4(&m_vDir))); 

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), yaw, 0.0f);

	_matrix matRotaion = XMMatrixRotationQuaternion(vQuaternion);

	XMStoreFloat4x4(m_pTransformCom->Get_WorldMatrix(), matRotaion * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) );

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));


	m_fMaxDistance = pDesc->fMaxDistance;
	

	return S_OK;
}

void CProjectile_Player::Priority_Update(_float fTimeDelta)
{
	// 삭제할 기준 만들어야됨?

	if (m_IsColl || m_fDuration < 0.f)
	{
		Set_Dead();

		CGameObject::GAMEOBJECT_DESC eDesc = {};
		eDesc.fRotationPerSec = 90.f;

		XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));

		if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Music_Note"),
			m_pGameInstance->Get_Current_Level(), TEXT("Layer_Paticle") , &eDesc)))
			return;

		
	}
		
}

void CProjectile_Player::Update(_float fTimeDelta)
{
	// 이동하는 기능 추가
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
	



	

	// 카메라 쳐다보도록
	//__super::Billboarding();

	
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
}

void CProjectile_Player::Late_Update(_float fTimeDelta)
{
	if(!m_IsColl)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Player::Render()
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

	m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CProjectile_Player::On_Collision(class CCollider* pCollider)
{
	m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));

	m_IsColl = true;

	m_pColliderCom->Set_Active(false);
	
	return S_OK;
}


HRESULT CProjectile_Player::Ready_Components()
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

CProjectile_Player* CProjectile_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Player* pInstance = new CProjectile_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjecttile_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Player::Clone(void* pArg)
{
	CProjectile_Player* pInstance = new CProjectile_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjecttile_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Player::Free()
{
	__super::Free();
}
