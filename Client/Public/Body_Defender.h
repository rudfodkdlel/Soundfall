#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Defender final : public CPartObject
{
private:
	CBody_Defender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Defender(const CBody_Defender& Prototype);
	virtual ~CBody_Defender() = default;

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);
	_bool Get_AnimFinished() { return m_IsFinished; }
	vector<_float4> Get_PushVectors() { return m_pushVectors; }
	void Clear_PushVectors() { m_pushVectors.clear(); }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual HRESULT On_Collision( class CCollider* pCollider) override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCombatStat*	m_pCombatCom = { nullptr };
	CCollider*		m_pColliderCom = {nullptr};

	_bool	m_IsFinished = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Defender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END