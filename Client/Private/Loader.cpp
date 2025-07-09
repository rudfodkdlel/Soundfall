#include "Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Model.h"
#include "Structure.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Model_Instance.h"
//#include "Effect.h"
#include "Sky.h"
#include "Player.h"
#include "Body_Player.h"
#include "Projectile_Monster.h"
#include "Projectile_Player.h"
#include "Projectile_Player_Ult_Sickle.h"
#include "Projectile_Artillery.h"
#include "Projectile_Shockwave.h"
#include "Projectile_Falling.h"
#include "Projectile_Shell.h"
#include "Discord.h"
#include "Body_Discord.h"
#include "Discord_Wall.h"
#include "Tentacle_Melee.h"
#include "Tentacle_Amp.h"
#include "Defender.h"
#include "Body_Defender.h"
#include "Weapon_Defender.h"
#include "Monster_HP.h"
#include "Trigger.h"
#include "Artillery.h"
#include "Body_Artillery.h"
#include "Peon.h"
#include "Body_Peon.h"
#include "Navigation.h"
#include "Music_Note.h"
#include "Structure_Instance.h"
#include "Hit_Effect_Texture.h"
#include "Sniper.h"
#include "Body_Sniper.h"
#include "Weapon_Sniper.h"
#include "Attack_Area_Monster.h"
#include "Navi_Icon.h"
#include "Item_Icon.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	/* 자원로딩한다. */
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;	

	return 0;

}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{

	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	HRESULT		hr = CoInitializeEx(nullptr, 0);
	if (FAILED(hr)) {
		OutputDebugString(L"CoInitializeEx 실패\n");
		return E_FAIL;
	} 	

	EnterCriticalSection(&m_CriticalSection);

	hr = S_OK;
	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;

	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL::EDIT:
		hr = Loading_For_Edit();
		break;
	case LEVEL::FOREST:
		hr = Loading_For_Forest();
		break;

	case LEVEL::SHOP:
		hr = Loading_For_Shop();
		break;
	case LEVEL::ARENA:
		hr = Loading_For_Arena();
	}

	

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	
	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));


	

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
	

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));



	

	
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));



	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));



	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	

	_matrix		PreTransformMatrix = XMMatrixIdentity();

 	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
 	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Discord"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Discord/Discord.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Discord_Wall"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Monster/Discord/Wall/Discord_Wall.bin", PreTransformMatrix))))
		return E_FAIL;

 	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tentacle_Melee"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Tentacle_Melee/Tentacle_Melee.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tentacle_Amp"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Tentacle_Amp/Tentacle_Amp.bin", PreTransformMatrix))))
		return E_FAIL;


	// bossmap
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CliffRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CliffRock/CliffRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CliffRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CliffRock/CliffRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CorruptionRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CorruptionRock/CorruptionRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CorruptionRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CorruptionRock/CorruptionRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_C"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_C.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Terrain_Chunk"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/TerrainChunk/Terrain_Chunk.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Snow */
	CVIBuffer_Model_Instance::Model_INSTANCE_DESC		structDesc{};
	structDesc.iNumInstance = 400;
	structDesc.vCenter = _float3(64.f, -0.1f, 104.0f);
	structDesc.vRange = _float3(64.f, 0.0f, 40.f);
	structDesc.vSize = _float2(1.f, 1.f);
	structDesc.strModelDesc = TEXT("Prototype_Component_Model_Terrain_Chunk");

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Struct_CorruptionRock"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext, &structDesc))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;
	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* Prototype_Component_Navigation */
  	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi_Boss.bin")))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	/* For.Prototype_GameObject_Discord */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord"),
		CDiscord::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Discord"),
		CBody_Discord::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Discord_Wall"),
		CDiscord_Wall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Tentacle_Melee"),
		CTentacle_Melee::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Tentacle_Amp"),
		CTentacle_Amp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Forest()
{


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sniper_Range"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Monster/Sniper/Sniper_Range_%d.dds"), 4))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// forest
	PreTransformMatrix = XMMatrixScaling(4.f, 4.f, 4.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_0"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope0.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope1.bin", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	PreTransformMatrix = XMMatrixScaling(4.f, 4.f, 4.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_2"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope2.bin", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Defender"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Defender/Defender.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Shield"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Monster/Defender/Shield/Shield.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Artillery"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Artillery/Artillery.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Peon/Peon.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sniper"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Sniper/Sniper.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sniper_Weapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Monster/Sniper/Weapon/Sniper_Weapon.bin", PreTransformMatrix))))
		return E_FAIL;



	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::FOREST), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi_Forest.bin")))))
		return E_FAIL;


	/* For.Prototype_GameObject_Defender */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Defender"),
		CDefender::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Defender"),
		CBody_Defender::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Defender_Weapon"),
		CWeapon_Defender::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Artillery */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Artillery"),
		CArtillery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Artillery"),
		CBody_Artillery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Peon"),
		CPeon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Peon"),
		CBody_Peon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Sniper"),
		CSniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Sniper"),
		CBody_Sniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sniper_Weapon"),
		CWeapon_Sniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Attack_Area_Monster"),
		CAttack_Area_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Edit()
{




	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));



	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// forest
	PreTransformMatrix = XMMatrixScaling(4.f, 4.f, 4.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_0"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope0.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope1.bin", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	PreTransformMatrix = XMMatrixScaling(4.f, 4.f, 4.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_2"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope2.bin", PreTransformMatrix))))
		return E_FAIL;
	// bossmap
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CliffRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CliffRock/CliffRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CliffRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CliffRock/CliffRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CorruptionRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CorruptionRock/CorruptionRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CorruptionRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/CorruptionRock/CorruptionRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_A"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_A.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_B"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_B.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_PlatformRock_C"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/BossMap/PlatformRock/PlatformRock_C.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Peon/Peon.bin", PreTransformMatrix))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	

	

	/* Prototype_Component_Navigation */
  	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi_Boss.bin")))))
		return E_FAIL;


	// 추가
	
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Shop()
{
	/* For.Prototype_Component_Texture_BackGround*/


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Arena()
{

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CoUninitialize();

	DeleteCriticalSection(&m_CriticalSection);
}
