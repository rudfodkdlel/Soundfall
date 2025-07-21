#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Metronome.h"
#include "Static_UI.h"
#include "HP_Bar.h"
#include "Terrain.h"
#include "Trigger.h"
#include "Structure.h"
#include "Structure_Instance.h"
#include "Camera.h"
#include "Level_Loading.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
		: CLevel { pDevice, pContext }
{

}

HRESULT CLevel_GamePlay::Initialize()
{

	if (FAILED(Ready_Map_Data("../Bin/Data/Map_Boss.bin")))
		return E_FAIL;

	// bpm 123
	m_pGameInstance->StopSound(SOUND_BGM);
	m_pGameInstance->PlaySound(TEXT("Discordance.ogg"), SOUND_BGM, 0.3f);
	m_pGameInstance->SetBPM(TEXT("Discordance.ogg"));

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	


	// Boss

	


	
	

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Up(DIK_T))
	{
		CTrigger::TRIGGER_DESC triggerDesc = {};

		triggerDesc.eType = TRIGGERTYPE::ZOOM_IN;
		triggerDesc.vPos = { 20.f,0.f,0.f,1.f };

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Trigger"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Trigger"), &triggerDesc)))
			return;

		triggerDesc.eType = TRIGGERTYPE::ZOOM_OUT;
		triggerDesc.vPos = { 0.f,0.f,20.f,1.f };

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Trigger"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Trigger"), &triggerDesc)))
			return;
	}

	if (m_pGameInstance->Key_Up(DIK_B))
	{
		CStatic_UI::STATIC_UI_DESC staticDesc = {};

		staticDesc.fX = g_iWinSizeX * 0.5f;
		staticDesc.fY = g_iWinSizeY * 0.1f;
		staticDesc.fSizeX = 600.f;
		staticDesc.fSizeY = 25.f;
		staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_HP_Frame");

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Boss_UI"), &staticDesc)))
			return;

		CHP_BAR::HPBARDESC hpDesc = {};

		hpDesc.fX = g_iWinSizeX * 0.5f;
		hpDesc.fY = g_iWinSizeY * 0.1f;
		hpDesc.fSizeX = 600.f;
		hpDesc.fSizeY = 25.f;
		hpDesc.IsPlayer = false;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_HP_Bar"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Boss_UI"), &hpDesc)))
			return;

		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Boss"))))
			return;
	}


	
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	CTerrain::TERRAIN_DESC pDesc = {};
	pDesc.iCurrentLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
	pDesc.bWired = false;
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pDesc)))
		return E_FAIL;

	CStructure_Instance::STRUCTURE_INSTANCE_DESC objDesc = {};
	objDesc.fRotationPerSec = 90;
	objDesc.vPos = { 0.f,0.f,0.f,1.f };
	objDesc.strBuffertag = TEXT("Prototype_Component_VIBuffer_Struct_CorruptionRock");

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Structure_Instance"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &objDesc)))
		return E_FAIL;

	objDesc.fRotationPerSec = 90;
	objDesc.vPos = { 0.f,0.f,0.f,1.f };
	objDesc.strBuffertag = TEXT("Prototype_Component_VIBuffer_Struct_CorruptionRock_B");

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Structure_Instance"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &objDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Aurora"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring strLayerTag)
{

	CCamera::CAMERA_DESC eDesc = {};

	eDesc.vEye = _float3(65.f, 40.f, -30.f);
	eDesc.vAt = _float3(65.f, 0.f, 0.f);
	eDesc.fFov = XMConvertToRadians(60.0f);
	eDesc.fNear = 0.1f;
	eDesc.fFar = 500.f;
	eDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	eDesc.fSpeedPerSec = 10.0f;
	eDesc.iProtoIndex = ENUM_CLASS(LEVEL::STATIC);
	eDesc.strPrototag = TEXT("Prototype_GameObject_Camera_TopDown");

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_TopDown"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &eDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	CGameObject::GAMEOBJECT_DESC eDesc = {};
	eDesc.vPos = { 65.f,0.f,15.f, 1.f };
	eDesc.iProtoIndex = ENUM_CLASS(LEVEL::GAMEPLAY);

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Player"), &eDesc)))
		return E_FAIL;

	CStatic_UI::STATIC_UI_DESC staticDesc{};

	staticDesc.fX = g_iWinSizeX * 0.5f;
	staticDesc.fY = g_iWinSizeY * 0.5f + 200.f;
	staticDesc.fSizeX = 450.f;
	staticDesc.fSizeY = 1.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Metronome_Bar");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fSizeX = 75.f;
	staticDesc.fSizeY = 75.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Metronome_Center_Ring");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.8f;
	staticDesc.fSizeX = 150.f;
	staticDesc.fSizeY = 150.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_Ky");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 250;
	staticDesc.fY = g_iWinSizeY * 0.75f;
	staticDesc.fSizeX = 200.f;
	staticDesc.fSizeY = 25.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_HP_Frame");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	CHP_BAR::HPBARDESC hpDesc = {};

	hpDesc.fX = 250;
	hpDesc.fY = g_iWinSizeY * 0.75f;
	hpDesc.fSizeX = 200.f;
	hpDesc.fSizeY = 25.f;
	hpDesc.IsPlayer = true;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_HP_Bar"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &hpDesc)))
		return E_FAIL;

	staticDesc.fX = 265;
	staticDesc.fY = g_iWinSizeY * 0.8f;
	staticDesc.fSizeX = 175.f;
	staticDesc.fSizeY = 10.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_Ult_Bar");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	CMetronome::METRONOME_DESC				metDesc{};

	metDesc.fX = g_iWinSizeX * 0.5f;
	metDesc.fY = g_iWinSizeY * 0.5f + 200.f;
	metDesc.fSizeX = 75.f;
	metDesc.fSizeY = 75.f;
	metDesc.IsWorldUI = false;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Ring"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Player_UI"), &metDesc)))
		return E_FAIL;

	metDesc.IsWorldUI = true;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome_Center_Ring"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 0.4f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	CShadow::SHADOW_DESC		Desc{};
	Desc.vEye = _float4(65.f, 40.f, -30.f, 1.f);
	Desc.vAt = _float4(65.f, 0.f, 60.f, 1.f);
	Desc.fFovy = XMConvertToRadians(90.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Light_For_Shadow(Desc)))
		return E_FAIL;

	/*LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(55.f, 5.0f, 10.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(70.f, 5.0f, 10.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = _float4(0.f, 1.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	

	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(65.f, 5.f, 130.f, 1.f);
	//LightDesc.fRange = 100.f;
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	//LightDesc.vSpecular = _float4(1.f, 0.f, 0.f, 1.f);

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map_Data(const char* pFliePath)
{
	std::ifstream fin(pFliePath, std::ios::binary);
	if (!fin)
		return E_FAIL;

	while (fin.peek() != EOF)
	{
		OBJECT_SAVE_DESC* desc = new OBJECT_SAVE_DESC{};

		// 월드 행렬 읽기
		fin.read(reinterpret_cast<char*>(&desc->matWorld), sizeof(_float4x4));
		if (fin.eof()) break;

		// 프로토타입 태그 문자열 읽기
		size_t protoLen = 0;
		fin.read(reinterpret_cast<char*>(&protoLen), sizeof(size_t));
		if (fin.eof()) break;

		if (protoLen > 0)
		{
			std::wstring temp(protoLen, L'\0');
			fin.read(reinterpret_cast<char*>(&temp[0]), sizeof(wchar_t) * protoLen);
			desc->strPrototypetag = temp;
		}

		// 모델 태그 문자열 읽기
		size_t modelLen = 0;
		fin.read(reinterpret_cast<char*>(&modelLen), sizeof(size_t));
		if (fin.eof()) break;

		if (modelLen > 0)
		{
			std::wstring temp(modelLen, L'\0');
			fin.read(reinterpret_cast<char*>(&temp[0]), sizeof(wchar_t) * modelLen);
			desc->strModeltag = temp;
		}

		// 프로토타입 레벨 인덱스 읽기
		fin.read(reinterpret_cast<char*>(&desc->PrototypeLevelIndex), sizeof(int));
		if (fin.eof()) break;

		CStructure::STRUCTURE_DESC structureDesc = {};
		structureDesc.iProtoIndex = desc->PrototypeLevelIndex;
		structureDesc.strPrototag = desc->strPrototypetag;
		structureDesc.strModeltag = desc->strModeltag;

		HRESULT hr = m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), desc->strPrototypetag, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Mapdata"), &structureDesc);

		if (hr < 0)
			return E_FAIL;

		auto pObject =(m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Mapdata")));



		memcpy(pObject->Get_Transform()->Get_WorldMatrix(), &desc->matWorld, sizeof(_float4x4));

		Safe_Delete(desc);
	}

	fin.close();


	return S_OK;

}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_GamePlay::Free()
{
	__super::Free();

}
