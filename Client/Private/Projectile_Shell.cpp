#include "Projectile_Shell.h"
#include "GameInstance.h"

CProjectile_Shell::CProjectile_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile_Base{pDevice, pContext}
{
}

CProjectile_Shell::CProjectile_Shell(const CProjectile_Shell& Prototype)
    :CProjectile_Base{ Prototype }
{
}

HRESULT CProjectile_Shell::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Shell::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	// 방향 수정 , y축 보정

	m_pTransformCom->Scaling(0.75f, 0.75f, 0.75f);

	XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));

	_vector vDir = XMLoadFloat4(&m_vDir);

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(90.f));

	XMVECTOR vRotatedDir = XMVector3TransformNormal(vDir, matRot);

	XMStoreFloat4(&m_vDir, vRotatedDir);

	return S_OK;
}

void CProjectile_Shell::Priority_Update(_float fTimeDelta)
{
	// 높이 이상이면 없애기 
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	if (vPos.m128_f32[1] < 0)
		Set_Dead();
}

void CProjectile_Shell::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = XMVector3Normalize(XMLoadFloat4(&m_vDir)); 
	
	vPos = XMLoadFloat4(&m_vStartPos) + vDir * m_fElapsedTime * 10.f + 0.5f * XMLoadFloat3(&m_vGravity)* m_fElapsedTime* m_fElapsedTime;


	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	// 카메라 쳐다보도록
	__super::Billboarding();

}

void CProjectile_Shell::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Shell::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CProjectile_Shell::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CProjectile_Shell* CProjectile_Shell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Shell* pInstance = new CProjectile_Shell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Shell::Clone(void* pArg)
{
	CProjectile_Shell* pInstance = new CProjectile_Shell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectile_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Shell::Free()
{
	__super::Free();
}

