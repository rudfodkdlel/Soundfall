#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Monster final : public CProjectile_Base
{

private:
	CProjectile_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Monster(const CProjectile_Monster& Prototype);
	virtual ~CProjectile_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT On_Collision(class CCollider* pCollider);

private:
	HRESULT Ready_Components();

public:
	static CProjectile_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END