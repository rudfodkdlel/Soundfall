#include "MyImgui.h"
#include "GameInstance.h"
#include "imgui.h"


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

	// 여기에 UI를 그릴 수 있습니다.
	ImGui::Begin("Hello, World!");   // 창 이름 설정
	ImGui::Text("This is a simple ImGui window.");


	ImGui::End();

	ImGui::Begin("Editor");

	// 탭 시스템 추가
	if (ImGui::BeginTabBar("EditorTabs"))
	{
		// 통합된 인스펙터 탭 (오브젝트 속성 + 기즈모 컨트롤)
		if (ImGui::BeginTabItem("Inspector"))
		{
			
			ImGui::EndTabItem();
		}

		// 오브젝트 생성 탭
		if (ImGui::BeginTabItem("Create Object"))
		{
			
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	// 그린 UI를 렌더링
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
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
