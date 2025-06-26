#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Shell final : public CProjectile_Base
{

private:
	CProjectile_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Shell(const CProjectile_Shell& Prototype);
	virtual ~CProjectile_Shell() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	HRESULT Ready_Components();

private:
	_float4	m_vStartPos = {};
	_float3 m_vGravity = { 0.f,-9.8f, 0.f };
	_float	m_fElapsedTime = {};

public:
	static CProjectile_Shell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END