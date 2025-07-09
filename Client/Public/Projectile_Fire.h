#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Fire final : public CProjectile_Base
{

private:
	CProjectile_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Fire(const CProjectile_Fire& Prototype);
	virtual ~CProjectile_Fire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT On_Collision(class CCollider* pCollider);



private:
	HRESULT Ready_Components();

private:

	// 유지 시간
	_float  m_fElaspedTime = {};

public:
	static CProjectile_Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END