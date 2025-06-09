#pragma once

#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
NS_END


NS_BEGIN(Client)

class CCombatStat final : public CComponent
{
public:
	typedef struct tagCombatStatDesc
	{
		_int    iMaxHp;
		_int	iCurrentHp;
		_int	iDamage;
		_float  fInvincibleDuration = { 0.f };
	}COMBAT_DESC;

	
	struct HPBUFFER {
		_float fDisplayTime;
		_float fCurrentHpRatio;
		_float fDeltaHpRatio;
		_float fDeltaDisplayTime;
	};


public:
	_int Get_Current_HP() { return m_iCurrentHp; }
	_int Get_Max_HP() { return m_iMaxHp; }
	_int Get_Damage() { return m_iDamage; }
	void Set_Damage(_int iDamage) { m_iDamage = iDamage; }
	_bool Get_bInvinsible() { return m_bInvincible; }
	HPBUFFER& Get_HP_BUFFER() {  return m_eBuffer; }
	void  Set_bInvinsible(_bool bInvincible) { m_bInvincible = bInvincible; }

protected:
	CCombatStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCombatStat(const CCombatStat& Prototype);
	virtual ~CCombatStat() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void Attack(CCombatStat* pTarget);
	void Hit(_int iDamage);
	void Heal(_float fRatio);
	
	// 체력 검사, 쉐이더에 보낼 값 계산하도록?
	_bool Update(_float fTimeDelta);
private:

	_bool	m_bDead = { false };

	_int    m_iMaxHp;
	_int	m_iCurrentHp;
	_int    m_iPreHp;
	_int	m_iDamage;

	_bool   m_bInvincible = { false };
	_float  m_fInvincibleTime = { 0.f };
	_float	m_fElapsedTime = { 0.f };
	_float  m_fInvincibleDuration = {};
	
	HPBUFFER m_eBuffer = {};

	_float  m_fDisplayTime = { 0.f };
	_float m_fDeltaDisplayTime = { 0.5f };
	

public:
	static CCombatStat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END