#include "Monster_Base.h"

#include "GameInstance.h"
#include "Model.h"

CMonster_Base::CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMonster_Base::CMonster_Base(const CMonster_Base& Prototype)
    : CGameObject{ Prototype }
{
}



HRESULT CMonster_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Base::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};

	Desc.fRotationPerSec = 90.f;
	Desc.fSpeedPerSec = 10.f;
	lstrcpy(Desc.szName, TEXT("Monster"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 1. 서로 다른 애니메이션을 셋팅했음에도 같은 동작이 재생된다. : 뼈가 공유되기때문에. */
	/* 2. 같은 애니메이션을 셋했다면 재생속도가 빨라진다. : */

	m_pModelCom->Set_Animation(m_iAnimnum, false);

	return S_OK;
}

void CMonster_Base::Priority_Update(_float fTimeDelta)
{
}

void CMonster_Base::Update(_float fTimeDelta)
{
	bool isFinished = m_pModelCom->Play_Animation(fTimeDelta);

	if (isFinished)
	{
		++m_iAnimnum;
		if (m_iAnimnum >= m_pModelCom->Get_NumAnimations())
		{
			m_iAnimnum = 0;
		}
		m_pModelCom->Set_Animation(m_iAnimnum, false);
	}
}

void CMonster_Base::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CMonster_Base::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
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

HRESULT CMonster_Base::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CMonster_Base* CMonster_Base::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Base* pInstance = new CMonster_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Base::Clone(void* pArg)
{
	CMonster_Base* pInstance = new CMonster_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Base::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
