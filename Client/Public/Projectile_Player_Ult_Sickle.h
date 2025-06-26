#pragma once

#include "Client_Defines.h"
#include "Projectile_Base.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CProjectile_Player_Ult_Sickle final : public CProjectile_Base
{

private:
	CProjectile_Player_Ult_Sickle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Player_Ult_Sickle(const CProjectile_Player_Ult_Sickle& Prototype);
	virtual ~CProjectile_Player_Ult_Sickle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT On_Collision( class CCollider* pCollider);



private:
	HRESULT Ready_Components();

private:
	_float		m_fDuration = {};

public:
	static CProjectile_Player_Ult_Sickle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END