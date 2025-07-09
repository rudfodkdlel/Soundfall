#pragma once

#include "Client_Defines.h"
#include "GameObject.h"#include "GameInstance.h"

NS_BEGIN(Client)

class CSpawner final : public CBase 
{

private:
	CSpawner();
	virtual ~CSpawner() = default;

public:
	HRESULT Initialize();
	void Find_Point();
	void Spawn_Normal();
	void Spawn_Loop();
	void Spawn_Boss();
	

private:
	// 스폰 트리거가 충돌하면 이제 가장 가까운 point를 찾아서 소환하도록
	vector<_float4> m_SpawnPoints = {};
	CGameObject* m_pTarget = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	_float4 m_vSpawnPoint = {};

public:
	static CSpawner* Create();
	void Free();


};

NS_END