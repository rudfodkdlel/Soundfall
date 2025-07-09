#include "Keyboard.h"
#include "GameInstance.h"
#include "Projectile_Player.h"

CKeyboard::CKeyboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeapon_Base{ pDevice, pContext }
{
}

CKeyboard::CKeyboard(const CKeyboard& Prototype)
    :CWeapon_Base{ Prototype }
{
}

HRESULT CKeyboard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKeyboard::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(1.25f, 1.25f, 1.25f);
    // m_pTransformCom->Rotation( XMConvertToRadians(-90.f), 0.f, 0.f);
    _fvector vDir = { 1.f,0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(-90.f));

    m_eWeaponType = WEAPON::KEYBOARD;

    m_DirMap = { {F,71}, {B,68}, {BL,69}, {BR,70}, {FL, 72}, {FR, 73}, {L, 72}, {R,73},  {NONE, 29} };

    m_HitReactMap = { {B,41}, {F,42}, {L,43}, {R,44} };

    m_fDelay = 0.f;

    m_vColor = { 0.f,0.f,1.f,1.f };

    return S_OK;
}

void CKeyboard::Priority_Update(_float fTimeDelta)
{
}

void CKeyboard::Update(_float fTimeDelta)
{
    m_fDelay -= fTimeDelta;


    if (m_pGameInstance->Key_Up(DIM::LBUTTON))
    {
        m_IsFire = false;
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

void CKeyboard::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CKeyboard::Render()
{
    __super::Render();

    return S_OK;
}

void CKeyboard::Attack(_vector vDir)
{
    // 투사체 생성해서 날아가게 해보자
    m_IsFire = true;

    if (m_fDelay >= 0.f)
        return;

    if (!m_IsPerfect)
    {
        --m_iOverloadCount;

        if (m_iOverloadCount == 0)
        {
            m_fOverloadTime = 0.5f;
        }
    }

    if (m_iCurrentAmmo <= 0)
    {
        return;
    }
    --m_iCurrentAmmo;

    CProjectile_Player::PROJECTILE_DESC eDesc = {};
    eDesc.fSpeedPerSec = 1.f;
    eDesc.fMaxDistance = 25;
    if ((m_IsPerfect))
    {
        eDesc.vColor = m_vColor;
    }
    else
    {
        eDesc.vColor = { 0.01f,0.f,0.f,1.f };
    }

    _vector vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42 + 0.8f , m_CombinedWorldMatrix._43 , 1.f };
    XMStoreFloat4(&eDesc.vPos, vPos);

    _vector vRotateDir = {};

    for (int i = 0; i < 5; ++i)
    {
        vRotateDir = XMVector3TransformNormal(vDir, XMMatrixRotationY(XMConvertToRadians(-30.f + 15 * i)));
        XMStoreFloat4(&eDesc.vDir, vRotateDir);
        // 투사체 생성해서 날아가게 해보자
        m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Player"), m_pGameInstance->Get_Current_Level(),
            TEXT("Layer_Projectile_Player"), &eDesc);
    }

    m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Shell"), m_pGameInstance->Get_Current_Level(),
        TEXT("Layer_Projectile_Shell"), &eDesc);


    // 소리 추가
    m_pGameInstance->StopSound(SOUND_WEAPON);
    m_pGameInstance->PlaySound(TEXT("Shotgun_Attack.wav"), SOUND_WEAPON, 0.7f);
   
    m_fDelay = 0.2f;
}

void CKeyboard::Reset()
{
    m_iCurrentAmmo = 1;
    m_iOverloadCount = 2;
}

HRESULT CKeyboard::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Keyboard"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    m_strModelTag = TEXT("Prototype_Component_Model_Keyboard");

    return S_OK;
}

HRESULT CKeyboard::Bind_ShaderResources()
{
    return S_OK;
}

CKeyboard* CKeyboard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKeyboard* pInstance = new CKeyboard(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CKeyboard");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKeyboard::Clone(void* pArg)
{
    CKeyboard* pInstance = new CKeyboard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CKeyboard");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKeyboard::Free()
{
    
    __super::Free();


}
