#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCollider;
class CGameObject;

class  CCollider_Manager final : public CBase
{
public:


private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;


public:

	// gameinstance에 등록해서 오브젝트에서 추가. 
	HRESULT Add_Collider(_uint iIndex, CCollider* Collider, CGameObject* pOwner);
	// 충돌 검사를 할 타입들을 미리 넣어둔다
	HRESULT Add_Collider_Group(pair<_uint, _uint> typePair);
	
	void CallbackCollider(CCollider* p);

	void Check_Nullptr();

	// free때 사용
	void Clear();

	// enum으로 원하는 타입만 충돌 처리 하도록
	void Update();

	// collider에서 만든 intersect를 순회하면서 다 처리하도록?
	void Instercects_Group(list<CCollider*> src, list<CCollider*> dst);

private:

	vector<list<CCollider*>> m_pColliders = {};

	// 클라에서 어떤 오브젝트 타입끼리 충동시킬지 정하기
	vector<pair<_uint, _uint>> m_CollisionPairs = {};



public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

NS_END
