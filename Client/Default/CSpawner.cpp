#include "CSpawner.h"
#include "GameObject.h"

CSpawner::CSpawner()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSpawner::Initialize()
{
	

	auto pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

	if (nullptr == pTarget)
		return E_FAIL;

	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);


	// 위치 추가
	m_SpawnPoints.push_back({ 20.f,0.f,65.f,1.f});
	m_SpawnPoints.push_back({ 115.f,0.f,115.f,1.f });
	m_SpawnPoints.push_back({ 115.f,0.f,15.f,1.f });

	return S_OK;
}

void CSpawner::Find_Point()
{
	// 순회하면서 가장 가까운 거리를 체크하고. spawnpoint를 결정

	if (nullptr == m_pTarget)
		return;

	_float fMinDist = {FLT_MAX};

	for (auto& vSpawnPos : m_SpawnPoints)
	{
		_float fDist ;

		

		if (fDist < fMinDist)
		{
			fMinDist = fDist;
			m_vSpawnPoint = vSpawnPos;
		}
			
	}

}

void CSpawner::Spawn_Normal()
{
	Find_Point();

	CGameObject::GAMEOBJECT_DESC eDesc = {};

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-10, 10);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-10, 10);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Defender"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-10, 10);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-10, 10);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Artillery"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-10, 10);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-10, 10);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Defender"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-10, 10);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-10, 10);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Artillery"), m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster"), &eDesc);


		
}

void CSpawner::Spawn_Loop()
{
}

void CSpawner::Spawn_Boss()
{
}

CSpawner* CSpawner::Create()
{
	CSpawner* pInstance = new CSpawner();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpawner::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pGameInstance);
}
