#include "Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Model.h"
#include "Player.h"
#include "Monster_Base.h"
#include "Structure.h"
//#include "Effect.h"
//#include "Sky.h"
#include "Metronome.h"
#include "Metronome_Counter.h"
#include "Projectile_Monster.h"

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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Metronome_Counter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Textures/UI/Metronome/Counter.dds"), 1))))
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

	///* For.Prototype_Component_Texture_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../Bin/Resources/Textures/Player/Player0.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;


	///* For.Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Ky"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Ky/Ky.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);// * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Peon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Resource/Models/Monster/Peon/Peon.bin", PreTransformMatrix))))
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

	/* For.Prototype_GameObject_Metronome */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome"),
		CMetronome::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Metronome_Counter"),
		CMetronome_Counter::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Base"),
		CMonster_Base::Create(m_pDevice, m_pContext))))
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
		CProjecttile_Monster::Create(m_pDevice, m_pContext))))
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
