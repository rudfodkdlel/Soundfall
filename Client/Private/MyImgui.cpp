#include "MyImgui.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Transform.h"
#include "VIBuffer.h"


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
	bool bCurrentLeftDown = (m_pGameInstance->Get_DIMouseState(DIM::LBUTTON) & 0x80) != 0;
	// 이제 피킹하는거? 
	if (bCurrentLeftDown && !bPrevLeftDown)
	{
		// 마우스랑 가장 가까운
		_float		vMinDist = FLT_MAX;
		for (auto& object : m_pObjects)
		{
			_float4 vTemp;
			_float4x4 worldInverse;
			XMStoreFloat4x4(&worldInverse, object->Get_Transform()->Get_WorldMatrix_Inverse());
			vTemp = static_cast<CVIBuffer*>(object->Get_Component(TEXT("Com_VIBuffer")))->Compute_PickedPosition(&worldInverse);
			if (vTemp.w != 0)
			{
				if (XMVectorGetX(XMVector4Length(XMLoadFloat4(&vTemp))) < vMinDist)
				{
					vMinDist = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vTemp)));
					m_pPickingObject = object;
					m_vPickingPos = vTemp;
				}
			}
		}

	}
	bPrevLeftDown = bCurrentLeftDown;
}

HRESULT CMyImgui::Render()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Render_Create_Window();

	Render_Gizmo();

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

		pDesc.vPos = { 0.f, 0.f,0.f };

		if (!m_strSelectKey.empty())
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), m_strSelectKey,
				ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit"), &pDesc);

		m_pObjects.push_back(m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit")));

		if(m_vPickingPos.w == 1.f)
			m_pObjects.back()->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPickingPos));
	}

	ImGui::End();



	
}

void CMyImgui::Render_Gizmo()
{
	if (ImGui::Begin("Gizmo Control"))
	{
		if (ImGui::RadioButton("Translate", m_currentOperation == ImGuizmo::TRANSLATE))
			m_currentOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_currentOperation == ImGuizmo::ROTATE))
			m_currentOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_currentOperation == ImGuizmo::SCALE))
			m_currentOperation = ImGuizmo::SCALE;
	}
	ImGui::End();

	if (m_pPickingObject == nullptr)
		return;

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	//ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);

	
	_float4x4* pWorld = m_pPickingObject->Get_Transform()->Get_WorldMatrix();
	const _float4x4*  pView = m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW);
	const _float4x4* pProj = m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ);

	float viewMat[16], projMat[16];
	memcpy(viewMat, pView, sizeof(float) * 16);
	memcpy(projMat, pProj, sizeof(float) * 16);


	// 5. Manipulate 호출 (직접 월드 행렬 수정됨)
	ImGuizmo::Manipulate(viewMat, projMat,
		m_currentOperation, ImGuizmo::LOCAL,
		reinterpret_cast<float*>(pWorld));

	
}

void CMyImgui::Save_Data()
{

}

void CMyImgui::Load_Data()
{
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
