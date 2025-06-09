#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "CombatStat.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CHP_BAR : public CUIObject
{
public:
	typedef struct tagHpBarDesc : public CUIObject::UIOBJECT_DESC
	{
		_bool IsPlayer;
	}HPBARDESC;

private:
	CHP_BAR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHP_BAR(const CHP_BAR& Prototype);
	virtual ~CHP_BAR() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureIdleCom = { nullptr };
	CTexture* m_pTextureDamagedCom = { nullptr };

	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:

	CGameObject* m_pTarget = { nullptr };
	_bool		 m_IsTargeted = { false };

	CCombatStat* m_pCombatCom = { nullptr };

	_bool		 m_IsPlayer = { true };


private:
	HRESULT Ready_Components();

public:
	static CHP_BAR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END