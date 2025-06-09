#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CDiscord_Wall final : public CGameObject
{
private:
	CDiscord_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDiscord_Wall(const CDiscord_Wall& Prototype);
	virtual ~CDiscord_Wall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CCombatStat* m_pCombatCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CDiscord_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END