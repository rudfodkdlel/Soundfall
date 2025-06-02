#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Shockwave final : public CProjectile_Base
{

private:
	CProjectile_Shockwave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Shockwave(const CProjectile_Shockwave& Prototype);
	virtual ~CProjectile_Shockwave() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float   m_fScale = {8.f};
	_float   m_fDuration = { 8.f };

private:
	HRESULT Ready_Components();

public:
	static CProjectile_Shockwave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END