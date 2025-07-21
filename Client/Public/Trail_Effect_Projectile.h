

#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CTrail_Effect_Projectile final : public CGameObject
{
public:
	typedef struct tagTrailBulletDesc : public GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		_bool		isMove;
		_bool		isScaleUp;
		_float		fLifeTime;
		_float4		vColor;
		_float4     vDir;
	}TRAIL_BULLET_DESC;
private:
	CTrail_Effect_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_Projectile(const CTrail_Effect_Projectile& Prototype);
	virtual ~CTrail_Effect_Projectile() = default;

public:
	void Set_CheckDead() { m_isCheckDead = true; }
	void Set_isMove(_bool isMove) { m_isMove = isMove; }
	void Set_isScaleUp(_bool isScaleUp) { m_isScaleUp = isScaleUp; }

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

	
	_float m_fElapsedTime = {0.f};

	// uv만 움직일거임? 월드 위치 조절하면서 이동할거임?
	_bool m_isMove = {false};
	// scale 조절하면서 크기 늘릴거?
	_bool m_isScaleUp = { false };
	// 본체가 죽으면 check를 true 바꾸고, lifetime을 줄이면서 다 줄면 삭제?
	_float m_fLifeTime = { 0.f };
	_bool  m_isCheckDead = {};



	_float4	m_vColor = {};
	_float4 m_vDir = {};

public:
	static CTrail_Effect_Projectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END