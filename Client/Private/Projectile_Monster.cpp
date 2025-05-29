#include "Projectile_Monster.h"
#include "GameInstance.h"

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

    return S_OK;
}

void CProjectile_Monster::Priority_Update(_float fTimeDelta)
{
	// 삭제할 기준 만들어야됨?
}

void CProjectile_Monster::Update(_float fTimeDelta)
{
    // 이동하는 기능 추가


	// 카메라 쳐다보도록
	__super::Billboarding();
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


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CProjectile_Monster::Ready_Components()
{
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
