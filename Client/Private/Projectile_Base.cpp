#include "Projectile_Base.h"
#include "GameInstance.h"
#include "CombatStat.h"

CProjectile_Base::CProjectile_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CProjectile_Base::CProjectile_Base(const CProjectile_Base& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CProjectile_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Base::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	PROJECTILE_DESC* pDesc = static_cast<PROJECTILE_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치 지정해주기,	방향 지정?

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPos));

	m_vDir = pDesc->vDir;

	m_vColor = pDesc->vColor;

	return S_OK;
}

void CProjectile_Base::Priority_Update(_float fTimeDelta)
{
}

void CProjectile_Base::Update(_float fTimeDelta)
{
	Billboarding();
}

void CProjectile_Base::Late_Update(_float fTimeDelta)
{
}

HRESULT CProjectile_Base::Render()
{
    return S_OK;
}

void CProjectile_Base::Make_Trail()
{
}


HRESULT CProjectile_Base::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

    return S_OK;
}


void CProjectile_Base::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCombatCom);
}
