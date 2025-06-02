#include "Weapon_Base.h"

NS_BEGIN(Client)

class CAxe : public CWeapon_Base
{
private:
	CAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAxe(const CAxe& Prototype);
	virtual ~CAxe() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Attack(_vector vDir) override;

public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END