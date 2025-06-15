#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CCollider;
NS_END

NS_BEGIN(Client)

class CProjectile_Base abstract : public CGameObject
{
public:
	typedef struct tagProjectileDesc : public CGameObject::GAMEOBJECT_DESC
	{

		_float4 vDir;
		_float	fSpeed;

		// 만드는 객체마다 색이 다름
		_float4 vColor;
	}PROJECTILE_DESC;

protected:
	CProjectile_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Base(const CProjectile_Base& Prototype);
	virtual ~CProjectile_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	// 궤적 나중에 생성하도록
	void Make_Trail();
protected:

	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	class CCombatStat* m_pCombatCom = { nullptr };
	_float4	m_vDir = {};
	_float4 m_vColor = {};
	_bool	m_IsColl = { false };
	_float	m_fSpeed = {};

private:
	HRESULT Ready_Components();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END

