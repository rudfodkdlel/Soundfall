#include "CMetronome.h"

CMetronome::CMetronome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CMetronome::CMetronome(const CMetronome& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CMetronome::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMetronome::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CMetronome::Priority_Update(_float fTimeDelta)
{
}

void CMetronome::Update(_float fTimeDelta)
{
}

void CMetronome::Late_Update(_float fTimeDelta)
{
}

HRESULT CMetronome::Render()
{
	return E_NOTIMPL;
}

HRESULT CMetronome::Ready_Components()
{
	return E_NOTIMPL;
}

CMetronome* CMetronome::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CMetronome::Clone(void* pArg)
{
	return nullptr;
}

void CMetronome::Free()
{
}
