

#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Ricochet final : public CProjectile_Base
{

private:
	CProjectile_Ricochet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Ricochet(const CProjectile_Ricochet& Prototype);
	virtual ~CProjectile_Ricochet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT On_Collision(class CCollider* pCollider);

	void Change_Dir();

private:
	HRESULT Ready_Components();

private:
	_int	m_iCountChangeDir = {};
	_bool	m_bChange = {false};
	_float  m_fCoolTime = { 1.f };
	

public:
	static CProjectile_Ricochet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END