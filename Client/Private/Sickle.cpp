#include "Sickle.h"
#include "GameInstance.h"
#include "CombatStat.h"

CSickle::CSickle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeapon_Base{ pDevice, pContext }
{
}

CSickle::CSickle(const CSickle& Prototype)
    :CWeapon_Base{ Prototype }
{
}

HRESULT CSickle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSickle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Scaling(1.25f, 1.25f, 1.25f);
    _fvector vDir = { 1.f,0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(-90.f));

    m_eWeaponType = WEAPON::SICKLE;

    m_pColliderCom->Set_Active(false);

    m_pGameInstance->Add_Collider(CG_WEAPON_PLAYER, m_pColliderCom, this);

    m_pCombatCom->Set_bInvinsible(true);

    return S_OK;
}

void CSickle::Priority_Update(_float fTimeDelta)
{
}

void CSickle::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSickle::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CSickle::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CSickle::On_Collision(CCollider* pCollider)
{
    if (m_HitObjectSet.find(pCollider) == m_HitObjectSet.end())
    {
        m_HitObjectSet.insert(pCollider);
        // 수정 필요함
        m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));
    }


    return S_OK;
}

void CSickle::Attack(_vector vDir)
{
    // 키로 나눠서 좀 생성하도록?
}

HRESULT CSickle::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sickle"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_Sphere::SPHERE_DESC eDesc{};
    eDesc.fRadius = 3.f;
    eDesc.vCenter = _float3(1.f, 1.5f, 1.f);
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &eDesc)))
        return E_FAIL;

    CCombatStat::COMBAT_DESC eCombatDesc = {};
    eCombatDesc.iCurrentHp = 1;
    eCombatDesc.iMaxHp = 1;
    eCombatDesc.iDamage = 30;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
        TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
        return E_FAIL;

    m_strModelTag = TEXT("Prototype_Component_Model_Sickle");

    return S_OK;
}

HRESULT CSickle::Bind_ShaderResources()
{
    return S_OK;
}

CSickle* CSickle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSickle* pInstance = new CSickle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSickle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSickle::Clone(void* pArg)
{
    CSickle* pInstance = new CSickle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSickle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSickle::Free()
{
    __super::Free();
    Safe_Release(m_pCombatCom);
}
