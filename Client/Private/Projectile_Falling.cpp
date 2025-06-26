#include "Projectile_Falling.h"
#include "GameInstance.h"

CProjectile_Falling::CProjectile_Falling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base{pDevice, pContext}
{
}

CProjectile_Falling::CProjectile_Falling(const CProjectile_Falling& Prototype)
	:CProjectile_Base{Prototype}
{
}

HRESULT CProjectile_Falling::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Falling::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return S_OK;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    m_pTransformCom->Scaling(1.f, 5.f, 1.f);


    _vector vPos = { 0.f, 100.f,0.f,0.f };
    vPos += m_pTransformCom->Get_State(STATE::POSITION);
    m_pTransformCom->Set_State(STATE::POSITION, vPos);


    return S_OK;
}

void CProjectile_Falling::Priority_Update(_float fTimeDelta)
{
    
    if (m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] < 0.f)
        Set_Dead();
}

void CProjectile_Falling::Update(_float fTimeDelta)
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vDir = XMLoadFloat4(&m_vDir);
    vPos += vDir * 50 * fTimeDelta;

    m_pTransformCom->Set_State(STATE::POSITION, vPos);


    __super::Billboarding();

    
}

void CProjectile_Falling::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Falling::Render()
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

HRESULT CProjectile_Falling::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
}

CProjectile_Falling* CProjectile_Falling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProjectile_Falling* pInstance = new CProjectile_Falling(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_Artilery");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_Falling::Clone(void* pArg)
{
    CProjectile_Falling* pInstance = new CProjectile_Falling(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_Artilery");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_Falling::Free()
{
    __super::Free();
}
