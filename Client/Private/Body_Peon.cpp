#include "Body_Peon.h"
#include "Model.h"
#include "GameInstance.h"
#include "CombatStat.h"

CBody_Peon::CBody_Peon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Peon::CBody_Peon(const CBody_Peon& Prototype)
    : CPartObject{ Prototype }
{
}

const _float4x4* CBody_Peon::Get_SocketMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Peon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Peon::Initialize(void* pArg)
{
    NORMAL_BODY_DESC* pDesc = static_cast<NORMAL_BODY_DESC*>(pArg);
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCombatCom = pDesc->pCombatCom;
    Safe_AddRef(m_pCombatCom);

    m_Components.emplace(TEXT("Com_Combat"), m_pCombatCom);

    m_pModelCom->Set_Animation(11, false);

    m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom, this);
    m_pGameInstance->Add_Collider(CG_WEAPON_MONSTER, m_pAttackColliderCom[0], this);
    m_pGameInstance->Add_Collider(CG_WEAPON_MONSTER, m_pAttackColliderCom[1], this);
    return S_OK;
}

void CBody_Peon::Priority_Update(_float fTimeDelta)
{
   
}

void CBody_Peon::Update(_float fTimeDelta)
{
    m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

    _matrix		RtBoneMatrix = XMLoadFloat4x4(m_pRtHandJntMatrix);
    _matrix     LfBoneMatrix = XMLoadFloat4x4(m_pLfHandJntMatrix);

    for (size_t i = 0; i < 3; i++)
    {
        RtBoneMatrix.r[i] = XMVector3Normalize(RtBoneMatrix.r[i]);
        LfBoneMatrix.r[i] = XMVector3Normalize(LfBoneMatrix.r[i]);
    }
       


    XMStoreFloat4x4(&m_RtHandCombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * RtBoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
    );
    XMStoreFloat4x4(&m_LfHandCombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * LfBoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
    );

    //XMStoreFloat4x4(&m_RtHandCombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));
    //XMStoreFloat4x4(&m_LfHandCombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
    m_pAttackColliderCom[0]->Update(XMLoadFloat4x4(&m_RtHandCombinedWorldMatrix));
    m_pAttackColliderCom[1]->Update(XMLoadFloat4x4(&m_LfHandCombinedWorldMatrix));
}

void CBody_Peon::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CBody_Peon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint		iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
            return E_FAIL;

        m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG


    m_pColliderCom->Render();
    m_pAttackColliderCom[0]->Render();
    m_pAttackColliderCom[1]->Render();

#endif


    return S_OK;
}

HRESULT CBody_Peon::On_Collision(CCollider* pCollider)
{
    
    if (CG_WEAPON_MONSTER == pCollider->Get_Group())
        return S_OK;

    
    m_pushVectors.push_back(m_pColliderCom->Calc_PushVector(pCollider));
    return S_OK;
}

HRESULT CBody_Peon::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_OBB::OBB_DESC	OBBDesc{};
    OBBDesc.vExtents = _float3(1.5f, 2.f, 1.f);
    OBBDesc.vCenter = _float3(0.0f, OBBDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    m_pRtHandJntMatrix = Get_SocketMatrix("rt_hand_jnt");
    m_pLfHandJntMatrix = Get_SocketMatrix("lf_hand_jnt");

    CBounding_Sphere::SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = 0.75f;
    SphereDesc.vCenter = { 0.f,1.f,0.f};

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Attack_Right"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom[0]), &SphereDesc)))
        return E_FAIL;

    SphereDesc.vCenter = { 0.f,-1.f,0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Attack_Left"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom[1]), &SphereDesc)))
        return E_FAIL;
     

    return S_OK;
}

HRESULT CBody_Peon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CBody_Peon* CBody_Peon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Peon* pInstance = new CBody_Peon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_Peon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Peon::Clone(void* pArg)
{
    CBody_Peon* pInstance = new CBody_Peon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody_Peon");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBody_Peon::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pCombatCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pAttackColliderCom[0]);
    Safe_Release(m_pAttackColliderCom[1]);

}
