#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CTentacle_Melee final : public CGameObject
{
private:
	CTentacle_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTentacle_Melee(const CTentacle_Melee& Prototype);
	virtual ~CTentacle_Melee() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Select_State();


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool	m_IsFinished = { false };

	_int	m_iAnimNum = {0};

	STATE_MAIN m_eState = { STATE_MAIN::SPWAN };

	CGameObject* m_pTarget = { nullptr };
	_bool		 m_IsTargeted = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTentacle_Melee* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END