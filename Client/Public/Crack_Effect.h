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

class CCrack_Effect final : public CGameObject
{
private:
	CCrack_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrack_Effect(const CCrack_Effect& Prototype);
	virtual ~CCrack_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT On_Collision(CCollider* pCollider);
private:

	HRESULT Ready_Components();
	
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	class CCombatStat* m_pCombatCom = { nullptr };



private:
	_float m_fElapsedTime = {};
	_bool	m_IsColl = { false };



public:
	static CCrack_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END