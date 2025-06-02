#include "Projectile_Shockwave.h"
#include "GameInstance.h"

CProjectile_Shockwave::CProjectile_Shockwave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile_Base(pDevice, pContext)
{
}

CProjectile_Shockwave::CProjectile_Shockwave(const CProjectile_Shockwave& Prototype)
	:CProjectile_Base(Prototype)
{
}

HRESULT CProjectile_Shockwave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Shockwave::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(8.f, 8.f, 8.f);
	_vector vDir = { 1.f, 0.f,0.f,0.f };
	m_pTransformCom->Rotation(vDir, XMConvertToRadians(90.f));


	_vector vPos = { 0.f,0.2f + m_pGameInstance->Compute_Random_Normal() * 0.005f,0.f,0.f };
	vPos += m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	return S_OK;
}

void CProjectile_Shockwave::Priority_Update(_float fTimeDelta)
{
	// 삭제할 기준 만들어야됨?
	if (m_fDuration < 0.f)
		Set_Dead();

	
}

void CProjectile_Shockwave::Update(_float fTimeDelta)
{
	m_fScale += fTimeDelta * 15.f;
	m_pTransformCom->Scaling(m_fScale, m_fScale, m_fScale);

	m_fDuration -= fTimeDelta;
	
}

void CProjectile_Shockwave::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CProjectile_Shockwave::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CProjectile_Shockwave::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shockwave"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CProjectile_Shockwave* CProjectile_Shockwave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Shockwave* pInstance = new CProjectile_Shockwave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjecttile_Shockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Shockwave::Clone(void* pArg)
{
	CProjectile_Shockwave* pInstance = new CProjectile_Shockwave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjecttile_Shockwave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Shockwave::Free()
{
	__super::Free();
}
