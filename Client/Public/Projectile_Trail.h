

#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"
#include "Trail_Effect_Projectile.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Trail final : public CProjectile_Base
{

private:
	CProjectile_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Trail(const CProjectile_Trail& Prototype);
	virtual ~CProjectile_Trail() = default;

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
	_int	m_iType = {};

	CTrail_Effect_Projectile* m_pTrail = {};

	_bool m_isMake = {};

public:
	static CProjectile_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END