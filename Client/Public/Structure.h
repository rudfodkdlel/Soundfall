#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CStructure final : public CGameObject
{
private:
	CStructure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStructure(const CStructure& Prototype);
	virtual ~CStructure() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };

	CModel* m_pModelCom = { nullptr };

	_uint	m_iAnimnum = { 0 };

	_bool	m_isAnim = { false };
private:
	HRESULT Ready_Components();

public:
	static CStructure* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END