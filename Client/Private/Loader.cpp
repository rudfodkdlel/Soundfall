#include "Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Model.h"
#include "Structure.h"
#include "VIBuffer_Cube.h"
//#include "Effect.h"
//#include "Sky.h"
#include "Static_UI.h"
#include "Metronome.h"
#include "Metronome_Counter.h"
#include "Metronome_Center_RIng.h"
#include "PlayerRing.h"
#include "Player.h"
#include "Body_Player.h"
#include "Rifle.h"
#include "Keyboard.h"
#include "Sickle.h"
#include "Axe.h"
#include "Projectile_Monster.h"
#include "Projectile_Player.h"
#include "Projectile_Artillery.h"
#include "Projectile_Shockwave.h"
#include "Discord.h"
#include "Body_Discord.h"
#include "Discord_Wall.h"
#include "Tentacle_Melee.h"
#include "Defender.h"
#include "Body_Defender.h"
#include "Weapon_Defender.h"
#include "Monster_HP.h"


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


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Center_Anchor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/UI/Metronome/Anchor.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Center_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/UI/Metronome/Center_Ring.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/UI/Metronome/MainBar.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Counter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/UI/Metronome/Counter.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Player/PlayerRing.dds"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Terrain/Forest_D_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Terrain_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Terrain/Forest_N_%d.dds"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Global/Projectile/Projectile_Core.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Artillery_Projectile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Monster/Artillery/Artillery_Projectile_%d.dds"), 4))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shockwave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Monster/Shockwave.dds"), 1))))
		return E_FAIL;


	///* For.Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;


	///* For.Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Ky"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Ky/Ky.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rifle"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Weapon/Rifle/Rifle.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity() ;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sickle"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Weapon/Sickle/Sickle.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity() ;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Keyboard"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Weapon/Keyboard/Keyboard.bin", PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Axe"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Weapon/Axe/Axe.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
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

	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Defender"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Defender/Defender.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Shield"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Monster/Defender/Shield/Shield.bin", PreTransformMatrix))))
		return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;
	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		CMetronome::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome_Center_Ring"),
		CMetronome_Center_RIng::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome_Counter"),
		CMetronome_Counter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Static_UI"),
		CStatic_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Ring"),
		CPlayerRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

	/* For.Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Rifle"),
		CRifle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Keyboard"),
		CKeyboard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Sickle"),
		CSickle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Axe"),
		CAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Projecttile_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Monster"),
		CProjectile_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Projecttile_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Artillery"),
		CProjectile_Artillery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Projecttile_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Shockwave"),
		CProjectile_Shockwave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Projecttile_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Player"),
		CProjectile_Player::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_HP"),
		CMonster_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Edit()
{

	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Projectile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Global/Projectile/Projectile_Core.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Terrain/Forest_D_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Terrain_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/Terrain/Forest_N_%d.dds"), 3))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// slope
	//PreTransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_0"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope0.bin", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Slope_1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Resource/Models/Slope/forest_slope1.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Ky"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Ky/Ky.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Peon/Peon.bin", PreTransformMatrix))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	/* For.Prototype_GameObject_Projecttile_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Projectile_Monster"),
		CProjectile_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Structure"),
		CStructure::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	// 추가 필요하고...

	
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
