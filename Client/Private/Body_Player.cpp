#include "Body_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "Model.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CBody_Player::CBody_Player(const CBody_Player& Prototype)
	: CPartObject{ Prototype }
{

}

const _float4x4* CBody_Player::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	BODY_PLAYER_DESC* pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pCombatCom = pDesc->pCombatcom;
	Safe_AddRef(m_pCombatCom);

	m_Components.emplace(TEXT("Com_Combat"), m_pCombatCom);

	m_pGameInstance->Add_Collider(CG_PLAYER, m_pColliderCom, this);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{

}

void CBody_Player::Update(_float fTimeDelta)
{

	/*m_bFinished = m_pModelCom->Play_Animation(fTimeDelta);
	
	if (m_bFinished)
	{
		m_pGameInstance->Notify(TEXT("Observer_Animation_Player"), TEXT("AnimEnd"));
	}*/

	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CBody_Player::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	if(m_pColliderCom->Get_Active())
		m_pColliderCom->Render();

#endif


	return S_OK;
}

HRESULT CBody_Player::On_Collision(CCollider* pCollider)
{
	_float3 otherCenter = pCollider->Get_Center();
	_float3 myCenter = m_pColliderCom->Get_Center();


	_vector vDir = XMVectorSetW(XMLoadFloat3(&otherCenter) - XMLoadFloat3(&myCenter), 0.f);

	_float3 f3Mtv = m_pColliderCom->Get_Bounding()->Get_Mtv();

	_float4 vMtv = {};
	XMStoreFloat4(&vMtv, XMVectorSetW(XMLoadFloat3(&f3Mtv), 0.f));

	vMtv.y = 0;

	m_eHitDir = Calc_Hit_Dir(XMVector3Normalize(XMLoadFloat4(&vMtv)));

	if(CG_MONSTER == pCollider->Get_Group() || CG_STRUCTURE_WALL == pCollider->Get_Group())
		m_pushVectors.push_back(vMtv);



	return S_OK;
}

eDirState CBody_Player::Calc_Hit_Dir(_vector vDir)
{
	_vector vLook = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_CombinedWorldMatrix.m[2]));
	vLook = XMVector3Normalize(vLook);

	if (XMVector3Equal(vDir, XMVectorZero()))
		return NONE;

	vDir = XMVector3Normalize(vDir);

	_float dot = XMVectorGetX(XMVector3Dot(vDir, vLook));

	// 기준 각도 값
	const _float COS_45 = 0.707f;
	const _float COS_135 = -0.707f;

	// 앞
	if (dot >= COS_45)
		return F;
	// 뒤
	else if (dot <= COS_135)
		return B;
	// 좌우 (dot이 중간이면)
	else
	{
		_vector vRight = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_CombinedWorldMatrix.m[0]));
		vRight = XMVector3Normalize(vRight);

		_float rightDot = XMVectorGetX(XMVector3Dot(vDir, vRight));

		if (rightDot >= 0)
			return R;
		else
			return L;
	}

	return DIR_STATE::NONE;
}

HRESULT CBody_Player::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Ky"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::AABB_DESC	AABBDesc{};
	AABBDesc.vExtents = _float3(2.f, 3.f, 2.f);
	AABBDesc.vCenter = _float3(0.0f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
	

	return S_OK;
}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
