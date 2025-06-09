#include "CombatStat.h"



CCombatStat::CCombatStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCombatStat::CCombatStat(const CCombatStat& Prototype)
	: CComponent{Prototype}
{
}

HRESULT CCombatStat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCombatStat::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		COMBAT_DESC* pDesc = static_cast<COMBAT_DESC*>(pArg);

		m_iCurrentHp = pDesc->iCurrentHp;
		m_iMaxHp = pDesc->iMaxHp;
		m_iDamage = pDesc->iDamage;
		m_iPreHp = m_iCurrentHp;
		m_fInvincibleDuration = pDesc->fInvincibleDuration;

	}


	return S_OK;
}

void CCombatStat::Attack(CCombatStat* pTarget)
{
	if (nullptr == pTarget)
		return;

	
	pTarget->Hit(m_iDamage);
	
}

void CCombatStat::Hit(_int iDamage)
{
	if (m_bInvincible)
		return;

	m_iCurrentHp -= iDamage;
}

void CCombatStat::Heal(_float fRatio)
{
	_int iAmount = m_iMaxHp * fRatio;
	
	m_iCurrentHp += iAmount;

	if (m_iCurrentHp > m_iMaxHp)
		m_iCurrentHp = m_iMaxHp;
}

_bool CCombatStat::Update(_float fTimeDelta)
{
	
	
	if (m_bInvincible)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime > m_fInvincibleTime)
		{
			m_bInvincible = false;
			m_fElapsedTime = 0.f;
		}
	}

	// 현재 체력 비율은 항상 갱신
	m_eBuffer.fCurrentHpRatio = float(m_iCurrentHp) / m_iMaxHp;

	if (m_iCurrentHp != m_iPreHp)
	{
		// 체력 변화 발생 시
		m_fDisplayTime = 1.f;       // 빨간색 표시 유지 시간 초기화
		m_fDeltaDisplayTime = 0.5f;  // 보라색 표시 유지 시간 초기화

		m_eBuffer.fDisplayTime = m_fDisplayTime;
		m_eBuffer.fDeltaDisplayTime = m_fDeltaDisplayTime;

		m_eBuffer.fDeltaHpRatio = float(m_iPreHp - m_iCurrentHp) / m_iMaxHp;

		m_iPreHp = m_iCurrentHp;     // 이전 체력 동기화 (즉시 해도 됨)

		m_bInvincible = true;
		m_fElapsedTime = 0.f;              // 무적 시간 초기화
		m_fInvincibleTime = m_fInvincibleDuration;
	}
	else
	{
		// 체력 변화 없을 때
		if (m_fDisplayTime > 0.f)
		{
			m_fDisplayTime -= fTimeDelta;
			if (m_fDisplayTime < 0.f) m_fDisplayTime = 0.f;
			m_eBuffer.fDisplayTime = m_fDisplayTime;
		}

		if (m_fDeltaDisplayTime > 0.f)
		{
			m_fDeltaDisplayTime -= fTimeDelta;
			if (m_fDeltaDisplayTime < 0.f)
			{
				m_fDeltaDisplayTime = 0.f;
				m_eBuffer.fDeltaHpRatio = 0.f; // 보라색 사라짐
			}
			m_eBuffer.fDeltaDisplayTime = m_fDeltaDisplayTime;
		}
	}

	if (m_iCurrentHp <= 0)
		return true;

	return false;
}

CCombatStat* CCombatStat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCombatStat* pInstance = new CCombatStat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCombatStat");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CCombatStat::Clone(void* pArg)
{
	CCombatStat* pInstance = new CCombatStat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCombatStat");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CCombatStat::Free()
{
	__super::Free();

}
