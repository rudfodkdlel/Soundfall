#include "CSpawner.h"

CSpawner::CSpawner()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSpawner::Initialize()
{
	// 위치 추가
	m_SpawnPoints.push_back({ 20.f,0.f,65.f,1.f });
	m_SpawnPoints.push_back({ 115.f,0.f,115.f,1.f });
	m_SpawnPoints.push_back({ 110.f,0.f,20.f,1.f });

	return S_OK;
}

void CSpawner::Find_Point()
{
	if (nullptr == m_pTarget)
	{
		auto pTarget = m_pGameInstance->GetLastObjectFromLayer(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Player"));

		if (nullptr == pTarget)
			return;

		m_pTarget = pTarget;
	
	}
	// 순회하면서 가장 가까운 거리를 체크하고. spawnpoint를 결정

	_float fMinDist = { FLT_MAX };

	for (auto& vSpawnPoint : m_SpawnPoints)
	{
		_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vSpawnPoint) - m_pTarget->Get_Transform()->Get_State(STATE::POSITION)));

		if (fDist < fMinDist)
		{
			fMinDist = fDist;
			m_vSpawnPoint = vSpawnPoint;
		}
	}

}

void CSpawner::Spawn_Normal_Forest()
{
	Find_Point();
	CGameObject::GAMEOBJECT_DESC eDesc = {};

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-5, 5);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-5, 5);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Defender"), ENUM_CLASS(LEVEL::FOREST), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-5, 5);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-5, 5);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Artillery"), ENUM_CLASS(LEVEL::FOREST), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-5, 5);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-5, 5);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Peon"), ENUM_CLASS(LEVEL::FOREST), TEXT("Layer_Monster"), &eDesc);

	eDesc.vPos = m_vSpawnPoint;
	eDesc.vPos.x = m_vSpawnPoint.x + m_pGameInstance->Compute_Random(-5, 5);
	eDesc.vPos.z = m_vSpawnPoint.z + m_pGameInstance->Compute_Random(-5, 5);

	m_pGameInstance->Add_GameObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Monster_Sniper"), ENUM_CLASS(LEVEL::FOREST), TEXT("Layer_Monster"), &eDesc);

}

void CSpawner::Spawn_Loop_Forest()
{
	Find_Point();
}

void CSpawner::Spawn_Boss()
{
	Find_Point();
}

void CSpawner::Spawn_Discord()
{
}

void CSpawner::Update()
{
	if (!m_bUseUpdate)
		return;

	// layer를 먼저 찾는다
	auto pMonsterLayer = m_pGameInstance->GetLayerList(m_pGameInstance->Get_Current_Level(), TEXT("Layer_Monster"));

	if (nullptr == pMonsterLayer)
		return;

	// removeif로 비어있는 곳을 삭제하고... 

	

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

	Safe_Release(m_pGameInstance);
}
