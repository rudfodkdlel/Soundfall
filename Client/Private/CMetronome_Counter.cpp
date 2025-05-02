#include "CMetronome_Counter.h"


CMetronome_Counter::CMetronome_Counter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CMetronome_Counter::CMetronome_Counter(const CMetronome_Counter& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CMetronome_Counter::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CMetronome_Counter::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

void CMetronome_Counter::Priority_Update(_float fTimeDelta)
{
}

void CMetronome_Counter::Update(_float fTimeDelta)
{
}

void CMetronome_Counter::Late_Update(_float fTimeDelta)
{
}

HRESULT CMetronome_Counter::Render()
{
    return E_NOTIMPL;
}

HRESULT CMetronome_Counter::Ready_Components()
{
    return E_NOTIMPL;
}

CMetronome_Counter* CMetronome_Counter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CMetronome_Counter::Clone(void* pArg)
{
    return nullptr;
}

void CMetronome_Counter::Free()
{
}
