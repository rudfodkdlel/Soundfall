#include "Level_Arena.h"
#include "GameInstance.h"
#include "Static_UI.h"
#include "HP_Bar.h"
#include "Metronome.h"
#include "CSpawner.h"
#include "Terrain.h"
#include "Camera.h"

CLevel_Arena::CLevel_Arena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel{pDevice, pContext}
{
}

HRESULT CLevel_Arena::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Arena::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime > 5.f)
	{
		m_fLimitTime -= m_fElapsedTime;
		m_fElapsedTime = 0.f;

		// 소환 로직 추가
		
	}
}

HRESULT CLevel_Arena::Render()
{
	// 시간 표시하고
	// 다음 으로 넘기고

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

	auto pObj = m_pGameInstance->GetLastObjectFromLayer(ENUM_CLASS(LEVEL::ARENA), strLayerTag);

	pObj->Get_Transform()->Scaling(100.f, 100.f, 100.f);


	return S_OK;
}

HRESULT CLevel_Arena::Ready_Layer_Camera(const _wstring strLayerTag)
{
	CCamera::CAMERA_DESC eDesc = {};

	eDesc.vEye = _float3(150.f, 40.f, 60.f);
	eDesc.vAt = _float3(150.f, 0.f, 100.f);
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
	eDesc.vPos = { 150.f,0.f,100.f, 1.f };
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