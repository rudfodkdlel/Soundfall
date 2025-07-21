#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CTrail_Effect_Axe final : public CGameObject
{
public:
	typedef struct tagTrailAxeDesc : public GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		_bool		isMove;
		_bool		isScaleUp;
		_float		fLifeTime;
		_float4		vColor;
		_float4     vDir;
	}TRAIL_AXE_DESC;
private:
	CTrail_Effect_Axe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_Axe(const CTrail_Effect_Axe& Prototype);
	virtual ~CTrail_Effect_Axe() = default;

public:
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:

	HRESULT Ready_Components();

	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


private:


	_float m_fElapsedTime = { 0.f };

	// 본체가 죽으면 check를 true 바꾸고, lifetime을 줄이면서 다 줄면 삭제?
	_float m_fLifeTime = { 0.f };
	_bool  m_isCheckDead = {};

	_float4	m_vColor = {};
	_float4 m_vDir = {};
	_int	m_iTextureType = {};

public:
	static CTrail_Effect_Axe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END