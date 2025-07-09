#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CNavi_Icon final : public CUIObject
{
public:
	typedef struct tagNaviUIDesc : public CUIObject::UIOBJECT_DESC
	{
		wstring strCaption;
	}NAVI_UI_DESC;

private:
	CNavi_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavi_Icon(const CNavi_Icon& Prototype);
	virtual ~CNavi_Icon() = default;

public:
	void Set_isClick(_bool isClick) { m_isClick = isClick; }
	_bool Get_isClick() { return m_isClick; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_bool Check_Click();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	// 어떤 글자를 적을건지
	_wstring m_strCaption = {};

	_bool	m_isClick = {};

	// 월드에 그릴 사이즈
	_float m_fScale = {};

private:
	HRESULT Ready_Components();

public:
	static CNavi_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END