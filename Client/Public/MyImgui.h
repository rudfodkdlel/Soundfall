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

	void Save_Data(const char* pFliePath);
	void Load_Data(const char* pFliePath);
	



private:

	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	const map<const _wstring, class CBase*>* m_pPrototypes = {};

	_wstring	m_strSelectKey = {};

	// 여기에 추가해서 오브젝트 저장 하고 불러오고 할거

	// 구조 일단 이대로 하고, 삭제 기능 만들면 없애기
	list<CGameObject*> m_pObjects = {};
	list<OBJECT_SAVE_DESC> m_ObjectDescs = {};

	// 이 구조로 나중에 바꾸기
	map<_uint, CGameObject*> m_pObjectMap = {};
	_uint		m_iObjectID = { 0 };
	

	ImGuizmo::OPERATION m_currentOperation = ImGuizmo::TRANSLATE;


	// 오브젝트 설치할 때 사용할 그리드 설정, 버튼으로 껏다 키기
	_bool				m_bCheckGrid = { false };
	CGameObject*		m_pGrid = { nullptr };

	_float4				m_vPickingPos = { 0.f,0.f,0.f,1.f };
	CGameObject* m_pPickingObject = { nullptr };

	//나중에 지우기
	_bool				bPrevLeftDown = { false }; 

public:
	static CMyImgui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

NS_END