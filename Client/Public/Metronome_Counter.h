#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMetronome_Counter final : public CUIObject
{
public:
	typedef struct tagMetCounterGroundDesc : public CUIObject::UIOBJECT_DESC
	{
		_float3 vDir;
		_float fStartPoint;
	}METRONOME_COUNTER_DESC;

private:
	CMetronome_Counter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMetronome_Counter(const CMetronome_Counter& Prototype);
	virtual ~CMetronome_Counter() = default;

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
	_float		m_fStartPoint = {};
	_float3		m_vDir = {};
	_float		m_fSpeed = {};
private:
	HRESULT Ready_Components();

public:
	static CMetronome_Counter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END