#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CPlayerRing final : public CUIObject
{
public:

private:
	CPlayerRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerRing(const CPlayerRing& Prototype);
	virtual ~CPlayerRing() = default;

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


	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	
	_bool		 m_IsTargeted = {false};
	CGameObject* m_pTarget = { nullptr };

	// 월드에 그릴 사이즈
	_float m_fScale = {8.f};


private:
	HRESULT Ready_Components();

public:
	static CPlayerRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END