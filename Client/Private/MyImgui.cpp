#include "MyImgui.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "imgui.h"
#include "ImGuizmo.h"


CMyImgui::CMyImgui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :m_pDevice{pDevice},
	m_pContext{pContext},
	m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMyImgui::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);

	ImGui_ImplDX11_Init(pDevice, pContext);

	// ImGuizmo 초기화
	ImGuizmo::Enable(true);

	ImGuizmo::Style& style = ImGuizmo::GetStyle();

	// 선 두께 조정
	style.TranslationLineThickness = 4.0f;
	style.RotationLineThickness = 3.0f;
	style.ScaleLineThickness = 3.0f;

	// 색상 조정 (더 눈에 잘 띄게)
	style.Colors[ImGuizmo::DIRECTION_X] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // X축 빨간색
	style.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.2f, 1.0f, 0.2f, 1.0f); // Y축 초록색
	style.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.2f, 0.2f, 1.0f, 1.0f); // Z축 파란색
	style.Colors[ImGuizmo::SELECTION] = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);   //

	m_pPrototypes = m_pGameInstance->Get_Prototypes();

	return S_OK;
}

void CMyImgui::Update(_float fTimeDelta)
{
	
}

HRESULT CMyImgui::Render()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Render_Create_Window();

	// 그린 UI를 렌더링
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	

	return S_OK;
}

void CMyImgui::Render_Create_Window()
{
	

	bool g_bBigWindow = false;
	// 토글에 따라 창 크기 설정
	ImVec2 windowSize = g_bBigWindow ? ImVec2(600, 400) : ImVec2(300, 200);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once); // 처음만 위치 설정

	// Begin에 flags 없이 하면 기본적으로 이동, 크기 조절 다 가능
	ImGui::Begin("Create Window");



	ImGui::Text("Press the button to toggle window size.");
	if (ImGui::Button("Toggle Size"))
	{
		g_bBigWindow = !g_bBigWindow;
	}

	if (m_pPrototypes) {
		for (const auto& pair : *m_pPrototypes) {
			std::string keyStr = WStringToString(pair.first);

			if (keyStr.find("GameObject") != keyStr.npos)
			{
				// 현재 항목이 선택된 항목인지 확인
				bool isSelected = (pair.first == m_strSelectKey);

				if (ImGui::Selectable(keyStr.c_str(), isSelected)) {
					m_strSelectKey = pair.first;
				}
			}

		}
	}

	if (ImGui::Button("Create")) {
		

		CGameObject::GAMEOBJECT_DESC pDesc = {};

		pDesc.fRotationPerSec = 90.f;
		pDesc.fSpeedPerSec = 10.f;

		if (!m_strSelectKey.empty())
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), m_strSelectKey,
				ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit"), &pDesc);

	}

	ImGui::End();



	
}

void CMyImgui::Render_Gizmo()
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	
}

CMyImgui* CMyImgui::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMyImgui* pInstance = new CMyImgui(pDevice, pContext);

	if (FAILED(pInstance->Initialize( pDevice, pContext)))
	{
		MSG_BOX("Failed to Created : CMyImgui");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyImgui::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}
