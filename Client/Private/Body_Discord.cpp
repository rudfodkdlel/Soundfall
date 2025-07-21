#include "Body_Discord.h"
#include "GameInstance.h"
#include "Model.h"
#include "Object_State_Spawn.h"
#include "Observer_Animation.h"

CBody_Discord::CBody_Discord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Discord::CBody_Discord(const CBody_Discord& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBody_Discord::Initialize_Prototype()
{




    return S_OK;
}

HRESULT CBody_Discord::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
    /* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

    m_pSocketMatrix = m_pModelCom->Get_BoneMatrix("cn_head_jnt");
    m_pCombatCom = pDesc->pCombatCom;

    Safe_AddRef(m_pCombatCom);

    m_Components.emplace(TEXT("Com_Combat"), m_pCombatCom);

    for (auto& collider : m_pColliderCom)
      m_pGameInstance->Add_Collider(CG_MONSTER, collider, this);

    for (auto& collider : m_pColliderCom)
        collider->Set_Active(false);

    return S_OK;
}

void CBody_Discord::Priority_Update(_float fTimeDelta)
{
    if (m_pColliderCom[0]->Get_IsColl())
        m_eHitDir = DIR_STATE::NONE;
}

void CBody_Discord::Update(_float fTimeDelta)
{


    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

    _matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    _float4x4 headMatrix = {};

    XMStoreFloat4x4(&headMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
    );

    m_pColliderCom[0]->Update(XMLoadFloat4x4(&headMatrix));

    for(int i =1; i<=4; ++i)
        m_pColliderCom[i]->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
 /*   for (auto& colliders : m_pColliderCom)
    {
      
    }*/
}

void CBody_Discord::Late_Update(_float fTimeDelta)
{

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);

}

HRESULT CBody_Discord::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint		iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, 6, 0)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
            return E_FAIL;

        m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

        if (m_isUseRimLight)
        {
            if (FAILED(m_pShaderCom->Begin(2)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }
       

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG

   
        
#endif


    return S_OK;
}

HRESULT CBody_Discord::On_Collision(CCollider* pCollider)
{

    _float3 otherCenter = pCollider->Get_Center();
    _float3 myCenter = m_pColliderCom[0]->Get_Center();


    _vector vDir = XMVectorSetW(XMLoadFloat3(&otherCenter) - XMLoadFloat3(&myCenter), 0.f);

    m_eHitDir = Calc_Hit_Dir(vDir);

    if (m_pCombatCom->Get_Damage() > 0)
    {
        m_pCombatCom->Attack(static_cast<CCombatStat*>(pCollider->Get_Owner()->Get_Component(TEXT("Com_Combat"))));
        for (int i = 1; i < 5; ++i)
            m_pColliderCom[i]->Set_Active(false);
        m_IsAttackHit = true;
    }
       

    return S_OK;
}

eDirState CBody_Discord::Calc_Hit_Dir(_vector vDir)
{
    _vector vLook = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_CombinedWorldMatrix.m[2]));
    vLook = XMVector3Normalize(vLook);

    if (XMVector3Equal(vDir, XMVectorZero()))
        return NONE;

    vDir = XMVector3Normalize(vDir);

    _float dot = XMVectorGetX(XMVector3Dot(vDir, vLook));

    // 기준 각도 값
    const _float COS_45 = 0.707f;
    const _float COS_135 = -0.707f;

    // 앞
    if (dot >= COS_45)
        return F;
    // 뒤
    else if (dot <= COS_135)
        return B;
    // 좌우 (dot이 중간이면)
    else
    {
        _vector vRight = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_CombinedWorldMatrix.m[0]));
        vRight = XMVector3Normalize(vRight);

        _float rightDot = XMVectorGetX(XMVector3Dot(vDir, vRight));

        if (rightDot >= 0)
            return R;
        else
            return L;
    }

    return DIR_STATE::NONE;
}

HRESULT CBody_Discord::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Discord"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::AABB_DESC	AABBDesc{};
    AABBDesc.vExtents = _float3(6.f, 6.f, 6.f);
    AABBDesc.vCenter = { 0.f,-3.f,0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_pColliderCom[0]), &AABBDesc)))
        return E_FAIL;

    AABBDesc.vExtents = _float3(4.f, 10.f, 30.f);
    AABBDesc.vCenter = { 20.f,5.f,-20.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Arm_Left"), reinterpret_cast<CComponent**>(&m_pColliderCom[1]), &AABBDesc)))
        return E_FAIL;

    
    AABBDesc.vCenter = { 8.f,5.f,-20.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Arm_Left_Front"), reinterpret_cast<CComponent**>(&m_pColliderCom[2]), &AABBDesc)))
        return E_FAIL;

  
    AABBDesc.vCenter = { -12.f,5.f,-20.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Arm_Right_Front"), reinterpret_cast<CComponent**>(&m_pColliderCom[3]), &AABBDesc)))
        return E_FAIL;

   
    AABBDesc.vCenter = { -24.f,5.f, -20.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Arm_Right"), reinterpret_cast<CComponent**>(&m_pColliderCom[4]), &AABBDesc)))
        return E_FAIL;

    //* For.Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Discord_Mask"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    

    return S_OK;
}

HRESULT CBody_Discord::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
   

    return S_OK;
}

CBody_Discord* CBody_Discord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Discord* pInstance = new CBody_Discord(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_Discord");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Discord::Clone(void* pArg)
{
    CBody_Discord* pInstance = new CBody_Discord(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody_Discord");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBody_Discord::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pCombatCom);
    for (auto& collider : m_pColliderCom)
        Safe_Release(collider);

}
