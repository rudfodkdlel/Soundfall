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

class CBody_Player final : public CPartObject
{
public:
	typedef struct tagBodyPlayerDesc : public CPartObject::PARTOBJECT_DESC
	{
		CCombatStat* pCombatcom;
	}BODY_PLAYER_DESC;
private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);
	eDirState Get_Hit_Dir() { return m_eHitDir; }
	vector<_float4> Get_PushVectors() { return m_pushVectors;}
	void Clear_PushVectors() { m_pushVectors.clear(); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

	virtual HRESULT On_Collision(CCollider* pCollider);

	eDirState Calc_Hit_Dir(_vector vDir);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CCombatStat* m_pCombatCom = { nullptr };

private:
	_bool	m_bFinished = { false };
	eDirState m_eHitDir = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END