#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "CombatStat.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CAttack_Area_Monster final : public CPartObject
{
public:
	typedef struct tagHpDesc : public CPartObject::PARTOBJECT_DESC
	{
		CCombatStat* pCombatCom = { nullptr };
	}HPBAR_DESC;
private:
	CAttack_Area_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttack_Area_Monster(const CAttack_Area_Monster& Prototype);
	virtual ~CAttack_Area_Monster() = default;

public:
	void Set_IsActive(_bool isActive) { m_isActive = isActive; m_fRatio = 1.f; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();



private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CCombatStat* m_pCombatCom = { nullptr };

	_float m_fRatio = {1.f};
	_bool  m_isActive = { false };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAttack_Area_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END