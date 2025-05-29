#include "Keyboard.h"
#include "GameInstance.h"

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

    m_DirMap = { {F,71}, {B,68}, {BL,69}, {BR,70}, {FL, 72}, {FR, 73}, {L, 72}, {R,73} };

    return S_OK;
}

void CKeyboard::Priority_Update(_float fTimeDelta)
{
}

void CKeyboard::Update(_float fTimeDelta)
{
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

void CKeyboard::Attack()
{
    // 투사체 생성해서 날아가게 해보자
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
