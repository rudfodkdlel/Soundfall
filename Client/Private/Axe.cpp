#include "Axe.h"
#include "GameInstance.h"
#include "CombatStat.h"

CAxe::CAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeapon_Base{ pDevice, pContext }
{
}

CAxe::CAxe(const CAxe& Prototype)
    :CWeapon_Base{ Prototype }
{
}

HRESULT CAxe::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAxe::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Scaling(1.25f, 1.25f, 1.25f);
    _fvector vDir = { 1.f,0.f,0.f,0.f };
    m_pTransformCom->Rotation(vDir, XMConvertToRadians(-90.f));

    m_eWeaponType = WEAPON::AXE;

    m_IsActive = false;

    m_pColliderCom->Set_Active(false);

    m_pGameInstance->Add_Collider(CG_WEAPON_PLAYER, m_pColliderCom, this);

    m_pCombatCom->Set_bInvinsible(true);

    return S_OK;
}

void CAxe::Priority_Update(_float fTimeDelta)
{
}

void CAxe::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CAxe::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CAxe::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CAxe::On_Collision(CGameObject* Other, CCollider* pCollider)
{
    if (m_HitObjectSet.find(pCollider) == m_HitObjectSet.end())
    {
        m_HitObjectSet.insert(pCollider);
        // 수정 필요함
        m_pCombatCom->Attack(static_cast<CCombatStat*>(Other->Get_Component(TEXT("Com_Combat"))));
    }
       

    return S_OK;
}

void CAxe::Attack(_vector vDir)
{
}

HRESULT CAxe::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Axe"),
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
    eCombatDesc.iDamage = 40;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CombatStat"),
        TEXT("Com_Combat"), reinterpret_cast<CComponent**>(&m_pCombatCom), &eCombatDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CAxe::Bind_ShaderResources()
{
    return S_OK;
}

CAxe* CAxe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAxe* pInstance = new CAxe(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CAxe");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAxe::Clone(void* pArg)
{
    CAxe* pInstance = new CAxe(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAxe");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAxe::Free()
{
    __super::Free();
    Safe_Release(m_pCombatCom);
}

