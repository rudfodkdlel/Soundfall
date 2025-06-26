#include "Rifle.h"
#include "GameInstance.h"
#include "Projectile_Player.h"

CRifle::CRifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeapon_Base{pDevice, pContext}
{
}

CRifle::CRifle(const CRifle& Prototype)
    :CWeapon_Base{Prototype}
{
}

HRESULT CRifle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRifle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(1.25f, 1.25f, 1.25f);
   // m_pTransformCom->Rotation( XMConvertToRadians(-90.f), 0.f, 0.f);
    _fvector vDir = { 1.f,0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(-90.f));

    m_eWeaponType = WEAPON::ASSAULT;

    m_DirMap = { {F,10}, {B,9}, {BL,50}, {BR,51}, {FL, 52}, {FR, 53}, {L, 52}, {R,53}, {NONE, 4} };

    m_HitReactMap = { {B,31}, {F,32}, {L,45}, {R,46} };

    // 4 beat 간격 동안 발사 가능함
    m_fMaxDuration = m_pGameInstance->Get_BeatInterval() * 4;

    m_vColor = { 0.f,1.f,0.f,1.f };

    return S_OK;
}

void CRifle::Priority_Update(_float fTimeDelta)
{
}

void CRifle::Update(_float fTimeDelta)
{
   
    m_fDelay -= fTimeDelta;

    if (m_IsPerfect)
    {
        m_fDuration += fTimeDelta;
    }

    if (m_pGameInstance->Key_Up(DIM::LBUTTON))
    {
        m_IsFire = false;
        m_pGameInstance->StopSound(SOUND_WEAPON);
    }

    if (!m_IsFire)
    {
        m_fOverloadTime -= fTimeDelta;

        if (m_fOverloadTime <= 0.f)
        {
            Reset();
            m_fOverloadTime = 0.f;
        }
            
    }



    __super::Update(fTimeDelta);
}

void CRifle::Late_Update(_float fTimeDelta)
{
   


    __super::Late_Update(fTimeDelta);
}

HRESULT CRifle::Render()
{
    __super::Render();

    return S_OK;
}

void CRifle::Attack(_vector vDir)
{
    // 탄창이랑 이런거 추가해서 관리하도록 해야될듯\

    m_IsFire = true;

    if (m_fDelay >= 0.f)
        return;

    if (!m_IsPerfect)
        --m_iCurrentAmmo;

    if(m_fDuration >= m_fMaxDuration)
        return;

    if (m_iCurrentAmmo <= 0)
    {
        m_fOverloadTime = 0.5f;
        return;
    }
  
   

    CProjectile_Player::PROJECTILE_DESC eDesc = {};
    eDesc.fSpeedPerSec = 1.f;
    eDesc.fMaxDistance = 50;
    if ((m_IsPerfect))
    {
        eDesc.vColor = m_vColor;
    }
    else
    {
        eDesc.vColor = { 0.f,0.f,0.f,1.f };
    }

    _vector vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42 , m_CombinedWorldMatrix._43 , 1.f };


    XMStoreFloat4(&eDesc.vPos, vPos);
    XMStoreFloat4(&eDesc.vDir, vDir);
    // 투사체 생성해서 날아가게 해보자
    m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Player"), m_pGameInstance->Get_Current_Level(),
        TEXT("Layer_Projectile_Player"), &eDesc);

    m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Shell"), m_pGameInstance->Get_Current_Level(),
        TEXT("Layer_Projectile_Shell"), &eDesc);


    // 소리 추가
    m_pGameInstance->StopSound(SOUND_WEAPON);
    m_pGameInstance->PlaySound(TEXT("Assault_Attack.wav"), SOUND_WEAPON, 0.7f);
   
    if (!m_IsPerfect)
        m_fDelay = 0.3f;
    else
        m_fDelay = 0.1f;
}

void CRifle::Reset()
{
    m_iCurrentAmmo = 30;
    m_fDuration = 0.f;
}

HRESULT CRifle::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rifle"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRifle::Bind_ShaderResources()
{
    return S_OK;
}

CRifle* CRifle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRifle* pInstance = new CRifle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRifle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRifle::Clone(void* pArg)
{
    CRifle* pInstance = new CRifle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CRifle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRifle::Free()
{
    
    __super::Free();


}
