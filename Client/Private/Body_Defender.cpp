#include "Body_Defender.h"
#include "GameInstance.h"
#include "Model.h"
#include "Object_State_Spawn.h"
#include "Observer_Animation.h"
#include "CombatStat.h"

CBody_Defender::CBody_Defender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Defender::CBody_Defender(const CBody_Defender& Prototype)
    : CPartObject{ Prototype }
{
}

const _float4x4* CBody_Defender::Get_SocketMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Defender::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CBody_Defender::Initialize(void* pArg)
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
    return S_OK;
}

void CBody_Defender::Priority_Update(_float fTimeDelta)
{

}

void CBody_Defender::Update(_float fTimeDelta)
{

    m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

}

void CBody_Defender::Late_Update(_float fTimeDelta)
{

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);

}

HRESULT CBody_Defender::Render()
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

#endif


    return S_OK;
}

HRESULT CBody_Defender::On_Collision(CGameObject* Other, class CCollider* pCollider)
{


    m_pushVectors.push_back(m_pColliderCom->Calc_PushVector(pCollider));
    return S_OK;
}

HRESULT CBody_Defender::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Defender"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_OBB::OBB_DESC	OBBDesc{};
    OBBDesc.vExtents = _float3(1.5f, 2.f, 1.f);
    OBBDesc.vCenter = _float3(0.0f, OBBDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Defender::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CBody_Defender* CBody_Defender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Defender* pInstance = new CBody_Defender(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_Defender");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Defender::Clone(void* pArg)
{
    CBody_Defender* pInstance = new CBody_Defender(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody_Defender");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBody_Defender::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pCombatCom);
    Safe_Release(m_pColliderCom);
}
