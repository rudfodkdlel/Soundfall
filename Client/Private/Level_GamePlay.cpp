#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Metronome.h"
#include "Static_UI.h"
#include "HP_Bar.h"
#include "Terrain.h"
#include "Trigger.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
		: CLevel { pDevice, pContext }
{

}

HRESULT CLevel_GamePlay::Initialize()
{

	// bpm 123
	m_pGameInstance->StopSound(SOUND_BGM);
	m_pGameInstance->PlaySound(TEXT("Discordance.ogg"), SOUND_BGM, 0.7f);
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

	if (m_pGameInstance->Key_Up(DIK_P))
	{
		if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Defender"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Monster"))))
			return;

		/*if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Artillery"),
			static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Monster"))))
			return;*/
	}

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

	wstring str = std::wstring(TEXT("현재 Interval :")) + std::to_wstring(m_pGameInstance->Get_BeatInterval());

	m_pGameInstance->Draw_Font(TEXT("Default"), str.c_str(), _float2(10.f, 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring strLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc = {};

	pDesc.bWired = false;
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_TopDown"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		static_cast<_uint>(LEVEL::GAMEPLAY), TEXT("Layer_Player"))))
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
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

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
