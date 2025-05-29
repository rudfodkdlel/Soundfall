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

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
    /* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */
    m_pModelCom->Set_Animation(33, false);

    return S_OK;
}

void CBody_Discord::Priority_Update(_float fTimeDelta)
{
}

void CBody_Discord::Update(_float fTimeDelta)
{

    m_IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

    if (m_IsFinished)
    {
        m_pGameInstance->Notify(TEXT("Observer_Animation_Discord"), TEXT("AnimEnd"));
    }
}

void CBody_Discord::Late_Update(_float fTimeDelta)
{
    
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);

}

HRESULT CBody_Discord::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint		iNumMesh = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, 1, 0)))
            return E_FAIL;

        m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
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

    return S_OK;
}

HRESULT CBody_Discord::Bind_ShaderResources()
{
    //
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
