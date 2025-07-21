#include "Level_Arena.h"
#include "GameInstance.h"
#include "Static_UI.h"
#include "HP_Bar.h"
#include "Metronome.h"
#include "CSpawner.h"
#include "Terrain.h"
#include "Camera.h"
#include "Level_Loading.h"
#include "Structure.h"

CLevel_Arena::CLevel_Arena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel{pDevice, pContext}
{
}

HRESULT CLevel_Arena::Initialize()
{
	if (FAILED(Ready_Map_Data("../Bin/Data/Map_Arena.bin")))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	m_pSpawner = CSpawner::Create();

	m_pGameInstance->StopSound(SOUND_BGM);
	m_pGameInstance->PlaySound(TEXT("Soundfall_TM_Ice_Snowbound.ogg"), SOUND_BGM, 0.4f);
	m_pGameInstance->SetBPM(TEXT("Soundfall_TM_Ice_Snowbound.ogg"));

	return S_OK;
}

void CLevel_Arena::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime > 3.f)
	{
		m_fLimitTime -= m_fElapsedTime;
		m_fElapsedTime = 0.f;

		// 소환 로직 추가
		m_pSpawner->Spawn_Loop_Forest();
		
	}
}

HRESULT CLevel_Arena::Render()
{
	// 시간 표시하고
	// 다음 으로 넘기고

		wstring str =  std::to_wstring((m_fLimitTime - m_fElapsedTime));

		m_pGameInstance->Draw_Font(TEXT("Default"), str.c_str(), _float2(g_iWinSizeX * 0.5f - 100.f, 0.f), XMVectorSet(2.f, 2.f, 2.f, 1.f), 0.f , {0.f,0.f}, 1.5f);

	if (m_fLimitTime < 0.f)
	{
		((m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::SHOP))));
	}

	return S_OK;
}

HRESULT CLevel_Arena::Ready_Layer_BackGround(const _wstring strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::ARENA), strLayerTag)))
		return E_FAIL;

	CTerrain::TERRAIN_DESC pDesc = {};

	pDesc.bWired = false;
	pDesc.iCurrentLevel = ENUM_CLASS(LEVEL::ARENA);
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::ARENA), strLayerTag, &pDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::ARENA), TEXT("Prototype_GameObject_Snow"),
		ENUM_CLASS(LEVEL::ARENA), strLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Arena::Ready_Layer_Camera(const _wstring strLayerTag)
{
	CCamera::CAMERA_DESC eDesc = {};

	eDesc.vEye = _float3(50.f, 40.f, 20.f);
	eDesc.vAt = _float3(50.f, 0.f, 60.f);
	eDesc.fFov = XMConvertToRadians(60.0f);
	eDesc.fNear = 0.1f;
	eDesc.fFar = 500.f;
	eDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	eDesc.fSpeedPerSec = 10.0f;
	eDesc.iProtoIndex = ENUM_CLASS(LEVEL::STATIC);
	eDesc.strPrototag = TEXT("Prototype_GameObject_Camera_TopDown");
	
	if (FAILED(m_pGameInstance->Add_GameObject(eDesc.iProtoIndex, eDesc.strPrototag,
		ENUM_CLASS(LEVEL::ARENA), strLayerTag, &eDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Arena::Ready_Layer_Player()
{
	CGameObject::GAMEOBJECT_DESC eDesc = {};
	eDesc.vPos = { 50.f,0.f,50.f, 1.f };
	eDesc.iProtoIndex = ENUM_CLASS(LEVEL::ARENA);

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Player"), &eDesc)))
		return E_FAIL;

	CStatic_UI::STATIC_UI_DESC staticDesc{};

	staticDesc.fX = g_iWinSizeX * 0.5f;
	staticDesc.fY = g_iWinSizeY * 0.5f + 200.f;
	staticDesc.fSizeX = 450.f;
	staticDesc.fSizeY = 1.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Metronome_Bar");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fSizeX = 75.f;
	staticDesc.fSizeY = 75.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Metronome_Center_Ring");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 100;
	staticDesc.fY = g_iWinSizeY * 0.8f;
	staticDesc.fSizeX = 150.f;
	staticDesc.fSizeY = 150.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_Ky");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	staticDesc.fX = 250;
	staticDesc.fY = g_iWinSizeY * 0.75f;
	staticDesc.fSizeX = 200.f;
	staticDesc.fSizeY = 25.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_HP_Frame");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	CHP_BAR::HPBARDESC hpDesc = {};

	hpDesc.fX = 250;
	hpDesc.fY = g_iWinSizeY * 0.75f;
	hpDesc.fSizeX = 200.f;
	hpDesc.fSizeY = 25.f;
	hpDesc.IsPlayer = true;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_HP_Bar"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &hpDesc)))
		return E_FAIL;

	staticDesc.fX = 265;
	staticDesc.fY = g_iWinSizeY * 0.8f;
	staticDesc.fSizeX = 175.f;
	staticDesc.fSizeY = 10.f;
	staticDesc.strTextureTag = TEXT("Prototype_Component_Texture_Player_Ult_Bar");

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &staticDesc)))
		return E_FAIL;

	CMetronome::METRONOME_DESC				metDesc{};

	metDesc.fX = g_iWinSizeX * 0.5f;
	metDesc.fY = g_iWinSizeY * 0.5f + 200.f;
	metDesc.fSizeX = 75.f;
	metDesc.fSizeY = 75.f;
	metDesc.IsWorldUI = false;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Ring"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Player_UI"), &metDesc)))
		return E_FAIL;

	metDesc.IsWorldUI = true;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome_Center_Ring"),
		static_cast<_uint>(LEVEL::ARENA), TEXT("Layer_Metronome"), &metDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Arena::Ready_Lights()
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
	Desc.vEye = _float4(50.f, 40.f, 20.f, 1.f);
	Desc.vAt = _float4(50, 0.f, 60.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Ready_Light_For_Shadow(Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Arena::Ready_Map_Data(const char* pFliePath)
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

		HRESULT hr = m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), desc->strPrototypetag, ENUM_CLASS(LEVEL::ARENA), TEXT("Layer_Mapdata"), &structureDesc);

		if (hr < 0)
			return E_FAIL;

		auto pObject = (m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::ARENA), TEXT("Layer_Mapdata")));



		memcpy(pObject->Get_Transform()->Get_WorldMatrix(), &desc->matWorld, sizeof(_float4x4));

		Safe_Delete(desc);
	}

	fin.close();


	return S_OK;
}

CLevel_Arena* CLevel_Arena::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Arena* pInstance = new CLevel_Arena(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Forest");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Arena::Free()
{
	__super::Free();

	Safe_Release(m_pSpawner);

}