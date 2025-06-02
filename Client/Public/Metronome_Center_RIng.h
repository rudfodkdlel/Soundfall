#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CMetronome_Center_RIng final : public CUIObject
{
public:

private:
	CMetronome_Center_RIng(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMetronome_Center_RIng(const CMetronome_Center_RIng& Prototype);
	virtual ~CMetronome_Center_RIng() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	// 좀 나중에 고치기 일단 기본 형태만 잡고
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:

	_float		m_fTime = { 0.f };

	_bool		 m_IsTargeted = { false };
	CGameObject* m_pTarget = { nullptr };

	// 월드에 그릴 사이즈
	_float m_fScale = { 3.f };


private:
	HRESULT Ready_Components();

public:
	static CMetronome_Center_RIng* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END