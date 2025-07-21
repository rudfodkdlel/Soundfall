#include "Weapon_Defender.h"
#include "GameInstance.h"
#include "Model.h"

CWeapon_Defender::CWeapon_Defender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{pDevice, pContext}
{
}

CWeapon_Defender::CWeapon_Defender(const CWeapon_Defender& Prototype)
	:CPartObject{Prototype}
{
}

HRESULT CWeapon_Defender::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CWeapon_Defender::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	m_eMainState = pDesc->eMainState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(1.25f, 1.25f, 1.25f);
	_vector vAxis = { 1.f,0.f,0.f,0.f };
	m_pTransformCom->Rotation(vAxis, XMConvertToRadians(-90.f));

	m_pGameInstance->Add_Collider(CG_WEAPON_MONSTER, m_pColliderCom, this);


	
	return S_OK;
}

void CWeapon_Defender::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Defender::Update(_float fTimeDelta)
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);


	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
	);

	if (STATE_MAIN::ATTACK == *m_eMainState)
		m_fDelay -= fTimeDelta;
	else
		m_fDelay = 0.6f;

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CWeapon_Defender::Late_Update(_float fTimeDelta)
{
	

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CWeapon_Defender::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		// alpha ¾È¾²´Â	
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->Begin(0)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG

	//m_pColliderCom->Render();

#endif


	return S_OK;
}

HRESULT CWeapon_Defender::On_Collision(CCollider* pCollider)
{
	if (STATE_MAIN::ATTACK == *m_eMainState && m_fDelay < 0.f)
	{
		m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));

		CGameObject::GAMEOBJECT_DESC eDesc = {};

		_float3 vMtv = m_pColliderCom->Get_Bounding()->Get_Mtv();
		_vector vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f };
		XMStoreFloat4(&eDesc.vPos, vPos + XMLoadFloat3(&vMtv));

		m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Hit_Effect_Paticle"), m_pGameInstance->Get_Current_Level(),
			TEXT("Layer_Effect"), &eDesc);
	}
	else
	{
		m_pGameInstance->StopSound(SOUND_MONSTER_EFFECT);
		m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Defender_ShieldDeploy_2.wav"), SOUND_MONSTER_EFFECT, 1.f);
	}
	

	return S_OK;
}

HRESULT CWeapon_Defender::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Shield"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::OBB_DESC	OBBDesc{};
	OBBDesc.vExtents = _float3(1.f, 2.f, 1.5f);
	OBBDesc.vCenter = _float3(0.75f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 20;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Defender::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	return S_OK;
}

CWeapon_Defender* CWeapon_Defender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Defender* pInstance = new CWeapon_Defender(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Defender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Defender::Clone(void* pArg)
{
	CWeapon_Defender* pInstance = new CWeapon_Defender(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Defender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Defender::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}

