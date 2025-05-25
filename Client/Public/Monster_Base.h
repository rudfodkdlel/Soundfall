#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


// 수정 필요
// base 역할만 하도록
NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CMonster_Base : public CGameObject
{

private:
	CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Base(const CMonster_Base& Prototype);
	virtual ~CMonster_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	// 공통 역할이 필요할듯?

private:
	CShader* m_pShaderCom = { nullptr };

	CModel* m_pModelCom = { nullptr };

	_uint	m_iAnimnum = { 0 };

private:
	HRESULT Ready_Components();

public:
	static CMonster_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END