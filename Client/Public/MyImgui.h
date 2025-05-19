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

	void Render_Create_Window();

	void Render_Gizmo();

private:
	// 함수 추가 필요

	void Save_Data();
	void Load_Data();
	



private:

	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	const map<const _wstring, class CBase*>* m_pPrototypes = {};

	_wstring	m_strSelectKey = {};

	CGameObject* m_pPickingObject = { nullptr };

	list<CGameObject*> m_pObjects = {};

	ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;

	_float4				m_vPickingPos = {0.f,0.f,0.f,1.f};

	//나중에 지우기
	_bool				bPrevLeftDown = { false }; 

public:
	static CMyImgui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

NS_END