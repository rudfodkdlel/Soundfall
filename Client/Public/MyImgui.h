#pragma once
#include "Base.h"
#include "Client_Defines.h"


NS_BEGIN(Engine)
class CEditor;
class CGameInstance;
class CGameObject;
class CTransform;
NS_END

NS_BEGIN(Client)

class CMyImgui  final : public CBase
{

private:
	CMyImgui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyImgui() = default;
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	bool show_demo_window = true;
	bool show_another_window = true;


	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CMyImgui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

NS_END