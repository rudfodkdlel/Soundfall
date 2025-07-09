#include "Projectile_Artillery.h"
#include "GameInstance.h"

CProjectile_Artillery::CProjectile_Artillery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile_Base(pDevice, pContext)
{
}

CProjectile_Artillery::CProjectile_Artillery(const CProjectile_Artillery& Prototype)
    :CProjectile_Base(Prototype)
{
}

HRESULT CProjectile_Artillery::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Artillery::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return S_OK;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    m_pTransformCom->Scaling(8.f, 8.f, 8.f);
    _vector vDir = { 1.f, 0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));


    _vector vPos = { 0.f,0.1f + m_pGameInstance->Compute_Random_Normal() * 0.005f,0.f,0.f};
    vPos += m_pTransformCom->Get_State(STATE::POSITION);
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
    

    return S_OK;
}

void CProjectile_Artillery::Priority_Update(_float fTimeDelta)
{
    if (4 == m_iTextureType && 0.f < m_fDuration)
        Set_Dead();
}

void CProjectile_Artillery::Update(_float fTimeDelta)
{
    m_fDuration -= fTimeDelta;

    if (m_fDuration < 0.f)
    {
        ++m_iTextureType;

        if (m_iTextureType == 2)
        {
            // falling 추가

            CProjectile_Base::PROJECTILE_DESC eDesc = {};
            eDesc.fSpeedPerSec = 1.f;

            eDesc.vColor = { 1.f,1.f,1.f,1.f };

            _vector vDir = {0.f,-1.f,0.f,0.f};

            vDir = XMVector3Normalize(vDir);

            XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
            XMStoreFloat4(&eDesc.vDir, vDir);
            // 투사체 생성해서 날아가게 해보자
            m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Falling"), m_pGameInstance->Get_Current_Level(),
                TEXT("Layer_Projectile_Monster"), &eDesc);
        }
        m_fDuration = 1.f;
    }
}

void CProjectile_Artillery::Late_Update(_float fTimeDelta)
{
    
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_BLEND, this);
}

HRESULT CProjectile_Artillery::Render()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureType)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_Artillery::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Artillery_Projectile"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CProjectile_Artillery* CProjectile_Artillery::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProjectile_Artillery* pInstance = new CProjectile_Artillery(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_Artilery");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_Artillery::Clone(void* pArg)
{
    CProjectile_Artillery* pInstance = new CProjectile_Artillery(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_Artilery");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_Artillery::Free()
{
    __super::Free();
}

