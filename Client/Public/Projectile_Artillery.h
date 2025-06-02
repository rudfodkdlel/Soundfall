#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Artillery final : public CProjectile_Base
{

private:
	CProjectile_Artillery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Artillery(const CProjectile_Artillery& Prototype);
	virtual ~CProjectile_Artillery() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	_float	m_fDuration = {1.5f};
	_int	m_iTextureType = { 0 };

private:
	HRESULT Ready_Components();


public:
	static CProjectile_Artillery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END