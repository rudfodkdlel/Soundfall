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

class CMonster_HP final : public CPartObject
{
public:
	typedef struct tagHpDesc : public CPartObject::PARTOBJECT_DESC
	{
		 CCombatStat* pCombatCom = { nullptr };
	}HPBAR_DESC;
private:
	CMonster_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_HP(const CMonster_HP& Prototype);
	virtual ~CMonster_HP() = default;

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
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMonster_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END