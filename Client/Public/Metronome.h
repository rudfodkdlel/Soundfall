#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CMetronome final : public CUIObject
{
public:
	typedef struct tagMetronomeDesc : public CUIObject::UIOBJECT_DESC
	{

	}METRONOME_DESC;

private:
	CMetronome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMetronome(const CMetronome& Prototype);
	virtual ~CMetronome() = default;

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
	CTexture* m_pMoving_TextureCom = { nullptr };
	CTexture* m_pMainBar_TextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:


	_uint m_iNumCount = { 0 };
	// 나중에 이제 값 얻어와서 고치기
	_float m_fSpawnInterval = {60.f / 120.f};
	_float m_fSpawnTime = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CMetronome* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END