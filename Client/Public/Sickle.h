#include "Weapon_Base.h"

NS_BEGIN(Client)

class CSickle : public CWeapon_Base
{
private:
	CSickle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSickle(const CSickle& Prototype);
	virtual ~CSickle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT On_Collision(CGameObject* Other, CCollider* pCollider);

	virtual void Attack(_vector vDir) override;

	void Reset() { m_HitObjectSet.clear(); }

public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


private:
	unordered_set<CCollider*>m_HitObjectSet;
	class CCombatStat* m_pCombatCom = { nullptr };

public:
	static CSickle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END