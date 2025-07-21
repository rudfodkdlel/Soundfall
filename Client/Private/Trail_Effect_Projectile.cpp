#include "Trail_Effect_Projectile.h"
#include "GameInstance.h"

CTrail_Effect_Projectile::CTrail_Effect_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CTrail_Effect_Projectile::CTrail_Effect_Projectile(const CTrail_Effect_Projectile& Prototype)
    :CGameObject{Prototype}
{
}

HRESULT CTrail_Effect_Projectile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrail_Effect_Projectile::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	TRAIL_BULLET_DESC* pTrailDesc = static_cast<TRAIL_BULLET_DESC*>(pArg);

	//
	m_fLifeTime = pTrailDesc->fLifeTime;
	m_isMove = pTrailDesc->isMove;
	m_isScaleUp = pTrailDesc->isScaleUp;
	
	m_vDir = pTrailDesc->vDir;
	m_vColor = pTrailDesc->vColor;

	float yaw = atan2f(XMVectorGetX(XMLoadFloat4(&m_vDir)), XMVectorGetZ(XMLoadFloat4(&m_vDir)));

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), yaw, 0.0f);

	_matrix matRotaion = XMMatrixRotationQuaternion(vQuaternion);

	XMStoreFloat4x4(m_pTransformCom->Get_WorldMatrix(), matRotaion * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));

	_vector vPos = XMLoadFloat4(&pDesc->vPos);

	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pTransformCom->Scaling(2.f, 1.f, 1.f);

	return S_OK;
}

void CTrail_Effect_Projectile::Priority_Update(_float fTimeDelta)
{
	if (m_fLifeTime < 0.f)
		Set_Dead();
}

void CTrail_Effect_Projectile::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	if (m_isCheckDead)
		m_fLifeTime -= fTimeDelta;

	if (m_isScaleUp)
	{

		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vDir = XMLoadFloat4(&m_vDir);
		vPos += vDir * 5 * fTimeDelta;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);

		_vector vOriginPos = m_pTransformCom->Get_State(STATE::POSITION);

		m_pTransformCom->Scaling(2.f, 10.f * (m_fElapsedTime), 1.f);


		/*float deltaZ = 10.f * m_fElapsedTime / 2.f;


		vOriginPos.m128_f32[2] = -deltaZ;
		m_pTransformCom->Set_State(STATE::POSITION, vOriginPos);*/
	}

	//Billboarding();
}

void CTrail_Effect_Projectile::Late_Update(_float fTimeDelta)
{

	// Billboarding();

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_BLUR, this);
}

HRESULT CTrail_Effect_Projectile::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fElapsedTime, sizeof(m_fElapsedTime))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vProjectileColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CTrail_Effect_Projectile::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile_Trail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CTrail_Effect_Projectile* CTrail_Effect_Projectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_Projectile* pInstance = new CTrail_Effect_Projectile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail_Effect_Projectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_Projectile::Clone(void* pArg)
{
	CTrail_Effect_Projectile* pInstance = new CTrail_Effect_Projectile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail_Effect_Projectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_Projectile::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
