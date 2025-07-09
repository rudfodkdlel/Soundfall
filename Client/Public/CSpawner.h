#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

NS_BEGIN(Client)

class CSpawner final : public CBase 
{

private:
	CSpawner();
	virtual ~CSpawner() = default;

public:
	void Set_bUseUpdate(_bool bUseUpdate) { m_bUseUpdate = bUseUpdate; }
	_bool Get_bUseUpdate() { return m_bUseUpdate; }

public:
	HRESULT Initialize();
	void Find_Point();
	void Spawn_Normal_Forest();
	void Spawn_Loop_Forest();
	void Spawn_Boss();
	void Spawn_Discord();

	void Update();
	

private:
	// 스폰 트리거가 충돌하면 이제 가장 가까운 point를 찾아서 소환하도록
	vector<_float4> m_SpawnPoints = {};
	CGameObject* m_pTarget = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	_float4 m_vSpawnPoint = {};

	_bool m_bUseUpdate = {false};

public:
	static CSpawner* Create();
	void Free();


};

NS_END