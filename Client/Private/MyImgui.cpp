#include "MyImgui.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "Model.h"
#include "Structure.h"


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

	m_pGrid = m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Grid"));

	Safe_AddRef(m_pGrid);

	return S_OK;
}

void CMyImgui::Update(_float fTimeDelta)
{
	
	
	if (m_pGameInstance->Key_Down(DIM::LBUTTON))
	{
		
			
		// 마우스랑 가장 가까운
		_float		vMinDist = FLT_MAX;
		if (!m_bCheckGrid)
		{
			
			for (auto& object : m_pObjects)
			{
				if (nullptr == object || object->Get_Dead())
					continue;
				_float4 vTemp;
				_float4x4 worldInverse;
				XMStoreFloat4x4(&worldInverse, object->Get_Transform()->Get_WorldMatrix_Inverse());
				if (nullptr != object->Get_Component(TEXT("Com_VIBuffer")))
					vTemp = static_cast<CVIBuffer*>(object->Get_Component(TEXT("Com_VIBuffer")))->Compute_PickedPosition(&worldInverse);
				else if (nullptr != object->Get_Component(TEXT("Com_Model")))
				{
					vTemp = static_cast<CModel*>(object->Get_Component(TEXT("Com_Model")))->Compute_PickedPosition(&worldInverse);
				}
				else
					return;
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
		else
		{
			_float4 vTemp;
			_float4x4 worldInverse;
			XMStoreFloat4x4(&worldInverse, m_pGrid->Get_Transform()->Get_WorldMatrix_Inverse());
			if (nullptr != m_pGrid->Get_Component(TEXT("Com_VIBuffer")))
				vTemp = static_cast<CVIBuffer*>(m_pGrid->Get_Component(TEXT("Com_VIBuffer")))->Compute_PickedPosition(&worldInverse);
			else
				return;
			if (vTemp.w != 0)
			{
				if (XMVectorGetX(XMVector4Length(XMLoadFloat4(&vTemp))) < vMinDist)
				{
					vMinDist = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vTemp)));
					m_pPickingObject = m_pGrid;
					m_vPickingPos = vTemp;

					if (m_bUseNaviPos)
					{
						if (m_NaviTriangles.empty())
						{
							// 점 찍고
							m_eTri.points[m_iPickCount] = { roundf(m_vPickingPos.x),  roundf(m_vPickingPos.y),  roundf(m_vPickingPos.z) };


							if (m_iPickCount == 2)
							{
								m_NaviTriangles.push_back(m_eTri);

								CCell* pCell = CCell::Create(m_pDevice, m_pContext, m_eTri.points, m_pCells.size());
								if (nullptr == pCell)
									return;
								m_pCells.push_back(pCell);

								ZeroMemory(&m_eTri, sizeof(Triangle));
								m_iPickCount = 0;
							}
							else
							{
								++m_iPickCount;
							}
						}
						else
						{
							_float3 newPoint = { roundf(m_vPickingPos.x), roundf(m_vPickingPos.y), roundf(m_vPickingPos.z) };

							// 스냅 거리 임계값
							const float snapThreshold = 2.0f;

							// 가까운 점으로 스냅 처리
							for (auto& tri : m_NaviTriangles)
							{
								for (int i = 0; i < 3; ++i)
								{
									_float3& pt = tri.points[i];
									_vector diff = XMLoadFloat3(&newPoint) - XMLoadFloat3(&pt);
									float dist = XMVectorGetX(XMVector3Length(diff));

									if (dist < snapThreshold)
									{
										newPoint = pt;
										break;
									}
								}
								if (newPoint.x != roundf(m_vPickingPos.x) ||
									newPoint.y != roundf(m_vPickingPos.y) ||
									newPoint.z != roundf(m_vPickingPos.z))
									break;
							}

							// 찍은 점 저장
							m_eTri.points[m_iPickCount] = newPoint;

							if (m_iPickCount == 2)
							{
								// 시계 방향 체크 및 교정
								_float3 a = m_eTri.points[0];
								_float3 b = m_eTri.points[1];
								_float3 c = m_eTri.points[2];

								_vector v1 = XMLoadFloat3(&b) - XMLoadFloat3(&a);
								_vector v2 = XMLoadFloat3(&c) - XMLoadFloat3(&a);
								_vector cross = XMVector3Cross(v1, v2);

								if (XMVectorGetY(cross) < 0)
									std::swap(b, c);

								m_eTri.points[0] = a;
								m_eTri.points[1] = b;
								m_eTri.points[2] = c;

								m_NaviTriangles.push_back(m_eTri);

								CCell* pCell = CCell::Create(m_pDevice, m_pContext, m_eTri.points, m_pCells.size());
								if (nullptr == pCell)
									return;
								m_pCells.push_back(pCell);

								ZeroMemory(&m_eTri, sizeof(Triangle));
								m_iPickCount = 0;
							}
							else
							{
								++m_iPickCount;
							}
						}


					}
				}


				
			}
		}
	
	}
	
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

	for (auto& cell : m_pCells)
		cell->Render();
	

	return S_OK;
}

void CMyImgui::Render_Create_Window()
{
	
	bool g_bBigWindow = false;

	// Grid Check 창 크기 줄임
	ImVec2 windowSize1 = ImVec2(200, 150);
	ImVec2 windowSize2 = ImVec2(300, 400); // Create Window 크기 유지

	ImGui::SetNextWindowSize(windowSize1, ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(250, 100), ImGuiCond_Once);

	if (ImGui::Begin("Util")) {
		ImGui::Checkbox("Grid", &m_bCheckGrid);
		ImGui::Checkbox("Navi_Point", &m_bUseNaviPos);
		if (ImGui::Button("Delete Object"))
		{
			// 지우는 로직 추가
			if(nullptr != m_pPickingObject)
				m_pPickingObject->Set_Dead();
			
		}
	}
	ImGui::End();

	ImGui::SetNextWindowSize(windowSize2, ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(50 , windowSize1.y + 100), ImGuiCond_Once);

	ImGui::Begin("Create Window");

	if (m_pPrototypes) {
		for (const auto& pair : *m_pPrototypes) {
			std::string keyStr = WStringToString(pair.first);

			if (keyStr.find("GameObject") != keyStr.npos) {
				bool isSelected = (pair.first == m_strSelectKey);
				if (ImGui::Selectable(keyStr.c_str(), isSelected)) {
					m_strSelectKey = pair.first;
				}
			}
		}
	}
	

	if (ImGui::Button("Create")) {
		CStructure::STRUCTURE_DESC pDesc = {};
		pDesc.vPos = { 0.f, 0.f, 0.f,1.f };
		pDesc.strModeltag = m_strSelectModelKey;
		pDesc.strPrototag = m_strSelectKey;
		pDesc.iProtoIndex = 0;

		if (!m_strSelectKey.empty())
			m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), m_strSelectKey,
				ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit"), &pDesc);

		m_pObjects.push_back(m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit")));


		if (m_vPickingPos.w == 1.f)
			m_pObjects.back()->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPickingPos));
	}

	ImGui::End();

	if (m_strSelectKey.find(L"Structure") != m_strSelectKey.npos)
	{
		ImGui::SetNextWindowSize(windowSize2, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(900, windowSize1.y + 100), ImGuiCond_Once);


		ImGui::Begin("Structure Models");

		if (m_pPrototypes) {
			for (const auto& pair : *m_pPrototypes) {
				std::string keyStr = WStringToString(pair.first);

				if (keyStr.find("Model") != std::string::npos) {
					bool isSelected = (pair.first == m_strSelectModelKey);
					
					if (ImGui::Selectable(keyStr.c_str(), isSelected)) {
						m_strSelectModelKey = pair.first;
					}
					
				}
			}
		}

		ImGui::End();
	}
	

	ImGui::SetNextWindowSize(windowSize1, ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(100 + windowSize2.x + 600,  100), ImGuiCond_Once);

	ImGui::Begin("Save Load");
	if (ImGui::Button("Save"))
	{
		Save_Data("../Bin/Data/test.bin");
	}

	if (ImGui::Button("Load"))
	{
		Load_Data("../Bin/Data/test.bin");
	}

	if (ImGui::Button("Save_Navi"))
	{
		Save_Navi("../Bin/Data/Navi_Test.bin");
	}

	if (ImGui::Button("Load_Navi"))
	{
		Load_Navi("../Bin/Data/Navi_Test.bin");
	}

	ImGui::End();
}

void CMyImgui::Render_Gizmo()
{
	if (m_pPickingObject == nullptr || m_pPickingObject->Get_Dead())
		return;

	_float4x4* pWorld = m_pPickingObject->Get_Transform()->Get_WorldMatrix();
	float worldMatrix[16];
	memcpy(worldMatrix, pWorld, sizeof(float) * 16);

	// GUI용 트랜스폼 컴포넌트
	static float position[3];
	static float rotation[3];
	static float scale[3];



	// 행렬을 position/rotation/scale로 분해
	ImGuizmo::DecomposeMatrixToComponents(worldMatrix, position, rotation, scale);

	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Always);

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

		//

		ImGui::Separator();

		// 값 입력 UI
		ImGui::DragFloat3("Position", position, 0.1f);
		ImGui::DragFloat3("Rotation", rotation, 0.1f);
		ImGui::DragFloat3("Scale", scale, 0.01f);

		// 값이 바뀌었을 경우 재조합
		ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, worldMatrix);
		memcpy(pWorld, worldMatrix, sizeof(float) * 16);
	}
	ImGui::End();

	

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	//ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);

	
	
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

void CMyImgui::Save_Data(const char* pFliePath)
{


	// 파일 만들기 
	ofstream fout(pFliePath, std::ios::binary);
	if (!fout)
	{
		return;
	}

	for (auto& object : m_pObjects)
	{
		if (nullptr != object && !(object->Get_Dead()))
		{
			OBJECT_SAVE_DESC eDesc = object->Get_Save_Desc();

			fout.write(reinterpret_cast<const char*>(&eDesc.matWorld), sizeof(_float4x4));

			// 문자열 저장 (길이 먼저, 그 다음 데이터)
			size_t len = eDesc.strPrototypetag.length();
			fout.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
			fout.write(reinterpret_cast<const char*>(eDesc.strPrototypetag.c_str()), sizeof(wchar_t) * len);

			size_t Moedeltaglen = eDesc.strModeltag.length();
			fout.write(reinterpret_cast<const char*>(&Moedeltaglen), sizeof(size_t));
			if (Moedeltaglen > 0)
			{
				fout.write(reinterpret_cast<const char*>(eDesc.strModeltag.c_str()), sizeof(wchar_t) * Moedeltaglen);
			}

		
			fout.write(reinterpret_cast<const char*>(&eDesc.PrototypeLevelIndex), sizeof(int));
		}
	
	}

	fout.close();



	
}

void CMyImgui::Load_Data(const char* pFliePath)
{
	std::ifstream fin(pFliePath, std::ios::binary);
	if (!fin)
		return;

	while (fin.peek() != EOF)
	{
		OBJECT_SAVE_DESC desc = {};

		// 월드 행렬 읽기
		fin.read(reinterpret_cast<char*>(&desc.matWorld), sizeof(_float4x4));
		if (fin.eof()) break;

		// 프로토타입 태그 문자열 읽기
		size_t protoLen = 0;
		fin.read(reinterpret_cast<char*>(&protoLen), sizeof(size_t));
		if (fin.eof()) break;

		if (protoLen > 0)
		{
			std::wstring temp(protoLen, L'\0');
			fin.read(reinterpret_cast<char*>(&temp[0]), sizeof(wchar_t) * protoLen);
			desc.strPrototypetag = temp;
		}

		// 모델 태그 문자열 읽기
		size_t modelLen = 0;
		fin.read(reinterpret_cast<char*>(&modelLen), sizeof(size_t));
		if (fin.eof()) break;

		if (modelLen > 0)
		{
			std::wstring temp(modelLen, L'\0');
			fin.read(reinterpret_cast<char*>(&temp[0]), sizeof(wchar_t) * modelLen);
			desc.strModeltag = temp;
		}

		// 프로토타입 레벨 인덱스 읽기
		fin.read(reinterpret_cast<char*>(&desc.PrototypeLevelIndex), sizeof(int));
		if (fin.eof()) break;

		m_ObjectDescs.push_back(desc);
	}

	fin.close();

	for (auto& desc : m_ObjectDescs)
	{
		CStructure::STRUCTURE_DESC pDesc = {};
		pDesc.iProtoIndex = desc.PrototypeLevelIndex;
		pDesc.strModeltag = desc.strModeltag;
		pDesc.strPrototag = desc.strPrototypetag;

		HRESULT hr = m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), pDesc.strPrototag, ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit"), &pDesc);

		if (hr < 0)
			return;

		m_pObjects.push_back(m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Edit")));
	


		memcpy(m_pObjects.back()->Get_Transform()->Get_WorldMatrix(), &desc.matWorld, sizeof(_float4x4));
	
	}

	m_ObjectDescs.clear();
}

void CMyImgui::Save_Navi(const char* pFliePath)
{
	ofstream fout(pFliePath, std::ios::binary);
	if (!fout)
	{
		return;
	}

	size_t count = m_NaviTriangles.size();

	fout.write(reinterpret_cast<const char*>(&count), sizeof(count));
	if (count > 0)
		fout.write(reinterpret_cast<const char*>(m_NaviTriangles.data()), sizeof(Triangle) * count);


	fout.close();
}

void CMyImgui::Load_Navi(const char* pFliePath)
{
	std::ifstream fin(pFliePath, std::ios::binary);
	if (!fin)
		return;
	m_NaviTriangles.clear();

	size_t count = 0;
	fin.read(reinterpret_cast<char*>(&count), sizeof(size_t));



	for (size_t i = 0; i < count; ++i)
	{
		Triangle tri;
		fin.read(reinterpret_cast<char*>(&tri), sizeof(Triangle));

		m_NaviTriangles.push_back(tri);
	}
	fin.close();
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

	Safe_Release(m_pGrid);

	for (auto& cell : m_pCells)
		Safe_Release(cell);
}
