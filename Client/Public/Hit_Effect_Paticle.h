#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(Client)

class CHit_Effect_Paticle final : public CGameObject
{
public:
	
private:
	CHit_Effect_Paticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHit_Effect_Paticle(const CHit_Effect_Paticle& Prototype);
	virtual ~CHit_Effect_Paticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	HRESULT Ready_Components();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	
	_float  m_fTotalTime = { 0.f };

public:
	static CHit_Effect_Paticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END