#include "Discord_Wall.h"
#include "GameInstance.h"
#include "Model.h"
#include "CombatStat.h"

CDiscord_Wall::CDiscord_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CDiscord_Wall::CDiscord_Wall(const CDiscord_Wall& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CDiscord_Wall::Initialize_Prototype()
{





	return S_OK;
}

HRESULT CDiscord_Wall::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));
	_vector vAxis = { 0.f, 1.f, 0.f,0.f };
	m_pTransformCom->Rotation(vAxis, XMConvertToRadians(90.f));

	m_pGameInstance->Add_Collider(CG_STRUCTURE_WALL, m_pColliderCom, this);

    return S_OK;
}

void CDiscord_Wall::Priority_Update(_float fTimeDelta)
{
	if (m_pCombatCom->Get_Current_HP() <= 0)
	{
		Set_Dead();
		m_pGameInstance->StopSound(SOUND_MONSTER_DEATH);
		m_pGameInstance->PlaySound(TEXT("NPC_Enemy_Discord_Wall_Popup_0.wav"), SOUND_MONSTER_DEATH, 1.f);
	}
		
}

void CDiscord_Wall::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

}

void CDiscord_Wall::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CDiscord_Wall::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		// alpha ¾È¾²´Â	
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


#ifdef _DEBUG

	

#endif

	return S_OK;
}

HRESULT CDiscord_Wall::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Discord_Wall"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CCombatStat::COMBAT_DESC eDesc = {};
	eDesc.iCurrentHp = 100;
	eDesc.iMaxHp = 100;
	eDesc.iDamage = 0;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
		TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::AABB_DESC	AABBDesc{};
	AABBDesc.vExtents = _float3(2.f, 4.f, 7.f);
	AABBDesc.vCenter = _float3(0.0f, 0.f, -5.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;


	return S_OK;
}

CDiscord_Wall* CDiscord_Wall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CDiscord_Wall* pInstance = new CDiscord_Wall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDiscord_Wall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDiscord_Wall::Clone(void* pArg)
{
	CDiscord_Wall* pInstance = new CDiscord_Wall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDiscord_Wall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDiscord_Wall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
