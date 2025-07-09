#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CItem_Icon final : public CUIObject
{
public:
	typedef struct tagItemUIDesc : public CUIObject::UIOBJECT_DESC
	{

		_int iTextureType;
	}ITEM_UI_DESC;

private:
	CItem_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Icon(const CItem_Icon& Prototype);
	virtual ~CItem_Icon() = default;

public:
	void Set_isClick(_bool isClick) { m_isClick = isClick; }
	_bool Get_isClick() { return m_isClick; }
	void Set_isRender(_bool isRender) { m_isRender = isRender; }

	pair<int, int> Get_Pos() { return { m_fX, m_fY }; }

	WEAPON_ICON_INFO_DESC Get_Weapon_Info_Desc();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Change_Position(_float fX, _float fY);

public:
	_bool Check_Click();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
	CTexture* m_pHighLightTextureCom = { nullptr };
private:

	_int m_iTextureType = {};
	_int m_iMoney = {0};

	_bool	m_isClick = {};
	_bool   m_isRender = { false };

	// 월드에 그릴 사이즈
	_float m_fScale = {};

private:
	HRESULT Ready_Components();

public:
	static CItem_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END