#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "CombatStat.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Peon final : public CPartObject
{
private:
	CBody_Peon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Peon(const CBody_Peon& Prototype);
	virtual ~CBody_Peon() = default;

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
	virtual HRESULT Render_Shadow();
	virtual HRESULT On_Collision(class CCollider* pCollider) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCombatStat* m_pCombatCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom[2] = {};


	const _float4x4* m_pRtHandJntMatrix = {};
	const _float4x4* m_pLfHandJntMatrix = {};
	_float4x4				m_RtHandCombinedWorldMatrix{};
	_float4x4				m_LfHandCombinedWorldMatrix{};

	_bool	m_IsFinished = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Peon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END